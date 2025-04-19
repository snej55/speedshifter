#include "timer.h"

Timer::Timer()
 : m_startTime{std::chrono::system_clock::now()}
{
    m_lastTime = m_startTime;
    m_lastDelta = m_lastTime - m_startTime;
}

void Timer::record()
{
    m_lastTime = std::chrono::system_clock::now();
    m_lastDelta = m_lastTime - m_startTime;
}

void Timer::reset()
{
    m_startTime = std::chrono::system_clock::now();
    m_lastTime = m_startTime;
    m_lastDelta = m_lastTime - m_startTime;
}

double Timer::getTime()
{
    record();
    double delta{m_lastDelta.count()};

    return delta;
}
