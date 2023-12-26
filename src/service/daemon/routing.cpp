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

#include "service/protocol/model/daemon_leaf.hxx"

#include "log/log.hpp"

namespace mega::service
{

DaemonRequestLogicalThread::DaemonRequestLogicalThread( Daemon&                         daemon,
                                                        const network::LogicalThreadID& logicalthreadID )
    : InThreadLogicalThread( daemon, logicalthreadID )
    , m_daemon( daemon )
{
}
DaemonRequestLogicalThread::~DaemonRequestLogicalThread()
{
}

network::Message DaemonRequestLogicalThread::dispatchInBoundRequest( const network::Message&     msg,
                                                              boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::mpo::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::leaf_daemon::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::root_daemon::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::enrole::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::status::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::report::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::job::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::memory::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::project::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::sim::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "DaemonRequestLogicalThread::dispatchInBoundRequest failed: " << msg.getName() );
}

network::Message DaemonRequestLogicalThread::TermRoot( const network::Message&     request,
                                                       boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestLogicalThread::TermRoot" );
    return getRootSender( yield_ctx ).TermRoot( request );
}

network::Message DaemonRequestLogicalThread::ExeRoot( const network::Message&     request,
                                                      boost::asio::yield_context& yield_ctx )
{
    return getRootSender( yield_ctx ).ExeRoot( request );
}

network::Message DaemonRequestLogicalThread::ToolRoot( const network::Message&     request,
                                                       boost::asio::yield_context& yield_ctx )
{
    return getRootSender( yield_ctx ).ToolRoot( request );
}

network::Message DaemonRequestLogicalThread::ToolDaemon( const network::Message&     request,
                                                         boost::asio::yield_context& yield_ctx )
{
    return dispatchInBoundRequest( request, yield_ctx );
}

network::Message DaemonRequestLogicalThread::PythonRoot( const network::Message&     request,
                                                         boost::asio::yield_context& yield_ctx )
{
    return getRootSender( yield_ctx ).PythonRoot( request );
}

network::Message DaemonRequestLogicalThread::ReportRoot( const network::Message&     request,
                                                         boost::asio::yield_context& yield_ctx )
{
    return getRootSender( yield_ctx ).ReportRoot( request );
}

network::Message DaemonRequestLogicalThread::PythonDaemon( const network::Message&     request,
                                                           boost::asio::yield_context& yield_ctx )
{
    return dispatchInBoundRequest( request, yield_ctx );
}

network::Message DaemonRequestLogicalThread::ReportDaemon( const network::Message&     request,
                                                           boost::asio::yield_context& yield_ctx )
{
    return dispatchInBoundRequest( request, yield_ctx );
}

network::Message DaemonRequestLogicalThread::LeafRoot( const network::Message&     request,
                                                       boost::asio::yield_context& yield_ctx )
{
    return getRootSender( yield_ctx ).LeafRoot( request );
}

network::Message DaemonRequestLogicalThread::LeafDaemon( const network::Message&     request,
                                                         boost::asio::yield_context& yield_ctx )
{
    return dispatchInBoundRequest( request, yield_ctx );
}

network::Message DaemonRequestLogicalThread::ExeDaemon( const network::Message&     request,
                                                        boost::asio::yield_context& yield_ctx )
{
    return dispatchInBoundRequest( request, yield_ctx );
}

// network::root_daemon::Impl
network::Message DaemonRequestLogicalThread::RootAllBroadcast( const network::Message&     request,
                                                               boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestLogicalThread::RootAllBroadcast" );
    // dispatch to children
    std::vector< network::Message > responses;
    {
        for( auto pConnection : m_daemon.m_server.getConnections() )
        {
            network::daemon_leaf::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
            const network::Message               response = sender.RootAllBroadcast( request );
            responses.push_back( response );
        }
    }

    network::Message aggregateRequest = request;
    network::aggregate( aggregateRequest, responses );

    // dispatch to this
    return dispatchInBoundRequest( aggregateRequest, yield_ctx );
}

