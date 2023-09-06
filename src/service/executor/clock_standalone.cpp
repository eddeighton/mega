
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

#include "service/executor/clock_standalone.hpp"

#include "service/network/log.hpp"

#include "service/protocol/common/logical_thread_id.hpp"
#include "service/protocol/common/logical_thread_base.hpp"
#include "service/protocol/model/messages.hxx"

#include "mega/reference_io.hpp"

namespace mega::service
{

ProcessClockStandalone::ProcessClockStandalone( boost::asio::io_context&                  ioContext,
                                                ProcessClockStandalone::FloatTickDuration tickRate )
    : m_ioContext( ioContext )
    , m_strand( boost::asio::make_strand( ioContext ) )
    , m_timer( m_strand )
    , m_startTimeStamp( std::chrono::steady_clock::now() )
    , m_lastTick( m_startTimeStamp )
    , m_tickRate( std::chrono::duration_cast< TickDuration >( tickRate ) )
{
}

void ProcessClockStandalone::setActiveProject( const Project& project, U64 unityDBHashCode )
{
    // ignor
}

void ProcessClockStandalone::registerMPO( network::SenderRef sender )
{
    ProcessClockStandalone* pThis = this;
    boost::asio::post( m_strand, [ pThis, sender ]() { pThis->registerMPOImpl( sender ); } );
}

void ProcessClockStandalone::unregisterMPO( network::SenderRef sender )
{
    ProcessClockStandalone* pThis = this;
    boost::asio::post( m_strand, [ pThis, sender ]() { pThis->unregisterMPOImpl( sender ); } );
}

void ProcessClockStandalone::requestClock( network::LogicalThreadBase* pSender, MPO mpo, log::Range )
{
    ProcessClockStandalone* pThis = this;
    boost::asio::post( m_strand, [ pThis, pSender, mpo ]() { pThis->requestClockImpl( pSender, mpo ); } );
}

void ProcessClockStandalone::requestMove( network::LogicalThreadBase* pSender, MPO mpo )
{
    ProcessClockStandalone* pThis = this;
    boost::asio::post( m_strand, [ pThis, pSender, mpo ]() { pThis->requestMoveImpl( pSender, mpo ); } );
}

void ProcessClockStandalone::registerMPOImpl( network::SenderRef sender )
{
    SPDLOG_TRACE( "ProcessClockStandalone::registerMPOImpl mpo:{}", sender.m_mpo );
    // When register set waitingForClockResponse to TRUE so that initial clock response is generated
    VERIFY_RTE_MSG( m_mpos.insert( { sender.m_mpo, State{ false, true, sender.m_pSender } } ).second,
                    "Duplicate MPO when registering: " << sender.m_mpo );
    checkClock();
}

void ProcessClockStandalone::unregisterMPOImpl( network::SenderRef sender )
{
    SPDLOG_TRACE( "ProcessClockStandalone::unregisterMPOImpl mpo:{}", sender.m_mpo );
    auto iFind = m_mpos.find( sender.m_mpo );
    VERIFY_RTE_MSG( iFind != m_mpos.end(), "Failed to locate mpo when unregister: " << sender.m_mpo );
    m_mpos.erase( iFind );
    checkClock();
}

void ProcessClockStandalone::requestMoveImpl( network::LogicalThreadBase* pSender, MPO mpo )
{
    {
        auto iFind = m_mpos.find( mpo );
        VERIFY_RTE_MSG( iFind != m_mpos.end(), "Failed to locate mpo when unregister: " << mpo );
        VERIFY_RTE_MSG( !iFind->second.m_bWaitingForMoveResponse, "Request move when not waiting for move request" );
        iFind->second.m_bWaitingForMoveResponse = true;
    }
    checkClock();
}

void ProcessClockStandalone::requestClockImpl( network::LogicalThreadBase* pSender, MPO mpo )
{
    {
        auto iFind = m_mpos.find( mpo );
        VERIFY_RTE_MSG( iFind != m_mpos.end(), "Failed to locate mpo when unregister: " << mpo );
        VERIFY_RTE_MSG( !iFind->second.m_bWaitingForClockResponse, "Request move when not waiting for move request" );
        iFind->second.m_bWaitingForClockResponse = true;
    }
    checkClock();
}

void ProcessClockStandalone::checkClock()
{
    bool bAllWaitingMoveResponse  = true;
    bool bAllWaitingClockResponse = true;
    for( auto& [ _, state ] : m_mpos )
    {
        if( !state.m_bWaitingForMoveResponse )
        {
            bAllWaitingMoveResponse = false;
        }
        if( !state.m_bWaitingForClockResponse )
        {
            bAllWaitingClockResponse = false;
        }
        if( !bAllWaitingMoveResponse && !bAllWaitingClockResponse )
            break;
    }

    if( bAllWaitingMoveResponse )
    {
        issueMove();
    }
    if( bAllWaitingClockResponse )
    {
        if( !m_bClockIssued )
        {
            issueClock();
        }
    }
}

void ProcessClockStandalone::clock()
{
    Tick timeNow     = std::chrono::steady_clock::now();
    m_clockTick.m_ct = FloatTickDuration( timeNow - m_startTimeStamp ).count();
    m_clockTick.m_dt = FloatTickDuration( timeNow - m_lastTick ).count();
    ++m_clockTick.m_cycle;

    for( auto& [ _, state ] : m_mpos )
    {
        if( state.m_bWaitingForClockResponse )
        {
            using namespace network::sim;
            auto msg = MSG_SimClock_Response::make(
                state.m_pSender->getID(), std::move( MSG_SimClock_Response{ m_clockTick } ) );
            state.m_pSender->send( msg );
            state.m_bWaitingForClockResponse = false;
        }
    }

    m_lastTick     = timeNow;
    m_bClockIssued = false;

    if( m_clockTick.m_cycle % 60 == 0 )
    {
        SPDLOG_TRACE( "ProcessClockStandalone clock ct:{} dt:{} cycle:{}",
                      m_clockTick.m_ct,
                      m_clockTick.m_dt,
                      m_clockTick.m_cycle );
    }
}

void ProcessClockStandalone::issueMove()
{
    for( auto& [ _, state ] : m_mpos )
    {
        if( state.m_bWaitingForMoveResponse )
        {
            SPDLOG_TRACE( "ProcessClockStandalone::issueMove clock ct:{} dt:{} cycle:{}",
                        m_clockTick.m_ct,
                        m_clockTick.m_dt,
                        m_clockTick.m_cycle );
            using namespace network::sim;
            auto msg = MSG_SimMoveComplete_Response::make(
                state.m_pSender->getID(), std::move( MSG_SimMoveComplete_Response{} ) );
            state.m_pSender->send( msg );
            state.m_bWaitingForMoveResponse = false;
        }
    }
}

void ProcessClockStandalone::issueClock()
{
    ProcessClockStandalone* pThis             = this;
    auto                    timeSinceLastTick = std::chrono::steady_clock::now() - m_lastTick;
    if( timeSinceLastTick >= m_tickRate )
    {
        // send immediately
        boost::asio::post( m_strand, [ pThis ]() { pThis->clock(); } );
    }
    else
    {
        using namespace std::chrono_literals;
        m_timer.expires_from_now( m_tickRate - timeSinceLastTick );
        m_timer.async_wait( [ pThis ]( boost::system::error_code ec ) { pThis->clock(); } );
    }
    m_bClockIssued = true;
}

} // namespace mega::service
