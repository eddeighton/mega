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

namespace mega
{
namespace service
{

// network::enrole::Impl
MP RootRequestConversation::EnroleDaemon( boost::asio::yield_context& yield_ctx )
{
    const mega::MP mp = m_root.m_mpoManager.newDaemon();
    SPDLOG_TRACE( "RootRequestConversation::EnroleDaemon: {}", mp );
    network::Server::Connection::Ptr pConnection = m_root.m_server.getConnection( getOriginatingEndPointID().value() );
    pConnection->setMP( mp );
    pConnection->setDisconnectCallback( [ mp, &root = m_root ]( const network::ConnectionID& connectionID )
                                        { root.onDaemonDisconnect( connectionID, mp ); } );
    m_root.m_server.mapConnection( mp, pConnection );
    return mp;
}

MP RootRequestConversation::EnroleLeafWithRoot( const MP& daemonMP, boost::asio::yield_context& yield_ctx )
{
    const MP mp = m_root.m_mpoManager.newLeaf( daemonMP );
    SPDLOG_TRACE( "RootRequestConversation::EnroleLeafWithRoot: {}", mp );
    return mp;
}

void RootRequestConversation::EnroleLeafDisconnect( const MP& mp, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "RootRequestConversation::EnroleLeafDisconnect {}", mp );
    m_root.m_mpoManager.leafDisconnected( mp );
}
} // namespace service
} // namespace mega