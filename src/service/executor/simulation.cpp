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

#include "service/executor/simulation.hpp"

#include "jit/jit_exception.hpp"
#include "jit/program_functions.hxx"

#include "service/mpo_visitor.hpp"

#include "service/executor/executor.hpp"

#include "service/network/logical_thread.hpp"

#include "service/protocol/common/context.hpp"

#include "service/protocol/model/memory.hxx"
#include "service/protocol/model/stash.hxx"
#include "service/protocol/model/messages.hxx"
#include "service/protocol/model/enrole.hxx"

#include "mega/bin_archive.hpp"
#include "mega/iterator.hpp"
#include "mega/logical_tree.hpp"
#include "mega/printer.hpp"

#include <boost/filesystem/operations.hpp>
#include <memory>

namespace mega::service
{

Simulation::Simulation( Executor& executor, const network::LogicalThreadID& logicalthreadID,
                        ProcessClock& processClock )
    : ExecutorRequestLogicalThread( executor, logicalthreadID )
    , MPOContext( getID() )
    , m_processClock( processClock )
    , m_simMoveManager( m_movedObjects )
    , m_stateMachine( m_simMoveManager )
{
    m_bEnableQueueing = true;
}

network::Message Simulation::dispatchInBoundRequest( const network::Message&     msg,
                                                     boost::asio::yield_context& yield_ctx )
{
    return ExecutorRequestLogicalThread::dispatchInBoundRequest( msg, yield_ctx );
}

// MPOContext
network::mpo::Request_Sender Simulation::getMPRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return ExecutorRequestLogicalThread::getMPRequest( *m_pYieldContext );
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
network::sim::Request_Encoder Simulation::getMPOSimRequest( MPO mpo )
{
    return { [ leafRequest = getMPRequest(), mpo ]( const network::Message& msg ) mutable
             { return leafRequest.MPOUp( msg, mpo ); },
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

void Simulation::runSimulation( boost::asio::yield_context& yield_ctx )
{
    try
    {
        VERIFY_RTE( m_mpo.has_value() );
        // SPDLOG_TRACE( "SIM: runSimulation {} {}", m_mpo.value(), getID() );

        m_processClock.registerMPO( network::SenderRef{ m_mpo.value(), this, {} } );

        // create the scheduler on the stack!
        Scheduler scheduler( getLog() );

        bool                    bRegistedAsTerminating = false;
        TimeStamp               lastCycle              = getLog().getTimeStamp();
        StateMachine::MsgVector tempMessages;
        while( !m_stateMachine.isTerminated() )
        {
            if( m_stateMachine.isTerminating() && !bRegistedAsTerminating )
            {
                Simulation::Ptr pSim = std::dynamic_pointer_cast< Simulation >( shared_from_this() );
                m_executor.simulationTerminating( pSim );
                bRegistedAsTerminating = true;
            }

            // acknowledge simulation requests
            {
                for( const auto& msg : m_stateMachine.acks() )
                {
                    if( m_stateMachine.getMsgID( msg ) == StateMachine::Block::ID )
                    {
                        SPDLOG_TRACE( "SIM: runSimulation Blocking Destroy msg" );
                        m_blockDestroyMsgOpt = msg;
                    }
                    else
                    {
                        acknowledgeInboundRequest( msg, yield_ctx );
                    }
                }
                m_stateMachine.resetAcks();
            }

            switch( m_stateMachine.getState() )
            {
                case StateMachine::SIM:
                {
                    {
                        QueueStackDepth queueMsgs( m_queueStack );
                        scheduler.cycle();
                    }

                    cycleComplete();

                    // NOTE: may be simCreate request - ensure transition OUT of SIM state
                    m_stateMachine.onCycle();

                    // SPDLOG_TRACE( "SIM: Cycle {} {}", getID(), m_mpo.value() );

                    if( !m_simMoveManager.sendMoveRequests() )
                    {
                        // no move requests so immediately send moveComplete
                        // SPDLOG_TRACE( "SIM: move request {}", m_mpo.value() );
                        m_processClock.requestMove( this, m_mpo.value() );
                    }
                }
                break;
                case StateMachine::MOVE:
                {
                    // SPDLOG_TRACE( "SIM: MOVE {} {}", getID(), m_mpo.value() );
                }
                break;
                case StateMachine::READ:
                {
                    // SPDLOG_TRACE( "SIM: READ {} {}", getID(), m_mpo.value() );
                }
                break;
                case StateMachine::WRITE:
                {
                    // SPDLOG_TRACE( "SIM: WRITE {} {}", getID(), m_mpo.value() );
                }
                break;
                case StateMachine::TERM:
                {
                    //SPDLOG_TRACE( "SIM: TERM {} {}", getID(), m_mpo.value() );
                    m_processClock.unregisterMPO( network::SenderRef{ m_mpo.value(), this, {} } );
                }
                break;
                case StateMachine::WAIT:
                {
                    // SPDLOG_TRACE( "SIM: WAIT {} {}", getID(), m_mpo.value() );
                }
                break;
            }

            // process a message
            if( !m_stateMachine.isTerminated() )
            {
                ASSERT( m_queueStack == 0 );
                unqueue();
                const network::ReceivedMessage msg = receiveDeferred( yield_ctx );
                // SPDLOG_TRACE( "SIM: MSG {} {}", msg.msg, m_mpo.value() );
                switch( StateMachine::getMsgID( msg ) )
                {
                    case StateMachine::Read::ID:
                    case StateMachine::Write::ID:
                    case StateMachine::Release::ID:
                    case StateMachine::Block::ID:
                    case StateMachine::Destroy::ID:
                    case StateMachine::Clock::ID:
                    case StateMachine::MoveComplete::ID:
                    case StateMachine::MoveRequest::ID:
                    case StateMachine::MoveResponse::ID:
                    {
                        switch( m_stateMachine.onMsg( { msg } ) )
                        {
                            case StateMachine::eNothing:
                            {
                                // do nothing
                            }
                            break;
                            case StateMachine::eClockTick:
                            {
                                // SPDLOG_TRACE( "SIM: clock request {}", m_mpo.value() );
                                m_processClock.requestClock( this, m_mpo.value(), getLog().getRange( lastCycle ) );
                                lastCycle = getLog().getTimeStamp();
                            }
                            break;
                            case StateMachine::eMoveComplete:
                            {
                                // SPDLOG_TRACE( "SIM: move request {}", m_mpo.value() );
                                m_processClock.requestMove( this, m_mpo.value() );
                            }
                            break;
                            default:
                            {
                                THROW_RTE( "Invalid state machine onMsg result" );
                            }
                            break;
                        }
                    }
                    break;
                    default:
                    {
                        QueueStackDepth queueMsgs( m_queueStack );
                        acknowledgeInboundRequest( msg, yield_ctx );
                    }
                    break;
                }
            }
        }
    }
    catch( std::exception& ex )
    {
        SPDLOG_WARN( "SIM: LogicalThread: {} exception: {}", getID(), ex.what() );
        getThreadManager().logicalthreadCompleted( shared_from_this() );
    }
    catch( mega::runtime::JITException& ex )
    {
        SPDLOG_WARN( "SIM: LogicalThread: {} exception: {}", getID(), ex.what() );
        getThreadManager().logicalthreadCompleted( shared_from_this() );
    }
}

void Simulation::unqueue()
{
    // at startup will queue the SimCreate request which should be
    // acknowledged once simulation has completed startup
    if( m_simCreateMsgOpt.has_value() )
    {
        // can test if RootSimRun has run by checking the m_mpo is set
        if( m_mpo.has_value() )
        {
            // SPDLOG_TRACE( "SIM::unqueue: {}", m_simCreateMsgOpt.value().msg );
            m_unqueuedMessages.push_back( m_simCreateMsgOpt.value() );
            m_simCreateMsgOpt.reset();
        }
    }
    else if( !m_messageQueue.empty() )
    {
        // SPDLOG_TRACE( "SIM::unqueue" );
        VERIFY_RTE( m_unqueuedMessages.empty() );
        m_unqueuedMessages.swap( m_messageQueue );
        std::reverse( m_unqueuedMessages.begin(), m_unqueuedMessages.end() );
    }
    else
    {
        // allow ordinary request processing
        ExecutorRequestLogicalThread::unqueue();
    }
}

bool Simulation::queue( const network::ReceivedMessage& msg )
{
    if( m_bShuttingDown )
    {
        switch( StateMachine::getMsgID( msg ) )
        {
            case StateMachine::Clock::ID:
            {
                // ignor further state machine msgs
                return true;
            }
            default:
                return ExecutorRequestLogicalThread::queue( msg );
        }
    }
    else
    {
        switch( StateMachine::getMsgID( msg ) )
        {
            case StateMachine::Read::ID:
            case StateMachine::Write::ID:
            case StateMachine::Release::ID:
            case StateMachine::Block::ID:
            case StateMachine::Destroy::ID:
            case StateMachine::Clock::ID:
            case StateMachine::MoveComplete::ID:
            case StateMachine::MoveRequest::ID:
            case StateMachine::MoveResponse::ID:
            {
                // if processing a request then postpone state machine messages
                if( m_queueStack != 0 )
                {
                    // SPDLOG_TRACE( "SIM::queue {}", msg.msg );
                    m_messageQueue.push_back( msg );
                    return true;
                }
                else
                {
                    return ExecutorRequestLogicalThread::queue( msg );
                }
            }
            break;
            case network::sim::MSG_SimCreate_Request::ID:
            {
                // queue SimCreate if RootSimRun has not run yet
                if( !m_mpo.has_value() )
                {
                    // SPDLOG_TRACE( "SIM::queue: {}", msg.msg );
                    VERIFY_RTE( !m_simCreateMsgOpt.has_value() );
                    m_simCreateMsgOpt = msg;
                    return true;
                }
            }
            break;
        }
        return ExecutorRequestLogicalThread::queue( msg );
    }
}

void Simulation::run( boost::asio::yield_context& yield_ctx )
{
    try
    {
        SPDLOG_TRACE( "SIM::run started" );
        // send request to root to start - will get request back to run
        network::sim::Request_Encoder request(
            [ rootRequest = ExecutorRequestLogicalThread::getMPRequest( yield_ctx ) ](
                const network::Message& msg ) mutable { return rootRequest.MPRoot( msg, MP{} ); },
            getID() );
        request.SimStart();
        SPDLOG_TRACE( "SIM::run complete" );

        dispatchRemaining( yield_ctx );
    }
    catch( std::exception& ex )
    {
        SPDLOG_ERROR( "SIM::run exception {}", ex.what() );
        m_strSimCreateError = ex.what();
    }
    catch( mega::runtime::JITException& ex )
    {
        SPDLOG_ERROR( "SIM::run JIT exception {}", ex.what() );
        m_strSimCreateError = ex.what();
    }

    if( m_blockDestroyMsgOpt.has_value() )
    {
        SPDLOG_TRACE( "SIM::run acknowledging blocking destroy" );
        // acknowledge block destroy
        acknowledgeInboundRequest( m_blockDestroyMsgOpt.value(), yield_ctx );
    }
}

void Simulation::SimErrorCheck( boost::asio::yield_context& yield_ctx )
{
    using ::operator<<;
    THROW_RTE( "Simulation::SimErrorCheck: " << getThisMPO() );
}

Snapshot Simulation::SimObjectSnapshot( const reference& object, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "SIM::SimObjectSnapshot: {} {}", getThisMPO(), object );
    QueueStackDepth queueMsgs( m_queueStack );

    using ::operator<<;
    VERIFY_RTE_MSG( object.getMPO() == getThisMPO(), "SimObjectSnapshot called on bad mpo: " << object );

    static thread_local mega::runtime::program::ObjectSaveBin objectSaveBin;

    reference heapAddress = m_pMemoryManager->networkToHeap( object );

    BinSaveArchive archive;
    archive.beginObject( heapAddress.getHeaderAddress() );
    objectSaveBin( object.getType(), heapAddress.getHeap(), &archive );

    return archive.makeSnapshot( getLog().getTimeStamp() );
}

reference Simulation::SimAllocate( const TypeID& objectTypeID, boost::asio::yield_context& )
{
    SPDLOG_TRACE( "SIM::SimAllocate: {} {}", getThisMPO(), objectTypeID );
    QueueStackDepth queueMsgs( m_queueStack );
    reference       allocated = m_pMemoryManager->New( objectTypeID );
    getLog().record( mega::log::Structure::Write( reference{}, allocated, 0, mega::log::Structure::eConstruct ) );
    return allocated.getHeaderAddress();
}

Snapshot Simulation::SimSnapshot( const MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "SIM::SimSnapshot: {}", mpo );
    THROW_TODO;
    return Snapshot{ getLog().getTimeStamp() };
}

TimeStamp Simulation::SimLockRead( const MPO& requestingMPO, const MPO& targetMPO,
                                   boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "SIM::SimLockRead: {} {}", requestingMPO, targetMPO );
    if( m_stateMachine.isTerminating() )
    {
        return 0U;
    }
    return getLog().getTimeStamp();
}

