
#include "service/executor/simulation.hpp"

#include "mega/common.hpp"

#include "runtime/runtime_api.hpp"
#include "runtime/mpo_context.hpp"

#include "service/executor.hpp"

#include "service/network/conversation.hpp"

#include "service/protocol/model/exe_leaf.hxx"
#include "service/protocol/model/exe_sim.hxx"
#include "service/protocol/model/memory.hxx"
#include "service/protocol/model/address.hxx"
#include "service/protocol/model/stash.hxx"
#include "service/protocol/model/messages.hxx"

namespace mega
{
namespace service
{

Simulation::Simulation( Executor& executor, const network::ConversationID& conversationID )
    : ExecutorRequestConversation( executor, conversationID, std::nullopt )
    //, m_requestChannel( executor.m_io_context )
    , m_timer( executor.m_io_context )
{
}

network::Message Simulation::dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    result = network::leaf_exe::Impl::dispatchRequest( msg, yield_ctx );
    if ( result )
        return result;
    return ExecutorRequestConversation::dispatchRequest( msg, yield_ctx );
}

Simulation::SimIDVector Simulation::getSimulationIDs() { THROW_RTE( "Unsupported getSimulationIDs from simulation" ); }
Simulation::SimID       Simulation::createSimulation() { THROW_RTE( "Unsupported createSimulation from simulation" ); }
mega::reference Simulation::getRoot( const SimID& simID ) { THROW_RTE( "Unsupported getRoot from simulation" ); }
mega::reference Simulation::getRoot() { return m_pExecutionRoot->root(); }

// mega::MPOContext
MPO Simulation::getThisMPO()
{
    ASSERT( m_mpo.has_value() );
    return m_mpo.value();
}

std::string Simulation::acquireMemory( MPO mpo )
{
    VERIFY_RTE( m_pYieldContext );
    return getDaemonRequest< network::memory::Request_Encoder >( *m_pYieldContext ).AcquireSharedMemory( mpo );
}
MPO Simulation::getNetworkAddressMPO( NetworkAddress networkAddress )
{
    VERIFY_RTE( m_pYieldContext );
    return getRootRequest< network::address::Request_Encoder >( *m_pYieldContext )
        .GetNetworkAddressMPO( networkAddress );
}
NetworkAddress Simulation::getRootNetworkAddress( MPO mpo )
{
    VERIFY_RTE( m_pYieldContext );
    return getRootRequest< network::address::Request_Encoder >( *m_pYieldContext ).GetRootNetworkAddress( mpo );
}
NetworkAddress Simulation::allocateNetworkAddress( MPO mpo, TypeID objectTypeID )
{
    VERIFY_RTE( m_pYieldContext );
    return getRootRequest< network::address::Request_Encoder >( *m_pYieldContext )
        .AllocateNetworkAddress( mpo, objectTypeID );
}

void Simulation::deAllocateNetworkAddress( MPO mpo, NetworkAddress networkAddress )
{
    VERIFY_RTE( m_pYieldContext );
    getRootRequest< network::address::Request_Encoder >( *m_pYieldContext )
        .DeAllocateNetworkAddress( mpo, networkAddress );
}

void Simulation::stash( const std::string& filePath, mega::U64 determinant )
{
    VERIFY_RTE( m_pYieldContext );
    getRootRequest< network::stash::Request_Encoder >( *m_pYieldContext ).StashStash( filePath, determinant );
}

bool Simulation::restore( const std::string& filePath, mega::U64 determinant )
{
    VERIFY_RTE( m_pYieldContext );
    return getRootRequest< network::stash::Request_Encoder >( *m_pYieldContext ).StashRestore( filePath, determinant );
}

bool Simulation::readLock( MPO mpo )
{
    VERIFY_RTE( m_pYieldContext );
    // const network::ConversationID id = getLeafRequest( *m_pYieldContext ).ExeGetMPOContextID( mpo );
    // if ( getLeafRequest( *m_pYieldContext ).ExeSimReadLock( id ) )
    //{
    //     m_lockTracker.onRead( mpo );
    //     return true;
    // }
    // else
    //{
    //     return false;
    // }
    THROW_RTE( "TODO" );
}

bool Simulation::writeLock( MPO mpo )
{
    VERIFY_RTE( m_pYieldContext );
    // const network::ConversationID id = getLeafRequest( *m_pYieldContext ).ExeGetMPOContextID( mpo );
    // if ( getLeafRequest( *m_pYieldContext ).ExeSimWriteLock( id ) )
    //{
    //     m_lockTracker.onWrite( mpo );
    //     return true;
    // }
    // else
    //{
    //     return false;
    // }
    THROW_RTE( "TODO" );
}

void Simulation::releaseLock( MPO mpo )
{
    VERIFY_RTE( m_pYieldContext );
    // const network::ConversationID id = getLeafRequest( *m_pYieldContext ).ExeGetMPOContextID( mpo );
    // getLeafRequest( *m_pYieldContext ).ExeSimReleaseLock( id );
    // m_lockTracker.onRelease( mpo );
    THROW_RTE( "TODO" );
}

void Simulation::issueClock()
{
    Simulation* pThis = this;
    using namespace std::chrono_literals;
    m_timer.expires_from_now( 200ms );
    m_timer.async_wait( [ pThis ]( boost::system::error_code ec ) { pThis->clock(); } );
}

void Simulation::clock()
{
    // SPDLOG_TRACE( "SIM: Clock {} {}", getID(), getElapsedTime() );
    //  send the clock tick msg
    using namespace network::sim;
    send( network::ReceivedMsg{ getConnectionID(), MSG_SimClock_Request::make( MSG_SimClock_Request{} ) } );
}

void Simulation::cycle()
{
    m_scheduler.cycle();

    // submit all transactions...
}

void Simulation::runSimulation( boost::asio::yield_context& yield_ctx )
{
    try
    {
        VERIFY_RTE( m_mpo.has_value() );
        m_pExecutionRoot = std::make_shared< mega::runtime::MPORoot >( m_mpo.value() );
        SPDLOG_TRACE( "SIM: runSimulation {} {}", m_mpo.value(), getID() );

        // issue first clock tick
        issueClock();
        bool bRegistedAsTerminating = false;

        StateMachine::MsgVector msgs, msgs2;
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
                    SPDLOG_TRACE( "SIM: SIM {} {}", getID(), getElapsedTime() );
                    cycle();
                }
                break;
                case StateMachine::READ:
                {
                    SPDLOG_TRACE( "SIM: READ {}", getID() );
                }
                break;
                case StateMachine::WRITE:
                {
                    SPDLOG_TRACE( "SIM: WRITE {}", getID() );
                }
                break;
                case StateMachine::TERM:
                {
                    SPDLOG_TRACE( "SIM: TERM {}", getID() );
                }
                break;
                case StateMachine::WAIT:
                {
                    SPDLOG_TRACE( "SIM: WAIT {}", getID() );
                }
                break;
            }

