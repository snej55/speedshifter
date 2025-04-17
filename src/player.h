#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QString>
#include <QMediaPlayer>

#include <qqml.h>
// gstreamer
#include <gst/gst.h>

// for gstreamer playback
struct StreamData {
    GstElement* playbin;

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
    QML_ELEMENT

public:
    explicit Player(QObject* parent = nullptr);

    void init(int argc, char* argv[]);
    void free();

    Q_INVOKABLE
    void play();

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

Q_SIGNALS:
    void filePathChanged();
    void playbackSpeedChanged();

private:
    QString m_filePath;
    float m_playBackSpeed{1.0}; // 0% - 200%

    bool m_initialized{false};
    bool m_freed{false};

    int m_timeElapsed{0}; // seconds
    int m_duration{0}; // in seconds

    // gstreamer stuff
    StreamData m_data{};
    GstBus* m_bus{nullptr};

    static void error_callback(GstBus* bus, GstMessage* msg, const StreamData* data);
    static void eos_callback(GstBus* bus, GstMessage* msg, const StreamData* data); // end of stream
    static void state_changed_callback(GstBus* bus, GstMessage* msg, StreamData* data);
    static gboolean handle_message(GstBus* bus, GstMessage* msg, StreamData* data);

    static void update_player(StreamData* data);
};
#endif
