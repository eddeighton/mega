
#include "request.hpp"

namespace mega
{
namespace service
{

// network::enrole::Impl
MPO DaemonRequestConversation::EnroleLeafWithDaemon( const mega::network::Node::Type& type,
                                                     boost::asio::yield_context&      yield_ctx )
{
    network::Server::Connection::Ptr pConnection
        = m_daemon.m_leafServer.getConnection( getOriginatingEndPointID().value() );
    VERIFY_RTE( pConnection );
    pConnection->setType( type );
    // SPDLOG_TRACE( "Leaf {} enroled as {}", pConnection->getName(), network::Node::toStr( type ) );

    const mega::MPO leafMPO
        = getRootRequest< network::enrole::Request_Encoder >( yield_ctx ).EnroleLeafWithRoot( m_daemon.m_mpo );

    pConnection->setMPO( leafMPO );
    pConnection->setDisconnectCallback( [ leafMPO, &daemon = m_daemon ]( const network::ConnectionID& connectionID )
                                        { daemon.onLeafDisconnect( connectionID, leafMPO ); } );

    return leafMPO;
}

} // namespace service
} // namespace mega