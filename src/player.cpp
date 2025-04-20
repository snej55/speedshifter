#include "player.h"

#include <string>
#include <iostream>
#include <cmath>

#include <QFile>

Player::Player(QObject *parent)
    : QObject{parent}, m_updateRateTimer{}, m_Timer{}, m_delayTimer{}
{
}

Player::~Player()
{
    free();
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
        std::cout << "File path changed!\n";
        std::cout << "Loading!\n";
        load();
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
        m_playBackSpeed = std::max(0.02f, std::min(2.0f, val));
        m_data.rate = static_cast<gdouble>(m_playBackSpeed);
        update_rate();
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
        // free objects
        gst_object_unref(m_bus);
        gst_element_set_state(m_data.playbin, GST_STATE_NULL);
        gst_object_unref(m_data.playbin);
        m_data = StreamData{};
        m_bus = nullptr;
        std::cout << "Unreferenced objects" << std::endl;
        m_loaded = false;
        m_freed = true;
    }
}

void Player::load()
{
    std::cout << "loading!\n";
    m_loaded = false;
    setPlaying(false);
    setTimeElapsed(0);
    setDuration(0);
    setDurationValid(false);

    free();

    gint flags;
    GstPad* pad;
    GstPad* ghostPad;
    GstPad* pitchPad;
    GstPad* ghostPitchPad;

    m_data = StreamData{};
    m_data.playing = FALSE;
    m_data.terminate = FALSE;
    m_data.seek_enabled = FALSE;
    m_data.seek_done = FALSE;
    m_data.duration = GST_CLOCK_TIME_NONE;
    m_data.player = this;
    m_data.rate = static_cast<gdouble>(m_playBackSpeed);

    // create elements
    m_data.playbin = gst_element_factory_make("playbin", "playbin");

    // scaletempo audio pipeline: audio_sink="scaletempo ! audioconvert ! audioresample ! autoaudiosink"
    m_data.scaletempo = gst_element_factory_make("scaletempo", "scaletempo");
    m_data.convert = gst_element_factory_make("audioconvert", "convert");
    m_data.resample = gst_element_factory_make("audioresample", "resample");
    m_data.sink = gst_element_factory_make("autoaudiosink", "audio_sink");
    m_data.pitch = gst_element_factory_make("pitch", "pitch");

    if (!m_data.playbin || !m_data.scaletempo || !m_data.convert || !m_data.resample || !m_data.sink || !m_data.pitch)
    {
        std::cout << "Not all elements could be created\n";
        return;
    }

    std::cout << "Created playbin object!\n";

    // load URI
    g_object_set(m_data.playbin, "uri", m_filePath.toStdString().c_str(), nullptr);
    std::cout << "Loading from uri: " << m_filePath.toStdString().c_str() << '\n';

    // create sink bin, add elements and link them
    m_data.bin = gst_bin_new("audio_sink_bin");
    gst_bin_add_many(GST_BIN(m_data.bin), m_data.scaletempo, m_data.pitch, m_data.convert, m_data.resample, m_data.sink, nullptr);
    gst_element_link_many(m_data.scaletempo, m_data.pitch, m_data.convert, m_data.resample, m_data.sink, nullptr);
    pad = gst_element_get_static_pad(m_data.scaletempo, "sink");
    ghostPad = gst_ghost_pad_new("sink", pad);
    gst_pad_set_active(ghostPad, TRUE);
    gst_element_add_pad(m_data.bin, ghostPad);
    gst_object_unref(pad);

    pitchPad = gst_element_get_static_pad(m_data.pitch, "sink");
    ghostPitchPad = gst_ghost_pad_new("sink", pitchPad);
    gst_pad_set_active(ghostPitchPad, TRUE);
    gst_element_add_pad(m_data.bin, ghostPitchPad);
    gst_object_unref(pitchPad);
    std::cout << "Created sink bin!\n";

    // configure scaletempo
    // NOTE: does not work, rate property is read only https://gitlab.freedesktop.org/gstreamer/gst-plugins-good/-/issues/261
    // g_object_set(G_OBJECT(m_data.scaletempo), "rate", static_cast<gdouble>(-2.0), nullptr);

    g_object_set(m_data.playbin, "audio-sink", m_data.bin, nullptr);

    // set flags to show audio but ignore video and subtitles
    g_object_get(m_data.playbin, "flags", &flags, nullptr);
    flags |= GST_PLAY_FLAG_AUDIO; // we want audio
    flags &= ~GST_PLAY_FLAG_TEXT; // we don't want subtitles
    flags &= ~GST_PLAY_FLAG_VIDEO; // we don't want video
    g_object_set(m_data.playbin, "flags", flags, nullptr);

    // set connection speed
    g_object_set(m_data.playbin, "connection-speed", 56, nullptr);

    // add bus watch, so we're notified when message arrives
    m_bus = gst_element_get_bus(m_data.playbin);
    gst_bus_add_watch(m_bus, reinterpret_cast<GstBusFunc>(handle_message), &m_data);

    std::cout << "Added bus watch!\n";

    m_loaded = true;
    m_freed = false;

    // add timeout to update player data every 50 milliseconds
    g_timeout_add(50, reinterpret_cast<GSourceFunc>(update_player), &m_data);

    // update rate
    update_rate();
    std::cout << "Playback rate: " << static_cast<double>(m_data.rate) << '\n';
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
            Player* player;
            player = static_cast<Player*>(data->player);
            player->stop();
            data->playing = false;
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
                g_print("\nPipeline state changed from %s to %s \n", gst_element_state_get_name(oldState),
                        gst_element_state_get_name(newState));

                data->playing = (newState == GST_STATE_PLAYING);
                data->state = newState;

                std::cout << "Playing: " << (data->playing ? "true" : "false") << '\n';
                if (data->playing)
                {
                    std::cout << "Querying seeking enabled\n";
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
                // g_print("Update rate after state change\n");
                // static_cast<Player*>(data->player)->update_rate();
            }
            break;
        default:
            break;
    }
    // NOTE: don't unref msg, it gives the corrupted smallbin error
    return TRUE;
}

