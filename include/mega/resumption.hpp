//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.




#ifndef MEGA_RESUMPTION_1_AUG_2023
#define MEGA_RESUMPTION_1_AUG_2023

#include "return_reason.hpp"

#include <chrono>
#include <string>

using namespace std::chrono_literals;
using namespace std::string_literals;

namespace mega
{
inline ReturnReason complete() { return { eReason_Complete }; }

////////////////////////////////////////////////////////////////////////////
// wait functions
inline ReturnReason wait() { return { eReason_Wait }; }

inline ReturnReason wait( const runtime::Pointer& event ) { return { eReason_Wait_All, event }; }

inline ReturnReason wait_all( std::initializer_list< runtime::Pointer > events )
{
    return { eReason_Wait_All, events };
}

template < typename... Args >
inline ReturnReason wait_all( Args... args )
{
    return wait_all( { args... } );
}

inline ReturnReason wait_any( std::initializer_list< runtime::Pointer > events )
{
    return { eReason_Wait_Any, events };
}

template < typename... Args >
inline ReturnReason wait_any( Args... args )
{
    return wait_any( { args... } );
}

////////////////////////////////////////////////////////////////////////////
// sleep functions
inline ReturnReason sleep() { return { eReason_Sleep }; }

inline ReturnReason sleep( const runtime::Pointer& event ) { return { eReason_Sleep_All, event }; }

inline ReturnReason sleep_all( std::initializer_list< runtime::Pointer > events )
{
    return { eReason_Sleep_All, events };
}

template < typename... Args >
inline ReturnReason sleep_all( Args... args )
{
    return sleep_all( { args... } );
}

inline ReturnReason sleep_any( std::initializer_list< runtime::Pointer > events )
{
    return { eReason_Sleep_Any, events };
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

#endif // MEGA_RESUMPTION_1_AUG_2023
