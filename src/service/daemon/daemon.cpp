
#include "service/daemon/daemon.hpp"

#include "service/network/activity.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"

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

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context yield_ctx )
    {
        return network::Activity::dispatchRequest( msg, yield_ctx )
               || network::host_daemon::Impl::dispatchRequest( msg, *this, yield_ctx )
               || network::worker_daemon::Impl::dispatchRequest( msg, *this, yield_ctx )
               || network::root_daemon::Impl::dispatchRequest( msg, *this, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connectionID, const std::string& strErrorMsg, boost::asio::yield_context yield_ctx )
    {
        if ( network::getConnectionID( m_daemon.m_rootClient.getSocket() ) == connectionID )
        {
            network::sendErrorResponse( getActivityID(), m_daemon.m_rootClient.getSocket(), strErrorMsg, yield_ctx );
        }
        else if ( network::Server::Connection::Ptr pHostConnection
                  = m_daemon.m_hostServer.getConnection( connectionID ) )
        {
            network::sendErrorResponse( getActivityID(), pHostConnection->getSocket(), strErrorMsg, yield_ctx );
        }
        else if ( network::Server::Connection::Ptr pWorkerConnection
                  = m_daemon.m_workerServer.getConnection( connectionID ) )
        {
            network::sendErrorResponse( getActivityID(), pWorkerConnection->getSocket(), strErrorMsg, yield_ctx );
        }
        else
        {
            // ?
        }
    }

    network::daemon_root::Request_Encode getRootRequest( boost::asio::yield_context yield_ctx )
    {
        return network::daemon_root::Request_Encode( *this, m_daemon.m_rootClient.getSocket(), yield_ctx );
    }
    network::root_daemon::Response_Encode getRootResponse( boost::asio::yield_context yield_ctx )
    {
        return network::root_daemon::Response_Encode( *this, m_daemon.m_rootClient.getSocket(), yield_ctx );
    }

    network::host_daemon::Response_Encode getOriginatingHostResponse( boost::asio::yield_context yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_hostServer.getConnection( getOriginatingEndPointID().value() ) )
        {
            return network::host_daemon::Response_Encode( *this, pConnection->getSocket(), yield_ctx );
        }
        THROW_RTE( "Could not locate originating connection" );
    }

    // network::host_daemon::Impl
    virtual void GetVersion( boost::asio::yield_context yield_ctx )
    {
        auto        root                    = getRootRequest( yield_ctx );
        std::string strMegaStructureVersion = root.GetVersion();
        root.Complete();
        getOriginatingHostResponse( yield_ctx ).GetVersion( strMegaStructureVersion );
    }

    virtual void ListHosts( boost::asio::yield_context yield_ctx )
    {
        std::vector< std::string > hosts;
        for ( auto& [ id, pConnection ] : m_daemon.m_hostServer.getConnections() )
            hosts.push_back( pConnection->getName() );
        getOriginatingHostResponse( yield_ctx ).ListHosts( hosts );
    }

    virtual void runTestPipeline( boost::asio::yield_context yield_ctx )
    {
        network::daemon_root::Request_Encode root( *this, m_daemon.m_rootClient.getSocket(), yield_ctx );
        std::string                          strTestPipelineResult = root.runTestPipeline();
        root.Complete();
        getOriginatingHostResponse( yield_ctx ).runTestPipeline( strTestPipelineResult );
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

        getRootResponse( yield_ctx ).ListWorkers( iTotalThreads );
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