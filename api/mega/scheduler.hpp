


#ifndef EG_SCHEDULER
#define EG_SCHEDULER

#include "common.hpp"
#include "event.hpp"
#include "coroutine.hpp"

#include <chrono>
#include <string>

using namespace std::chrono_literals;
using namespace std::string_literals;

namespace mega
{
inline ReturnReason done() { return ReturnReason(); }

////////////////////////////////////////////////////////////////////////////
// wait functions
inline ReturnReason wait() { return ReturnReason( eReason_Wait ); }

inline ReturnReason wait( const Event& event ) { return ReturnReason( eReason_Wait_All, event ); }

inline ReturnReason wait_all( std::initializer_list< Event > events )
{
    return ReturnReason( eReason_Wait_All, events );
}

template < typename... Args >
inline ReturnReason wait_all( Args... args )
{
    return wait_all( { args... } );
}

inline ReturnReason wait_any( std::initializer_list< Event > events )
{
    return ReturnReason( eReason_Wait_Any, events );
}

template < typename... Args >
inline ReturnReason wait_any( Args... args )
{
    return wait_any( { args... } );
}

////////////////////////////////////////////////////////////////////////////
// sleep functions
inline ReturnReason sleep() { return ReturnReason( eReason_Sleep ); }

inline ReturnReason sleep( const Event& event ) { return ReturnReason( eReason_Sleep_All, event ); }

inline ReturnReason sleep_all( std::initializer_list< Event > events )
{
    return ReturnReason( eReason_Sleep_All, events );
}

template < typename... Args >
inline ReturnReason sleep_all( Args... args )
{
    return sleep_all( { args... } );
}

inline ReturnReason sleep_any( std::initializer_list< Event > events )
{
    return ReturnReason( eReason_Sleep_Any, events );
}

template < typename... Args >
inline ReturnReason sleep_any( Args... args )
{
    return sleep_any( { args... } );
}

template < typename Clock, typename Duration >
inline ReturnReason sleep_until( std::chrono::time_point< Clock, Duration > const& sleep_time )
{
    return ReturnReason( sleep_time );
}

template < typename Rep, typename Period >
inline ReturnReason sleep_until( std::chrono::duration< Rep, Period > const& timeout_duration )
{
    return ReturnReason( std::chrono::steady_clock::now() + timeout_duration );
}

inline ReturnReason sleep_until( F32 fDuration )
{
    auto floatDuration   = std::chrono::duration< F32, std::ratio< 1 > >( fDuration );
    auto intMilliseconds = std::chrono::duration_cast< std::chrono::milliseconds >( floatDuration );
    return sleep_until( intMilliseconds );
}

inline ReturnReason sleep_until( F64 dbDuration )
{
    auto doubleDuration  = std::chrono::duration< F64, std::ratio< 1 > >( dbDuration );
    auto intMilliseconds = std::chrono::duration_cast< std::chrono::milliseconds >( doubleDuration );
    return sleep_until( intMilliseconds );
}

} // namespace mega

#endif // EG_SCHEDULER
