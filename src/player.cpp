#include "player.h"

#include <string>

Player::Player(QObject* parent)
 : QObject{parent}
{
}

QString Player::filePath() const
{
    return m_filePath;
}

void Player::setFilePath(const QString& val)
{
    if (val != m_filePath) {
        m_filePath = val;
        Q_EMIT filePathChanged();
    }
}

float Player::playbackSpeed() const
{
    return m_playBackSpeed;
}

void Player::setPlaybackSpeed(const float& val)
{
    if (val != m_playBackSpeed) {
        m_playBackSpeed = val;
        Q_EMIT playbackSpeedChanged();
    }
}

void Player::init(int argc, char* argv[])
{
    if (!m_initialized) {
        gst_init(&argc, &argv);
    }
}


void Player::play()
{
    StreamData data;
    GstBus* bus;
    GstStateChangeReturn ret;
    gint flags;
    GIOChannel* io_stdin;

    // create elements
    data.playbin = gst_element_factory_make("playbin", "playbin");

    if (!data.playbin)
    {
        g_printerr("Not all elements could be created\n");
        return;
    }

    // load URI
    g_object_set(data.playbin, "uri", m_filePath.toStdString().c_str(), nullptr);

    // set flags to show audio but ignore video and subtitles
    g_object_get(data.playbin, "flags", &flags, nullptr);
    flags |= GST_PLAY_FLAG_AUDIO; // we want audio
    flags &= ~GST_PLAY_FLAG_TEXT; // we don't want subtitles
    flags &= ~GST_PLAY_FLAG_VIDEO; // we don't want video
    g_object_set(data.playbin, "flags", flags, nullptr);

    // set connection speed
    g_object_set(data.playbin, "connection-speed", 56, nullptr);

    // add bus watch, so we're notified when message arrives
    bus = gst_element_get_bus(data.playbin);
    gst_bus_add_watch(bus, (GstBusFunc)handle_message, &data);
}
