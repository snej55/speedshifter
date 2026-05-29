// Created by Jens Kromdijk 23/05/2026

#include "player.h"
#include <chrono>
#include <qnamespace.h>
#include <thread>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

void maDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    Player* player{static_cast<Player*>(pDevice->pUserData)};
    if (!player || !player->playing())
    {
        // zero output
        std::memset(pOutput, 0, frameCount * pDevice->playback.channels * sizeof(float));
        return;
    }

    float* outputBuffer{static_cast<float*>(pOutput)};
    ma_uint32 frames{frameCount};
    void* pReadBuffer;

    ma_result result{ma_pcm_rb_acquire_read(&player->m_ringBuffer, &frames, &pReadBuffer)};

    if (result == MA_SUCCESS && frames > 0)
    {
        // copy memory from ring buffer
        std::memcpy(outputBuffer, pReadBuffer, frames * DEVICE_CHANNELS * sizeof(float));
        ma_pcm_rb_commit_read(&player->m_ringBuffer, frames);

        player->m_frameCount.fetch_add(static_cast<std::size_t>(static_cast<float>(frameCount) * player->m_speed.load()));

        if (frames < frameCount)
        {
            // clear rest of frames
            std::memset(
                outputBuffer + (frames * DEVICE_CHANNELS), 0, (frameCount - frames) * DEVICE_CHANNELS * sizeof(float));
        }
    }
    else
    {
        // zero output
        std::memset(pOutput, 0, frameCount * pDevice->playback.channels * sizeof(float));
    }

    player->updatePositionCallback();
}

Player::Player(QObject* parent) : QObject{parent}
{
    connect(this, &Player::signalStop, this, &Player::handleStop, Qt::QueuedConnection);
    connect(this, &Player::signalPositionUpdate, this, &Player::updatePosition, Qt::QueuedConnection);

    // get ready to process data
    m_processData.store(true);
    initWorkerThread();
}

Player::~Player()
{
    // stop processing before device is destroyed
    m_processData.store(false);
    if (m_processor.joinable())
    {
        m_processor.join();
    }

    if (m_deviceInit)
    {
        ma_device_uninit(&m_device);
    }

    if (m_converterInit)
    {
        ma_data_converter_uninit(&m_converter, nullptr);
    }

    if (m_rbInit)
    {
        ma_pcm_rb_uninit(&m_ringBuffer);
    }
}

void Player::setFilePath(const QString& path)
{
    if (m_filePath != path)
    {
        m_filePath = path;
        Q_EMIT filePathChanged();
    }
}

void Player::stopPlaybackCallback() { Q_EMIT signalStop(); }

void Player::updatePositionCallback() { Q_EMIT signalPositionUpdate(); }

void Player::handleStop() { pause(); }

void Player::updatePosition()
{
    float pos{static_cast<float>(m_frameCount.load()) / static_cast<float>(m_sampleRate)};
    if (std::abs(pos - m_position) > 0.05f)
    {
        m_position = pos;
        if (m_position >= m_duration)
        {
            stopPlaybackCallback();
        }
        Q_EMIT positionChanged();
    }
}

void Player::setPosition(float seconds)
{
    const bool playing{m_playing.load()};
    m_playing.store(false); // stop processing data

    if (m_rbInit)
    {
        // flush ring buffer
        ma_pcm_rb_reset(&m_ringBuffer);
    }

    seconds = std::clamp(seconds, 0.0f, m_duration);

    long targetIndex{static_cast<long>(seconds * static_cast<float>(m_channels) * static_cast<float>(m_sampleRate))};
    targetIndex -= (targetIndex % m_channels);
    m_readIndex.store(
        static_cast<std::size_t>(std::clamp<long>(targetIndex, 0, static_cast<long>(m_pcmBuffer.size()))));

    m_frameCount.store(static_cast<std::size_t>(seconds * static_cast<float>(m_sampleRate)));

    m_position = seconds;
    m_stretcher.reset();

    if (m_deviceInit && m_converterInit)
    {
        ma_data_converter_reset(&m_converter);
    }

    m_playing.store(playing);

    Q_EMIT positionChanged();
}

void Player::play()
{
    if (!m_playing)
    {
        m_playing = true;
        Q_EMIT playingChanged();
        if (m_deviceInit)
        {
            ma_device_start(&m_device);
        }
    }
}

void Player::pause()
{
    if (m_playing)
    {
        m_playing = false;
        Q_EMIT playingChanged();
        if (m_deviceInit)
        {
            ma_device_stop(&m_device);
        }
    }
}

void Player::setSpeed(const float t)
{
    m_speed.store(std::clamp(t, MIN_SPEED, MAX_SPEED));
    /*if (m_rbInit)
    {
        ma_pcm_rb_reset(&m_ringBuffer);
    }*/
    Q_EMIT speedChanged();
}

