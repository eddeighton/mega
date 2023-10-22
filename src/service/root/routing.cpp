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

#include "mega/values/runtime/reference_io.hpp"

#include "root.hpp"

#include "service/network/log.hpp"

#include "common/unreachable.hpp"

namespace mega::service
{

RootRequestLogicalThread::RootRequestLogicalThread( Root& root, const network::LogicalThreadID& logicalthreadID )
    : InThreadLogicalThread( root, logicalthreadID )
    , m_root( root )
{
}
RootRequestLogicalThread::~RootRequestLogicalThread()
{
}

network::Message RootRequestLogicalThread::dispatchInBoundRequest( const network::Message&     msg,
                                                            boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::dispatchInBoundRequest {}", msg );
    network::Message result;
    if( result = network::daemon_root::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::mpo::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::project::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::enrole::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::status::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::report::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::stash::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::job::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::sim::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "RootRequestLogicalThread::dispatchInBoundRequest failed: " << msg );
    UNREACHABLE;
}

network::root_daemon::Request_Sender RootRequestLogicalThread::getDaemonSender( boost::asio::yield_context& yield_ctx )
{
    auto pOriginalRequestResponseSender = getOriginatingStackResponseSender();
    VERIFY_RTE( pOriginalRequestResponseSender );
    auto pConnection = m_root.m_server.getConnection( pOriginalRequestResponseSender );
    VERIFY_RTE( pConnection );
    network::root_daemon::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
    return sender;
}

network::Message RootRequestLogicalThread::broadcastAll( const network::Message&     msg,
                                                         boost::asio::yield_context& yield_ctx )
{
    // dispatch to children
    std::vector< network::Message > responses;
    {
        for( auto pConnection : m_root.m_server.getConnections() )
        {
            network::root_daemon::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
            const network::Message               response = sender.RootAllBroadcast( msg );
            responses.push_back( response );
        }
    }

    network::Message aggregateRequest = msg;
    network::aggregate( aggregateRequest, responses );

    // dispatch to this
    return dispatchInBoundRequest( aggregateRequest, yield_ctx );
}

network::Message RootRequestLogicalThread::broadcastExe( const network::Message&     msg,
                                                         boost::asio::yield_context& yield_ctx )
{
    // dispatch to children
    std::vector< network::Message > responses;
    {
        for( auto pConnection : m_root.m_server.getConnections() )
        {
            network::root_daemon::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
            const network::Message               response = sender.RootExeBroadcast( msg );
            responses.push_back( response );
        }
    }

    network::Message aggregateRequest = msg;
    network::aggregate( aggregateRequest, responses );

    // dispatch to this
    return dispatchInBoundRequest( aggregateRequest, yield_ctx );
}
// network::daemon_root::Impl
network::Message RootRequestLogicalThread::TermRoot( const network::Message&     request,
                                                     boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::TermRoot" );
    return dispatchInBoundRequest( request, yield_ctx );
}

network::Message RootRequestLogicalThread::ExeRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::ExeRoot" );
    return dispatchInBoundRequest( request, yield_ctx );
}

network::Message RootRequestLogicalThread::ToolRoot( const network::Message&     request,
                                                     boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::ToolRoot" );
    return dispatchInBoundRequest( request, yield_ctx );
}

network::Message RootRequestLogicalThread::PythonRoot( const network::Message&     request,
                                                       boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::PythonRoot" );
    return dispatchInBoundRequest( request, yield_ctx );
}

network::Message RootRequestLogicalThread::ReportRoot( const network::Message&     request,
                                                       boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::ReportRoot" );
    return dispatchInBoundRequest( request, yield_ctx );
}

network::Message RootRequestLogicalThread::LeafRoot( const network::Message&     request,
                                                     boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::LeafRoot" );
    return dispatchInBoundRequest( request, yield_ctx );
}

network::Message RootRequestLogicalThread::DaemonRoot( const network::Message&     request,
                                                       boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::DaemonRoot" );
    return dispatchInBoundRequest( request, yield_ctx );
}

network::Message RootRequestLogicalThread::MPRoot( const network::Message& request, const MP& mp,
                                                   boost::asio::yield_context& yield_ctx )
{
    return dispatchInBoundRequest( request, yield_ctx );
}
network::Message RootRequestLogicalThread::MPDown( const network::Message& request, const MP& mp,
                                                   boost::asio::yield_context& yield_ctx )
{
    if( network::Server::Connection::Ptr pCon = m_root.m_server.findConnection( mp.getMachineID() ) )
    {
        network::mpo::Request_Sender sender( *this, pCon->getSender(), yield_ctx );
        return sender.MPDown( request, mp );
    }
    else
    {
        using ::operator<<;
        THROW_RTE( "Failed to route to mp: " << mp );
    }
    UNREACHABLE;
}
network::Message RootRequestLogicalThread::MPUp( const network::Message& request, const MP& mp,
                                                 boost::asio::yield_context& yield_ctx )
{
    return MPDown( request, mp, yield_ctx );
}
network::Message RootRequestLogicalThread::MPODown( const network::Message&     request,
                                                    const MPO&                  mpo,
                                                    boost::asio::yield_context& yield_ctx )
{
    if( network::Server::Connection::Ptr pCon = m_root.m_server.findConnection( mpo.getMachineID() ) )
    {
        network::mpo::Request_Sender sender( *this, pCon->getSender(), yield_ctx );
        return sender.MPODown( request, mpo );
    }
    else
    {
        using ::operator<<;
        THROW_RTE( "Failed to route to mpo: " << mpo );
    }
    UNREACHABLE;
}
network::Message RootRequestLogicalThread::MPOUp( const network::Message& request, const MPO& mpo,
                                                  boost::asio::yield_context& yield_ctx )
{
    return MPODown( request, mpo, yield_ctx );
}

TimeStamp RootRequestLogicalThread::SimLockRead( const MPO& requestingMPO, const MPO& targetMPO,
                                                 boost::asio::yield_context& yield_ctx )
{
    if( network::Server::Connection::Ptr pConnection = m_root.m_server.findConnection( targetMPO.getMachineID() ) )
    {
        network::sim::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
        return sender.SimLockRead( requestingMPO, targetMPO );
    }
    else
    {
        using ::operator<<;
        THROW_RTE( "Failed to route to mpo: " << targetMPO );
    }
    UNREACHABLE;
}

TimeStamp RootRequestLogicalThread::SimLockWrite( const MPO& requestingMPO, const MPO& targetMPO,
                                                  boost::asio::yield_context& yield_ctx )
{
    if( network::Server::Connection::Ptr pConnection = m_root.m_server.findConnection( targetMPO.getMachineID() ) )
    {
        network::sim::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
        return sender.SimLockWrite( requestingMPO, targetMPO );
    }
    else
    {
        using ::operator<<;
        THROW_RTE( "Failed to route to mpo: " << targetMPO );
    }
    UNREACHABLE;
}

void RootRequestLogicalThread::SimLockRelease( const MPO&                  requestingMPO,
                                               const MPO&                  targetMPO,
                                               const network::Transaction& transaction,
                                               boost::asio::yield_context& yield_ctx )
{
    if( network::Server::Connection::Ptr pConnection = m_root.m_server.findConnection( targetMPO.getMachineID() ) )
    {
        network::sim::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
        return sender.SimLockRelease( requestingMPO, targetMPO, transaction );
    }
    else
    {
        using ::operator<<;
        THROW_RTE( "Failed to route to mpo: " << targetMPO );
    }
    UNREACHABLE;
}

} // namespace mega::service
