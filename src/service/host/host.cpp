
#include "service/host/host.hpp"

#include "service/network/activity_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/host_daemon.hxx"

#include "common/requireSemicolon.hpp"
#include "service/protocol/model/messages.hxx"

#include <optional>
#include <future>

namespace mega
{
namespace service
{

class RequestActivity : public network::Activity, public network::daemon_host::Impl
{
    Host& m_host;

public:
    RequestActivity( Host& host, const network::ActivityID& activityID,
                     const network::ConnectionID& originatingConnectionID )
        : Activity( host.m_activityManager, activityID, originatingConnectionID )
        , m_host( host )
    {
    }

    virtual bool dispatch( const network::MessageVariant& msg, boost::asio::yield_context yield_ctx )
    {
        return network::Activity::dispatch( msg, yield_ctx )
               || network::daemon_host::Impl::dispatch( msg, *this, yield_ctx );
    }
    /*
        virtual void DaemonToHost( const int& anInt, boost::asio::yield_context yield_ctx )
        {
            network::daemon_host::Response_Encode daemon( *this, m_host.m_client.getSocket(), yield_ctx );
            daemon.DaemonToHost( {} );
        }*/
};

network::Activity::Ptr
Host::HostActivityFactory::createRequestActivity( const network::ActivityID&   activityID,
                                                  const network::ConnectionID& originatingConnectionID ) const
{
    return network::Activity::Ptr( new RequestActivity( m_host, activityID, originatingConnectionID ) );
}

Host::Host( std::optional< const std::string > optName /* = std::nullopt*/ )
    : m_activityFactory( *this )
    , m_activityManager( m_io_context )
    , m_client(
          m_io_context, m_activityManager, m_activityFactory, "localhost", mega::network::MegaDaemonServiceName() )
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
class GenericActivity : public RequestActivity
{
    Host&            m_host;
    network::Client& m_client;
    TPromiseType&    m_promise;
    TActivityFunctor m_functor;

public:
    GenericActivity( Host& host, network::Client& client, const network::ConnectionID& originatingConnectionID,
                     TPromiseType& promise, TActivityFunctor&& functor )
        : RequestActivity(
            host, network::ActivityID( g_activity_id_counter++, originatingConnectionID ), originatingConnectionID )
        , m_host( host )
        , m_client( client )
        , m_promise( promise )
        , m_functor( functor )
    {
    }

    void run( boost::asio::yield_context yield_ctx )
    {
        // std::cout << "Started: " << getActivityID() << std::endl;
        m_functor( m_promise, m_client, *this, yield_ctx );

        RequestActivity::run( yield_ctx );
    }
};

#define SIMPLE_REQUEST( TResult, Code )                                                                               \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( using ResultType                  = TResult; std::promise< ResultType > promise; \
                                     std::future< ResultType > fResult = promise.get_future();                        \
                                     m_activityManager.activityStarted( network::Activity::Ptr( new GenericActivity(  \
                                         *this, m_client, network::getConnectionID( m_client.getSocket() ), promise,  \
                                         []( std::promise< ResultType >& promise, network::Client& client,            \
                                             network::Activity& activity, boost::asio::yield_context yield_ctx )      \
                                         {                                                                            \
                                             network::host_daemon::Request_Encode daemon(                             \
                                                 activity, client.getSocket(), yield_ctx );                           \
                                             {                                                                        \
                                                 Code;                                                                \
                                             }                                                                        \
                                             daemon.Complete();                                                       \
                                         } ) ) );                                                                     \
                                     return fResult.get(); )

std::string Host::GetVersion()
{
    //
    SIMPLE_REQUEST( std::string, promise.set_value( daemon.GetVersion() ) );
}

std::vector< std::string > Host::ListHosts()
{
    SIMPLE_REQUEST( std::vector< std::string >, promise.set_value( daemon.ListHosts() ) );
}

std::string Host::runTestPipeline() { SIMPLE_REQUEST( std::string, promise.set_value( daemon.runTestPipeline() ) ); }
} // namespace service
} // namespace mega