network::Message DaemonRequestLogicalThread::RootExeBroadcast( const network::Message&     request,
                                                               boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestLogicalThread::RootExeBroadcast" );
    // dispatch to children
    std::vector< network::Message > responses;
    {
        for( auto pConnection : m_daemon.m_server.getConnections() )
        {
            if( pConnection->getTypeOpt().value() == network::Node::Executor
                || pConnection->getTypeOpt().value() == network::Node::Plugin )
            {
                network::daemon_leaf::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
                const network::Message               response = sender.RootExeBroadcast( request );
                responses.push_back( response );
            }
        }
    }

    network::Message aggregateRequest = request;
    network::aggregate( aggregateRequest, responses );

    // dispatch to this
    return dispatchInBoundRequest( aggregateRequest, yield_ctx );
}

network::Message DaemonRequestLogicalThread::RootExe( const network::Message&     request,
                                                      boost::asio::yield_context& yield_ctx )
{
    auto pOriginalRequestResponseSender = getOriginatingStackResponseSender();
    VERIFY_RTE( pOriginalRequestResponseSender );
    auto pConnection = m_daemon.m_server.getConnection( pOriginalRequestResponseSender );
    VERIFY_RTE( pConnection );
    VERIFY_RTE( pConnection->getTypeOpt().value() == network::Node::Executor
                || pConnection->getTypeOpt().value() == network::Node::Plugin );
    network::daemon_leaf::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
    return sender.RootExe( request );
}

// network::mpo::Impl
network::Message DaemonRequestLogicalThread::MPRoot( const network::Message&     request,
                                                     const MP&                   mp,
                                                     boost::asio::yield_context& yield_ctx )
{
    network::mpo::Request_Sender sender( *this, m_daemon.m_rootClient.getSender(), yield_ctx );
    return sender.MPRoot( request, mp );
}

network::Message DaemonRequestLogicalThread::MPDown( const network::Message&     request,
                                                     const MP&                   mp,
                                                     boost::asio::yield_context& yield_ctx )
{
    network::Server::Connection::Ptr pConnection = m_daemon.m_server.findConnection( mp );
    VERIFY_RTE_MSG( pConnection, "Failed to locate connection for mp: " << mp );
    network::mpo::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
    return sender.MPDown( request, mp );
}

network::Message DaemonRequestLogicalThread::MPUp( const network::Message& request, const MP& mp,
                                                   boost::asio::yield_context& yield_ctx )
{
    if( network::Server::Connection::Ptr pConnection = m_daemon.m_server.findConnection( mp ) )
    {
        network::mpo::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
        return sender.MPDown( request, mp );
    }
    else
    {
        network::mpo::Request_Sender sender( *this, m_daemon.m_rootClient.getSender(), yield_ctx );
        return sender.MPUp( request, mp );
    }
}

network::Message DaemonRequestLogicalThread::MPODown( const network::Message&     request,
                                                      const MPO&                  mpo,
                                                      boost::asio::yield_context& yield_ctx )
{
    if( network::Server::Connection::Ptr pConnection = m_daemon.m_server.findConnection( MP( mpo ) ) )
    {
        network::mpo::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
        return sender.MPODown( request, mpo );
    }
    else
    {
        THROW_RTE( "Routing error cannot locate mpo: " << mpo );
        return network::Message{};
    }
}
network::Message DaemonRequestLogicalThread::MPOUp( const network::Message&     request,
                                                    const MPO&                  mpo,
                                                    boost::asio::yield_context& yield_ctx )
{
    if( network::Server::Connection::Ptr pConnection = m_daemon.m_server.findConnection( MP( mpo ) ) )
    {
        network::mpo::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
        return sender.MPODown( request, mpo );
    }
    else
    {
        network::mpo::Request_Sender sender( *this, m_daemon.m_rootClient.getSender(), yield_ctx );
        return sender.MPOUp( request, mpo );
    }
}

} // namespace mega::service
