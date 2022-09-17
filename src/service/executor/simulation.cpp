
#include "service/executor/simulation.hpp"

#include "mega/common.hpp"
#include "runtime/runtime.hpp"
#include "service/executor.hpp"

#include "service/network/conversation.hpp"

#include "service/protocol/model/exe_leaf.hxx"
#include "service/protocol/model/exe_sim.hxx"
#include "service/protocol/model/messages.hxx"

#include <boost/asio/execution_context.hpp>

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

bool Simulation::dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    return network::leaf_exe::Impl::dispatchRequest( msg, yield_ctx )
           || network::exe_sim::Impl::dispatchRequest( msg, yield_ctx );
}

void Simulation::error( const network::ConnectionID& connectionID, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx )
{
    if ( m_executor.getLeafSender().getConnectionID() == connectionID )
    {
        m_executor.getLeafSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "SIM: Cannot resolve connection in error handler: {} for error: {}", connectionID, strErrorMsg );
        THROW_RTE( "Simulation: Executor Critical error in error handler: " << connectionID << " : " << strErrorMsg );
    }
}

// mega::ExecutionContext
MPE Simulation::getThisMPE() { return m_executionIndex.value(); }

mega::reference Simulation::getRoot() { return m_pExecutionRoot->root(); }

std::string Simulation::acquireMemory( MPE mpe )
{
    VERIFY_RTE( m_pYieldContext );
    return getLeafRequest( *m_pYieldContext ).ExeAcquireMemory( mpe );
}
NetworkAddress Simulation::allocateNetworkAddress( MPE mpe, TypeID objectTypeID )
{
    VERIFY_RTE( m_pYieldContext );
    return NetworkAddress{ getLeafRequest( *m_pYieldContext ).ExeAllocateNetworkAddress( mpe, objectTypeID ) };
}

void Simulation::deAllocateNetworkAddress( MPE mpe, NetworkAddress networkAddress )
{
    VERIFY_RTE( m_pYieldContext );
    getLeafRequest( *m_pYieldContext ).ExeDeAllocateNetworkAddress( mpe, networkAddress );
}

void Simulation::stash( const std::string& filePath, std::size_t determinant )
{
    VERIFY_RTE( m_pYieldContext );
    getLeafRequest( *m_pYieldContext ).ExeStash( filePath, determinant );
}

bool Simulation::restore( const std::string& filePath, std::size_t determinant )
{
    VERIFY_RTE( m_pYieldContext );
    return getLeafRequest( *m_pYieldContext ).ExeRestore( filePath, determinant );
}

bool Simulation::readLock( MPE mpe )
{
    VERIFY_RTE( m_pYieldContext );

    // if( m_executor.m_simulations

    const network::ConversationID id = getLeafRequest( *m_pYieldContext ).ExeGetExecutionContextID( mpe );
    return getLeafRequest( *m_pYieldContext ).ExeSimReadLock( id );
}

bool Simulation::writeLock( MPE mpe )
{
    VERIFY_RTE( m_pYieldContext );
    const network::ConversationID id = getLeafRequest( *m_pYieldContext ).ExeGetExecutionContextID( mpe );
    return getLeafRequest( *m_pYieldContext ).ExeSimWriteLock( id );
}

void Simulation::releaseLock( MPE mpe )
{
    VERIFY_RTE( m_pYieldContext );
    const network::ConversationID id = getLeafRequest( *m_pYieldContext ).ExeGetExecutionContextID( mpe );
    getLeafRequest( *m_pYieldContext ).ExeSimReleaseLock( id );
}

void Simulation::acknowledgeMessage( const network::ChannelMsg&                            msg,
                                     const std::optional< mega::network::ConversationID >& requestingID,
                                     boost::asio::yield_context&                           yield_ctx )
{
    do
    {
        ConversationBase::RequestStack stack( getMsgName( msg.msg ), *this, getConnectionID() );
        try
        {
            ASSERT( isRequest( msg.msg ) );
            if ( !dispatchRequest( msg.msg, yield_ctx ) )
            {
                SPDLOG_ERROR( "SIM: Failed to dispatch request: {} on conversation: {}", msg.msg, getID() );
                THROW_RTE( "Failed to dispatch request message: " << msg.msg );
            }
        }
        catch ( std::exception& ex )
        {
            if ( requestingID.has_value() )
            {
                Conversation::Ptr pRequestCon = m_executor.findExistingConversation( requestingID.value() );
                pRequestCon->sendErrorResponse( getID(), ex.what(), yield_ctx );
            }
            else
            {
                error( m_stack.back(), ex.what(), yield_ctx );
            }
        }
    } while ( !m_stack.empty() );
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
    SPDLOG_TRACE( "SIM: Clock {} {}", getID(), getElapsedTime() );
    // send the clock tick msg
    using namespace network::exe_sim;
    const network::Message    msg = MSG_ExeExeClock_Request::make( MSG_ExeExeClock_Request{} );
    const network::ChannelMsg channelMsg{
        network::Header{ static_cast< network::MessageID >( getMsgID( msg ) ), getID() }, msg };
    boost::system::error_code ec;
    m_requestChannel.async_send( ec, channelMsg, []( boost::system::error_code ec ) {} );
}

