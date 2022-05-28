
#include "service/root/root.hpp"

#include "service/network/activity.hpp"
#include "service/network/network.hpp"
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

    virtual bool dispatch( const network::MessageVariant& msg, boost::asio::yield_context yield_ctx ) 
    {
        return network::Activity::dispatch( msg, yield_ctx ) ||
            network::daemon_root::Impl::dispatch( msg, *this, yield_ctx );
    }

    virtual void GetVersion( boost::asio::yield_context yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_root.m_server.getConnection( getOriginatingEndPointID().value() ) )
        {
            network::daemon_root::Response_Encode daemonResponse( *this, pConnection->getSocket(), yield_ctx );
            daemonResponse.GetVersion( network::getVersion() );
            //std::cout << "Hello from root" << std::endl;
        }
    }
    virtual void runTestPipeline( boost::asio::yield_context yield_ctx )
    {
        int totalThreads = 0;
        for( auto & [ id, pDeamon ] :
            m_root.m_server.getConnections() )
        {
            network::root_daemon::Request_Encode daemonRequest( *this, pDeamon->getSocket(), yield_ctx );
            int threads = daemonRequest.ListWorkers();
            totalThreads += threads;
            daemonRequest.Complete();
        }
        
        network::Server::Connection::Ptr pConnection
             = m_root.m_server.getConnection( getOriginatingEndPointID().value() );

        network::daemon_root::Response_Encode daemonResponse( *this, pConnection->getSocket(), yield_ctx );
        std::ostringstream os;
        os << "Found: " << totalThreads << " threads";
        daemonResponse.runTestPipeline( os.str() );
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