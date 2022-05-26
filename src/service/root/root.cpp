
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

    virtual void run( boost::asio::yield_context yield_ctx )
    {
        while ( network::daemon_root::Impl::dispatch( receiveRequest( yield_ctx ), yield_ctx ) )
            ;
        completed();
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