gboolean Player::update_player(StreamData *data)
{
    gint64 current{-1};

    Player* player {static_cast<Player*>(data->player)};
    player->setPlaying(data->playing);

    // we don't want to update unless state is PLAYING or PAUSED
    if (data->state != GST_STATE_PAUSED && data->state != GST_STATE_PLAYING)
    {
        return TRUE;
    }

    if (player->isSeeking())
    {
        return TRUE;
    } else {
        if (player->getTargetSeek() != 0)
        {
            player->seek(player->getTargetSeek(), true);
        }
    }

    // query current position
    if (!gst_element_query_position(data->playbin, GST_FORMAT_TIME, &current))
    {
        g_printerr("Could not query current position.\n");
        return TRUE; // we can't update rate if position is invalid
    } else {
        // prevent from triggering unwanted seek
        player->setSeekingEnabled(false);
        // calculate time in seconds
        // setter method sends qt signal to update qml
        player->setTimeElapsed(static_cast<int>((gdouble)current / GST_SECOND));
        // we can re-enable seeking now
        player->setSeekingEnabled(true);
    }

    // if it's unknown query stream duration
    if (!GST_CLOCK_TIME_IS_VALID(data->duration))
    {
        if (!player->getDurationValid())
        {
            data->rate = 1.0;
            player->update_rate(true);
            std::cout << "Querying duration...\n";
            if (!gst_element_query_duration(data->playbin, GST_FORMAT_TIME, &data->duration))
            {
                g_printerr("Could not query duration.\n");
                player->setDurationValid(false);
            } else {
                if (!player->getDurationValid())
                {
                    player->setDuration(static_cast<int>((gdouble)data->duration / GST_SECOND));
                    player->setDurationValid(true);

                }
            }
            data->rate = static_cast<double>(player->playbackSpeed());
            player->update_rate(true);
        }

    }

    g_print("Position %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r", GST_TIME_ARGS(current),
            GST_TIME_ARGS(data->duration));

    if (player->getShouldUpdateRate())
    {
        g_print("Updating rate...\n");
        player->update_rate();
        player->setUpdateRate(false);
    }

    return TRUE;
}

