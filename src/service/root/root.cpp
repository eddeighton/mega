
#include "service/root/root.hpp"

#include "service/network/activity.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/model/root_daemon.hxx"
#include "service/protocol/model/daemon_root.hxx"

#include <iostream>

namespace mega
{
namespace service
{

class RequestActivity : public network::Activity, public network::daemon_root::Impl
{
    Root& m_root;

public:
    RequestActivity( Root&                        root,
                     const network::ActivityID&   activityID,
                     const network::ConnectionID& originatingConnectionID )
        : Activity( root.m_activityManager, activityID, originatingConnectionID )
        , m_root( root )
    {
    }

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context yield_ctx )
    {
        return network::Activity::dispatchRequest( msg, yield_ctx )
               || network::daemon_root::Impl::dispatchRequest( msg, *this, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connectionID,
                        const std::string&           strErrorMsg,
                        boost::asio::yield_context   yield_ctx )
    {
        if ( network::Server::Connection::Ptr pHostConnection = m_root.m_server.getConnection( connectionID ) )
        {
            network::sendErrorResponse( getActivityID(), pHostConnection->getSocket(), strErrorMsg, yield_ctx );
        }
        else
        {
            // ?
        }
    }

    network::daemon_root::Response_Encode getOriginatingDaemonResponse( boost::asio::yield_context yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_root.m_server.getConnection( getOriginatingEndPointID().value() ) )
        {
            return network::daemon_root::Response_Encode( *this, pConnection->getSocket(), yield_ctx );
        }
        THROW_RTE( "Connection to daemon lost" );
    }

    network::root_daemon::Request_Encode getDaemonRequest( network::Server::Connection::Ptr pConnection,
                                                           boost::asio::yield_context       yield_ctx )
    {
        return network::root_daemon::Request_Encode( *this, pConnection->getSocket(), yield_ctx );
    }

    // network::daemon_root::Impl
    virtual void GetVersion( boost::asio::yield_context yield_ctx )
    {
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.GetVersion( network::getVersion() );
    }

    virtual void runTestPipeline( boost::asio::yield_context yield_ctx )
    {
        int totalThreads = 0;
        for ( auto& [ id, pDaemon ] : m_root.m_server.getConnections() )
        {
            auto daemon = getDaemonRequest( pDaemon, yield_ctx );
            try
            {
                int threads = daemon.ListWorkers();
                totalThreads += threads;
            }
            catch ( std::exception& ex )
            {
                SPDLOG_WARN( "Caught exception from ListWorkers: {}", ex.what() );
            }
            daemon.Complete();
        }

        auto               daemon = getOriginatingDaemonResponse( yield_ctx );
        std::ostringstream os;
        os << "Found: " << totalThreads << " threads";
        daemon.runTestPipeline( os.str() );
    }
};

network::Activity::Ptr
Root::RootActivityFactory::createRequestActivity( const network::ActivityID&   activityID,
                                                  const network::ConnectionID& originatingConnectionID ) const
{
    return network::Activity::Ptr( new RequestActivity( m_root, activityID, originatingConnectionID ) );
}

Root::Root( boost::asio::io_context& ioContext )
    : m_activityFactory( *this )
    , m_activityManager( ioContext )
    , m_server( ioContext, m_activityManager, m_activityFactory, network::MegaRootPort() )
{
    m_server.waitForConnection();
}

} // namespace service
} // namespace mega