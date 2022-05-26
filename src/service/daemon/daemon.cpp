
#include "service/daemon/daemon.hpp"

#include "service/network/activity.hpp"

#include <iostream>

namespace mega
{
namespace service
{

class RequestActivity : public network::Activity, public network::host_daemon::Impl
{
    Daemon& m_daemon;

public:
    RequestActivity( Daemon&                      daemon,
                     network::ActivityManager&    activityManager,
                     const network::ActivityID&   activityID,
                     const network::ConnectionID& originatingConnectionID )
        : Activity( activityManager, activityID, originatingConnectionID )
        , m_daemon( daemon )
    {
    }

    virtual void run( boost::asio::yield_context yield_ctx )
    {
        while ( network::host_daemon::Impl::dispatch( receiveMessage( yield_ctx ), yield_ctx ) )
            ;
        completed();
    }

    virtual void GetVersion( const std::string& version, boost::asio::yield_context yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_server.getConnection( getOriginatingEndPointID().value() ) )
        {
            network::host_daemon::Response_Encode hostResponse( *this, pConnection->getSocket(), yield_ctx );
            std::ostringstream                   os;
            os << "Received: " << version;
            hostResponse.GetVersion( os.str() );
        }
    }
};

Daemon::DaemonActivityFactory::DaemonActivityFactory( Daemon& daemon )
    : m_daemon( daemon )
{
}

network::Activity::Ptr
Daemon::DaemonActivityFactory::createRequestActivity( network::ActivityManager&    activityManager,
                                                      const network::ActivityID&   activityID,
                                                      const network::ConnectionID& originatingConnectionID ) const
{
    return network::Activity::Ptr(
        new RequestActivity( m_daemon, activityManager, activityID, originatingConnectionID ) );
}

Daemon::Daemon( boost::asio::io_context& ioContext )
    : m_activityFactory( *this )
    , m_server( ioContext, m_activityFactory )
{
    m_server.waitForConnection();
}

} // namespace service
} // namespace mega