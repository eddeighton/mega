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

#include "mega/reference_io.hpp"

#include "root.hpp"

#include "service/network/log.hpp"

#include "common/unreachable.hpp"

namespace mega::service
{

RootRequestLogicalThread::RootRequestLogicalThread( Root&                          root,
                                                  const network::LogicalThreadID& logicalthreadID,
                                                  const network::ConnectionID&   originatingConnectionID )
    : InThreadLogicalThread( root, logicalthreadID, originatingConnectionID )
    , m_root( root )
{
}

network::Message RootRequestLogicalThread::dispatchRequest( const network::Message&     msg,
                                                           boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::dispatchRequest {}", msg );
    network::Message result;
    if ( result = network::daemon_root::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::mpo::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::project::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::enrole::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::status::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::stash::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::job::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::sim::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "RootRequestLogicalThread::dispatchRequest failed: " << msg );
    UNREACHABLE;
}

void RootRequestLogicalThread::dispatchResponse( const network::ConnectionID& connectionID,
                                                const network::Message&      msg,
                                                boost::asio::yield_context&  yield_ctx )
{
    if ( network::Server::Connection::Ptr pHostConnection = m_root.m_server.getConnection( connectionID ) )
    {
        pHostConnection->send( msg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "Root cannot resolve connection: {} on response: {}", connectionID, msg );
    }
}

void RootRequestLogicalThread::error( const network::ReceivedMsg& msg,
                                     const std::string&          strErrorMsg,
                                     boost::asio::yield_context& yield_ctx )
{
    if ( network::Server::Connection::Ptr pHostConnection = m_root.m_server.getConnection( msg.connectionID ) )
    {
        pHostConnection->sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "Root cannot resolve connection: {} on error: {}", msg.connectionID, strErrorMsg );
    }
}

network::root_daemon::Request_Sender RootRequestLogicalThread::getDaemonSender( boost::asio::yield_context& yield_ctx )
{
    auto stackCon = getOriginatingEndPointID();
    VERIFY_RTE( stackCon.has_value() );
    auto pConnection = m_root.m_server.getConnection( stackCon.value() );
    VERIFY_RTE( pConnection );
    network::root_daemon::Request_Sender sender( *this, *pConnection, yield_ctx );
    return sender;
}

network::Message RootRequestLogicalThread::broadcastAll( const network::Message&     msg,
                                                        boost::asio::yield_context& yield_ctx )
{
    // dispatch to children
    std::vector< network::Message > responses;
    {
        for ( auto& [ id, pConnection ] : m_root.m_server.getConnections() )
        {
            network::root_daemon::Request_Sender sender( *this, *pConnection, yield_ctx );
            const network::Message               response = sender.RootAllBroadcast( msg );
            responses.push_back( response );
        }
    }

    network::Message aggregateRequest = msg;
    network::aggregate( aggregateRequest, responses );

    // dispatch to this
    return dispatchRequest( aggregateRequest, yield_ctx );
}

network::Message RootRequestLogicalThread::broadcastExe( const network::Message&     msg,
                                                        boost::asio::yield_context& yield_ctx )
{
    // dispatch to children
    std::vector< network::Message > responses;
    {
        for ( auto& [ id, pConnection ] : m_root.m_server.getConnections() )
        {
            network::root_daemon::Request_Sender sender( *this, *pConnection, yield_ctx );
            const network::Message               response = sender.RootExeBroadcast( msg );
            responses.push_back( response );
        }
    }

    network::Message aggregateRequest = msg;
    network::aggregate( aggregateRequest, responses );

    // dispatch to this
    return dispatchRequest( aggregateRequest, yield_ctx );
}
// network::daemon_root::Impl
network::Message RootRequestLogicalThread::TermRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::TermRoot" );
    return dispatchRequest( request, yield_ctx );
}

network::Message RootRequestLogicalThread::ExeRoot( const network::Message&     request,
                                                   boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::ExeRoot" );
    return dispatchRequest( request, yield_ctx );
}

network::Message RootRequestLogicalThread::ToolRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::ToolRoot" );
    return dispatchRequest( request, yield_ctx );
}

network::Message RootRequestLogicalThread::PythonRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::PythonRoot" );
    return dispatchRequest( request, yield_ctx );
}

network::Message RootRequestLogicalThread::LeafRoot( const network::Message&     request,
                                                    boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::LeafRoot" );
    return dispatchRequest( request, yield_ctx );
}

network::Message RootRequestLogicalThread::DaemonRoot( const network::Message&     request,
                                                      boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestLogicalThread::DaemonRoot" );
    return dispatchRequest( request, yield_ctx );
}

network::Message RootRequestLogicalThread::MPRoot( const network::Message& request, const MP& mp,
                                                  boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}
network::Message RootRequestLogicalThread::MPDown( const network::Message& request, const MP& mp,
                                                  boost::asio::yield_context& yield_ctx )
{
    if ( network::Server::Connection::Ptr pCon = m_root.m_server.findConnection( mp.getMachineID() ) )
    {
        network::mpo::Request_Sender sender( *this, *pCon, yield_ctx );
        return sender.MPDown( request, mp );
    }
    else
    {
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
    if ( network::Server::Connection::Ptr pCon = m_root.m_server.findConnection( mpo.getMachineID() ) )
    {
        network::mpo::Request_Sender sender( *this, *pCon, yield_ctx );
        return sender.MPODown( request, mpo );
    }
    else
    {
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
    if ( network::Server::Connection::Ptr pConnection = m_root.m_server.findConnection( targetMPO.getMachineID() ) )
    {
        network::sim::Request_Sender sender( *this, *pConnection, yield_ctx );
        return sender.SimLockRead( requestingMPO, targetMPO );
    }
    else
    {
        THROW_RTE( "Failed to route to mpo: " << targetMPO );
    }
    UNREACHABLE;
}

TimeStamp RootRequestLogicalThread::SimLockWrite( const MPO& requestingMPO, const MPO& targetMPO,
                                                boost::asio::yield_context& yield_ctx )
{
    if ( network::Server::Connection::Ptr pConnection = m_root.m_server.findConnection( targetMPO.getMachineID() ) )
    {
        network::sim::Request_Sender sender( *this, *pConnection, yield_ctx );
        return sender.SimLockWrite( requestingMPO, targetMPO );
    }
    else
    {
        THROW_RTE( "Failed to route to mpo: " << targetMPO );
    }
    UNREACHABLE;
}

void RootRequestLogicalThread::SimLockRelease( const MPO&                  requestingMPO,
                                              const MPO&                  targetMPO,
                                              const network::Transaction& transaction,
                                              boost::asio::yield_context& yield_ctx )
{
    if ( network::Server::Connection::Ptr pConnection = m_root.m_server.findConnection( targetMPO.getMachineID() ) )
    {
        network::sim::Request_Sender sender( *this, *pConnection, yield_ctx );
        return sender.SimLockRelease( requestingMPO, targetMPO, transaction );
    }
    else
    {
        THROW_RTE( "Failed to route to mpo: " << targetMPO );
    }
    UNREACHABLE;
}

} // namespace mega::service
