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