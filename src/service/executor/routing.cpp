
#include "request.hpp"

#include "service/executor.hpp"

namespace mega
{
namespace service
{

ExecutorRequestConversation::ExecutorRequestConversation( Executor&                      executor,
                                                          const network::ConversationID& conversationID,
                                                          std::optional< network::ConnectionID >
                                                              originatingConnectionID )
    : ConcurrentConversation( executor, conversationID, originatingConnectionID )
    , m_executor( executor )
{
}

network::Message ExecutorRequestConversation::dispatchRequest( const network::Message&     msg,
                                                               boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if ( result = network::leaf_exe::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::exe_leaf::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::mpo_leaf::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::job::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::sim::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "ExecutorRequestConversation::dispatchRequest failed" );
}

void ExecutorRequestConversation::dispatchResponse( const network::ConnectionID& connectionID,
                                                    const network::Message&      msg,
                                                    boost::asio::yield_context&  yield_ctx )
{
    if ( ( m_executor.getLeafSender().getConnectionID() == connectionID ) )
    {
        m_executor.getLeafSender().send( getID(), msg, yield_ctx );
    }
    else if ( m_executor.m_receiverChannel.getSender()->getConnectionID() == connectionID )
    {
        m_executor.getLeafSender().send( getID(), msg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "ExecutorRequestConversation: Cannot resolve connection for response: {}", connectionID );
        THROW_RTE( "ExecutorRequestConversation: Executor Critical error in response handler: " << connectionID );
    }
}

void ExecutorRequestConversation::error( const network::ConnectionID& connectionID, const std::string& strErrorMsg,
                                         boost::asio::yield_context& yield_ctx )
{
    if ( ( m_executor.getLeafSender().getConnectionID() == connectionID )
         || ( m_executor.m_receiverChannel.getSender()->getConnectionID() == connectionID ) )
    {
        m_executor.getLeafSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
    }
    else if ( m_executor.m_receiverChannel.getSender()->getConnectionID() == connectionID )
    {
        m_executor.m_receiverChannel.getSender()->sendErrorResponse( getID(), strErrorMsg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "ExecutorRequestConversation: Cannot resolve connection in error handler: {} for error: {}",
                      connectionID, strErrorMsg );
        THROW_RTE( "ExecutorRequestConversation: Executor Critical error in error handler: " << connectionID << " : "
                                                                                             << strErrorMsg );
    }
}

network::exe_leaf::Request_Sender ExecutorRequestConversation::getLeafRequest( boost::asio::yield_context& yield_ctx )
{
    return network::exe_leaf::Request_Sender( *this, m_executor.getLeafSender(), yield_ctx );
}
network::mpo_leaf::Request_Sender ExecutorRequestConversation::getMPORequest( boost::asio::yield_context& yield_ctx )
{
    return network::mpo_leaf::Request_Sender( *this, m_executor.getLeafSender(), yield_ctx );
}

network::Message ExecutorRequestConversation::RootExeBroadcast( const network::Message&     request,
                                                                boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}
network::Message ExecutorRequestConversation::RootExe( const network::Message&     request,
                                                       boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

network::Message ExecutorRequestConversation::ExeRoot( const network::Message&     request,
                                                       boost::asio::yield_context& yield_ctx )
{
    return getLeafRequest( yield_ctx ).ExeRoot( request );
}

network::Message ExecutorRequestConversation::MPORoot( const network::Message& request,
                                                       boost::asio::yield_context& yield_ctx )
{
    return getMPORequest( yield_ctx ).MPORoot( request );
}

} // namespace service
} // namespace mega
