
#include "service/executor/simulation.hpp"

#include "service/executor.hpp"

#include "service/network/conversation.hpp"

#include "service/protocol/model/exe_leaf.hxx"
#include "service/protocol/model/exe_sim.hxx"
#include "service/protocol/model/messages.hxx"

#include <boost/asio/execution_context.hpp>
#include <boost/asio/this_coro.hpp>

extern mega::ExecutionContext* g_pExecutionContext;

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

std::string Simulation::mapBuffer( const mega::reference& reference )
{
    // getLeafRequest( *m_pYieldContext ).ExeGetProject()

    return "";
}

void Simulation::run( boost::asio::yield_context& yield_ctx )
{
    //ConversationBase::RequestStack stack( "Simulation testing", *this, getConnectionID() );

    SPDLOG_INFO( "Simulation Started: {}", getID() );

    // create the root
    // mega::ExecutionContext*                      pThis = this;
    m_pYieldContext     = &yield_ctx;
    g_pExecutionContext = this;
    std::unique_ptr< Root, void ( * )( Root* ) > pRoot(
        mega::runtime::allocateRoot( *g_pExecutionContext, getID() ),
        []( Root* pRoot ) { mega::runtime::releaseRoot( *g_pExecutionContext, pRoot ); } );

    try
    {
        do
        {
            // m_requestChannel.ready()

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
                case network::exe_sim::MSG_ExeSimReadLockRelease_Request::ID:
                {
                    requestingConversationID
                        = network::exe_sim::MSG_ExeSimReadLockRelease_Request::get( msg.msg ).simulationID;
                }
                break;
                case network::exe_sim::MSG_ExeSimWriteLockRelease_Request::ID:
                {
                    requestingConversationID
                        = network::exe_sim::MSG_ExeSimWriteLockRelease_Request::get( msg.msg ).simulationID;
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
    catch ( std::exception& ex )
    {
        SPDLOG_WARN( "Conversation: {} exception: {}", getID(), ex.what() );
        m_conversationManager.conversationCompleted( shared_from_this() );
    }
}

void Simulation::ExeSimReadLockAcquire( const mega::network::ConversationID& requestingConID,
                                        boost::asio::yield_context&          yield_ctx )
{
    SPDLOG_INFO( "Simulation::RootSimReadLock: {}", requestingConID );

    Conversation::Ptr pRequestCon = m_executor.findExistingConversation( requestingConID );
    VERIFY_RTE( pRequestCon );

    // THROW_RTE( "Test exception from Simulation::RootSimReadLock" );

    network::exe_sim::Response_Encode response( *this, *pRequestCon, yield_ctx );

    mega::TimeStamp timeStamp = 123;
    response.ExeSimReadLockAcquire( timeStamp );
}

void Simulation::ExeSimReadLockRelease( const mega::network::ConversationID& requestingConID,
                                        boost::asio::yield_context&          yield_ctx )
{
    SPDLOG_INFO( "Simulation::RootSimReadLock: {}", requestingConID );

    Conversation::Ptr pRequestCon = m_executor.findExistingConversation( requestingConID );
    VERIFY_RTE( pRequestCon );

    network::exe_sim::Response_Encode response( *this, *pRequestCon, yield_ctx );

    response.ExeSimReadLockRelease();
}

void Simulation::ExeSimWriteLockAcquire( const mega::network::ConversationID& requestingConID,
                                         boost::asio::yield_context&          yield_ctx )
{
    SPDLOG_INFO( "Simulation::RootSimReadLock: {}", requestingConID );

    Conversation::Ptr pRequestCon = m_executor.findExistingConversation( requestingConID );
    VERIFY_RTE( pRequestCon );

    // THROW_RTE( "Test exception from Simulation::RootSimReadLock" );

    network::exe_sim::Response_Encode response( *this, *pRequestCon, yield_ctx );

    mega::TimeStamp timeStamp = 123;
    response.ExeSimWriteLockAcquire( timeStamp );
}

void Simulation::ExeSimWriteLockRelease( const mega::network::ConversationID& requestingConID,
                                         boost::asio::yield_context&          yield_ctx )
{
    SPDLOG_INFO( "Simulation::RootSimReadLock: {}", requestingConID );

    Conversation::Ptr pRequestCon = m_executor.findExistingConversation( requestingConID );
    VERIFY_RTE( pRequestCon );

    network::exe_sim::Response_Encode response( *this, *pRequestCon, yield_ctx );

    response.ExeSimWriteLockRelease();
}

} // namespace service
} // namespace mega