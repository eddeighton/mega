
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

#ifndef GUARD_2023_July_20_clock_standalone
#define GUARD_2023_July_20_clock_standalone

#include "service/clock.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>

#include <chrono>
#include <unordered_map>

namespace mega::service
{

class ProcessClockStandalone : public ProcessClock
{
    struct State
    {
        bool                        m_bNew;
        bool                        m_bWaitingForMoveResponse;
        bool                        m_bWaitingForClockResponse;
        network::LogicalThreadBase* m_pSender;
    };
    using MPOMap = std::unordered_map< runtime::MPO, State, runtime::MPO::Hash >;

    using ClockType    = std::chrono::steady_clock;
    using Tick         = ClockType::time_point;
    using TickDuration = ClockType::duration;
    using Strand       = boost::asio::strand< boost::asio::io_context::executor_type >;

public:
    using FloatTickDuration = std::chrono::duration< mega::F32, std::ratio< 1 > >;

public:
    ProcessClockStandalone( boost::asio::io_context& ioContext, FloatTickDuration tickRate );
    virtual void registerMPO( network::SenderRef sender ) override;
    virtual void unregisterMPO( network::SenderRef sender ) override;
    virtual void requestClock( network::LogicalThreadBase* pSender, runtime::MPO mpo, event::Range ) override;
    virtual bool unrequestClock( network::LogicalThreadBase* pSender, runtime::MPO mpo ) override;
    virtual void requestMove( network::LogicalThreadBase* pSender, runtime::MPO mpo ) override;

private:
    void registerMPOImpl( network::SenderRef sender );
    void unregisterMPOImpl( network::SenderRef sender );
    void requestMoveImpl( network::LogicalThreadBase* pSender, runtime::MPO mpo );
    void requestClockImpl( network::LogicalThreadBase* pSender, runtime::MPO mpo );
    bool unrequestClockImpl( network::LogicalThreadBase* pSender, runtime::MPO mpo );
    void checkClock();
    void clock();
    void issueMove();
    void issueClock();

private:
    network::ClockTick        m_clockTick;
    bool                      m_bClockIssued = false;
    boost::asio::io_context&  m_ioContext;
    Strand                    m_strand;
    boost::asio::steady_timer m_timer;
    Tick                      m_startTimeStamp;
    Tick                      m_lastTick;
    TickDuration              m_tickRate;
    MPOMap                    m_mpos;
};
} // namespace mega::service

#endif // GUARD_2023_July_20_clock_standalone
