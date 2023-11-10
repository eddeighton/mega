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
#include "service/executor/action_function_cache.hpp"
#include "service/executor/decision_function_cache.hpp"

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

#include "common/time.hpp"

#include <boost/filesystem/operations.hpp>

#include <memory>

namespace mega::service
{

Simulation::Simulation( Executor& executor, const network::LogicalThreadID& logicalthreadID,
                        ProcessClock& processClock )
    : ExecutorRequestLogicalThread( executor, logicalthreadID )
    , MPOContext( getID() )
    , m_processClock( processClock )
    , m_transactionMachine( *this, m_ackVector )
    , m_simMoveMachine( *this, m_ackVector, m_movedObjects )
    , m_stateMachine( *this, m_transactionMachine, m_simMoveMachine, m_ackVector )
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

bool Simulation::unrequestClock()
{
    return m_processClock.unrequestClock( this, m_mpo.value() );
}

void Simulation::runSimulation( boost::asio::yield_context& yield_ctx )
{
    try
    {
        VERIFY_RTE( m_mpo.has_value() );
        // SPDLOG_TRACE( "SIM: runSimulation {} {}", m_mpo.value(), getID() );

        static mega::runtime::program::Enumerate funcEnumerate;
        static mega::runtime::program::Dispatch  funcDispatch;

        m_processClock.registerMPO( network::SenderRef{ m_mpo.value(), this, {} } );

        log::FileIterator< log::Event::Read >      m_iter_events      = m_pLog->begin< log::Event::Read >();
        log::FileIterator< log::Transition::Read > m_iter_transitions = m_pLog->begin< log::Transition::Read >();
        log::FileIterator< log::Structure::Read >  m_iter_structure   = m_pLog->begin< log::Structure::Read >();

        ActionFunctionCache   actionFunctionCache;
        DecisionFunctionCache decisionFunctionCache;

        TimeStamp cycle     = getLog().getTimeStamp();
        TimeStamp lastCycle = cycle;

        while( !m_stateMachine.isTerminated() )
        {
            ASSERT( m_queueStack == 0 );
            unqueue();
            const network::ReceivedMessage msg = receiveDeferred( yield_ctx );

            const auto result = m_stateMachine.onMessage( msg );

            // acknowledge simulation requests
            {
                for( const auto& msg : m_ackVector )
                {
                    if( SM::isBlock( msg ) )
                    {
                        SPDLOG_TRACE( "SIM: runSimulation Blocking Destroy msg" );
                        m_blockDestroyMsgOpt = msg;
                    }
                    else
                    {
                        acknowledgeInboundRequest( msg, yield_ctx );
                    }
                }
                m_ackVector.clear();
            }

            switch( result )
            {
                case SM::eSendClockRequest:
                {
                    m_processClock.requestClock( this, m_mpo.value(), getLog().getRange( cycle ) );
                    lastCycle = cycle;
                    cycle     = getLog().getTimeStamp();
                }
                break;
                case SM::eSendMoveComplete:
                {
                    m_processClock.requestMove( this, m_mpo.value() );
                }
                break;
                case SM::eRunCycle:
                {
                    // process all events
                    {
                        for( ; m_iter_events != m_pLog->end< log::Event::Read >(); ++m_iter_events )
                        {
                            const auto& event = *m_iter_events;
                            funcDispatch( event.getRef() );

                            switch( event.getType() )
                            {
                                case log::Event::eComplete:
                                    SPDLOG_TRACE( "Got completion event: {}", event.getRef() );
                                    break;
                                case log::Event::eStart:
                                    SPDLOG_TRACE( "Got start event: {}", event.getRef() );
                                    break;
                                case log::Event::eSignal:
                                    SPDLOG_TRACE( "Got signal event: {}", event.getRef() );
                                    break;
                                default:
                                {
                                    THROW_RTE( "Unknown event type" );
                                }
                            }
                        }
                    }

                    // process all transitions
                    {
                        for( ; m_iter_transitions != m_pLog->end< log::Transition::Read >(); ++m_iter_transitions )
                        {
                            const auto& transition = *m_iter_transitions;
                            const auto& ref = transition.getRef();
                            SPDLOG_TRACE( "Got transition: {}", ref );

                            auto pDecision = decisionFunctionCache.getDecisionFunction( ref.getType() );
                            pDecision( &ref );
                        }
                    }

                    // run all active actions
                    {
                        QueueStackDepth queueMsgs( m_queueStack );

                        for( auto i = m_pMemoryManager->begin(), iEnd = m_pMemoryManager->end(); i != iEnd; ++i )
                        {
                            reference ref      = i->first;
                            U32       iterator = 0;
                            while( true )
                            {
                                SubTypeInstance subTypeInstance = funcEnumerate( ref, iterator );
                                if( iterator == 0 )
                                {
                                    break;
                                }

                                auto actionContext = mega::reference::make(
                                    ref, TypeInstance{ ref.getType().getObjectID(), subTypeInstance } );
                                auto pAction = actionFunctionCache.getActionFunction( actionContext.getType() );
                                mega::ActionCoroutine actionCoroutine = pAction( &actionContext );
                                while( !actionCoroutine.done() )
                                {
                                    actionCoroutine.resume();
                                }

                                const auto& reason = actionCoroutine.getReason();
                                switch( reason.reason )
                                {
                                    case eReason_Wait:
                                    case eReason_Wait_All:
                                    case eReason_Wait_Any:
                                    case eReason_Sleep:
                                    case eReason_Sleep_All:
                                    case eReason_Sleep_Any:
                                    case eReason_Timeout:
                                        break;
                                    case eReason_Complete:
                                    {
                                        SPDLOG_TRACE( "Generating completion event: {}", actionContext );
                                        // generate completion event
                                        m_pLog->record(
                                            mega::log::Event::Write( actionContext, mega::log::Event::eComplete ) );
                                    }
                                    break;
                                    default:
                                    {
                                        THROW_RTE( "Unknown return reason" );
                                    }
                                }
                            }
                        }
                    }

                    // process all structure records
                    {
                        for( ; m_iter_structure != m_pLog->end< log::Structure::Read >(); ++m_iter_structure )
                        {
                            const auto& structure = *m_iter_structure;

                            SPDLOG_INFO( "Got structure: {} {} {}", structure.getSource(), structure.getTarget(),
                                         structure.getRelation() );

                            // structure.getSource();
                            // structure.getTarget();
                            // structure.getRelation();
                        }
                    }

                    cycleComplete();

                    // NOTE: may be simCreate request - ensure transition OUT of SIM state
                    if( m_stateMachine.onCycle() )
                    {
                        m_processClock.requestMove( this, m_mpo.value() );
                    }
                }
                break;
                case SM::eUnregister:
                {
                    m_processClock.unregisterMPO( network::SenderRef{ m_mpo.value(), this, {} } );
                    m_executor.simulationTerminating( std::dynamic_pointer_cast< Simulation >( shared_from_this() ) );
                }
                break;
                case SM::eRecognised:
                {
                    // do nothing
                }
                break;
                case SM::eIgnored:
                {
                    QueueStackDepth queueMsgs( m_queueStack );
                    acknowledgeInboundRequest( msg, yield_ctx );
                }
                break;
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
        switch( SM::getMsgID( msg ) )
        {
            case SM::Clock::ID:
            {
                // ignor further state machine msgs
                return true;
            }
            default:
            {
                return ExecutorRequestLogicalThread::queue( msg );
            }
        }
    }
    else
    {
        if( SM::isMsg( msg ) )
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
        else
        {
            switch( SM::getMsgID( msg ) )
            {
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
                default:
                {
                    // Do Nothing
                }
                break;
            }
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
    getLog().record(
        mega::log::Structure::Write( allocated, allocated.getNetworkAddress(), 0, mega::log::Structure::eConstruct ) );
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
    if( m_stateMachine.isTerminated() )
    {
        return 0U;
    }
    return getLog().getTimeStamp();
}

TimeStamp Simulation::SimLockWrite( const MPO& requestingMPO, const MPO& targetMPO,
                                    boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "SIM::SimLockWrite: {} {}", requestingMPO, targetMPO );
    if( m_stateMachine.isTerminated() )
    {
        return 0U;
    }
    return getLog().getTimeStamp();
}

void Simulation::SimLockRelease( const MPO& requestingMPO, const MPO& targetMPO,
                                 const network::Transaction& transaction, boost::asio::yield_context& )
{
    SPDLOG_TRACE( "SIM::SimLockRelease: {} {}", requestingMPO, targetMPO );
    if( !m_stateMachine.isTerminated() )
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
        m_stateMachine.status( status );

        status.setMemory( m_pMemoryManager->getStatus() );
    }

    return status;
}

mega::reports::Container Simulation::GetReport( const mega::reports::URL&                      url,
                                                const std::vector< mega::reports::Container >& report,
                                                boost::asio::yield_context&                    yield_ctx )
{
    SPDLOG_TRACE( "Simulation::GetReport" );
    VERIFY_RTE( report.empty() );
    using namespace mega::reports;
    using namespace std::string_literals;
    Table table;
    table.m_rows.push_back( { Line{ "   Thread ID: "s }, Line{ getID() } } );
    MPOContext::getBasicReport( url, table );

    {
        Table locks{ { "Lock Type"s, "MPO"s } };
        for( auto mpo : m_transactionMachine.reads() )
        {
            locks.m_rows.push_back( { Line{ "READ"s }, Line{ mpo } } );
        }
        if( auto writerOpt = m_transactionMachine.writer(); writerOpt.has_value() )
        {
            locks.m_rows.push_back( { Line{ "WRITE"s }, Line{ writerOpt.value() } } );
        }
        table.m_rows.push_back( { Line{ "   Out Locks: "s }, locks } );
    }

    return table;
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
