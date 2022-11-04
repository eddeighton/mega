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

#include "request.hpp"
#include "mpo_lifetime.hpp"

#include "mega/shared_memory_header.hpp"

#include "service/network/log.hpp"
#include "service/protocol/model/memory.hxx"

namespace mega::service
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
    if ( result = network::memory::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::sim::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::jit::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
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
    return { *this, m_leaf.getDaemonSender(), yield_ctx };
}
network::leaf_exe::Request_Sender LeafRequestConversation::getExeSender( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_leaf.getNodeChannelSender(), yield_ctx };
}
network::leaf_tool::Request_Sender LeafRequestConversation::getToolSender( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_leaf.getNodeChannelSender(), yield_ctx };
}
network::leaf_term::Request_Sender LeafRequestConversation::getTermSender( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_leaf.getNodeChannelSender(), yield_ctx };
}
network::mpo::Request_Sender LeafRequestConversation::getMPOUpSender( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_leaf.getDaemonSender(), yield_ctx };
}
network::mpo::Request_Sender LeafRequestConversation::getMPODownSender( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_leaf.getNodeChannelSender(), yield_ctx };
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

network::Message LeafRequestConversation::ToolDaemon( const network::Message&     request,
                                                      boost::asio::yield_context& yield_ctx )
{
    return getDaemonSender( yield_ctx ).ToolDaemon( request );
}

// network::mpo::Impl
network::Message LeafRequestConversation::MPRoot( const network::Message&     request, const mega::MP&,
                                                  boost::asio::yield_context& yield_ctx )
{
    // ignor the passed MP and use the leaf MP
    SPDLOG_TRACE( "LeafRequestConversation::MPRoot: {}", m_leaf.m_mp );
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
    SPDLOG_TRACE( "LeafRequestConversation::MPUp: {} {}", mp, request.getName() );
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
network::Message LeafRequestConversation::RootAllBroadcast( const network::Message&     request,
                                                            boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::RootAllBroadcast" );
    std::vector< network::Message > responses;
    {
        switch ( m_leaf.m_nodeType )
        {
            case network::Node::Executor:
            {
                responses.push_back( getExeSender( yield_ctx ).RootAllBroadcast( request ) );
            }
            break;
            case network::Node::Terminal:
            {
                responses.push_back( getTermSender( yield_ctx ).RootAllBroadcast( request ) );
            }
            break;
            case network::Node::Tool:
            {
                responses.push_back( getToolSender( yield_ctx ).RootAllBroadcast( request ) );
            }
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

    network::Message aggregateRequest = std::move( request );
    network::aggregate( aggregateRequest, responses );

    // dispatch to this
    return dispatchRequest( aggregateRequest, yield_ctx );
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


void LeafRequestConversation::RootSimRun( const reference& root, const std::string& strMemory,
                                          boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::RootSimRun {} {}", root, strMemory );
    VERIFY_RTE_MSG( m_leaf.m_pJIT.get(), "JIT not initialised in RootSimRun" );
    switch ( m_leaf.m_nodeType )
    {
        case network::Node::Executor:
        {
            MPOLifetime mpoLifetime( m_leaf, *this, root, strMemory, yield_ctx );
            return getExeSender( yield_ctx ).RootSimRun( mpoLifetime.getRoot(), network::convert( &mpoLifetime.getSharedMemory() ) );
        }
        case network::Node::Tool:
        {
            MPOLifetime mpoLifetime( m_leaf, *this, root, strMemory, yield_ctx );
            return getToolSender( yield_ctx ).RootSimRun( mpoLifetime.getRoot(), network::convert( &mpoLifetime.getSharedMemory() ) );
        }
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

network::Message LeafRequestConversation::DaemonLeafBroadcast( const network::Message&     request,
                                                               boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

} // namespace mega::service