void Simulation::runCycle( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "SIM: runCycle {}", getID() );
    issueClock();
    m_scheduler.cycle();
}

void Simulation::runSimulation( boost::asio::yield_context& yield_ctx )
{
    try
    {
        m_executionIndex = getLeafRequest( yield_ctx ).ExeCreateExecutionContext();
        m_pExecutionRoot = std::make_shared< mega::runtime::ExecutionRoot >( m_executionIndex.value() );
        SPDLOG_TRACE( "SIM: root acquired {}", getID() );

        SimulationStateMachine::MsgVector msgs;
        bool                              bContinue = true;
        while ( bContinue )
        {
            switch ( m_stateMachine.getState() )
            {
                case SimulationStateMachine::SIM:
                {
                    SPDLOG_TRACE( "SIM: SIM {}", getID() );
                    runCycle( yield_ctx );
                }
                break;
                case SimulationStateMachine::READ:
                {
                    SPDLOG_TRACE( "SIM: READ {}", getID() );
                    for ( const auto& msg : m_stateMachine.acks() )
                    {
                        acknowledgeMessage( msg.first, msg.second, yield_ctx );
                    }
                }
                break;
                case SimulationStateMachine::WRITE:
                {
                    SPDLOG_TRACE( "SIM: WRITE {}", getID() );
                    for ( const auto& msg : m_stateMachine.acks() )
                    {
                        acknowledgeMessage( msg.first, msg.second, yield_ctx );
                    }
                }
                break;
                case SimulationStateMachine::TERM:
                {
                    SPDLOG_TRACE( "SIM: TERM {}", getID() );
                    for ( const auto& msg : m_stateMachine.acks() )
                    {
                        acknowledgeMessage( msg.first, msg.second, yield_ctx );
                    }
                    bContinue = false;
                }
                break;
                case SimulationStateMachine::WAIT:
                {
                    SPDLOG_TRACE( "SIM: WAIT {}", getID() );
                    // do nothing and wait on receive channel
                }
                break;
            }

            // process a message
            if ( bContinue )
            {
                SUSPEND_EXECUTION_CONTEXT();

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

                RESUME_EXECUTION_CONTEXT();
                SPDLOG_TRACE( "SIM: Msgs {}", msgs.size() );
                m_stateMachine.onMsg( msgs );
                msgs.clear();
            }
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
    SPDLOG_TRACE( "SIM: run: {}", getID() );
    ExecutionContext::resume( this );
    m_pYieldContext = &yield_ctx;
    runSimulation( yield_ctx );
    ExecutionContext::suspend();
}

void Simulation::ExeSimDestroy( const mega::network::ConversationID& requestingConID,
                                boost::asio::yield_context&          yield_ctx )
{
    SPDLOG_TRACE( "SIM: Simulation::ExeSimDestroy: {}", requestingConID );
    Conversation::Ptr pRequestCon = m_executor.findExistingConversation( requestingConID );
    VERIFY_RTE( pRequestCon );
    network::exe_sim::Response_Encode response( *this, *pRequestCon, yield_ctx );
    response.ExeSimDestroy();
}

void Simulation::ExeSimReadLockAcquire( const mega::network::ConversationID& requestingConID,
                                        boost::asio::yield_context&          yield_ctx )
{
    SPDLOG_TRACE( "SIM: Simulation::ExeSimReadLockAcquire: {}", requestingConID );
    Conversation::Ptr pRequestCon = m_executor.findExistingConversation( requestingConID );
    VERIFY_RTE( pRequestCon );
    network::exe_sim::Response_Encode response( *this, *pRequestCon, yield_ctx );
    response.ExeSimReadLockAcquire( m_stateMachine.isTerminating() );
}

void Simulation::ExeSimWriteLockAcquire( const mega::network::ConversationID& requestingConID,
                                         boost::asio::yield_context&          yield_ctx )
{
    SPDLOG_TRACE( "SIM: Simulation::ExeSimWriteLockAcquire: {}", requestingConID );
    Conversation::Ptr pRequestCon = m_executor.findExistingConversation( requestingConID );
    VERIFY_RTE( pRequestCon );
    network::exe_sim::Response_Encode response( *this, *pRequestCon, yield_ctx );
    response.ExeSimWriteLockAcquire( m_stateMachine.isTerminating() );
}

void Simulation::ExeSimLockRelease( const mega::network::ConversationID& requestingConID,
                                    boost::asio::yield_context&          yield_ctx )
{
    SPDLOG_TRACE( "SIM: Simulation::ExeSimLockRelease: {}", requestingConID );
    Conversation::Ptr pRequestCon = m_executor.findExistingConversation( requestingConID );
    VERIFY_RTE( pRequestCon );
    network::exe_sim::Response_Encode response( *this, *pRequestCon, yield_ctx );
    response.ExeSimLockRelease();
}

} // namespace service
} // namespace mega