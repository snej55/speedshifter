#ifndef TIMER_H
#define TIMER_H

#include <gst/gst.h>

// simple timer class
class Timer
{
public:
    Timer(GstElement* element);

    // records current time as lastTime
    // and recalculates lastDelta
    void record();

    // resets base time and all fields
    void reset();

    // returns current time since base time in milliseconds
    int getTime();

    // get raw fields
    [[nodiscard]] GstClockTime getRawBaseTime() const {return m_startTime;}
    [[nodiscard]] GstClockTime getRawLastTime() const {return m_lastTime;}
    [[nodiscard]] GstClockTimeDiff getRawLastDelta() const {return m_lastDelta;}
    [[nodiscard]] GstElement* getElement() const {return m_element;}
    [[nodiscard]] GstClock* getClock() const {return m_clock;}

private:
    GstElement* m_element;
    GstClock* m_clock;
    GstClockTime m_startTime{}; // base time
    GstClockTime m_lastTime{}; // last time recorded
    GstClockTimeDiff m_lastDelta{}; // last difference recorded
};

#endif
