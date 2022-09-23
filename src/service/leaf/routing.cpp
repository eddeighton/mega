
#include "request.hpp"

#include "service/network/log.hpp"

namespace mega
{
namespace service
{

LeafRequestConversation::LeafRequestConversation( Leaf& leaf, const network::ConversationID& conversationID,
                                                  const network::ConnectionID& originatingConnectionID )
    : InThreadConversation( leaf, conversationID, originatingConnectionID )
    , m_leaf( leaf )
{
}

network::Message LeafRequestConversation::dispatchRequest( const network::Message&     msg,
                                                           boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if ( result = network::term_leaf::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::daemon_leaf::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::exe_leaf::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::tool_leaf::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    return result;
}

void LeafRequestConversation::dispatchResponse( const network::ConnectionID& connectionID,
                                                const network::Message&      msg,
                                                boost::asio::yield_context&  yield_ctx )
{
    if ( ( m_leaf.getNodeChannelSender().getConnectionID() == connectionID )
         || ( m_leaf.m_pSelfSender->getConnectionID() == connectionID ) )
    {
        m_leaf.getNodeChannelSender().send( getID(), msg, yield_ctx );
    }
    else if ( m_leaf.getDaemonSender().getConnectionID() == connectionID )
    {
        m_leaf.getDaemonSender().send( getID(), msg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "Leaf cannot resolve connection: {} on response: {}", connectionID, msg );
    }
}

void LeafRequestConversation::error( const network::ConnectionID& connection, const std::string& strErrorMsg,
                                     boost::asio::yield_context& yield_ctx )
{
    if ( ( m_leaf.getNodeChannelSender().getConnectionID() == connection )
         || ( m_leaf.m_pSelfSender->getConnectionID() == connection ) )
    {
        m_leaf.getNodeChannelSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
    }
    else if ( m_leaf.getDaemonSender().getConnectionID() == connection )
    {
        m_leaf.getDaemonSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "Leaf cannot resolve connection: {} on error: {}", connection, strErrorMsg );
    }
}

network::leaf_daemon::Request_Sender LeafRequestConversation::getDaemonSender( boost::asio::yield_context& yield_ctx )
{
    return network::leaf_daemon::Request_Sender( *this, m_leaf.getDaemonSender(), yield_ctx );
}

// network::term_leaf::Impl
network::Message LeafRequestConversation::TermRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    return getDaemonSender( yield_ctx ).TermRoot( request );
}

// network::exe_leaf::Impl
network::Message LeafRequestConversation::ExeRoot( const network::Message&     request,
                                                   boost::asio::yield_context& yield_ctx )
{
    return getDaemonSender( yield_ctx ).ExeRoot( request );
}

// network::tool_leaf::Impl
network::Message LeafRequestConversation::ToolRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    return getDaemonSender( yield_ctx ).ToolRoot( request );
}

} // namespace service
} // namespace mega