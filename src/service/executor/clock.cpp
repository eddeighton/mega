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

#include "clock.hpp"

#include "mega/native_types.hpp"
#include "mega/reference.hpp"
#include "mega/clock.hpp"

#include "service/protocol/common/context.hpp"

namespace mega
{
Clock::Clock()
{
    m_lastTick = m_startTick = ClockType::now();
    m_cycle                  = 1U;
    m_ct = m_dt = 0.0f;
}

void Clock::nextCycle()
{
    const Tick nowTick = ClockType::now();
    m_dt               = FloatTickDuration( nowTick - m_lastTick ).count();
    m_ct               = FloatTickDuration( nowTick - m_startTick ).count();
    m_lastTick         = nowTick;
    ++m_cycle;
}

Clock::Tick Clock::actual() const { return ClockType::now(); }
TimeStamp   Clock::cycle() const { return m_cycle; }
F32         Clock::ct() const { return m_ct; }
F32         Clock::dt() const { return m_dt; }

} // namespace mega

// implement mega/clock.hpp

mega::TimeStamp Clock::cycle() { return mega::Context::get()->cycle(); }
mega::F32 Clock::ct() { return mega::Context::get()->ct(); }
mega::F32 Clock::dt() { return mega::Context::get()->dt(); }
