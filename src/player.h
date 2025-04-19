#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QString>
#include <QMediaPlayer>

#include <qqml.h>
// gstreamer
#include <gst/gst.h>

#include "timer.h"

// for gstreamer playback
struct StreamData {
    GstElement* playbin;
    GstElement* bin;
    GstElement* scaletempo;
    GstElement* convert;
    GstElement* resample;
    GstElement* sink;
    GstElement* src;

    gint n_audio; // num of embedded audio streams
    gint current_audio; // currently playing audio stream

    GMainLoop* main_loop; // GLibs main loop

    gboolean playing;
    gboolean terminate;
    gboolean seek_enabled;
    gboolean seek_done;
    gint64 duration;

    GstState state;
    void* player;

    gdouble rate; // playback rate
};

enum GstPlayFlags {
    GST_PLAY_FLAG_VIDEO = (1 << 0), // if we want video
    GST_PLAY_FLAG_AUDIO = (1 << 1), // if we want audio
    GST_PLAY_FLAG_TEXT = (1 << 2) // if we want text
};

class Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(float playbackSpeed READ playbackSpeed WRITE setPlaybackSpeed NOTIFY playbackSpeedChanged)
    Q_PROPERTY(int duration READ getDuration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(int timeElapsed READ getTimeElapsed WRITE setTimeElapsed NOTIFY timeElapsedChanged)
    Q_PROPERTY(bool playing READ getPlaying NOTIFY playingChanged)
    QML_ELEMENT

public:
    explicit Player(QObject* parent = nullptr);
    ~Player();

    void init(int argc, char* argv[]);
    void free();

    void load();

    Q_INVOKABLE
    void play();

    Q_INVOKABLE
    void pause();

    Q_INVOKABLE
    void stop();

    Q_INVOKABLE
    void seek(const int& pos, bool force=false);
    bool isSeeking();

    // sends a seek event to change rate
    void update_rate();

    [[nodiscard]] QString filePath() const;
    void setFilePath(const QString& val);

    // seconds
    [[nodiscard]] float playbackSpeed() const;
    void setPlaybackSpeed(const float& val);

    // getters & setters
    void setDuration(const int& val);
    [[nodiscard]] int getDuration() const;

    void setTimeElapsed(const int& val);
    [[nodiscard]] int getTimeElapsed() const;

    void setPlaying(const bool& val);
    [[nodiscard]] bool getPlaying() const;

    void setSeekingEnabled(const bool& val);
    [[nodiscard]] bool getSeekingEnabled() const;
    [[nodiscard]] int getTargetSeek() const {return m_targetSeek;}

    [[nodiscard]] Timer* getSeekTimer() {return &m_Timer;}
    [[nodiscard]] Timer* getDelayTimer() {return &m_delayTimer;}

    void setUpdateRate(const bool& val);
    [[nodiscard]] bool getShouldUpdateRate() const {return m_updateRate;}

Q_SIGNALS:
    void filePathChanged();
    void playbackSpeedChanged();
    void durationChanged();
    void timeElapsedChanged();
    void playingChanged();

private:
    QString m_filePath;
    float m_playBackSpeed{1.0}; // 0% - 200%

    bool m_initialized{false};
    bool m_freed{false};

    bool m_playing{false};
    bool m_loaded{false};

    int m_timeElapsed{0}; // seconds
    int m_duration{0}; // in seconds

    bool m_seekingEnabled{true};
    int m_targetSeek{0}; // seek buffer

    float m_lastRate{1.0};
    bool m_updateRate{false}; // should we update rate?
    Timer m_updateRateTimer;

    Timer m_Timer;
    Timer m_delayTimer; // to block player updates while seeking

    // gstreamer stuff
    StreamData m_data{};
    GstBus* m_bus{nullptr};

    // static void error_callback(GstBus* bus, GstMessage* msg, const StreamData* data);
    // static void eos_callback(GstBus* bus, GstMessage* msg, const StreamData* data);
    // static void state_changed_callback(GstBus* bus, GstMessage* msg, StreamData* data);
    static gboolean handle_message(GstBus* bus, GstMessage* msg, StreamData* data);

    static gboolean update_player(StreamData* data);
};
#endif