void Player::loadFile(const QUrl& fileUrl)
{
    pause();
    setPosition(0);

    QString filePath{fileUrl.toLocalFile()};
    AVFormatContext* formatContext{nullptr};
    int ret{avformat_open_input(&formatContext, filePath.toStdString().c_str(), nullptr, nullptr)};
    if (ret < 0)
    {
        qWarning() << "ERROR decoding media: Failed to open file: `" << filePath << "`!";
        return;
    }

    ret = avformat_find_stream_info(formatContext, nullptr);
    if (ret < 0)
    {
        qWarning() << "ERROR decoding media: Failed to find stream info!";
        avformat_close_input(&formatContext);
        return;
    }

    int streamIndex{av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0)};
    if (streamIndex < 0)
    {
        qWarning() << "ERROR decoding media: No audio stream found in `" << filePath << "`";
        avformat_close_input(&formatContext);
        return;
    }

    const AVStream* stream{formatContext->streams[streamIndex]};
    const AVCodec* avDecoder{avcodec_find_decoder(stream->codecpar->codec_id)};
    if (!avDecoder)
    {
        qWarning() << "ERROR decoding media: no decoder found!";
        avformat_close_input(&formatContext);
        return;
    }

    AVCodecContext* decoderCtx{avcodec_alloc_context3(avDecoder)};
    avcodec_parameters_to_context(decoderCtx, stream->codecpar);

    ret = avcodec_open2(decoderCtx, avDecoder, nullptr);
    if (ret < 0)
    {
        qWarning() << "ERROR decoding media: failed to open decoder!";
        return;
    }

    m_sampleRate = decoderCtx->sample_rate;
    m_channels = decoderCtx->ch_layout.nb_channels;

    AVChannelLayout inChannelLayout{decoderCtx->ch_layout};
    if (inChannelLayout.order == AV_CHANNEL_ORDER_UNSPEC || inChannelLayout.nb_channels == 0)
    {
        av_channel_layout_default(&inChannelLayout, m_channels);
    }
    else
    {
        av_channel_layout_copy(&inChannelLayout, &decoderCtx->ch_layout);
    }

    AVChannelLayout outChannelLayout;
    av_channel_layout_default(&outChannelLayout, m_channels);

    AVPacket* packet{av_packet_alloc()};
    AVFrame* frame{av_frame_alloc()};

    SwrContext* swrContext{swr_alloc()};
    ret = swr_alloc_set_opts2(
        &swrContext,
        &outChannelLayout,
        AV_SAMPLE_FMT_FLT,
        m_sampleRate,
        &inChannelLayout,
        decoderCtx->sample_fmt,
        m_sampleRate,
        0,
        nullptr);

    if (ret < 0 || !swrContext)
    {
        qWarning() << "ERROR decoding media: Failed to allocate SwrContext options!";
        av_packet_free(&packet);
        av_frame_free(&frame);
        avformat_close_input(&formatContext);
        return;
    }

    ret = swr_init(swrContext);
    if (ret < 0)
    {
        qWarning() << "ERROR decoding media: swr_init() failed!";
        swr_free(&swrContext);
        av_packet_free(&packet);
        av_frame_free(&frame);
        avformat_close_input(&formatContext);
        return;
    }

    av_channel_layout_uninit(&inChannelLayout);
    av_channel_layout_uninit(&outChannelLayout);

    m_pcmBuffer.clear();
    // preallocate memory if possible to prevent vector resizing too much
    if (formatContext->duration != AV_NOPTS_VALUE)
    {
        double durationSec{static_cast<double>(formatContext->duration) / static_cast<double>(AV_TIME_BASE)};
        m_pcmBuffer.reserve(static_cast<std::size_t>(durationSec * m_sampleRate * m_channels));
    }

    while (av_read_frame(formatContext, packet) >= 0)
    {
        if (packet->stream_index != streamIndex)
            continue;

        ret = avcodec_send_packet(decoderCtx, packet);
        if (ret < 0 && (ret != AVERROR(EAGAIN)))
        {
            qWarning() << "Failed to decode frame!";
            continue;
        }

        while (avcodec_receive_frame(decoderCtx, frame) >= 0)
        {
            int maxSamples{swr_get_out_samples(swrContext, frame->nb_samples)};
            std::vector<float> tempBuffer(maxSamples * m_channels);

            uint8_t* data{reinterpret_cast<uint8_t*>(tempBuffer.data())};
            int outSamples{swr_convert(swrContext, &data, maxSamples, (const uint8_t**)frame->data, frame->nb_samples)};
            if (outSamples > 0)
            {
                m_pcmBuffer.insert(
                    m_pcmBuffer.end(), tempBuffer.begin(), tempBuffer.begin() + (outSamples * m_channels));
            }
        }

        av_packet_unref(packet);
    }

    // tidy up
    av_packet_free(&packet);
    av_frame_free(&frame);
    swr_free(&swrContext);
    avcodec_free_context(&decoderCtx);
    avformat_close_input(&formatContext);

    const bool convert{convertPCM(m_sampleRate, m_channels) < 0};

    m_readIndex = 0;
    m_duration = static_cast<float>(m_pcmBuffer.size()) / m_sampleRate / m_channels;
    m_position = 0.0f;

    Q_EMIT durationChanged();
    Q_EMIT positionChanged();

    if (!m_rbInit)
    {
        if (ma_pcm_rb_init(ma_format_f32, DEVICE_CHANNELS, MAX_FRAMES * 4, nullptr, nullptr, &m_ringBuffer) !=
            MA_SUCCESS)
        {
            qWarning() << "Failed to initialize ring buffer!";
            return;
        }
        m_rbInit = true;
    }
    ma_pcm_rb_reset(&m_ringBuffer);

    if (!m_deviceInit)
    {
        ma_device_config deviceConfig{ma_device_config_init(ma_device_type_playback)};
        deviceConfig.playback.format = ma_format_f32;
        // fix audio settings so always good quality
        deviceConfig.playback.channels = DEVICE_CHANNELS;
        deviceConfig.sampleRate = DEVICE_SAMPLERATE;
        deviceConfig.dataCallback = maDataCallback;
        deviceConfig.periodSizeInFrames = MAX_FRAMES;
        deviceConfig.pUserData = this;

        if (ma_device_init(nullptr, &deviceConfig, &m_device) != MA_SUCCESS)
        {
            qWarning() << "Failed to initialize MiniAudio device!";
            return;
        }
        else
        {
            m_deviceInit = true;
        }

        m_stretcher.presetDefault(m_channels, m_sampleRate);
        initBuffers();
    }
    m_stretcher.reset();
    m_frameCount.store(0);

    if (m_deviceInit && convert)
    {
        if (m_converterInit)
        {
            ma_data_converter_uninit(&m_converter, nullptr);
            m_converterInit = false;
        }

        if (m_sampleRate != DEVICE_SAMPLERATE || m_channels != DEVICE_CHANNELS)
        {
            qWarning() << "Input audio stream differs from device sample rate and channels...";
            qWarning() << "Setting up ma_data_converter (" << static_cast<float>(m_sampleRate) * 0.001f << "kHz => "
                       << static_cast<float>(DEVICE_SAMPLERATE) * 0.001f << "kHz, " << m_channels << " channels => "
                       << DEVICE_CHANNELS << " channels)";
            m_converterInit = true;
            ma_data_converter_config converterConfig{ma_data_converter_config_init(
                ma_format_f32, ma_format_f32, m_channels, DEVICE_CHANNELS, m_sampleRate, DEVICE_SAMPLERATE)};

            if (ma_data_converter_init(&converterConfig, nullptr, &m_converter) != MA_SUCCESS)
            {
                qWarning() << "Failed to initialize data converter!";
                m_converterInit = false;
                return;
            }
        }
    }
}

