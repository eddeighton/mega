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

#include "service/protocol/model/memory.hxx"

#include "service/protocol/model/daemon_leaf.hxx"

#include "log/log.hpp"

namespace mega::service
{

// network::memory::Impl
void DaemonRequestLogicalThread::MPODestroyed( const MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    for( auto pCon : m_daemon.m_server.getConnections() )
    {
        network::memory::Request_Sender sender( *this, pCon->getSender(), yield_ctx );
        sender.MPODestroyed( mpo );
    }
}

void DaemonRequestLogicalThread::RootSimRun( const MPO& mpo,
                                             boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestLogicalThread::RootSimRun: {}", mpo );

    auto pOriginalRequestResponseSender = getOriginatingStackResponseSender();
    VERIFY_RTE( pOriginalRequestResponseSender );
    auto pConnection = m_daemon.m_server.getConnection( pOriginalRequestResponseSender );
    VERIFY_RTE( pConnection );
    VERIFY_RTE( pConnection->getTypeOpt().has_value() );
    VERIFY_RTE( pConnection->getTypeOpt().value().canAllocateObjects() );

    {
        // network::Server::ConnectionLabelRAII connectionLabel( m_daemon.m_server, mpo, pConnection );
        network::daemon_leaf::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
        sender.RootSimRun( mpo );
    }
}

TimeStamp DaemonRequestLogicalThread::SimLockRead( const MPO& requestingMPO, const MPO& targetMPO,
                                                   boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestLogicalThread::SimLockRead from: {} to: {}", requestingMPO, targetMPO );
    if( network::Server::Connection::Ptr pConnection = m_daemon.m_server.findConnection( MP( targetMPO ) ) )
    {
        ASSERT( m_daemon.m_machineID == targetMPO.getMachineID() );
        network::sim::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
        return sender.SimLockRead( requestingMPO, targetMPO );
    }
    else
    {
        ASSERT( m_daemon.m_machineID != targetMPO.getMachineID() );

        network::sim::Request_Sender sender( *this, m_daemon.m_rootClient.getSender(), yield_ctx );
        return sender.SimLockRead( requestingMPO, targetMPO );
    }
}

TimeStamp DaemonRequestLogicalThread::SimLockWrite( const MPO& requestingMPO, const MPO& targetMPO,
                                                    boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestLogicalThread::SimLockWrite from: {} to: {}", requestingMPO, targetMPO );
    if( network::Server::Connection::Ptr pConnection = m_daemon.m_server.findConnection( MP( targetMPO ) ) )
    {
        network::sim::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
        return sender.SimLockWrite( requestingMPO, targetMPO );
    }
    else
    {
        network::sim::Request_Sender sender( *this, m_daemon.m_rootClient.getSender(), yield_ctx );
        return sender.SimLockWrite( requestingMPO, targetMPO );
    }
}

void DaemonRequestLogicalThread::SimLockRelease( const MPO&                  requestingMPO,
                                                 const MPO&                  targetMPO,
                                                 const network::Transaction& transaction,
                                                 boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestLogicalThread::SimLockRelease from: {} to: {}", requestingMPO, targetMPO );
    if( network::Server::Connection::Ptr pConnection = m_daemon.m_server.findConnection( MP( targetMPO ) ) )
    {
        network::sim::Request_Sender sender( *this, pConnection->getSender(), yield_ctx );
        return sender.SimLockRelease( requestingMPO, targetMPO, transaction );
    }
    else
    {
        network::sim::Request_Sender sender( *this, m_daemon.m_rootClient.getSender(), yield_ctx );
        return sender.SimLockRelease( requestingMPO, targetMPO, transaction );
    }
}

} // namespace mega::service
