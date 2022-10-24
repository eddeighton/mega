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

#include "simulation.hpp"

#include "service/context.hpp"

#include "service/executor.hpp"

#include "service/network/conversation.hpp"

#include "service/protocol/model/memory.hxx"
#include "service/protocol/model/address.hxx"
#include "service/protocol/model/stash.hxx"
#include "service/protocol/model/messages.hxx"
#include "service/protocol/model/enrole.hxx"

#include <boost/filesystem/operations.hpp>

namespace mega::service
{

Simulation::Simulation( Executor& executor, const network::ConversationID& conversationID )
    : ExecutorRequestConversation( executor, conversationID, std::nullopt )
    , MPOContext( conversationID )
    , m_timer( executor.m_io_context )
{
}

network::Message Simulation::dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if ( result = network::leaf_exe::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    return ExecutorRequestConversation::dispatchRequest( msg, yield_ctx );
}

// MPOContext
network::mpo::Request_Sender Simulation::getMPRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return ExecutorRequestConversation::getMPRequest( *m_pYieldContext );
}
network::address::Request_Encoder Simulation::getRootAddressRequest()
{
    return { [ leafRequest = getLeafRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.ExeRoot( msg ); },
             getID() };
}
network::enrole::Request_Encoder Simulation::getRootEnroleRequest()
{
    return { [ leafRequest = getLeafRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.ExeRoot( msg ); },
             getID() };
}
network::stash::Request_Encoder Simulation::getRootStashRequest()
{
    return { [ leafRequest = getLeafRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.ExeRoot( msg ); },
             getID() };
}
network::memory::Request_Encoder Simulation::getDaemonMemoryRequest()
{
    return { [ leafRequest = getLeafRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.ExeDaemon( msg ); },
             getID() };
}
network::memory::Request_Sender Simulation::getLeafMemoryRequest()
{
    return { *this, m_executor.getLeafSender(), *m_pYieldContext };
}

network::jit::Request_Sender Simulation::getLeafJITRequest()
{
    return { *this, m_executor.getLeafSender(), *m_pYieldContext };
}

void Simulation::issueClock()
{
    Simulation* pThis = this;
    using namespace std::chrono_literals;
    m_timer.expires_from_now( 15ms );
    m_timer.async_wait( [ pThis ]( boost::system::error_code ec ) { pThis->clock(); } );
}

void Simulation::clock()
{
    // SPDLOG_TRACE( "SIM: Clock {} {}", getID(), getElapsedTime() );
    //  send the clock tick msg
    using namespace network::sim;
    send( network::ReceivedMsg{ getConnectionID(), MSG_SimClock_Request::make( getID(), MSG_SimClock_Request{} ) } );
}

