
#include "request.hpp"

#include "service/network/log.hpp"

#include "mega/reference_io.hpp"

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
    if ( result = network::mpo::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::status::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::job::Impl::dispatchRequest( msg, yield_ctx ); result )
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
        m_leaf.getNodeChannelSender().send( msg, yield_ctx );
    }
    else if ( m_leaf.getDaemonSender().getConnectionID() == connectionID )
    {
        m_leaf.getDaemonSender().send( msg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "Leaf cannot resolve connection: {} on response: {}", connectionID, msg );
    }
}

void LeafRequestConversation::error( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                     boost::asio::yield_context& yield_ctx )
{
    if ( ( m_leaf.getNodeChannelSender().getConnectionID() == msg.connectionID )
         || ( m_leaf.m_pSelfSender->getConnectionID() == msg.connectionID ) )
    {
        m_leaf.getNodeChannelSender().sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else if ( m_leaf.getDaemonSender().getConnectionID() == msg.connectionID )
    {
        m_leaf.getDaemonSender().sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "Leaf cannot resolve connection: {} on error: {}", msg.connectionID, strErrorMsg );
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
network::mpo::Request_Sender LeafRequestConversation::getMPOUpSender( boost::asio::yield_context& yield_ctx )
{
    return network::mpo::Request_Sender( *this, m_leaf.getDaemonSender(), yield_ctx );
}
network::mpo::Request_Sender LeafRequestConversation::getMPODownSender( boost::asio::yield_context& yield_ctx )
{
    return network::mpo::Request_Sender( *this, m_leaf.getNodeChannelSender(), yield_ctx );
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
network::Message LeafRequestConversation::ExeDaemon( const network::Message&     request,
                                                     boost::asio::yield_context& yield_ctx )
{
    return getDaemonSender( yield_ctx ).ExeDaemon( request );
}

// network::tool_leaf::Impl
network::Message LeafRequestConversation::ToolRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    return getDaemonSender( yield_ctx ).ToolRoot( request );
}

// network::mpo::Impl
network::Message LeafRequestConversation::MPRoot( const network::Message&     request, const mega::MP&,
                                                  boost::asio::yield_context& yield_ctx )
{
    // ignor the passed MP and use the leaf MP
    return getMPOUpSender( yield_ctx ).MPRoot( request, m_leaf.m_mp );
}
network::Message LeafRequestConversation::MPDown( const network::Message& request, const mega::MP& mp,
                                                  boost::asio::yield_context& yield_ctx )
{
    switch ( m_leaf.m_nodeType )
    {
        case network::Node::Executor:
            return getMPODownSender( yield_ctx ).MPDown( request, mp );
        case network::Node::Tool:
            return getMPODownSender( yield_ctx ).MPDown( request, mp );
        case network::Node::Terminal:
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::Leaf:
        case network::Node::TOTAL_NODE_TYPES:
            THROW_RTE( "Unreachable" );
        default:
            THROW_RTE( "Unknown node type" );
    }
}
network::Message LeafRequestConversation::MPUp( const network::Message& request, const mega::MP& mp,
                                                boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::MPUp: {} {}", mp, network::getMsgName( request ) );
    if ( m_leaf.m_mp == mp )
    {
        return dispatchRequest( request, yield_ctx );
    }
    else
    {
        return getMPOUpSender( yield_ctx ).MPUp( request, mp );
    }
}

network::Message LeafRequestConversation::MPODown( const network::Message& request, const mega::MPO& mpo,
                                                   boost::asio::yield_context& yield_ctx )
{
    ASSERT( m_leaf.m_mpos.count( mpo ) );
    switch ( m_leaf.m_nodeType )
    {
        case network::Node::Executor:
        case network::Node::Tool:
            return getMPODownSender( yield_ctx ).MPODown( request, mpo );
        case network::Node::Terminal:
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::Leaf:
        case network::Node::TOTAL_NODE_TYPES:
            THROW_RTE( "Unreachable" );
        default:
            THROW_RTE( "Unknown node type" );
    }
}
network::Message LeafRequestConversation::MPOUp( const network::Message& request, const mega::MPO& mpo,
                                                 boost::asio::yield_context& yield_ctx )
{
    if ( m_leaf.m_mpos.count( mpo ) )
    {
        return MPODown( request, mpo, yield_ctx );
    }
    else
    {
        return getMPOUpSender( yield_ctx ).MPOUp( request, mpo );
    }
}

// network::daemon_leaf::Impl
network::Message LeafRequestConversation::RootLeafBroadcast( const network::Message&     request,
                                                             boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );

    /*SPDLOG_TRACE( "LeafRequestConversation::RootLeafBroadcast" );
    // dispatch to children
    std::vector< network::Message > responses;
    {
        switch ( m_leaf.m_nodeType )
        {
            case network::Node::Executor:
                const network::Message response = getExeSender( yield_ctx ).RootLeafBroadcast( request );
                responses.push_back( response );
                break;
            case network::Node::Terminal:
                const network::Message response = getTermSender( yield_ctx ).RootLeafBroadcast( request );
                responses.push_back( response );
                break;
            case network::Node::Tool:
                const network::Message response = getToolSender( yield_ctx ).RootLeafBroadcast( request );
                responses.push_back( response );
                break;
            case network::Node::Daemon:
            case network::Node::Root:
            case network::Node::Leaf:
            case network::Node::TOTAL_NODE_TYPES:
                THROW_RTE( "Unreachable" );
            default:
                THROW_RTE( "Unknown node type" );
        }
    }

    network::Message aggregateRequest = request;
    network::aggregate( aggregateRequest, responses );

    // dispatch to this
    return dispatchRequest( aggregateRequest, yield_ctx );*/
}

network::Message LeafRequestConversation::RootExeBroadcast( const network::Message&     request,
                                                            boost::asio::yield_context& yield_ctx )
{
    switch ( m_leaf.m_nodeType )
    {
        case network::Node::Executor:
            return getExeSender( yield_ctx ).RootExeBroadcast( request );
        case network::Node::Terminal:
        case network::Node::Tool:
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::Leaf:
        case network::Node::TOTAL_NODE_TYPES:
            THROW_RTE( "Unreachable" );
        default:
            THROW_RTE( "Unknown node type" );
    }
}
network::Message LeafRequestConversation::RootExe( const network::Message&     request,
                                                   boost::asio::yield_context& yield_ctx )
{
    switch ( m_leaf.m_nodeType )
    {
        case network::Node::Executor:
            return getExeSender( yield_ctx ).RootExe( request );
        case network::Node::Terminal:
        case network::Node::Tool:
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::Leaf:
        case network::Node::TOTAL_NODE_TYPES:
            THROW_RTE( "Unreachable" );
        default:
            THROW_RTE( "Unknown node type" );
    }
}
void LeafRequestConversation::RootSimRun( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::RootSimRun {}", mpo );
    switch ( m_leaf.m_nodeType )
    {
        case network::Node::Executor:
        {
            struct MPOEntry
            {
                std::set< mega::MPO >& mpos;
                const mega::MPO&       mpo;
                MPOEntry( std::set< mega::MPO >& mpos, const mega::MPO& mpo )
                    : mpos( mpos )
                    , mpo( mpo )
                {
                    mpos.insert( mpo );
                }
                ~MPOEntry() { mpos.erase( mpo ); }
            } mpoEntry( m_leaf.m_mpos, mpo );
            return getExeSender( yield_ctx ).RootSimRun( mpo );
        }
        case network::Node::Terminal:
        case network::Node::Tool:
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::Leaf:
        case network::Node::TOTAL_NODE_TYPES:
            THROW_RTE( "Unreachable" );
        default:
            THROW_RTE( "Unknown node type" );
    }
}

network::Message LeafRequestConversation::DaemonLeafBroadcast( const network::Message&     request,
                                                               boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

} // namespace service
} // namespace mega