
#include "service/daemon.hpp"

#include "service/network/activity.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/model/daemon_host.hxx"
#include "service/protocol/model/daemon_worker.hxx"
#include "service/protocol/model/host_daemon.hxx"
#include "service/protocol/model/messages.hxx"
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
class DaemonRequestActivity : public network::Activity,
                              public network::host_daemon::Impl,
                              public network::worker_daemon::Impl,
                              public network::root_daemon::Impl
{
protected:
    Daemon& m_daemon;

public:
    DaemonRequestActivity( Daemon&                      daemon,
                           const network::ActivityID&   activityID,
                           const network::ConnectionID& originatingConnectionID )
        : Activity( daemon.m_activityManager, activityID, originatingConnectionID )
        , m_daemon( daemon )
    {
    }

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context yield_ctx ) override
    {
        return network::Activity::dispatchRequest( msg, yield_ctx )
               || network::host_daemon::Impl::dispatchRequest( msg, *this, yield_ctx )
               || network::worker_daemon::Impl::dispatchRequest( msg, *this, yield_ctx )
               || network::root_daemon::Impl::dispatchRequest( msg, *this, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connectionID,
                        const std::string&           strErrorMsg,
                        boost::asio::yield_context   yield_ctx ) override
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

    // helpers
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

    network::worker_daemon::Response_Encode getOriginatingWorkerResponse( boost::asio::yield_context yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_workerServer.getConnection( getOriginatingEndPointID().value() ) )
        {
            return network::worker_daemon::Response_Encode( *this, pConnection->getSocket(), yield_ctx );
        }
        THROW_RTE( "Could not locate originating connection" );
    }

    network::worker_daemon::Response_Encode getMostRecentWorkerResponse( boost::asio::yield_context yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_workerServer.getConnection( getMostRecentRequestConnectionID() ) )
        {
            return network::worker_daemon::Response_Encode( *this, pConnection->getSocket(), yield_ctx );
        }
        THROW_RTE( "Could not locate originating connection" );
    }

    network::daemon_worker::Request_Encode getMostRecentWorkerRequest( boost::asio::yield_context yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_workerServer.getConnection( getMostRecentRequestConnectionID() ) )
        {
            return network::daemon_worker::Request_Encode( *this, pConnection->getSocket(), yield_ctx );
        }
        THROW_RTE( "Could not locate originating connection" );
    }

    network::daemon_worker::Request_Encode getWorkerRequest( network::Server::Connection::Ptr pWorker,
                                                             boost::asio::yield_context       yield_ctx )
    {
        return network::daemon_worker::Request_Encode( *this, pWorker->getSocket(), yield_ctx );
    }

    network::daemon_host::Request_Encode getHostRequest( network::Server::Connection::Ptr pHost,
                                                         boost::asio::yield_context       yield_ctx )
    {
        return network::daemon_host::Request_Encode( *this, pHost->getSocket(), yield_ctx );
    }

    // network::host_daemon::Impl
    virtual void GetVersion( boost::asio::yield_context yield_ctx ) override
    {
        auto        root                    = getRootRequest( yield_ctx );
        std::string strMegaStructureVersion = root.GetVersion();
        root.Complete();
        getOriginatingHostResponse( yield_ctx ).GetVersion( strMegaStructureVersion );
    }

    virtual void ListHosts( boost::asio::yield_context yield_ctx ) override
    {
        std::vector< std::string > hosts;
        for ( auto& [ id, pConnection ] : m_daemon.m_hostServer.getConnections() )
            hosts.push_back( pConnection->getName() );
        getOriginatingHostResponse( yield_ctx ).ListHosts( hosts );
    }

    virtual void ListActivities( boost::asio::yield_context yield_ctx ) override
    {
        auto root   = getRootRequest( yield_ctx );
        auto result = root.ListActivities();
        root.Complete();
        getOriginatingHostResponse( yield_ctx ).ListActivities( result );
    }

    virtual void PipelineRun( const mega::pipeline::Pipeline::ID&  pipelineID,
                              const mega::pipeline::Configuration& configuration,
                              boost::asio::yield_context           yield_ctx ) override
    {
        auto        root                  = getRootRequest( yield_ctx );
        std::string strTestPipelineResult = root.PipelineRun( pipelineID, configuration );
        root.Complete();
        getOriginatingHostResponse( yield_ctx ).PipelineRun( strTestPipelineResult );
    }

    // network::worker_daemon::Impl
    virtual void PipelineReadyForWork( const network::ActivityID& rootActivityID,
                                       boost::asio::yield_context yield_ctx ) override
    {
        SPDLOG_INFO( "PipelineReadyForWork: {}", getActivityID() );
        auto                           root = getRootRequest( yield_ctx );
        mega::pipeline::TaskDescriptor task = root.PipelineReadyForWork( rootActivityID );
        root.Complete();
        auto worker = getOriginatingWorkerResponse( yield_ctx );
        worker.PipelineReadyForWork( task );
    }

    virtual void PipelineWorkProgress( const network::ActivityID&            rootActivityID,
                                       const mega::pipeline::TaskDescriptor& task,
                                       const std::string&                    strMessage,
                                       boost::asio::yield_context            yield_ctx ) override
    {
        auto root = getRootRequest( yield_ctx );
        root.PipelineWorkProgress( rootActivityID, task, strMessage );
        root.Complete();
        auto worker = getOriginatingWorkerResponse( yield_ctx );
        worker.PipelineWorkProgress();
    }

    virtual void PipelineWorkFailed( const network::ActivityID&            rootActivityID,
                                     const mega::pipeline::TaskDescriptor& task, const std::string& strMessage,
                                     boost::asio::yield_context yield_ctx ) override
    {
        auto root = getRootRequest( yield_ctx );
        root.PipelineWorkFailed( rootActivityID, task, strMessage );
        root.Complete();
        auto worker = getOriginatingWorkerResponse( yield_ctx );
        worker.PipelineWorkFailed();
    }

    virtual void PipelineWorkCompleted( const network::ActivityID&            rootActivityID,
                                        const mega::pipeline::TaskDescriptor& task, const std::string& strMessage,
                                        boost::asio::yield_context yield_ctx ) override
    {
        auto root = getRootRequest( yield_ctx );
        root.PipelineWorkCompleted( rootActivityID, task, strMessage );
        root.Complete();
        auto worker = getOriginatingWorkerResponse( yield_ctx );
        worker.PipelineWorkCompleted();
    }

    // network::root_daemon::Impl
    virtual void ReportActivities( boost::asio::yield_context yield_ctx ) override
    {
        std::vector< network::ActivityID > activities;

        for ( const auto& id : m_activityManager.reportActivities() )
        {
            activities.push_back( id );
        }

        for ( const auto& [ id, pDeamon ] : m_daemon.m_hostServer.getConnections() )
        {
            auto host           = getHostRequest( pDeamon, yield_ctx );
            auto hostActivities = host.ReportActivities();
            host.Complete();
            std::copy( hostActivities.begin(), hostActivities.end(), std::back_inserter( activities ) );
        }

        for ( const auto& [ id, pDeamon ] : m_daemon.m_workerServer.getConnections() )
        {
            auto worker           = getWorkerRequest( pDeamon, yield_ctx );
            auto workerActivities = worker.ReportActivities();
            worker.Complete();
            std::copy( workerActivities.begin(), workerActivities.end(), std::back_inserter( activities ) );
        }

        getRootResponse( yield_ctx ).ReportActivities( activities );
    }

    virtual void ListWorkers( boost::asio::yield_context yield_ctx ) override
    {
        int iTotalThreads = 0;
        for ( auto& [ id, pWorker ] : m_daemon.m_workerServer.getConnections() )
        {
            auto worker = getWorkerRequest( pWorker, yield_ctx );
            iTotalThreads += worker.ListThreads();
            worker.Complete();
        }
        getRootResponse( yield_ctx ).ListWorkers( iTotalThreads );
    }

    virtual void PipelineStartJobs( const mega::pipeline::Pipeline::ID&  pipelineID,
                                    const mega::pipeline::Configuration& configuration,
                                    const network::ActivityID&           rootActivityID,
                                    boost::asio::yield_context           yield_ctx ) override
    {
        std::vector< network::ActivityID > allJobs;
        for ( auto& [ id, pWorker ] : m_daemon.m_workerServer.getConnections() )
        {
            auto                               worker = getWorkerRequest( pWorker, yield_ctx );
            std::vector< network::ActivityID > jobs
                = worker.PipelineStartJobs( pipelineID, configuration, rootActivityID );
            worker.Complete();
            std::copy( jobs.begin(), jobs.end(), std::back_inserter( allJobs ) );
        }
        getRootResponse( yield_ctx ).PipelineStartJobs( allJobs );
    }
};

network::Activity::Ptr
Daemon::RequestActivityFactory::createRequestActivity( const network::Header&       msgHeader,
                                                       const network::ConnectionID& originatingConnectionID ) const
{
    switch ( msgHeader.getMessageID() )
    {
        default:
            return network::Activity::Ptr(
                new DaemonRequestActivity( m_daemon, msgHeader.getActivityID(), originatingConnectionID ) );
    }
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