void Player::play()
{
    if (m_loaded)
    {
        // start playing
        if (gst_element_set_state(m_data.playbin, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
        {
            g_printerr("Unable to set pipeline to playing state.\n");
            gst_object_unref(m_data.playbin); // free
            return;
        }
        setPlaying(true);
        std::cout << "Set pipeline to playing state!\n";
        m_updateRate = true;
    }
}

void Player::pause()
{
    if (m_loaded)
    {
        if (gst_element_set_state(m_data.playbin, GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE)
        {
            g_printerr("Failed to set playbin to playing state.\n");
            gst_object_unref(m_data.playbin); // free
            return;
        }
        setPlaying(false);
        std::cout << "Paused pipeline.\n";
        m_updateRate = true;
    }
}

void Player::stop()
{
    if (m_loaded)
    {
        if (gst_element_set_state(m_data.playbin, GST_STATE_READY) == GST_STATE_CHANGE_FAILURE)
        {
            g_printerr("Failed to set playbin to ready state.\n");
            gst_object_unref(m_data.playbin); // free
            return;
        }
        // return to start
        setPlaying(false);
        setTimeElapsed(0);
        setDurationValid(false);
        std::cout << "Set playbin to ready state\n";
    }
}

void Player::seek(const int& pos, bool force, bool update_rate)
{
    constexpr double seekTime {0.2};
    m_delayTimer.reset(); // to block player updates for like 1/2 a second
    if (m_loaded && m_seekingEnabled)
    {
        m_targetSeek = pos;
        if ((m_data.seek_enabled && m_Timer.getTime() > seekTime) || force)
        {
            g_print("Seeking to position: %" GST_TIME_FORMAT "\r", GST_TIME_ARGS(m_targetSeek * GST_SECOND));
            gst_element_seek_simple(m_data.playbin, GST_FORMAT_TIME, static_cast<GstSeekFlags>(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT), static_cast<gint64>(m_targetSeek * GST_SECOND));
            std::cout << m_Timer.getTime() << "s since last seek" << std::endl;
            m_Timer.reset();
            m_targetSeek = 0;
            std::cout << "Updating rate after seek...\n";
            if (update_rate)
            {
                m_updateRate = true;
            }
        }
    }
}

// sends a seek event to update playback rate
void Player::update_rate(bool force)
{
    if (!force)
    {
        constexpr double rateChangeDelay{0.2};
        if (m_updateRateTimer.getTime() < rateChangeDelay)
        {
            m_updateRate = true;
            return;
        }
    }

    if (!m_data.seek_enabled)
    {
        g_printerr("Seeking is disabled.\n");
        return;
    }
    gint64 position;
    GstEvent* seekEvent;

    // obtain current position (need for seek event)
    if (!gst_element_query_position(m_data.playbin, GST_FORMAT_TIME, &position))
    {
        g_printerr("Unable to query current position.\n");
        return;
    }

    // create seek event
    // // we seek to same position, since we only want to change the rate
    if (m_data.rate > 0.0)
    {
        seekEvent = gst_event_new_seek(m_data.rate, GST_FORMAT_TIME, static_cast<GstSeekFlags>(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE), GST_SEEK_TYPE_SET, position, GST_SEEK_TYPE_END, 0);
    } else {
        seekEvent = gst_event_new_seek(m_data.rate, GST_FORMAT_TIME, static_cast<GstSeekFlags>(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_SET, position);
    }

    // send the event
    gst_element_send_event(m_data.sink, seekEvent);

    // update pitch tempo (this makes mp3 files work for some reason)
    g_object_set(m_data.pitch, "tempo", m_data.rate, nullptr);

    g_print("Current rate: %g\n", m_data.rate);

    m_updateRateTimer.reset();
}

// --------------------------- getters & setters --------------------------- //

void Player::setDuration(const int& val)
{
    if (val != m_duration)
    {
        m_duration = val;
        Q_EMIT durationChanged();
    }
}

int Player::getDuration() const
{
    return m_duration;
}

void Player::setTimeElapsed(const int& val)
{
    if (val != m_timeElapsed)
    {
        m_timeElapsed = val;
        Q_EMIT timeElapsedChanged();
    }
}

int Player::getTimeElapsed() const
{
    return m_timeElapsed;
}

void Player::setPlaying(const bool& val)
{
    if (val != m_playing)
    {
        m_playing = val;
        Q_EMIT playingChanged();
    }
}

bool Player::getPlaying() const
{
    std::cout << "RETURNING: " << static_cast<bool>(m_playing) << '\n';
    return static_cast<bool>(m_playing);
}

void Player::setSeekingEnabled(const bool& val)
{
    if (val != m_seekingEnabled)
    {
        m_seekingEnabled = val;
    }
}

bool Player::getSeekingEnabled() const
{
    return m_seekingEnabled;
}

bool Player::isSeeking()
{
    constexpr double delayTime {0.3}; // slightly longer than seek time
    return m_delayTimer.getTime() < delayTime;
}

void Player::setUpdateRate(const bool& val)
{
    if (val != m_updateRate)
    {
        m_updateRate = val;
    }
}

void Player::setDurationValid(const bool& val)
{
    if (val != m_durationValid)
    {
        m_durationValid = val;
    }
}