void Player::initBuffers()
{
    const std::size_t maxInputFrames{static_cast<std::size_t>(MAX_FRAMES * MAX_SPEED * 1.2f)};
    m_inputBuffer[0].resize(maxInputFrames);
    m_inputBuffer[1].resize(maxInputFrames);

    constexpr std::size_t maxSize{MAX_FRAMES * static_cast<std::size_t>(1.0 / MIN_SPEED)};
    m_outputBuffer[0].resize(maxSize * 1.2);
    m_outputBuffer[1].resize(maxSize * 1.2);
}

int Player::convertPCM(const int sampleRate, const int channels)
{
    ma_data_converter converter;
    ma_data_converter_config converterConfig{ma_data_converter_config_init(
        ma_format_f32, ma_format_f32, channels, DEVICE_CHANNELS, sampleRate, DEVICE_SAMPLERATE)};
    if (ma_data_converter_init(&converterConfig, nullptr, &converter) != MA_SUCCESS)
    {
        qWarning() << "Failed to initialize data converter!";
        return -1;
    }

    const std::size_t totalFrames{m_pcmBuffer.size() / channels};

    ma_uint64 bufferSize;
    ma_data_converter_get_expected_output_frame_count(&converter, totalFrames, &bufferSize);

    std::vector<float> tempBuffer(bufferSize * DEVICE_CHANNELS);

    std::size_t inputIndex{0};
    std::size_t writeIndex{0};
    constexpr ma_uint64 stepSize{512};

    while (inputIndex < totalFrames)
    {
        ma_uint64 framesIn{std::min(static_cast<ma_uint64>(totalFrames - inputIndex), stepSize)};
        ma_uint64 framesOut{static_cast<ma_uint64>(bufferSize - writeIndex)};

        const float* pInputData{m_pcmBuffer.data() + inputIndex * channels};
        float* pOutputData{tempBuffer.data() + writeIndex * DEVICE_CHANNELS};

        ma_data_converter_process_pcm_frames(&converter, pInputData, &framesIn, pOutputData, &framesOut);

        inputIndex += framesIn;
        writeIndex += framesOut;

        // if it's empty exit
        if (framesIn == 0 && framesOut == 0)
        {
            break;
        }
    }

    m_pcmBuffer.swap(tempBuffer);
    tempBuffer.clear();

    m_sampleRate = DEVICE_SAMPLERATE;
    m_channels = DEVICE_CHANNELS;

    ma_data_converter_uninit(&converter, nullptr);

    return 0;
}

