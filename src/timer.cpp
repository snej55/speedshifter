#include "timer.h"

Timer::Timer(GstElement* element)
 : m_element {element}, m_clock{gst_element_get_clock(element)}
{
    // calculate base time
    m_startTime = gst_clock_get_time(m_clock);
    m_lastTime = m_startTime;
    m_lastDelta = 0;
}

void Timer::record()
{
    // calculate last time and new delta
    m_lastTime = gst_clock_get_time(m_clock);
    m_lastDelta = m_lastTime - m_startTime;
}

void Timer::reset()
{
    // reset base time
    m_startTime = gst_clock_get_time(m_clock);
    m_lastTime = m_startTime;
    m_lastDelta = 0;
}

int Timer::getTime()
{
    // record new delta
    record();
    // convert to milliseconds
    int timeDelta {static_cast<int>((gdouble)m_lastDelta / GST_MSECOND)};
    return timeDelta;
}
