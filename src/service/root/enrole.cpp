#include "request.hpp"

#include "root.hpp"

namespace mega
{
namespace service
{

// network::enrole::Impl
MPO RootRequestConversation::EnroleDaemon( boost::asio::yield_context& yield_ctx )
{
    const mega::MPO                  mpo         = m_root.m_mpoManager.newDaemon();
    network::Server::Connection::Ptr pConnection = m_root.m_server.getConnection( getOriginatingEndPointID().value() );
    pConnection->setMPO( mpo );
    pConnection->setDisconnectCallback( [ mpo, &root = m_root ]( const network::ConnectionID& connectionID )
                                        { root.onDaemonDisconnect( connectionID, mpo ); } );
    return mpo;
}

MPO RootRequestConversation::EnroleLeafWithRoot( const MPO& daemonMPO, boost::asio::yield_context& yield_ctx )
{
    return m_root.m_mpoManager.newLeaf( daemonMPO );
}

void RootRequestConversation::EnroleLeafDisconnect( const MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "EnroleLeafDisconnect {}", mpo );
    m_root.m_mpoManager.leafDisconnected( mpo );
}
}
}