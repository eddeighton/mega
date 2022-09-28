

#include <chrono>

#include "common.hpp"
#include "clock.hpp"

namespace
{

struct HostClock
{
public:
    typedef std::chrono::steady_clock                           ClockType;
    typedef ClockType::time_point                               Tick;
    typedef ClockType::duration                                 TickDuration;
    typedef std::chrono::duration< mega::F32, std::ratio< 1 > > FloatTickDuration;

    HostClock()
    {
        m_lastTick = m_startTick = ClockType::now();
        m_cycle                  = 1U;
        m_ct = m_dt = 0.0f;
    }

    inline void nextCycle()
    {
        const Tick nowTick = ClockType::now();
        m_dt               = FloatTickDuration( nowTick - m_lastTick ).count();
        m_ct               = FloatTickDuration( nowTick - m_startTick ).count();
        m_lastTick         = nowTick;
        ++m_cycle;
    }

    inline Tick            actual() const { return ClockType::now(); }
    inline mega::TimeStamp cycle() const { return m_cycle; }
    inline mega::F32       ct() const { return m_ct; }
    inline mega::F32       dt() const { return m_dt; }

private:
    Tick            m_lastTick, m_startTick;
    mega::TimeStamp m_cycle;
    mega::F32       m_ct, m_dt;
}

theClock;

} // namespace

mega::TimeStamp clock::cycle( mega::TypeID type ) { return theClock.cycle(); }
mega::F32       clock::ct() { return theClock.ct(); }
mega::F32       clock::dt() { return theClock.dt(); }
void            clock::next() { theClock.nextCycle(); }