TimeStamp Simulation::SimLockWrite( const MPO& requestingMPO, const MPO& targetMPO,
                                    boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "SIM::SimLockWrite: {} {}", requestingMPO, targetMPO );
    if( m_stateMachine.isTerminating() )
    {
        return 0U;
    }
    return getLog().getTimeStamp();
}

void Simulation::SimLockRelease( const MPO& requestingMPO, const MPO& targetMPO,
                                 const network::Transaction& transaction, boost::asio::yield_context& )
{
    SPDLOG_TRACE( "SIM::SimLockRelease: {} {}", requestingMPO, targetMPO );
    if( !m_stateMachine.isTerminating() )
    {
        // NOTE: how SimLockRelease is acknowledged when the simulation routine goes
        // through its simulation requests - INCLUDING the clock response
        // need to avoid the timer generating a clock response WHILE process other request
        // since this could interupt expected responses
        QueueStackDepth queueMsgs( m_queueStack );
        applyTransaction( transaction );
    }
}

MPO Simulation::SimCreate( boost::asio::yield_context& )
{
    SPDLOG_TRACE( "SIM::SimCreate" );

    if( !m_strSimCreateError.empty() )
    {
        THROW_RTE( m_strSimCreateError );
    }
    VERIFY_RTE_MSG( m_mpo.has_value(), "SimCreate failed and has no mpo" );
    // This is called when RootSimRun acks the pending SimCreate from ExecutorRequestLogicalThread::SimCreate
    return m_mpo.value();
}