void processPCM(void* data)
{
    if (!data)
        return;

    Player* player{static_cast<Player*>(data)};

    while (player->m_processData.load())
    {
        if (!player->m_playing.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }

        ma_uint32 space{ma_pcm_rb_available_write(&player->m_ringBuffer)};

        if (space >= MAX_FRAMES)
        {
            const std::size_t currentReadIndex{player->m_readIndex.load()};
            const float speed{player->m_speed.load()};

            // calculate frame io size
            const std::size_t inputFrames{
                std::max<std::size_t>(static_cast<std::size_t>(static_cast<float>(MAX_FRAMES) * speed + 0.5f), 1)};
            const std::size_t totalSize{player->m_pcmBuffer.size()};

            if (currentReadIndex > totalSize)
            {
                // zero output
                void* pWriteBuffer;
                ma_uint32 dataSize{MAX_FRAMES};
                ma_result result{ma_pcm_rb_acquire_write(&player->m_ringBuffer, &dataSize, &pWriteBuffer)};
                if (result == MA_SUCCESS && dataSize != 0)
                {
                    float* chunk{static_cast<float*>(pWriteBuffer)};
                    for (std::size_t i{0}; i < dataSize * DEVICE_CHANNELS; ++i)
                    {
                        chunk[i] = 0.0f;
                    }
                    ma_pcm_rb_commit_write(&player->m_ringBuffer, dataSize);
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            const std::size_t availableSamples{currentReadIndex <= totalSize ? totalSize - currentReadIndex : 0};
            const std::size_t frames{std::min(inputFrames, availableSamples)};

            // make sure buffers' capacity is big enough
            if (player->m_inputBuffer[0].size() < inputFrames)
            {
                player->m_inputBuffer[0].resize(inputFrames);
                player->m_inputBuffer[1].resize(inputFrames);
            }

            if (player->m_outputBuffer[0].size() < MAX_FRAMES)
            {
                player->m_outputBuffer[0].resize(MAX_FRAMES);
                player->m_outputBuffer[1].resize(MAX_FRAMES);
            }

            // de-interleave data
            for (std::size_t i{0}; i < inputFrames; ++i)
            {
                if (i < frames && currentReadIndex + i * DEVICE_CHANNELS + 1 < totalSize)
                {
                    player->m_inputBuffer[0][i] = player->m_pcmBuffer[currentReadIndex + i * DEVICE_CHANNELS];
                    player->m_inputBuffer[1][i] = player->m_pcmBuffer[currentReadIndex + i * DEVICE_CHANNELS + 1];
                    continue;
                }

                player->m_inputBuffer[0][i] = 0.0f;
                player->m_inputBuffer[1][i] = 0.0f;
            }

            player->m_stretcher.process(
                player->m_inputBuffer.data(), inputFrames, player->m_outputBuffer.data(), MAX_FRAMES);
            player->m_readIndex.store(currentReadIndex + frames * DEVICE_CHANNELS);

            // write processed data to ring buffer
            void* pWriteBuffer;
            ma_uint32 dataSize{MAX_FRAMES}; // NOTE: dataSize could be less than MAX_FRAMES
            ma_result result{ma_pcm_rb_acquire_write(&player->m_ringBuffer, &dataSize, &pWriteBuffer)};
            if (result != MA_SUCCESS || dataSize == 0)
            {
                // ring buffer is full
                continue;
            }

            float* chunk{static_cast<float*>(pWriteBuffer)};
            for (std::size_t i{0}; i < dataSize; ++i)
            {
                chunk[i * 2] = player->m_outputBuffer[0][i];
                chunk[i * 2 + 1] = player->m_outputBuffer[1][i];
            }

            ma_pcm_rb_commit_write(&player->m_ringBuffer, dataSize);
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(6));
        }
    }
}

void Player::initWorkerThread() { m_processor = std::thread(processPCM, static_cast<void*>(this)); }
