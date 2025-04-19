#ifndef TIMER_H
#define TIMER_H

#include <ctime>
#include <chrono>

using t_timepoint = std::chrono::time_point<std::chrono::system_clock>;
using t_duration = std::chrono::duration<double>;
// simple timer class
class Timer
{
public:
    Timer();

    // record new last time and calculate new delta
    void record();

    // reset base time
    void reset();

    // get time in seconds
    double getTime();

    [[nodiscard]] t_timepoint getRawStartTime() const {return m_startTime;}
    [[nodiscard]] t_timepoint getRawLastTime() const {return m_lastTime;}
    [[nodiscard]] t_duration getRawLastDelta() const {return m_lastDelta;}

private:
    t_timepoint m_startTime;
    t_timepoint m_lastTime{};

    t_duration m_lastDelta{};
};

#endif
