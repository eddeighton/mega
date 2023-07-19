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

#include "service/network/log.hpp"

#include "boost/process.hpp"

namespace mega::service
{
// network::enrole::Impl
MP DaemonRequestConversation::EnroleLeafWithDaemon( const std::string&               startupUUID,
                                                    const mega::network::Node::Type& type,
                                                    boost::asio::yield_context&      yield_ctx )
{
    network::Server::Connection::Ptr pConnection
        = m_daemon.m_server.getConnection( getOriginatingEndPointID().value() );
    VERIFY_RTE( pConnection );
    pConnection->setType( type );

    const mega::MP leafMP
        = getRootRequest< network::enrole::Request_Encoder >( yield_ctx ).EnroleLeafWithRoot( startupUUID, m_daemon.m_machineID );
    SPDLOG_TRACE( "Leaf enroled as {}", leafMP );

    m_daemon.m_server.labelConnection( leafMP, pConnection );
    pConnection->setDisconnectCallback( [ leafMP, &daemon = m_daemon ]( const network::ConnectionID& connectionID )
                                        { daemon.onLeafDisconnect( connectionID, leafMP ); } );

    return leafMP;
}

void DaemonRequestConversation::EnroleDaemonSpawn( const std::string& strProgram, const std::string& strStartupUUID, boost::asio::yield_context& yield_ctx )
{
    auto env = boost::this_process::environment();
    env[ network::ENV_PROCESS_UUID ] = strStartupUUID;
    boost::process::spawn( strProgram, env );
}

} // namespace mega::service