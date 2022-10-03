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



#ifndef EG_CLOCK_12_06_2019
#define EG_CLOCK_12_06_2019

#include "mega/reference.hpp"

#include <chrono>

namespace mega
{
    struct Clock
    {
    public:
        typedef std::chrono::steady_clock                           ClockType;
        typedef ClockType::time_point                               Tick;
        typedef ClockType::duration                                 TickDuration;
        typedef std::chrono::duration< mega::F32, std::ratio< 1 > > FloatTickDuration;

        Clock();

        void nextCycle();

        Tick            actual() const;
        mega::TimeStamp cycle() const;
        mega::F32       ct() const;
        mega::F32       dt() const;

    private:
        Tick            m_lastTick, m_startTick;
        mega::TimeStamp m_cycle;
        mega::F32       m_ct, m_dt;
    };
}

#endif //EG_CLOCK_12_06_2019