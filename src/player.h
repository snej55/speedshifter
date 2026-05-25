// Created by Jens Kromdijk 23/05/2026

#ifndef SPEEDSHIFTER_PLAYER_H
#define SPEEDSHIFTER_PLAYER_H

#include <QObject>
#include <qqml.h>

#include <miniaudio.h>
#include <signalsmith-stretch.h>

#include <vector>
#include <atomic>
#include <mutex>
#include <array>

// Default: 48kHz stereo
#define DEVICE_CHANNELS 2
#define DEVICE_SAMPLERATE 48000
#define MAX_FRAMES 1024

#define MIN_SPEED 0.2f
#define MAX_SPEED 2.f

class Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
    Q_PROPERTY(float position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(float duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(float speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(float minSpeed READ minSpeed)
    Q_PROPERTY(float maxSpeed READ maxSpeed)
    QML_ELEMENT

public:
    explicit Player(QObject* parent = nullptr);
    ~Player();

    [[nodiscard]] QString filePath() const { return m_filePath; }
    void setFilePath(const QString& path);

    [[nodiscard]] bool playing() const { return m_playing.load(); }
    [[nodiscard]] float position() const { return m_position; }
    void setPosition(float seconds);
    [[nodiscard]] float duration() const { return m_duration; }

    [[nodiscard]] std::vector<float>* getPCM_Buffer() { return &m_pcmBuffer; }
    [[nodiscard]] std::atomic<std::size_t>& getReadIndex() { return m_readIndex; }
    [[nodiscard]] int getSampleRate() const { return m_sampleRate; }
    [[nodiscard]] int getChannels() const { return m_channels; }

    [[nodiscard]] ma_data_converter* getConverter() { return &m_converter; }
    [[nodiscard]] bool getConverterInit() const { return m_converterInit; }

    Q_INVOKABLE
    void play();
    Q_INVOKABLE
    void pause();
    Q_INVOKABLE
    void loadFile(const QUrl& fileUrl);

    // only get called from maDataCallback
    void stopPlaybackCallback();
    void updatePositionCallback();

    [[nodiscard]] float speed() const {return m_speed.load();};
    Q_INVOKABLE
    void setSpeed(float t) {m_speed.store(std::clamp(t, MIN_SPEED, MAX_SPEED));}

    [[nodiscard]] float minSpeed() const {return m_minSpeed;}
    [[nodiscard]] float maxSpeed() const {return m_maxSpeed;}

    [[nodiscard]] signalsmith::stretch::SignalsmithStretch<float>& getStretcher() {return m_stretcher;}

signals:
    void filePathChanged();
    void playingChanged();
    void positionChanged();
    void durationChanged();

    void signalStop();
    void signalPositionUpdate();

    void speedChanged();

private slots:
    void handleStop();
    void updatePosition();

private:
    QString m_filePath;

    ma_device m_device;
    bool m_deviceInit{false};

    ma_data_converter m_converter;
    bool m_converterInit{false};

    // raw PCM data in RAM
    std::vector<float> m_pcmBuffer{};
    std::atomic<std::size_t> m_readIndex{0};
    // defaults (44.1 kHz stereo)
    int m_sampleRate{44100};
    int m_channels{2};

    // Miniaudio PCM data: [R L R L R L R L] (interleaved)
    // Signalsmith stretch input: [R R R R], [L L L L] (split data)
    std::array<std::vector<float>, 2> m_inputBuffer{}; // input from m_pcmBuffer
    std::array<std::vector<float>, 2> m_outputBuffer{}; // output from stretcher.processs()

    signalsmith::stretch::SignalsmithStretch<float> m_stretcher;
    std::atomic<float> m_speed{1.0f};
    static constexpr float m_minSpeed{MIN_SPEED};
    static constexpr float m_maxSpeed{MAX_SPEED};

    std::atomic<bool> m_playing{false};
    float m_position{0.0f};
    float m_duration{0.0f};

    // setup miniaudio backend
    friend void maDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

    [[nodiscard]] int convertPCM(int sampleRate, int channels);
    void initBuffers();
};

#endif // SPEEDSHIFTER_PLAYER_H
