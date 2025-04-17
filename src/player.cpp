#include "player.h"

#include <string>
#include <iostream>
#include <cmath>

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
        m_playBackSpeed = std::max(0.0f, std::min(2.0f, val));
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

    data.playing = FALSE;
    data.terminate = FALSE;
    data.seek_enabled = FALSE;
    data.seek_done = FALSE;
    data.duration = GST_CLOCK_TIME_NONE;

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

    // start playing
    ret = gst_element_set_state(data.playbin, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set pipeline to playing state.\n");
        std::cout << "Unable to change gst pipeline state!" << std::endl;
        return;
    }
    std::cout << "Set pipeline to playing state!\n";

    // add bus watch, so we're notified when message arrives
    bus = gst_element_get_bus(data.playbin);

    // main loop
    do {
        GstMessage* msg;

        // get the message
        msg = gst_bus_timed_pop_filtered(bus, 100 * GST_MSECOND, static_cast<GstMessageType>(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_DURATION));

        // parse the message
        if (msg != nullptr) {
            handle_message(bus, msg, &data);
        } else {
            // no message, so timeout expired
            if (data.playing) {
                gint64 current {-1};

                // query current position
                if (!gst_element_query_position(data.playbin, GST_FORMAT_TIME, &current)) {
                    g_printerr("Could not query current position.\n");
                }

                // if it's unknown query stream duration
                if (!GST_CLOCK_TIME_IS_VALID(data.duration)) {
                    if (!gst_element_query_duration(data.playbin, GST_FORMAT_TIME, &data.duration)) {
                        g_printerr("Could not query duration.\n");
                    }
                }
            }
        }
    } while (!data.terminate);

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
            data->terminate = TRUE;
            break;
        case GST_MESSAGE_EOS:
            // end of stream reached
            g_print("\nEnd of Stream reached.\n");
            data->terminate = TRUE;
            break;
        case GST_MESSAGE_DURATION:
            // duration has changed so current duration is invalid
            data->duration = GST_CLOCK_TIME_NONE;
            break;
        case GST_MESSAGE_STATE_CHANGED:
            GstState oldState, newState, pendingState;
            gst_message_parse_state_changed(msg, &oldState, &newState, &pendingState);
            if (GST_MESSAGE_SRC(msg) == GST_OBJECT(data->playbin)) {
                g_print("\nPipeline state changed from %s to %s", gst_element_state_get_name(oldState), gst_element_state_get_name(newState));

                data->playing = (newState == GST_STATE_PLAYING);

                if (data->playing) {
                    // check if seeking is possible
                    GstQuery* query;
                    gint64 start, end;
                    query = gst_query_new_seeking(GST_FORMAT_TIME);
                    if (gst_element_query(data->playbin, query)) {
                        gst_query_parse_seeking(query, nullptr, &data->seek_enabled, &start, &end);
                        if (data->seek_enabled) {
                            g_print("Seeking is enabled from %" GST_TIME_FORMAT " to %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(start), GST_TIME_ARGS(end));
                        } else {
                            g_print("Seeking is disabled for this stream\n");
                        }
                    } else {
                        g_print("Seeking query failed.");
                    }
                    gst_query_unref(query);
                }
            }
            break;
        default:
            break;
    }

    gst_message_unref(msg);
    return TRUE;
}