void Simulation::runSimulation( boost::asio::yield_context& yield_ctx )
{
    try
    {
        VERIFY_RTE( m_mpo.has_value() );
        SPDLOG_TRACE( "SIM: runSimulation {} {}", m_mpo.value(), getID() );

        // issue first clock tick
        issueClock();
        bool bRegistedAsTerminating = false;

        StateMachine::MsgVector tempMessages;
        while ( !m_stateMachine.isTerminated() )
        {
            if ( m_stateMachine.isTerminating() && !bRegistedAsTerminating )
            {
                Simulation::Ptr pSim = std::dynamic_pointer_cast< Simulation >( shared_from_this() );
                m_executor.simulationTerminating( pSim );
                bRegistedAsTerminating = true;
            }

            for ( const auto& msg : m_stateMachine.acks() )
            {
                dispatchRequestImpl( msg, yield_ctx );
            }
            m_stateMachine.resetAcks();

            switch ( m_stateMachine.getState() )
            {
                case StateMachine::SIM:
                {
                    std::ostringstream osLog;
                    osLog << "SIM: " << getID() << " " << m_mpo.value();
                    // << " " << getElapsedTime();
                    // SPDLOG_TRACE( "SIM: SIM {} {} {}", getID(), m_mpo.value(), getElapsedTime() );

                    m_scheduler.cycle();
                    m_log.log( log::LogMsg( log::LogMsg::eInfo, osLog.str() ) );
                    m_log.cycle();

                    cycleComplete();
                }
                break;
                case StateMachine::READ:
                {
                    SPDLOG_TRACE( "SIM: READ {} {}", getID(), m_mpo.value() );
                }
                break;
                case StateMachine::WRITE:
                {
                    SPDLOG_TRACE( "SIM: WRITE {} {}", getID(), m_mpo.value() );
                }
                break;
                case StateMachine::TERM:
                {
                    SPDLOG_TRACE( "SIM: TERM {} {}", getID(), m_mpo.value() );
                }
                break;
                case StateMachine::WAIT:
                {
                    SPDLOG_TRACE( "SIM: WAIT {} {}", getID(), m_mpo.value() );
                }
                break;
            }

            // process a message
            {
                SUSPEND_MPO_CONTEXT();

                {
                    const network::ReceivedMsg msg = receive( yield_ctx );
                    m_messageQueue.push_back( msg );
                }
                while ( m_channel.try_receive(
                    [ &msgs = m_messageQueue ]( boost::system::error_code ec, const network::ReceivedMsg& msg )
                    {
                        if ( !ec )
                        {
                            msgs.push_back( msg );
                        }
                        else
                        {
                            /// TODO.
                        }
                    } ) )
                    ;

                RESUME_MPO_CONTEXT();
            }

            // process non-state messages
            {
                tempMessages.clear();
                for ( const auto& msg : m_messageQueue )
                {
                    switch ( StateMachine::getMsgID( msg ) )
                    {
                        case StateMachine::Read::ID:
                        case StateMachine::Write::ID:
                        case StateMachine::Release::ID:
                        case StateMachine::Destroy::ID:
                        case StateMachine::Clock::ID:
                            tempMessages.push_back( msg );
                            break;
                        default:
                            dispatchRequestImpl( msg, yield_ctx );
                            break;
                    }
                }
                m_messageQueue.clear();
            }

            // returns whether there was clock tick
            if ( m_stateMachine.onMsg( tempMessages ) )
            {
                if ( !m_stateMachine.isTerminated() )
                {
                    issueClock();
                }
            }
            tempMessages.clear();
        }
    }
    catch ( std::exception& ex )
    {
        SPDLOG_WARN( "SIM: Conversation: {} exception: {}", getID(), ex.what() );
        m_conversationManager.conversationCompleted( shared_from_this() );
    }
}

network::Message Simulation::dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx )
{
    network::ReceivedMsg msg;
    while ( true )
    {
        msg = receive( yield_ctx );

        // simulation is running so process as normal
        if ( isRequest( msg.msg ) )
        {
            if ( m_mpo.has_value() || ( msg.msg.getID() == network::leaf_exe::MSG_RootSimRun_Request::ID ) )
            {
                dispatchRequestImpl( msg, yield_ctx );

                // check if connection has disconnected
                if ( m_disconnections.empty() )
                {
                    ASSERT( !m_stack.empty() );
                    if ( m_disconnections.count( m_stack.back() ) )
                    {
                        SPDLOG_ERROR(
                            "Generating disconnect on conversation: {} for connection: {}", getID(), m_stack.back() );
                        const network::ReceivedMsg rMsg{
                            m_stack.back(), network::make_error_msg( msg.msg.getReceiverID(), "Disconnection" ) };
                        send( rMsg );
                    }
                }
            }
            else
            {
                // queue the messages
                SPDLOG_TRACE( "SIM::dispatchRequestsUntilResponse queued: {}", msg.msg );
                m_messageQueue.push_back( msg );
            }
        }
        else
        {
            break;
        }
    }
    if ( msg.msg.getID() == network::MSG_Error_Response::ID )
    {
        throw std::runtime_error( network::MSG_Error_Response::get( msg.msg ).what );
    }
    return msg.msg;
}

