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

#include "service/network/log.hpp"
#include "service/protocol/model/memory.hxx"

namespace mega::service
{

LeafRequestLogicalThread::LeafRequestLogicalThread( Leaf& leaf, const network::LogicalThreadID& logicalthreadID )
    : InThreadLogicalThread( leaf, logicalthreadID )
    , m_leaf( leaf )
{
}
LeafRequestLogicalThread::~LeafRequestLogicalThread()
{
}

network::Message LeafRequestLogicalThread::dispatchInBoundRequest( const network::Message&     msg,
                                                           boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::memory::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::jit::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::term_leaf::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::daemon_leaf::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::exe_leaf::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::tool_leaf::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::python_leaf::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::mpo::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::status::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::job::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::project::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::enrole::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "LeafRequestLogicalThread::dispatchInBoundRequest failed on msg: " << msg );
}

network::leaf_daemon::Request_Sender LeafRequestLogicalThread::getDaemonSender( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_leaf.getDaemonSender(), yield_ctx };
}
network::leaf_exe::Request_Sender LeafRequestLogicalThread::getExeSender( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_leaf.getNodeChannelSender(), yield_ctx };
}
network::leaf_tool::Request_Sender LeafRequestLogicalThread::getToolSender( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_leaf.getNodeChannelSender(), yield_ctx };
}
network::leaf_python::Request_Sender LeafRequestLogicalThread::getPythonSender( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_leaf.getNodeChannelSender(), yield_ctx };
}
network::leaf_term::Request_Sender LeafRequestLogicalThread::getTermSender( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_leaf.getNodeChannelSender(), yield_ctx };
}
network::mpo::Request_Sender LeafRequestLogicalThread::getMPOUpSender( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_leaf.getDaemonSender(), yield_ctx };
}
network::mpo::Request_Sender LeafRequestLogicalThread::getMPODownSender( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_leaf.getNodeChannelSender(), yield_ctx };
}
// network::term_leaf::Impl
network::Message LeafRequestLogicalThread::TermRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestLogicalThread::TermRoot" );
    return getDaemonSender( yield_ctx ).TermRoot( request );
}

// network::exe_leaf::Impl
network::Message LeafRequestLogicalThread::ExeRoot( const network::Message&     request,
                                                   boost::asio::yield_context& yield_ctx )
{
    return getDaemonSender( yield_ctx ).ExeRoot( request );
}
network::Message LeafRequestLogicalThread::ExeDaemon( const network::Message&     request,
                                                     boost::asio::yield_context& yield_ctx )
{
    return getDaemonSender( yield_ctx ).ExeDaemon( request );
}

// network::tool_leaf::Impl
network::Message LeafRequestLogicalThread::ToolRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    return getDaemonSender( yield_ctx ).ToolRoot( request );
}

network::Message LeafRequestLogicalThread::ToolDaemon( const network::Message&     request,
                                                      boost::asio::yield_context& yield_ctx )
{
    return getDaemonSender( yield_ctx ).ToolDaemon( request );
}

// network::python_leaf::Impl
network::Message LeafRequestLogicalThread::PythonRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    return getDaemonSender( yield_ctx ).PythonRoot( request );
}

network::Message LeafRequestLogicalThread::PythonDaemon( const network::Message&     request,
                                                      boost::asio::yield_context& yield_ctx )
{
    return getDaemonSender( yield_ctx ).PythonDaemon( request );
}

