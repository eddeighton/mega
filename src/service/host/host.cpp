
#include "service/host/host.hpp"

#include "service/network/activity_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"

#include "boost/bind/bind.hpp"
#include "boost/asio/thread_pool.hpp"
#include "service/protocol/common/header.hpp"
#include <boost/bind/placeholders.hpp>
#include <optional>

namespace mega
{
namespace service
{

class ActivityGetVersion : public network::Activity
{
    network::Client&             client;
    const std::string&           str;
    std::promise< std::string >& promise;

public:
    ActivityGetVersion( network::Client& client, const network::ActivityID& activityID, const std::string& str,
                        std::promise< std::string >& promise )
        : Activity( client, activityID, std::nullopt )
        , client( client )
        , str( str )
        , promise( promise )
    {
    }

    void run( boost::asio::yield_context yield_ctx )
    {
        network::host_daemon_Request_Encode daemon( *this, client.getSocket(), yield_ctx );

        std::string strResult = daemon.GetVersion( str );

        promise.set_value( strResult );

        completed();
    }
};

class RequestActivity : public network::Activity//, public network::host_daemon_Impl
{
public:
    RequestActivity( network::ActivityManager& activityManager, const network::ActivityID& activityID,
                     const network::ConnectionID& originatingConnectionID )
        : Activity( activityManager, activityID, originatingConnectionID )
    {
    }

    virtual void run( boost::asio::yield_context yield_ctx )
    {
        /*while ( true )
        {
            // get decoded request
            network::MessageVariant msg = receiveMessage( yield_ctx );
            // dispatch
            if( host_daemon_Impl::dispatch( msg, yield_ctx ) == false )
                break;
        }*/
        completed();
    }
    /*virtual void GetVersion( const std::string& version, boost::asio::yield_context yield_ctx )
    {
        std::cout << "Received GetVersion request with: " << version << std::endl;

        // network::daemon_host_Response_Encode daemon( *this, client.getSocket(), yield_ctx );


    }*/
};

network::Activity::Ptr
Host::HostActivityFactory::createRequestActivity( network::ActivityManager&  activityManager,
                                                  const network::ActivityID& activityID,
                                                  const network::ConnectionID& originatingConnectionID ) const
{
    return network::Activity::Ptr( new RequestActivity( activityManager, activityID, originatingConnectionID ) );
}

Host::Host( std::optional< const std::string > optName /* = std::nullopt*/ )
    : m_client( m_io_context, m_activityFactory, "localhost" )
    , m_work_guard( m_io_context.get_executor() )
    , m_io_thread( [ &io_context = m_io_context ]() { io_context.run(); } )
{
}

Host::~Host()
{
    m_work_guard.reset();
    m_io_thread.join();
}

std::string Host::GetVersion( const std::string& str )
{
    std::promise< std::string > promise;
    std::future< std::string >  fResult = promise.get_future();

    const network::ConnectionID endpointID = network::getConnectionID( m_client.getSocket() );

    static network::ActivityID::ID id = 1U;
    network::ActivityID            activityID( id++, endpointID );

    std::cout << "Starting: " << activityID << std::endl;

    network::Activity::Ptr pActivity( new ActivityGetVersion( m_client, activityID, str, promise ) );

    m_client.spawnActivity( pActivity );

    return fResult.get();
}

} // namespace service
} // namespace mega