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

#include "root.hpp"

#include "service/protocol/model/memory.hxx"

namespace mega::service
{

// network::enrole::Impl
MachineID RootRequestConversation::EnroleDaemon( boost::asio::yield_context& yield_ctx )
{
    const MachineID machineID = m_root.m_mpoManager.newDaemon();
    SPDLOG_TRACE( "RootRequestConversation::EnroleDaemon: {}", machineID );
    network::Server::Connection::Ptr pConnection = m_root.m_server.getConnection( getOriginatingEndPointID().value() );
    pConnection->setDisconnectCallback( [ machineID, &root = m_root ]( const network::ConnectionID& connectionID )
                                        { root.onDaemonDisconnect( connectionID, machineID ); } );

    network::Server::Connection::Label label { machineID } ;
    VERIFY_RTE_MSG( std::get< MachineID >( label ) == machineID, "std::variant sucks!" );

    m_root.m_server.labelConnection( label, pConnection );

    return machineID;
}

MP RootRequestConversation::EnroleLeafWithRoot( const MachineID& machineID, boost::asio::yield_context& yield_ctx )
{
    const MP mp = m_root.m_mpoManager.newLeaf( machineID );
    SPDLOG_TRACE( "RootRequestConversation::EnroleLeafWithRoot: {}", mp );
    return mp;
}

void RootRequestConversation::EnroleLeafDisconnect( const MP& mp, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestConversation::EnroleLeafDisconnect {}", mp );
    const auto terminatedMPOS = m_root.m_mpoManager.leafDisconnected( mp );

    auto stackCon = getOriginatingEndPointID();
    VERIFY_RTE( stackCon.has_value() );
    auto pConnection = m_root.m_server.getConnection( stackCon.value() );
    VERIFY_RTE( pConnection );
    VERIFY_RTE( pConnection->getLabel().has_value() );
    VERIFY_RTE( std::get< MachineID >( pConnection->getLabel().value() ) == mp.getMachineID() );

    network::memory::Request_Sender sender( *this, *pConnection, yield_ctx );
    for ( MPO mpo : terminatedMPOS )
    {
        sender.MPODestroyed( mpo );
    }
}

std::vector< MachineID > RootRequestConversation::EnroleGetDaemons( boost::asio::yield_context& yield_ctx )
{
    return m_root.m_mpoManager.getMachines();
}
std::vector< MP > RootRequestConversation::EnroleGetProcesses( const MachineID&            machineID,
                                                               boost::asio::yield_context& yield_ctx )
{
    return m_root.m_mpoManager.getMachineProcesses( machineID );
}
std::vector< MPO > RootRequestConversation::EnroleGetMPO( const MP&                   machineProcess,
                                                          boost::asio::yield_context& yield_ctx )
{
    return m_root.m_mpoManager.getMPO( machineProcess );
}

} // namespace mega::service
