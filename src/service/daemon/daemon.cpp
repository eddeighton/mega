
#include "service/daemon/daemon.hpp"

#include "service/network/activity.hpp"
#include "service/network/network.hpp"

#include "service/protocol/model/daemon_worker.hxx"
#include "service/protocol/model/host_daemon.hxx"
#include "service/protocol/model/worker_daemon.hxx"
#include "service/protocol/model/root_daemon.hxx"
#include "service/protocol/model/daemon_root.hxx"

#include <iostream>

namespace mega
{
namespace service
{

////////////////////////////////////////////////////////////////
// HostRequestActivity
class RequestActivity : public network::Activity,
                        public network::host_daemon::Impl,
                        public network::worker_daemon::Impl,
                        public network::root_daemon::Impl
{
    Daemon& m_daemon;

public:
    RequestActivity( Daemon&                      daemon,
                     const network::ActivityID&   activityID,
                     const network::ConnectionID& originatingConnectionID )
        : Activity( daemon.m_activityManager, activityID, originatingConnectionID )
        , m_daemon( daemon )
    {
    }

    virtual bool dispatch( const network::MessageVariant& msg, boost::asio::yield_context yield_ctx )
    {
        return network::Activity::dispatch( msg, yield_ctx )
               || network::host_daemon::Impl::dispatch( msg, *this, yield_ctx )
               || network::worker_daemon::Impl::dispatch( msg, *this, yield_ctx )
               || network::root_daemon::Impl::dispatch( msg, *this, yield_ctx );
    }

    // network::host_daemon::Impl
    virtual void GetVersion( boost::asio::yield_context yield_ctx )
    {
        network::daemon_root::Request_Encode root( *this, m_daemon.m_rootClient.getSocket(), yield_ctx );
        std::string                          strMegaStructureVersion = root.GetVersion();
        root.Complete();
        // std::cout << "Got version from Root of: " << strMegaStructureVersion << std::endl;

        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_hostServer.getConnection( getOriginatingEndPointID().value() ) )
        {
            network::host_daemon::Response_Encode hostResponse( *this, pConnection->getSocket(), yield_ctx );
            hostResponse.GetVersion( strMegaStructureVersion );
        }
    }
    virtual void ListHosts( boost::asio::yield_context yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_hostServer.getConnection( getOriginatingEndPointID().value() ) )
        {
            network::host_daemon::Response_Encode hostResponse( *this, pConnection->getSocket(), yield_ctx );
            std::vector< std::string >            hosts;
            for ( auto& [ id, pConnection ] : m_daemon.m_hostServer.getConnections() )
                hosts.push_back( pConnection->getName() );
            hostResponse.ListHosts( hosts );
        }
    }
    virtual void runTestPipeline( boost::asio::yield_context yield_ctx )
    {
        network::daemon_root::Request_Encode root( *this, m_daemon.m_rootClient.getSocket(), yield_ctx );
        std::string                          strTestPipelineResult = root.runTestPipeline();
        root.Complete();

        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_hostServer.getConnection( getOriginatingEndPointID().value() ) )
        {
            network::host_daemon::Response_Encode hostResponse( *this, pConnection->getSocket(), yield_ctx );
            hostResponse.runTestPipeline( strTestPipelineResult );
        }
    }

    // network::worker_daemon::Impl

    // network::root_daemon::Impl
    virtual void ListWorkers( boost::asio::yield_context yield_ctx )
    {
        int iTotalThreads = 0;
        for ( auto& [ id, pWorker ] : m_daemon.m_workerServer.getConnections() )
        {
            network::daemon_worker::Request_Encode worker( *this, pWorker->getSocket(), yield_ctx );
            iTotalThreads += worker.ListThreads();
            worker.Complete();
        }

        network::root_daemon::Response_Encode rootResponse( *this, m_daemon.m_rootClient.getSocket(), yield_ctx );
        rootResponse.ListWorkers( iTotalThreads );
    }
};

network::Activity::Ptr
Daemon::RequestActivityFactory::createRequestActivity( const network::ActivityID&   activityID,
                                                       const network::ConnectionID& originatingConnectionID ) const
{
    return network::Activity::Ptr( new RequestActivity( m_daemon, activityID, originatingConnectionID ) );
}

////////////////////////////////////////////////////////////////
// Daemon
Daemon::Daemon( boost::asio::io_context& ioContext, const std::string& strRootIP )
    : m_requestActivityFactory( *this )
    , m_activityManager( ioContext )
    , m_rootClient(
          ioContext, m_activityManager, m_requestActivityFactory, strRootIP, mega::network::MegaRootServiceName() )
    , m_hostServer( ioContext, m_activityManager, m_requestActivityFactory, network::MegaDaemonPort() )
    , m_workerServer( ioContext, m_activityManager, m_requestActivityFactory, network::MegaWorkerPort() )
{
    m_hostServer.waitForConnection();
    m_workerServer.waitForConnection();
}

Daemon::~Daemon()
{
    m_rootClient.stop();
    m_hostServer.stop();
    m_workerServer.stop();
}

} // namespace service
} // namespace mega