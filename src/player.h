#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QString>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QtMultimedia/QMediaMetaData>

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
    GstElement* pitch; // to change playback rate

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
    void seek(const int& pos, bool force=false, bool update_rate=true);
    bool isSeeking();

    // sends a seek event to change rate
    void update_rate(bool force=false);

    // get's metadata from audio file
    void getMetaData(QMediaPlayer* player) const;

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

    void setDurationValid(const bool& val);
    [[nodiscard]] bool getDurationValid() const {return m_durationValid;}

    enum MediaPlaybackType
    {
        MEDIA_TYPE_NONE,
        MEDIA_DURATION_STATIC,
        MEDIA_DURATION_VARIABLE
    };

    void setPlaybackType(const Player::MediaPlaybackType type);
    [[nodiscard]] Player::MediaPlaybackType getPlaybackType() const {return m_playbackType;}

Q_SIGNALS:
    void filePathChanged();
    void playbackSpeedChanged();
    void durationChanged();
    void timeElapsedChanged();
    void playingChanged();

public Q_SLOTS:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    QString m_filePath;
    float m_playBackSpeed{1.0}; // 0% - 200%

    bool m_initialized{false};
    bool m_freed{false};

    bool m_playing{false};
    bool m_loaded{false};

    int m_timeElapsed{0}; // seconds
    int m_duration{0}; // in seconds
    bool m_durationValid{false};

    bool m_seekingEnabled{true};
    int m_targetSeek{0}; // seek buffer

    float m_lastRate{1.0};
    bool m_updateRate{false}; // should we update rate?
    Timer m_updateRateTimer;

    Timer m_Timer;
    Timer m_delayTimer; // to block player updates while seeking

    // for metadata
    QMediaPlayer* m_Player{nullptr};
    Player::MediaPlaybackType m_playbackType{MEDIA_TYPE_NONE};

    // gstreamer stuff
    StreamData m_data{};
    GstBus* m_bus{nullptr};

    // gstreamer callbacks
    static gboolean handle_message(GstBus* bus, GstMessage* msg, StreamData* data);

    static gboolean update_player(StreamData* data);
};
#endif
