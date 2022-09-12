
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
        SPDLOG_ERROR(
            "Simulation: Cannot resolve connection in error handler: {} for error: {}", connectionID, strErrorMsg );
        THROW_RTE( "Simulation: Executor Critical error in error handler: " << connectionID << " : " << strErrorMsg );
    }
}

// mega::ExecutionContext
ExecutionIndex  Simulation::getThisExecutionIndex() { return m_executionRoot->index(); }
mega::reference Simulation::getRoot() { return m_executionRoot->root(); }
std::string     Simulation::acquireMemory( ExecutionIndex executionIndex )
{
    VERIFY_RTE( m_pYieldContext );
    return getLeafRequest( *m_pYieldContext ).ExeAcquireMemory( executionIndex );
}
LogicalAddress Simulation::allocateLogical( ExecutionIndex executionIndex, TypeID objectTypeID )
{
    VERIFY_RTE( m_pYieldContext );
    return LogicalAddress{ getLeafRequest( *m_pYieldContext ).ExeAllocateLogical( executionIndex, objectTypeID ) };
}

void Simulation::deAllocateLogical( ExecutionIndex executionIndex, LogicalAddress logicalAddress )
{
    VERIFY_RTE( m_pYieldContext );
    getLeafRequest( *m_pYieldContext ).ExeDeAllocateLogical( executionIndex, Address{ logicalAddress } );
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

void Simulation::readLock( ExecutionIndex executionIndex )
{
    VERIFY_RTE( m_pYieldContext );

    //if( m_executor.m_simulations

    const network::ConversationID id = getLeafRequest( *m_pYieldContext ).ExeGetExecutionContextID( executionIndex );
    getLeafRequest( *m_pYieldContext ).ExeSimReadLock( id );
}

void Simulation::writeLock( ExecutionIndex executionIndex )
{
    VERIFY_RTE( m_pYieldContext );
    const network::ConversationID id = getLeafRequest( *m_pYieldContext ).ExeGetExecutionContextID( executionIndex );
    getLeafRequest( *m_pYieldContext ).ExeSimWriteLock( id );
}
void Simulation::releaseLock( ExecutionIndex executionIndex )
{
    VERIFY_RTE( m_pYieldContext );
    const network::ConversationID id = getLeafRequest( *m_pYieldContext ).ExeGetExecutionContextID( executionIndex );
    getLeafRequest( *m_pYieldContext ).ExeSimReleaseLock( id );
}

void Simulation::runSimulation( boost::asio::yield_context& yield_ctx )
{
    try
    {
        const std::pair< bool, mega::ExecutionIndex > result = getLeafRequest( yield_ctx ).ExeCreateExecutionContext();
        VERIFY_RTE_MSG( result.first, "Failed to acquire execution index" );
        m_executionRoot = std::move( mega::runtime::ExecutionRoot( result.second ) );

        // process incoming requests
        SUSPEND_EXECUTION_CONTEXT();
        {
            do
            {
                const network::ChannelMsg msg = m_requestChannel.async_receive( yield_ctx );

                std::optional< mega::network::ConversationID > requestingConversationID;

                switch ( network::getMsgID( msg.msg ) )
                {
                    case network::exe_sim::MSG_ExeSimReadLockAcquire_Request::ID:
                    {
                        requestingConversationID
                            = network::exe_sim::MSG_ExeSimReadLockAcquire_Request::get( msg.msg ).simulationID;
                    }
                    break;
                    case network::exe_sim::MSG_ExeSimWriteLockAcquire_Request::ID:
                    {
                        requestingConversationID
                            = network::exe_sim::MSG_ExeSimWriteLockAcquire_Request::get( msg.msg ).simulationID;
                    }
                    break;
                    case network::exe_sim::MSG_ExeSimLockRelease_Request::ID:
                    {
                        requestingConversationID
                            = network::exe_sim::MSG_ExeSimLockRelease_Request::get( msg.msg ).simulationID;
                    }
                    break;
                    default:
                        break;
                }

                ConversationBase::RequestStack stack( getMsgName( msg.msg ), *this, getConnectionID() );
                try
                {
                    ASSERT( isRequest( msg.msg ) );
                    if ( !dispatchRequest( msg.msg, yield_ctx ) )
                    {
                        SPDLOG_ERROR( "Failed to dispatch request: {} on conversation: {}", msg.msg, getID() );
                        THROW_RTE( "Failed to dispatch request message: " << msg.msg );
                    }
                }
                catch ( std::exception& ex )
                {
                    if ( requestingConversationID.has_value() )
                    {
                        Conversation::Ptr pRequestCon
                            = m_executor.findExistingConversation( requestingConversationID.value() );
                        pRequestCon->sendErrorResponse( getID(), ex.what(), yield_ctx );
                    }
                    else
                    {
                        error( m_stack.back(), ex.what(), yield_ctx );
                    }
                }
            } while ( !m_stack.empty() );
        }
        RESUME_EXECUTION_CONTEXT();
    }
    catch ( std::exception& ex )
    {
        SPDLOG_WARN( "Conversation: {} exception: {}", getID(), ex.what() );
        m_conversationManager.conversationCompleted( shared_from_this() );
    }
}

void Simulation::run( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "Simulation Started: {}", getID() );

    ExecutionContext::resume( this );

    m_pYieldContext = &yield_ctx;

    runSimulation( yield_ctx );

    ExecutionContext::suspend();
}

void Simulation::ExeSimReadLockAcquire( const mega::network::ConversationID& requestingConID,
                                        boost::asio::yield_context&          yield_ctx )
{
    SPDLOG_TRACE( "Simulation::RootSimReadLock: {}", requestingConID );

    Conversation::Ptr pRequestCon = m_executor.findExistingConversation( requestingConID );
    VERIFY_RTE( pRequestCon );

    // THROW_RTE( "Test exception from Simulation::RootSimReadLock" );

    network::exe_sim::Response_Encode response( *this, *pRequestCon, yield_ctx );

    mega::TimeStamp timeStamp = 123;
    response.ExeSimReadLockAcquire( timeStamp );
}

void Simulation::ExeSimWriteLockAcquire( const mega::network::ConversationID& requestingConID,
                                         boost::asio::yield_context&          yield_ctx )
{
    SPDLOG_TRACE( "Simulation::RootSimReadLock: {}", requestingConID );

    Conversation::Ptr pRequestCon = m_executor.findExistingConversation( requestingConID );
    VERIFY_RTE( pRequestCon );

    // THROW_RTE( "Test exception from Simulation::RootSimReadLock" );

    network::exe_sim::Response_Encode response( *this, *pRequestCon, yield_ctx );

    mega::TimeStamp timeStamp = 123;
    response.ExeSimWriteLockAcquire( timeStamp );
}

void Simulation::ExeSimLockRelease( const mega::network::ConversationID& requestingConID,
                                    boost::asio::yield_context&          yield_ctx )
{
    SPDLOG_TRACE( "Simulation::RootSimReadLock: {}", requestingConID );

    Conversation::Ptr pRequestCon = m_executor.findExistingConversation( requestingConID );
    VERIFY_RTE( pRequestCon );

    network::exe_sim::Response_Encode response( *this, *pRequestCon, yield_ctx );
    response.ExeSimLockRelease();
}

} // namespace service
} // namespace mega