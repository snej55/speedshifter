#include "player.h"

#include <string>
#include <iostream>
#include <cmath>

Player::Player(QObject *parent)
    : QObject{parent}
{
}

QString Player::filePath() const
{
    return m_filePath;
}

void Player::setFilePath(const QString &val)
{
    if (val != m_filePath)
    {
        m_filePath = val;
        Q_EMIT filePathChanged();
    }
}

float Player::playbackSpeed() const
{
    return m_playBackSpeed;
}

void Player::setPlaybackSpeed(const float &val)
{
    if (val != m_playBackSpeed)
    {
        m_playBackSpeed = std::max(0.0f, std::min(2.0f, val));
        Q_EMIT playbackSpeedChanged();
    }
}

void Player::init(int argc, char *argv[])
{
    if (!m_initialized)
    {
        gst_init(&argc, &argv);
        m_initialized = true;
    }
}

void Player::free()
{
    if (!m_freed)
    {
        m_freed = true;
    }
}

void Player::play()
{
    std::cout << "playing!\n";
    gint flags;

    m_data.playing = FALSE;
    m_data.terminate = FALSE;
    m_data.seek_enabled = FALSE;
    m_data.seek_done = FALSE;
    m_data.duration = GST_CLOCK_TIME_NONE;
    m_data.player = this;

    // create elements
    m_data.playbin = gst_element_factory_make("playbin", "playbin");

    if (!m_data.playbin)
    {
        std::cout << "Not all elements could be created\n";
        return;
    }

    std::cout << "Created playbin object!\n";

    // load URI
    g_object_set(m_data.playbin, "uri", m_filePath.toStdString().c_str(), nullptr);
    std::cout << "Loading from uri: " << m_filePath.toStdString().c_str() << '\n';

    // set flags to show audio but ignore video and subtitles
    g_object_get(m_data.playbin, "flags", &flags, nullptr);
    flags |= GST_PLAY_FLAG_AUDIO; // we want audio
    flags &= ~GST_PLAY_FLAG_TEXT; // we don't want subtitles
    flags &= ~GST_PLAY_FLAG_VIDEO; // we don't want video
    g_object_set(m_data.playbin, "flags", flags, nullptr);

    // set connection speed
    g_object_set(m_data.playbin, "connection-speed", 56, nullptr);

    // start playing
    if (gst_element_set_state(m_data.playbin, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Unable to set pipeline to playing state.\n");
        gst_object_unref(m_data.playbin); // free
        return;
    }
    std::cout << "Set pipeline to playing state!\n";

    // add bus watch, so we're notified when message arrives
    m_bus = gst_element_get_bus(m_data.playbin);

    // main loop
    do
    {
        // get the message
        GstMessage* msg {gst_bus_timed_pop_filtered(m_bus, 100 * GST_MSECOND,
                                                     static_cast<GstMessageType>(
                                                         GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS |
                                                         GST_MESSAGE_DURATION))};

        // parse the message
        if (msg != nullptr)
        {
            handle_message(m_bus, msg, &m_data);
        } else
        {
            // no message, so timeout expired
            if (m_data.playing)
            {
                gint64 current{-1};

                // query current position
                if (!gst_element_query_position(m_data.playbin, GST_FORMAT_TIME, &current))
                {
                    g_printerr("Could not query current position.\n");
                }

                // if it's unknown query stream duration
                if (!GST_CLOCK_TIME_IS_VALID(m_data.duration))
                {
                    if (!gst_element_query_duration(m_data.playbin, GST_FORMAT_TIME, &m_data.duration))
                    {
                        g_printerr("Could not query duration.\n");
                    }
                }

                g_print("Position %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r", GST_TIME_ARGS(current),
                        GST_TIME_ARGS(m_data.duration));
            }
        }
    } while (!m_data.terminate);

    // free objects
    gst_object_unref(m_bus);
    gst_element_set_state(m_data.playbin, GST_STATE_NULL);
    gst_object_unref(m_data.playbin);
    std::cout << "Unreferenced objects" << std::endl;
}

gboolean Player::handle_message(GstBus *bus, GstMessage *msg, StreamData *data)
{
    GError *err;
    gchar *debug_info;

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
            if (GST_MESSAGE_SRC(msg) == GST_OBJECT(data->playbin))
            {
                g_print("\nPipeline state changed from %s to %s", gst_element_state_get_name(oldState),
                        gst_element_state_get_name(newState));

                data->playing = (newState == GST_STATE_PLAYING);

                if (data->playing)
                {
                    // check if seeking is possible
                    gint64 start, end;
                    GstQuery *query{gst_query_new_seeking(GST_FORMAT_TIME)};
                    if (gst_element_query(data->playbin, query))
                    {
                        gst_query_parse_seeking(query, nullptr, &data->seek_enabled, &start, &end);
                        if (data->seek_enabled)
                        {
                            g_print("Seeking is enabled from %" GST_TIME_FORMAT " to %" GST_TIME_FORMAT "\n",
                                    GST_TIME_ARGS(start), GST_TIME_ARGS(end));
                        } else
                        {
                            g_print("Seeking is disabled for this stream\n");
                        }
                    } else
                    {
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

void Player::error_callback(GstBus* bus, GstMessage* msg, const StreamData* data)
{
    GError *err;
    gchar *debug_info;

    gst_message_parse_error(msg, &err, &debug_info);
    g_printerr("Error recieved from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
    g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
    g_clear_error(&err);
    g_free(debug_info);

    gst_element_set_state(data->playbin, GST_STATE_READY);
}

void Player::eos_callback(GstBus* bus, GstMessage* msg, const StreamData* data)
{
    g_print("End of stream reached.\n");
    gst_element_set_state(data->playbin, GST_STATE_READY);
}

void Player::state_changed_callback(GstBus *bus, GstMessage *msg, StreamData *data)
{
    GstState oldState, newState, pendingState;
    gst_message_parse_state_changed(msg, &oldState, &newState, &pendingState);
    if (GST_MESSAGE_SRC(msg) == GST_OBJECT(data->playbin))
    {
        data->state = newState;
        g_print("\nPipeline state changed from %s to %s", gst_element_state_get_name(oldState),
                gst_element_state_get_name(newState));

        if (oldState == GST_STATE_READY && newState == GST_STATE_PAUSED)
        {
            update_player(data);
        }
    }
}

void Player::update_player(StreamData *data)
{
    gint64 current{-1};

    // we don't want to update unless state is PLAYING or PAUSED
    if (data->state < GST_STATE_PAUSED)
    {
        return;
    }

    // query current position
    if (!gst_element_query_position(data->playbin, GST_FORMAT_TIME, &current))
    {
        g_printerr("Could not query current position.\n");
    }

    // if it's unknown query stream duration
    if (!GST_CLOCK_TIME_IS_VALID(data->duration))
    {
        if (!gst_element_query_duration(data->playbin, GST_FORMAT_TIME, &data->duration))
        {
            g_printerr("Could not query duration.\n");
        }
    }

    g_print("Position %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r", GST_TIME_ARGS(current),
            GST_TIME_ARGS(data->duration));
}

// --------------------------- getters & setters --------------------------- //

void Player::setDuration(const int &val)
{
    if (val != m_duration)
    {
        m_duration = val;
    }
}

int Player::getDuration() const
{
    return m_duration;
}

void Player::setTimeElapsed(const int &val)
{
    if (val != m_timeElapsed)
    {
        m_timeElapsed = val;
    }
}

int Player::getTimeElapsed() const
{
    return m_timeElapsed;
}