            // process a message
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
            // SPDLOG_TRACE( "SIM: Msgs {}", msgs.size() );

            // process non-state messages
            {
                msgs2.clear();
                for ( const auto& msg : m_messageQueue )
                {
                    switch ( StateMachine::getMsgID( msg ) )
                    {
                        case StateMachine::Read::ID:
                        case StateMachine::Write::ID:
                        case StateMachine::Release::ID:
                        case StateMachine::Destroy::ID:
                        case StateMachine::Clock::ID:
                            msgs2.push_back( msg );
                            break;
                        default:
                            dispatchRequestImpl( msg, yield_ctx );
                            break;
                    }
                }
                m_messageQueue.clear();
            }

            // returns whether there was clock tick
            if ( m_stateMachine.onMsg( msgs2 ) )
            {
                if ( !m_stateMachine.isTerminated() )
                {
                    issueClock();
                }
            }
            msgs2.clear();
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
    SPDLOG_TRACE( "SIM::dispatchRequestsUntilResponse" );
    network::ReceivedMsg msg;
    while ( true )
    {
        msg = receive( yield_ctx );

        // simulation is running so process as normal
        if ( isRequest( msg.msg ) )
        {
            if ( m_mpo.has_value() || ( getMsgID( msg.msg ) == network::leaf_exe::MSG_RootSimRun_Request::ID ) )
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
                        const network::ReceivedMsg rMsg{ m_stack.back(), network::make_error_msg( "Disconnection" ) };
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
    if ( getMsgID( msg.msg ) == network::MSG_Error_Response::ID )
    {
        throw std::runtime_error( network::MSG_Error_Response::get( msg.msg ).what );
    }
    return msg.msg;
}

void Simulation::run( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "SIM: run" );
    // send request to root to start - will get request back to run
    network::sim::Request_Encoder request(
        [ rootRequest = getMPRequest( yield_ctx ) ]( const network::Message& msg ) mutable
        { return rootRequest.MPRoot( msg, mega::MP{} ); } );
    request.SimStart();
}

bool Simulation::SimLockRead( const mega::MPO&, const mega::MPO&, boost::asio::yield_context& )
{
    return !m_stateMachine.isTerminating();
}
bool Simulation::SimLockWrite( const mega::MPO&, const mega::MPO&, boost::asio::yield_context& )
{
    return !m_stateMachine.isTerminating();
}
void Simulation::SimLockRelease( const mega::MPO&, const mega::MPO&, boost::asio::yield_context& )
{
    // Do nothing just return
}
void Simulation::SimClock( boost::asio::yield_context& )
{
    // Do nothing just return
}

mega::MPO Simulation::SimCreate( boost::asio::yield_context& )
{
    VERIFY_RTE( m_mpo.has_value() );
    SPDLOG_TRACE( "SIM: SimCreate {}", m_mpo.value() );
    // This is called when RootSimRun acks the pending SimCreate from ExecutorRequestConversation::SimCreate
    return m_mpo.value();
}

void Simulation::RootSimRun( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "SIM: RootSimRun {}", mpo );
    m_mpo = mpo;

    // now start running the simulation
    MPOContext::resume( this );
    m_pYieldContext = &yield_ctx;
    runSimulation( yield_ctx );
    MPOContext::suspend();
}
void Simulation::SimDestroy( boost::asio::yield_context& )
{
    // do nothing
}

} // namespace service
} // namespace mega