void Simulation::run( boost::asio::yield_context& yield_ctx )
{
    // send request to root to start - will get request back to run
    network::sim::Request_Encoder request(
        [ rootRequest = ExecutorRequestConversation::getMPRequest( yield_ctx ) ]( const network::Message& msg ) mutable
        { return rootRequest.MPRoot( msg, MP{} ); },
        getID() );
    request.SimStart();
}

TimeStamp Simulation::SimLockRead( const MPO&, boost::asio::yield_context& )
{
    if( m_stateMachine.isTerminating() )
    {
        return {};
    }
    else
    {
        return m_log.getTimeStamp();
    }
}
TimeStamp Simulation::SimLockWrite( const MPO&, boost::asio::yield_context& )
{
    if( m_stateMachine.isTerminating() )
    {
        return {};
    }
    else
    {
        return m_log.getTimeStamp();
    }
}
void Simulation::SimLockRelease( const MPO&,
                                 const network::Transaction& transaction,
                                 boost::asio::yield_context& )
{
    for ( const auto& shedulingRecord : transaction.getSchedulingRecords() )
    {
        SPDLOG_INFO( "Got scheduling record: {} {}", shedulingRecord.first, shedulingRecord.second );
    }

    for ( auto i = 0; i != log::toInt( log::TrackType::TOTAL ); ++i )
    {
        for ( const auto& memoryRecord : transaction.getMemoryRecords( log::MemoryTrackType( i ) ) )
        {
            SPDLOG_INFO( "Got memory record: {} {}", memoryRecord.first, memoryRecord.second );
        }
    }
}
void Simulation::SimClock( boost::asio::yield_context& ) { m_clock.nextCycle(); }

MPO Simulation::SimCreate( boost::asio::yield_context& )
{
    VERIFY_RTE( m_mpo.has_value() );
    // This is called when RootSimRun acks the pending SimCreate from ExecutorRequestConversation::SimCreate
    return m_mpo.value();
}

void Simulation::RootSimRun( const reference&             root,
                             const network::JITMemoryPtr& pMemory,
                             boost::asio::yield_context&  yield_ctx )
{
    initSharedMemory( root, pMemory );

    // now start running the simulation
    setMPOContext( this );
    m_pYieldContext = &yield_ctx;
    runSimulation( yield_ctx );
    resetMPOContext();
}
void Simulation::SimDestroy( boost::asio::yield_context& )
{
    // do nothing
}

// network::status::Impl
network::Status Simulation::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                       boost::asio::yield_context&           yield_ctx )
{
    SPDLOG_TRACE( "Simulation::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        status.setConversationID( { getID() } );
        status.setMPO( m_mpo.value() );
        std::ostringstream os;
        os << "Simulation: " << m_log.getTimeStamp();
        status.setLogIterator( m_log.getIterator() );

        using MPOVec = std::vector< MPO >;
        if ( const auto& reads = m_lockTracker.getReads(); !reads.empty() )
            status.setReads( MPOVec{ reads.begin(), reads.end() } );
        if ( const auto& writes = m_lockTracker.getWrites(); !writes.empty() )
            status.setWrites( MPOVec{ writes.begin(), writes.end() } );
        if ( const auto& readers = m_stateMachine.reads(); !readers.empty() )
            status.setReaders( MPOVec{ readers.begin(), readers.end() } );
        if ( const auto& writer = m_stateMachine.writer(); writer.has_value() )
            status.setWriter( writer.value() );

        status.setDescription( os.str() );
    }

    return status;
}

std::string Simulation::Ping( boost::asio::yield_context& yield_ctx )
{
    std::ostringstream os;
    os << "Ping from Simulation: " << m_mpo.value();
    return os.str();
}

} // namespace mega::service
