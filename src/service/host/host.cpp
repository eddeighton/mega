
#include "service/host/host.hpp"

#include "service/network/activity_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/host_daemon.hxx"

#include <optional>
#include <future>

namespace mega
{
namespace service
{

class RequestActivity : public network::Activity, public network::daemon_host::Impl
{
public:
    RequestActivity( network::ActivityManager& activityManager, const network::ActivityID& activityID,
                     const network::ConnectionID& originatingConnectionID )
        : Activity( activityManager, activityID, originatingConnectionID )
    {
    }

    virtual void run( boost::asio::yield_context yield_ctx )
    {
        while ( network::daemon_host::Impl::dispatch( receiveRequest( yield_ctx ), yield_ctx ) )
            ;
        completed();
    }

    virtual void DaemonToHost( const int& anInt, boost::asio::yield_context yield_ctx )
    {
        // network::daemon_host::Response_Encode daemon( *this, client.getSocket(), yield_ctx );
    }
};

network::Activity::Ptr
Host::HostActivityFactory::createRequestActivity( network::ActivityManager&    activityManager,
                                                  const network::ActivityID&   activityID,
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
    m_client.stop();
    m_work_guard.reset();
    m_io_thread.join();
}

static network::ActivityID::ID g_activity_id_counter = 1;

template < typename TPromiseType, typename TActivityFunctor >
class GenericActivity : public network::Activity
{
    network::Client& m_client;
    TPromiseType&    m_promise;
    TActivityFunctor m_functor;

public:
    GenericActivity( network::Client& client, TPromiseType& promise, TActivityFunctor&& functor )
        : Activity(
            client, network::ActivityID( g_activity_id_counter++, network::getConnectionID( client.getSocket() ) ) )
        , m_client( client )
        , m_promise( promise )
        , m_functor( functor )
    {
    }

    void run( boost::asio::yield_context yield_ctx )
    {
        //std::cout << "Started: " << getActivityID() << std::endl;
        m_functor( m_promise, m_client, *this, yield_ctx );
        completed();
    }
};

std::string Host::GetVersion()
{
    std::promise< std::string > promise;
    std::future< std::string >  fResult = promise.get_future();

    m_client.spawnActivity( network::Activity::Ptr(
        new GenericActivity( m_client, promise,
                             []( std::promise< std::string >& promise, network::Client& client,
                                 network::Activity& activity, boost::asio::yield_context yield_ctx )
                             {
                                 using namespace network::host_daemon;
                                 Request_Encode daemon( activity, client.getSocket(), yield_ctx );
                                 promise.set_value( daemon.GetVersion() );
                                 daemon.Complete();
                             } ) ) );

    return fResult.get();
}

std::vector< std::string > Host::ListHosts()
{
    using ResultType = std::vector< std::string >;
    std::promise< ResultType > promise;
    std::future< ResultType >  fResult = promise.get_future();

    m_client.spawnActivity( network::Activity::Ptr(
        new GenericActivity( m_client, promise,
                             []( std::promise< ResultType >& promise, network::Client& client,
                                 network::Activity& activity, boost::asio::yield_context yield_ctx )
                             {
                                 using namespace network::host_daemon;
                                 Request_Encode daemon( activity, client.getSocket(), yield_ctx );
                                 promise.set_value( daemon.ListHosts() );
                                 daemon.Complete();
                             } ) ) );

    return fResult.get();
}

} // namespace service
} // namespace mega