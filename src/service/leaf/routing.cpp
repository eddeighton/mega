
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
    if ( result = network::project::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "LeafRequestConversation::dispatchRequest failed on msg: " << msg );
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
network::leaf_exe::Request_Sender LeafRequestConversation::getExeSender( boost::asio::yield_context& yield_ctx )
{
    return network::leaf_exe::Request_Sender( *this, m_leaf.getNodeChannelSender(), yield_ctx );
}
network::leaf_tool::Request_Sender LeafRequestConversation::getToolSender( boost::asio::yield_context& yield_ctx )
{
    return network::leaf_tool::Request_Sender( *this, m_leaf.getNodeChannelSender(), yield_ctx );
}
network::leaf_term::Request_Sender LeafRequestConversation::getTermSender( boost::asio::yield_context& yield_ctx )
{
    return network::leaf_term::Request_Sender( *this, m_leaf.getNodeChannelSender(), yield_ctx );
}

// network::term_leaf::Impl
network::Message LeafRequestConversation::TermRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::TermRoot" );
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

// network::daemon_leaf::Impl
network::Message LeafRequestConversation::RootLeafBroadcast( const network::Message&     request,
                                                             boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::RootLeafBroadcast" );
    return dispatchRequest( request, yield_ctx );
}
network::Message LeafRequestConversation::DaemonLeafBroadcast( const network::Message&     request,
                                                               boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::DaemonLeafBroadcast" );
    return dispatchRequest( request, yield_ctx );
}

/*switch ( m_leaf.m_nodeType )
{
    case network::Node::Terminal:
        return getTermSender( yield_ctx ).RootAll( request );
    case network::Node::Tool:
        return getToolSender( yield_ctx ).RootAll( request );
    case network::Node::Executor:
        return getExeSender( yield_ctx ).RootAll( request );
    case network::Node::Daemon:
    case network::Node::Root:
    case network::Node::Leaf:
    case network::Node::TOTAL_NODE_TYPES:
        THROW_RTE( "Unreachable" );
    default:
        THROW_RTE( "Unknown node type" );
}*/

} // namespace service
} // namespace mega