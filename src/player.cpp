#include "player.h"

#include <string>
#include <iostream>

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
    std::cout << "playing!\n";
    StreamData data;
    GstBus* bus;
    GstStateChangeReturn ret;
    gint flags;

    // create elements
    data.playbin = gst_element_factory_make("playbin", "playbin");

    if (!data.playbin)
    {
        std::cout << "Not all elements could be created\n";
        return;
    }

    std::cout << "Created playbin object!\n";

    // load URI
    g_object_set(data.playbin, "uri", m_filePath.toStdString().c_str(), nullptr);
    std::cout << "Loading from uri: " << m_filePath.toStdString().c_str() << '\n';

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

    // start playing
    ret = gst_element_set_state(data.playbin, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set pipeline to playing state.\n");
        std::cout << "Unable to change gst pipeline state!" << std::endl;
        return;
    }
    std::cout << "Set pipeline to playing state!\n";

    data.main_loop = g_main_loop_new(nullptr, FALSE);
    g_main_loop_run(data.main_loop);
    std::cout << "Created main loop!\n";

    g_main_loop_unref(data.main_loop);
    gst_object_unref(bus);
    gst_element_set_state(data.playbin, GST_STATE_NULL);
    gst_object_unref(data.playbin);
    std::cout << "Unreferenced objects" << std::endl;
}

gboolean Player::handle_message(GstBus* bus, GstMessage* msg, StreamData* data)
{
    GError* err;
    gchar* debug_info;

    switch (GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error recieved from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            g_main_loop_quit (data->main_loop);
            break;
        default:
            break;
    }

    return TRUE;
}
