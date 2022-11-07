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
#include "service/protocol/model/address.hxx"

#include "service/protocol/model/daemon_leaf.hxx"

namespace mega::service
{

// network::memory::Impl
void DaemonRequestConversation::MPODestroyed( const MPO& mpo, const bool& bDeleteShared,
                                              boost::asio::yield_context& yield_ctx )
{
    bool bFirst = true;
    for( auto& [ id, pCon ] : m_daemon.m_server.getConnections() )
    {
        network::memory::Request_Sender sender( *this, *pCon, yield_ctx );
        sender.MPODestroyed( mpo, bFirst );
        bFirst = false;
    }

    m_daemon.getMemoryManager().release( mpo );
}

reference DaemonRequestConversation::Allocate( const MPO& mpo, const TypeID& objectTypeID,
                                               boost::asio::yield_context& yield_ctx )
{
    const NetworkAddress networkAddress
        = getRootRequest< network::address::Request_Encoder >( yield_ctx ).AllocateNetworkAddress( mpo, objectTypeID );
    return m_daemon.getMemoryManager().allocate( mpo, objectTypeID, networkAddress );
}

std::string DaemonRequestConversation::Acquire( const MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    return m_daemon.getMemoryManager().acquire( mpo );
}

reference DaemonRequestConversation::NetworkToMachine( const reference& ref, boost::asio::yield_context& yield_ctx )
{
    return m_daemon.getMemoryManager().networkToMachine( ref );
}

void DaemonRequestConversation::RootSimRun( const MPO&                  mpo,
                                            const NetworkAddress&       networkAddress,
                                            boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestConversation::RootSimRun: {}", mpo );

    auto stackCon = getOriginatingEndPointID();
    VERIFY_RTE( stackCon.has_value() );
    auto pConnection = m_daemon.m_server.getConnection( stackCon.value() );
    VERIFY_RTE( pConnection );
    VERIFY_RTE( pConnection->getTypeOpt().value() == network::Node::Executor
                || pConnection->getTypeOpt().value() == network::Node::Tool );

    // establish shared memory region for MPO
    struct Memory
    {
        Daemon&        daemon;
        std::string    strMemory;
        MPO            mpo;
        NetworkAddress networkAddress;
        reference      root;
        Memory( Daemon& daemon, const MPO& mpo, const NetworkAddress& networkAddress )
            : daemon( daemon )
            , mpo( mpo )
            , networkAddress( networkAddress )
        {
            strMemory = daemon.getMemoryManager().acquire( mpo );
            root      = daemon.getMemoryManager().allocateRoot( mpo, networkAddress );
            SPDLOG_TRACE( "DaemonRequestConversation::RootSimRun created root: {} in memory: {}", root, strMemory );
        }
        ~Memory()
        {
            //
            daemon.getMemoryManager().release( mpo );
        }
    } wrapper( m_daemon, mpo, networkAddress );

    {
        // network::Server::ConnectionLabelRAII connectionLabel( m_daemon.m_server, mpo, pConnection );
        network::daemon_leaf::Request_Sender sender( *this, *pConnection, yield_ctx );
        sender.RootSimRun( wrapper.root, wrapper.strMemory );
    }
}

Snapshot DaemonRequestConversation::SimLockRead( const MPO& requestingMPO, const MPO& targetMPO,
                                                 boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestConversation::SimLockRead from: {} to: {}", requestingMPO, targetMPO );
    if( network::Server::Connection::Ptr pConnection = m_daemon.m_server.findConnection( MP( targetMPO ) ) )
    {
        ASSERT( m_daemon.m_machineID == targetMPO.getMachineID() );

        network::sim::Request_Sender sender( *this, *pConnection, yield_ctx );
        Snapshot                     snapshot = sender.SimLockRead( requestingMPO, targetMPO );

        if( m_daemon.m_machineID == requestingMPO.getMachineID() )
        {
            // response is to this machine
            m_daemon.getMemoryManager().networkToMachine( snapshot );
        }
        else
        {
            // response is to different machine
            m_daemon.getMemoryManager().machineToNetwork( snapshot );
        }

        return snapshot;
    }
    else
    {
        ASSERT( m_daemon.m_machineID != targetMPO.getMachineID() );

        network::sim::Request_Sender sender( *this, m_daemon.m_rootClient, yield_ctx );
        Snapshot snapshot = sender.SimLockRead( requestingMPO, targetMPO );

        m_daemon.getMemoryManager().networkToMachine( snapshot );

        return snapshot;
    }
}

Snapshot DaemonRequestConversation::SimLockWrite( const MPO& requestingMPO, const MPO& targetMPO,
                                                  boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestConversation::SimLockWrite from: {} to: {}", requestingMPO, targetMPO );
    if( network::Server::Connection::Ptr pConnection = m_daemon.m_server.findConnection( MP( targetMPO ) ) )
    {
        network::sim::Request_Sender sender( *this, *pConnection, yield_ctx );
        return sender.SimLockWrite( requestingMPO, targetMPO );
    }
    else
    {
        network::sim::Request_Sender sender( *this, m_daemon.m_rootClient, yield_ctx );
        return sender.SimLockWrite( requestingMPO, targetMPO );
    }
}

void DaemonRequestConversation::SimLockRelease( const MPO&                  requestingMPO,
                                                const MPO&                  targetMPO,
                                                const network::Transaction& transaction,
                                                boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestConversation::SimLockRelease from: {} to: {}", requestingMPO, targetMPO );
    if( network::Server::Connection::Ptr pConnection = m_daemon.m_server.findConnection( MP( targetMPO ) ) )
    {
        network::sim::Request_Sender sender( *this, *pConnection, yield_ctx );
        return sender.SimLockRelease( requestingMPO, targetMPO, transaction );
    }
    else
    {
        network::sim::Request_Sender sender( *this, m_daemon.m_rootClient, yield_ctx );
        return sender.SimLockRelease( requestingMPO, targetMPO, transaction );
    }
}

} // namespace mega::service
