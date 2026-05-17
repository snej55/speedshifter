// Created by Jens Kromdijk 23/05/2026

#include "player.h"

void maDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    Player* player{static_cast<Player*>(pDevice->pUserData)};
    if (!player || !player->playing())
    {
        std::memset(pOutput, 0, frameCount * pDevice->playback.channels * sizeof(float));
    }

    float* outputBuffer{static_cast<float*>(pOutput)};
    std::size_t targetChannels{pDevice->playback.channels}; // should always be two
    if (!player->getConverterInit())
    {
        std::size_t samplesNeeded{frameCount * targetChannels};
        for (std::size_t i{0}; i < samplesNeeded; ++i)
        {
            std::size_t currentIndex{player->m_readIndex.load()};
            if (player->m_readIndex < player->m_pcmBuffer.size())
            {
                outputBuffer[i] = player->m_pcmBuffer[currentIndex];
                ++player->m_readIndex;
            }
            else
            {
                outputBuffer[i] = 0.0f;
                player->pause();
            }
        }

        player->setPosition(
            static_cast<float>(player->m_readIndex) / static_cast<float>(player->getChannels()) /
            static_cast<float>(player->getSampleRate()));
        return;
    }

    ma_uint64 outputFrames{frameCount};
    ma_uint64 readFrames;
    ma_result result{ma_data_converter_get_required_input_frame_count(player->getConverter(), frameCount, &readFrames)};
    if (result != MA_SUCCESS)
    {
        std::memset(pOutput, 0, frameCount * pDevice->playback.channels * sizeof(float));
        return;
    }

    std::size_t totalSamples{readFrames * player->getChannels()};
    std::vector<float> tempBuffer(totalSamples, 0.0f);

    std::size_t samplesLeft{player->m_pcmBuffer.size() - player->m_readIndex};
    std::size_t copySamples{std::min(totalSamples, samplesLeft)};
    if (copySamples > 0)
    {
        std::copy(
            player->m_pcmBuffer.begin() + player->m_readIndex,
            player->m_pcmBuffer.begin() + player->m_readIndex + copySamples,
            tempBuffer.begin());
        player->m_readIndex += copySamples;
    }
    else
    {
        player->m_playing = false;
        return;
    }

    ma_uint64 actualFrames{readFrames};
    ma_data_converter_process_pcm_frames(
        player->getConverter(), tempBuffer.data(), &actualFrames, pOutput, &outputFrames);
    player->setPosition(
        static_cast<float>(player->m_readIndex) / static_cast<float>(player->getChannels()) /
        static_cast<float>(player->getSampleRate()));
}

Player::Player(QObject* parent) : QObject{parent} {}

Player::~Player() = default;

void Player::setFilePath(const QString& path)
{
    if (m_filePath != path)
    {
        m_filePath = path;
        Q_EMIT filePathChanged();
    }
}

void Player::play()
{
    m_playing = true;
    m_updateState = true;
}

void Player::pause()
{
    m_playing = false;
    m_updateState = true;
}

void Player::update()
{
    if (m_updateState)
    {
        if (m_playing)
        {
            ma_device_start(&m_device);
        }
        else
        {
            ma_device_stop(&m_device);
        }
        m_updateState = false;
    }
}

void Player::loadFile(const QUrl& fileUrl)
{
    pause();

    ma_result result;
    ma_decoder decoder; // temporary decoder

    QString filePath{fileUrl.toLocalFile()};
    result = ma_decoder_init_file(filePath.toStdString().c_str(), nullptr, &decoder);
    if (result != MA_SUCCESS)
    {
        qWarning() << "Failed to open file: " << filePath;
        return;
    }

    m_sampleRate = decoder.outputSampleRate;
    m_channels = decoder.outputChannels;

    // get total number of PCM frames
    ma_uint64 totalFrames{0};
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);

    m_pcmBuffer.resize(m_channels * totalFrames);

    ma_uint64 framesRead{0};
    ma_decoder_read_pcm_frames(&decoder, m_pcmBuffer.data(), totalFrames, &framesRead);

    ma_decoder_uninit(&decoder);

    m_readIndex = 0;
    m_duration = static_cast<float>(totalFrames) / m_sampleRate;
    m_position = 0.0f;

    Q_EMIT durationChanged();
    Q_EMIT positionChanged();

    if (!m_deviceInit)
    {
        ma_device_config deviceConfig{ma_device_config_init(ma_device_type_playback)};
        deviceConfig.playback.format = ma_format_f32; // required for soundtouch
        // fix audio settings so always good quality
        deviceConfig.playback.channels = 2;
        deviceConfig.sampleRate = 48000;
        deviceConfig.dataCallback = maDataCallback;
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
    }

    if (m_deviceInit)
    {
        if (m_converterInit)
        {
            ma_data_converter_uninit(&m_converter, nullptr);
            m_converterInit = false;
        }

        if (m_sampleRate != 48000 || m_channels != 2)
        {
            m_converterInit = true;
            ma_data_converter_config converterConfig{
                ma_data_converter_config_init(ma_format_f32, ma_format_f32, m_channels, 2, m_sampleRate, 48000)};

            if (ma_data_converter_init(&converterConfig, nullptr, &m_converter) != MA_SUCCESS)
            {
                qWarning() << "Failed to initialize data converter!";
                m_converterInit = false;
                return;
            }
        }
    }
}
