
#include "service/worker/worker.hpp"

#include "service/network/activity_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/worker_daemon.hxx"
#include "service/protocol/model/daemon_worker.hxx"

#include "common/requireSemicolon.hpp"

#include <optional>
#include <future>

namespace mega
{
namespace service
{

class RequestActivity : public network::Activity, public network::daemon_worker::Impl
{
    Worker& m_worker;

public:
    RequestActivity( Worker& worker, const network::ActivityID& activityID,
                     const network::ConnectionID& originatingConnectionID )
        : Activity( worker.m_activityManager, activityID, originatingConnectionID )
        , m_worker( worker )
    {
    }

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context yield_ctx )
    {
        return network::Activity::dispatchRequest( msg, yield_ctx )
               || network::daemon_worker::Impl::dispatchRequest( msg, *this, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connectionID, const std::string& strErrorMsg, boost::asio::yield_context yield_ctx )
    {
        if ( network::getConnectionID( m_worker.m_client.getSocket() ) == connectionID )
        {
            network::sendErrorResponse( getActivityID(), m_worker.m_client.getSocket(), strErrorMsg, yield_ctx );
        }
        else
        {
            // ?
        }
    }

    virtual void ListThreads( boost::asio::yield_context yield_ctx )
    {
        THROW_RTE( "Test exception" );

        network::daemon_worker::Response_Encode daemon( *this, m_worker.m_client.getSocket(), yield_ctx );
        daemon.ListThreads( m_worker.getNumThreads() );
    }
};

network::Activity::Ptr
Worker::HostActivityFactory::createRequestActivity( const network::ActivityID&   activityID,
                                                    const network::ConnectionID& originatingConnectionID ) const
{
    return network::Activity::Ptr( new RequestActivity( m_worker, activityID, originatingConnectionID ) );
}

Worker::Worker( boost::asio::io_context& io_context, int numThreads )
    : m_activityFactory( *this )
    , m_io_context( io_context )
    , m_numThreads( numThreads )
    , m_activityManager( m_io_context )
    , m_client(
          m_io_context, m_activityManager, m_activityFactory, "localhost", mega::network::MegaWorkerServiceName() )
    , m_work_guard( m_io_context.get_executor() )
{
}

Worker::~Worker()
{
    m_client.stop();
    m_work_guard.reset();
}

static network::ActivityID::ID g_activity_id_counter = 1;

template < typename TPromiseType, typename TActivityFunctor >
class GenericActivity : public network::Activity
{
    network::Client& m_client;
    TPromiseType&    m_promise;
    TActivityFunctor m_functor;

public:
    GenericActivity( network::Client& client, network::ActivityManager& activityManager, TPromiseType& promise,
                     TActivityFunctor&& functor )
        : Activity( activityManager,
                    network::ActivityID( g_activity_id_counter++, network::getConnectionID( client.getSocket() ) ) )
        , m_client( client )
        , m_promise( promise )
        , m_functor( functor )
    {
    }

    void run( boost::asio::yield_context yield_ctx )
    {
        m_functor( m_promise, m_client, *this, yield_ctx );
    }
};

#define SIMPLE_REQUEST( TResult, Code )                                                                               \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( using ResultType                  = TResult; std::promise< ResultType > promise; \
                                     std::future< ResultType > fResult = promise.get_future();                        \
                                     m_activityManager.activityStarted( network::Activity::Ptr( new GenericActivity(  \
                                         m_client, m_activityManager, promise,                                        \
                                         []( std::promise< ResultType >& promise, network::Client& client,            \
                                             network::Activity& activity, boost::asio::yield_context yield_ctx )      \
                                         {                                                                            \
                                             network::worker_daemon::Request_Encode daemon(                           \
                                                 activity, client.getSocket(), yield_ctx );                           \
                                             {                                                                        \
                                                 Code;                                                                \
                                             }                                                                        \
                                             daemon.Complete();                                                       \
                                         } ) ) );                                                                     \
                                     return fResult.get(); )

/*
std::vector< std::string > Worker::ListHosts()
{
    SIMPLE_REQUEST( std::vector< std::string >, promise.set_value( daemon.ListHosts() ) );
}
*/

} // namespace service
} // namespace mega
