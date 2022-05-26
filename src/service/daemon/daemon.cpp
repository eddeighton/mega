
#include "service/daemon/daemon.hpp"

#include "service/network/activity.hpp"
#include "service/network/network.hpp"
#include "service/protocol/model/daemon_host.hxx"
#include "service/protocol/model/daemon_root.hxx"
#include "service/protocol/model/root_daemon.hxx"

#include <iostream>

namespace mega
{
namespace service
{
class HostRequestActivity : public network::Activity, public network::host_daemon::Impl
{
    Daemon& m_daemon;

public:
    HostRequestActivity( Daemon&                      daemon,
                         const network::ActivityID&   activityID,
                         const network::ConnectionID& originatingConnectionID )
        : Activity( daemon.m_activityManager, activityID, originatingConnectionID )
        , m_daemon( daemon )
    {
    }

    virtual void run( boost::asio::yield_context yield_ctx )
    {
        while ( network::host_daemon::Impl::dispatch( receiveRequest( yield_ctx ), yield_ctx ) )
            ;
        completed();
    }

    virtual void GetVersion( boost::asio::yield_context yield_ctx )
    {
        std::string                          strMegaStructureVersion;
        network::daemon_root::Request_Encode root( *this, m_daemon.m_client.getSocket(), yield_ctx );
        strMegaStructureVersion = root.GetVersion();
        root.Complete();
        //std::cout << "Got version from Root of: " << strMegaStructureVersion << std::endl;

        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_server.getConnection( getOriginatingEndPointID().value() ) )
        {
            network::host_daemon::Response_Encode hostResponse( *this, pConnection->getSocket(), yield_ctx );
            hostResponse.GetVersion( strMegaStructureVersion );
        }
    }
    virtual void ListHosts( boost::asio::yield_context yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_server.getConnection( getOriginatingEndPointID().value() ) )
        {
            network::host_daemon::Response_Encode hostResponse( *this, pConnection->getSocket(), yield_ctx );
            std::vector< std::string > hosts;
            for ( auto& [ id, pConnection ] : m_daemon.m_server.getConnections() )
                hosts.push_back( pConnection->getName() );
            hostResponse.ListHosts( hosts );
        }
    }
};

network::Activity::Ptr
Daemon::HostRequestActivityFactory::createRequestActivity( const network::ActivityID&   activityID,
                                                           const network::ConnectionID& originatingConnectionID ) const
{
    return network::Activity::Ptr( new HostRequestActivity( m_daemon, activityID, originatingConnectionID ) );
}

class RootRequestActivity : public network::Activity, public network::root_daemon::Impl
{
    Daemon& m_daemon;

public:
    RootRequestActivity( Daemon&                      daemon,
                         const network::ActivityID&   activityID,
                         const network::ConnectionID& originatingConnectionID )
        : Activity( daemon.m_activityManager, activityID, originatingConnectionID )
        , m_daemon( daemon )
    {
    }

    virtual void run( boost::asio::yield_context yield_ctx )
    {
        while ( network::root_daemon::Impl::dispatch( receiveRequest( yield_ctx ), yield_ctx ) )
            ;
        completed();
    }
};

network::Activity::Ptr
Daemon::RootRequestActivityFactory::createRequestActivity( const network::ActivityID&   activityID,
                                                           const network::ConnectionID& originatingConnectionID ) const
{
    return network::Activity::Ptr( new RootRequestActivity( m_daemon, activityID, originatingConnectionID ) );
}

Daemon::Daemon( boost::asio::io_context& ioContext, const std::string& strRootIP )
    : m_rootRequestActivityFactory( *this )
    , m_hostRequestActivityFactory( *this )
    , m_activityManager( ioContext )
    , m_client(
          ioContext, m_activityManager, m_rootRequestActivityFactory, strRootIP, mega::network::MegaRootServiceName() )
    , m_server( ioContext, m_activityManager, m_hostRequestActivityFactory, network::MegaDaemonPort() )
{
    m_server.waitForConnection();
}

Daemon::~Daemon()
{
    m_client.stop();
    m_server.stop();
}

} // namespace service
} // namespace mega