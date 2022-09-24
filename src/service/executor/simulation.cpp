
#include "service/executor/simulation.hpp"

#include "mega/common.hpp"

#include "runtime/runtime_api.hpp"
#include "runtime/mpo_context.hpp"

#include "service/executor.hpp"

#include "service/network/conversation.hpp"

#include "service/protocol/model/exe_leaf.hxx"
#include "service/protocol/model/exe_sim.hxx"
#include "service/protocol/model/messages.hxx"

namespace mega
{
namespace service
{

Simulation::Simulation( Executor& executor, const network::ConversationID& conversationID )
    : ExecutorRequestConversation( executor, conversationID, std::nullopt )
    , m_requestChannel( executor.m_io_context )
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
MPO Simulation::getThisMPO() { return m_mpo.value(); }

std::string Simulation::acquireMemory( MPO mpo )
{
    VERIFY_RTE( m_pYieldContext );
    // return getLeafRequest( *m_pYieldContext ).ExeAcquireMemory( mpo );
    THROW_RTE( "TODO" );
}
MPO Simulation::getNetworkAddressMPO( NetworkAddress networkAddress )
{
    VERIFY_RTE( m_pYieldContext );
    // return getLeafRequest( *m_pYieldContext ).ExeGetNetworkAddressMPO( networkAddress );
    THROW_RTE( "TODO" );
}
NetworkAddress Simulation::getRootNetworkAddress( MPO mpo )
{
    VERIFY_RTE( m_pYieldContext );
    // return NetworkAddress{ getLeafRequest( *m_pYieldContext ).ExeGetRootNetworkAddress( mpo ) };
    THROW_RTE( "TODO" );
}
NetworkAddress Simulation::allocateNetworkAddress( MPO mpo, TypeID objectTypeID )
{
    VERIFY_RTE( m_pYieldContext );
    // return NetworkAddress{ getLeafRequest( *m_pYieldContext ).ExeAllocateNetworkAddress( mpo, objectTypeID ) };
    THROW_RTE( "TODO" );
}

void Simulation::deAllocateNetworkAddress( MPO mpo, NetworkAddress networkAddress )
{
    VERIFY_RTE( m_pYieldContext );
    // getLeafRequest( *m_pYieldContext ).ExeDeAllocateNetworkAddress( mpo, networkAddress );
    THROW_RTE( "TODO" );
}

void Simulation::stash( const std::string& filePath, mega::U64 determinant )
{
    VERIFY_RTE( m_pYieldContext );
    // getLeafRequest( *m_pYieldContext ).ExeStash( filePath, determinant );
    THROW_RTE( "TODO" );
}

bool Simulation::restore( const std::string& filePath, mega::U64 determinant )
{
    VERIFY_RTE( m_pYieldContext );
    // return getLeafRequest( *m_pYieldContext ).ExeRestore( filePath, determinant );
    THROW_RTE( "TODO" );
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

void Simulation::acknowledgeMessage( const network::ChannelMsg& msg, boost::asio::yield_context& yield_ctx )
{
    {
        ConversationBase::RequestStack stack( getMsgName( msg.msg ), *this, getConnectionID() );
        try
        {
            // SPDLOG_TRACE( "SIM: acknowledgeMessage {}", getMsgName( msg.msg ) );
            ASSERT( isRequest( msg.msg ) );
            const network::Message result = dispatchRequest( msg.msg, yield_ctx );
            if ( !result )
            {
                SPDLOG_ERROR( "SIM: Failed to dispatch request: {} on conversation: {}", msg.msg, getID() );
                THROW_RTE( "Failed to dispatch request message: " << msg.msg );
            }
        }
        catch ( std::exception& ex )
        {
            Conversation::Ptr pRequestCon = m_executor.findExistingConversation( msg.header.getConversationID() );
            pRequestCon->sendErrorResponse( getID(), ex.what(), yield_ctx );
            // if ( requestingID.has_value() )
            // {
            //     Conversation::Ptr pRequestCon = m_executor.findExistingConversation( msg.header.getConversationID()
            //     ); pRequestCon->sendErrorResponse( getID(), ex.what(), yield_ctx );
            // }
            // else
            // {
            //     error( m_stack.back(), ex.what(), yield_ctx );
            // }
        }
    }
    VERIFY_RTE_MSG( m_stack.size() == 1, "Unexpected stack size after acknowledgeMessage" );
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
    const network::Message    msg = MSG_SimClock_Request::make( MSG_SimClock_Request{} );
    const network::ChannelMsg channelMsg{
        network::Header{ static_cast< network::MessageID >( getMsgID( msg ) ), getID() }, msg };
    boost::system::error_code ec;
    m_requestChannel.async_send( ec, channelMsg, []( boost::system::error_code ec ) {} );
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
        m_pExecutionRoot = std::make_shared< mega::runtime::MPORoot >( m_mpo.value() );
        SPDLOG_TRACE( "SIM: runSimulation {} {}", m_mpo.value(), getID() );

        // issue first clock tick
        issueClock();
        bool bRegistedAsTerminating = false;

        SimulationStateMachine::MsgVector msgs;
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
                acknowledgeMessage( msg, yield_ctx );
            }
            m_stateMachine.resetAcks();

            switch ( m_stateMachine.getState() )
            {
                case SimulationStateMachine::SIM:
                {
                    // SPDLOG_TRACE( "SIM: SIM {}", getID() );
                    cycle();
                }
                break;
                case SimulationStateMachine::READ:
                {
                    SPDLOG_TRACE( "SIM: READ {}", getID() );
                }
                break;
                case SimulationStateMachine::WRITE:
                {
                    SPDLOG_TRACE( "SIM: WRITE {}", getID() );
                }
                break;
                case SimulationStateMachine::TERM:
                {
                    SPDLOG_TRACE( "SIM: TERM {}", getID() );
                }
                break;
                case SimulationStateMachine::WAIT:
                {
                    SPDLOG_TRACE( "SIM: WAIT {}", getID() );
                }
                break;
            }

            // process a message
            SUSPEND_MPO_CONTEXT();

            {
                const network::ChannelMsg msg = m_requestChannel.async_receive( yield_ctx );
                msgs.push_back( msg );
            }
            while ( m_requestChannel.try_receive(
                [ &msgs ]( boost::system::error_code ec, const network::ChannelMsg& msg )
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

            // returns whether there was clock tick
            if ( m_stateMachine.onMsg( msgs ) )
            {
                if ( !m_stateMachine.isTerminated() )
                {
                    issueClock();
                }
            }
            msgs.clear();
        }
    }
    catch ( std::exception& ex )
    {
        SPDLOG_WARN( "SIM: Conversation: {} exception: {}", getID(), ex.what() );
        m_conversationManager.conversationCompleted( shared_from_this() );
    }
}

void Simulation::run( boost::asio::yield_context& yield_ctx )
{
    // send request to root to start - will get request back to run
    network::sim::Request_Encoder request(
        [ rootRequest = getMPORequest( yield_ctx ) ]( const network::Message& msg ) mutable
        { return rootRequest.MPORoot( msg ); } );
    request.SimStart();
}

bool Simulation::SimLockRead( const mega::MPO&            owningID,
                              const mega::MPO&            requestID,
                              boost::asio::yield_context& yield_ctx )
{
    return !m_stateMachine.isTerminating();
}
bool Simulation::SimLockWrite( const mega::MPO&            owningID,
                               const mega::MPO&            requestID,
                               boost::asio::yield_context& yield_ctx )
{
    return !m_stateMachine.isTerminating();
}
void Simulation::SimLockRelease( const mega::MPO&            owningID,
                                 const mega::MPO&            requestID,
                                 boost::asio::yield_context& yield_ctx )
{
    // Do nothing just return
}
void Simulation::SimClock( boost::asio::yield_context& yield_ctx )
{
    // Do nothing just return
}

mega::MPO Simulation::SimCreate( boost::asio::yield_context& yield_ctx )
{
    // called by request handler while waiting for sim to start running
    VERIFY_RTE( m_mpo.has_value() );
    return m_mpo.value();
}

void Simulation::RootSimRun( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    m_mpo = mpo;

    m_executor.simulationInitiated( std::dynamic_pointer_cast< Simulation >( shared_from_this() ) );

    // acknowledge the pending SimCreate from request handler
    {
        const network::ChannelMsg msg = m_requestChannel.async_receive( yield_ctx );
        VERIFY_RTE( msg.header.getMessageID() == network::sim::MSG_SimCreate_Request::ID );
        acknowledgeMessage( msg, yield_ctx );
    }

    // now start running the simulation
    MPOContext::resume( this );
    m_pYieldContext = &yield_ctx;
    runSimulation( yield_ctx );
    MPOContext::suspend();
}
void Simulation::SimDestroy( const mega::MPO& requestID, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "SIM: Simulation::SimDestroy: {}", requestID );
}

} // namespace service
} // namespace mega