void Simulation::SetProject( const Project& project, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "SIM::SetProject: {}", m_mpo.has_value() ? m_mpo.value() : MPO{} );

    m_pDatabase.reset();
    m_pDatabase = std::make_unique< runtime::MPODatabase >( project.getProjectDatabase() );
}

void Simulation::RootSimRun( const Project& project, const MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "SIM::RootSimRun: {}", mpo );

    // now start running the simulation
    setMPOContext( this );
    m_pYieldContext = &yield_ctx;

    createRoot( project, mpo );
    runSimulation( yield_ctx );

    resetMPOContext();

    m_bShuttingDown = true;

    SPDLOG_TRACE( "SIM::RootSimRun complete: {}", mpo );
}
void Simulation::SimDestroy( boost::asio::yield_context& )
{
    if( m_mpo.has_value() )
    {
        SPDLOG_TRACE( "SIM::SimDestroy: {}", m_mpo.value() );
    }
    // do nothing
}
void Simulation::SimDestroyBlocking( boost::asio::yield_context& )
{
    SPDLOG_TRACE( "SIM::SimDestroyBlocking" );
    // do nothing
}

// network::status::Impl
network::Status Simulation::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                       boost::asio::yield_context&           yield_ctx )
{
    SPDLOG_TRACE( "Simulation::GetStatus" );
    QueueStackDepth queueMsgs( m_queueStack );

    network::Status status{ childNodeStatus };
    {
        status.setLogicalThreadID( { getID() } );
        status.setMPO( m_mpo.value() );
        {
            std::ostringstream os;
            os << "Simulation Timestamp:" << getLog().getTimeStamp();
            status.setDescription( os.str() );
        }
        status.setLogIterator( getLog().getIterator() );
        status.setLogFolder( getLog().getLogFolderPath().string() );

        using MPOTimeStampVec = std::vector< std::pair< MPO, TimeStamp > >;
        using MPOVec          = std::vector< MPO >;
        if( const auto& reads = m_lockTracker.getReads(); !reads.empty() )
            status.setReads( MPOTimeStampVec{ reads.begin(), reads.end() } );
        if( const auto& writes = m_lockTracker.getWrites(); !writes.empty() )
            status.setWrites( MPOTimeStampVec{ writes.begin(), writes.end() } );
        if( const auto& readers = m_stateMachine.reads(); !readers.empty() )
            status.setReaders( MPOVec{ readers.begin(), readers.end() } );
        if( const auto& writer = m_stateMachine.writer(); writer.has_value() )
            status.setWriter( writer.value() );

        status.setMemory( m_pMemoryManager->getStatus() );
    }

    return status;
}

std::string Simulation::Ping( const std::string& strMsg, boost::asio::yield_context& yield_ctx )
{
    using ::           operator<<;
    std::ostringstream os;
    os << "Ping reached: " << common::ProcessID::get() << " simulation: " << m_root << " got: " << strMsg.size()
       << " bytes";
    return os.str();
}

} // namespace mega::service
