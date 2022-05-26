
#include "service/daemon/daemon.hpp"

#include "service/network/activity.hpp"

#include <iostream>

namespace mega
{

namespace service
{

class RequestActivity : public network::Activity, public network::host_daemon_Impl
{
    Daemon& m_daemon;

public:
    RequestActivity( Daemon&                    daemon,
                     network::ActivityManager&  activityManager,
                     const network::ActivityID& activityID,
                     const network::ConnectionID& originatingConnectionID )
        : Activity( activityManager, activityID, originatingConnectionID )
        , m_daemon( daemon )
    {
    }

    virtual void run( boost::asio::yield_context yield_ctx )
    {
        while ( true )
        {
            // get decoded request
            network::MessageVariant msg = receiveMessage( yield_ctx );
            // dispatch
            if ( host_daemon_Impl::dispatch( msg, yield_ctx ) == false )
                break;
        }
        completed();
    }

    virtual void GetVersion( const std::string& version, boost::asio::yield_context yield_ctx )
    {
        std::cout << "Received GetVersion request with: " << version << std::endl;

        network::Server::Connection::Ptr pConnection
            = m_daemon.m_server.getConnection( getOriginatingEndPointID().value() );

        VERIFY_RTE( pConnection );

        network::host_daemon_Response_Encode daemonToHostResponse( *this, pConnection->getSocket(), yield_ctx );

        std::ostringstream os;
        os << "Received: " << version;
        std::cout << "Sending: " << os.str();

        daemonToHostResponse.GetVersion( os.str() );

        completed();
    }
};

Daemon::DaemonActivityFactory::DaemonActivityFactory( Daemon& daemon )
    : m_daemon( daemon )
{
}

network::Activity::Ptr
Daemon::DaemonActivityFactory::createRequestActivity( network::ActivityManager&  activityManager,
                                                      const network::ActivityID& activityID,
                                                      const network::ConnectionID& originatingConnectionID ) const
{
    return network::Activity::Ptr( new RequestActivity( m_daemon, activityManager, activityID, originatingConnectionID ) );
}

Daemon::Daemon( boost::asio::io_context& ioContext )
    : m_activityFactory( *this )
    , m_server( ioContext, m_activityFactory )
{
    //
    m_server.waitForConnection();
}

} // namespace service
} // namespace mega