// network::mpo::Impl
network::Message LeafRequestLogicalThread::MPRoot( const network::Message&     request, const mega::MP&,
                                                  boost::asio::yield_context& yield_ctx )
{
    // ignor the passed MP and use the leaf MP
    SPDLOG_TRACE( "LeafRequestLogicalThread::MPRoot: {}", m_leaf.m_mp );
    return getMPOUpSender( yield_ctx ).MPRoot( request, m_leaf.m_mp );
}
network::Message LeafRequestLogicalThread::MPDown( const network::Message& request, const mega::MP& mp,
                                                  boost::asio::yield_context& yield_ctx )
{
    switch( m_leaf.m_nodeType )
    {
        case network::Node::Executor:
        case network::Node::Tool:
        case network::Node::Python:
        case network::Node::Plugin:
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
network::Message LeafRequestLogicalThread::MPUp( const network::Message& request, const mega::MP& mp,
                                                boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestLogicalThread::MPUp: {} {}", mp, request.getName() );
    if( m_leaf.m_mp == mp )
    {
        return dispatchInBoundRequest( request, yield_ctx );
    }
    else
    {
        return getMPOUpSender( yield_ctx ).MPUp( request, mp );
    }
}

network::Message LeafRequestLogicalThread::MPODown( const network::Message& request, const mega::MPO& mpo,
                                                   boost::asio::yield_context& yield_ctx )
{
    ASSERT( m_leaf.m_mpos.count( mpo ) );
    switch( m_leaf.m_nodeType )
    {
        case network::Node::Executor:
        case network::Node::Tool:
        case network::Node::Python:
        case network::Node::Plugin:
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
network::Message LeafRequestLogicalThread::MPOUp( const network::Message& request, const mega::MPO& mpo,
                                                 boost::asio::yield_context& yield_ctx )
{
    if( m_leaf.m_mpos.count( mpo ) )
    {
        return MPODown( request, mpo, yield_ctx );
    }
    else
    {
        return getMPOUpSender( yield_ctx ).MPOUp( request, mpo );
    }
}

// network::daemon_leaf::Impl
network::Message LeafRequestLogicalThread::RootAllBroadcast( const network::Message&     request,
                                                            boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestLogicalThread::RootAllBroadcast" );
    std::vector< network::Message > responses;
    {
        switch( m_leaf.m_nodeType )
        {
            case network::Node::Executor:
            case network::Node::Plugin:
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
            case network::Node::Python:
            {
                responses.push_back( getPythonSender( yield_ctx ).RootAllBroadcast( request ) );
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
    return dispatchInBoundRequest( aggregateRequest, yield_ctx );
}

network::Message LeafRequestLogicalThread::RootExeBroadcast( const network::Message&     request,
                                                            boost::asio::yield_context& yield_ctx )
{
    switch( m_leaf.m_nodeType )
    {
        case network::Node::Plugin:
        case network::Node::Executor:
            return getExeSender( yield_ctx ).RootExeBroadcast( request );
        case network::Node::Terminal:
        case network::Node::Tool:
        case network::Node::Python:
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::Leaf:
        case network::Node::TOTAL_NODE_TYPES:
            THROW_RTE( "Unreachable" );
        default:
            THROW_RTE( "Unknown node type" );
    }
}
network::Message LeafRequestLogicalThread::RootExe( const network::Message&     request,
                                                   boost::asio::yield_context& yield_ctx )
{
    switch( m_leaf.m_nodeType )
    {
        case network::Node::Plugin:
        case network::Node::Executor:
            return getExeSender( yield_ctx ).RootExe( request );
        case network::Node::Terminal:
        case network::Node::Tool:
        case network::Node::Python:
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::Leaf:
        case network::Node::TOTAL_NODE_TYPES:
            THROW_RTE( "Unreachable" );
        default:
            THROW_RTE( "Unknown node type" );
    }
}

void LeafRequestLogicalThread::RootSimRun( const Project& project, const MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestLogicalThread::RootSimRun {}", mpo );
    VERIFY_RTE_MSG( m_leaf.m_pJIT.get(), "JIT not initialised in RootSimRun" );
    switch( m_leaf.m_nodeType )
    {
        case network::Node::Plugin:
        case network::Node::Executor:
        {
            MPOLifetime mpoLifetime( m_leaf, *this, mpo, yield_ctx );
            getExeSender( yield_ctx ).RootSimRun( project, mpo );
        }
        break;
        case network::Node::Tool:
        {
            MPOLifetime mpoLifetime( m_leaf, *this, mpo, yield_ctx );
            getToolSender( yield_ctx ).RootSimRun( project, mpo );
        }
        break;
        case network::Node::Python:
        {
            MPOLifetime mpoLifetime( m_leaf, *this, mpo, yield_ctx );
            getPythonSender( yield_ctx ).RootSimRun( project, mpo );
        }
        break;
        case network::Node::Terminal:
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::Leaf:
        case network::Node::TOTAL_NODE_TYPES:
            THROW_RTE( "Unreachable" );
        default:
            THROW_RTE( "Unknown node type" );
    }
    SPDLOG_TRACE( "LeafRequestLogicalThread::RootSimRun complete {}", mpo );
}

network::Message LeafRequestLogicalThread::DaemonLeafBroadcast( const network::Message&     request,
                                                               boost::asio::yield_context& yield_ctx )
{
    return dispatchInBoundRequest( request, yield_ctx );
}

} // namespace mega::service
