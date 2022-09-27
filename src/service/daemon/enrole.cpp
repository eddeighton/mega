
#include "request.hpp"

namespace mega
{
namespace service
{

// network::enrole::Impl
MP DaemonRequestConversation::EnroleLeafWithDaemon( const mega::network::Node::Type& type,
                                                    boost::asio::yield_context&      yield_ctx )
{
    network::Server::Connection::Ptr pConnection
        = m_daemon.m_server.getConnection( getOriginatingEndPointID().value() );
    VERIFY_RTE( pConnection );
    pConnection->setType( type );

    const mega::MP leafMP
        = getRootRequest< network::enrole::Request_Encoder >( yield_ctx ).EnroleLeafWithRoot( m_daemon.m_mp );
    SPDLOG_TRACE( "Leaf enroled as {}", leafMP );

    m_daemon.m_server.mapConnection( leafMP, pConnection );

    pConnection->setMP( leafMP );
    pConnection->setDisconnectCallback( [ leafMP, &daemon = m_daemon ]( const network::ConnectionID& connectionID )
                                        { daemon.onLeafDisconnect( connectionID, leafMP ); } );

    return leafMP;
}

} // namespace service
} // namespace mega