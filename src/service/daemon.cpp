
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

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context& yield_ctx ) override
    {
        return network::host_daemon::Impl::dispatchRequest( msg, *this, yield_ctx )
               || network::worker_daemon::Impl::dispatchRequest( msg, *this, yield_ctx )
               || network::root_daemon::Impl::dispatchRequest( msg, *this, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connectionID,
                        const std::string&           strErrorMsg,
                        boost::asio::yield_context&  yield_ctx ) override
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
            SPDLOG_ERROR( "Cannot resolve connection in error handler" );
            THROW_RTE( "Daemon Critical error in error handler" );
        }
    }

    // helpers
    network::daemon_root::Request_Encode getRootRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::daemon_root::Request_Encode( *this, m_daemon.m_rootClient.getSocket(), yield_ctx );
    }
    network::root_daemon::Response_Encode getRootResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::root_daemon::Response_Encode( *this, m_daemon.m_rootClient.getSocket(), yield_ctx );
    }

    network::host_daemon::Response_Encode getOriginatingHostResponse( boost::asio::yield_context& yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_hostServer.getConnection( getOriginatingEndPointID().value() ) )
        {
            return network::host_daemon::Response_Encode( *this, pConnection->getSocket(), yield_ctx );
        }
        THROW_RTE( "Could not locate originating connection" );
    }

    network::worker_daemon::Response_Encode getOriginatingWorkerResponse( boost::asio::yield_context& yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_workerServer.getConnection( getOriginatingEndPointID().value() ) )
        {
            return network::worker_daemon::Response_Encode( *this, pConnection->getSocket(), yield_ctx );
        }
        THROW_RTE( "Could not locate originating connection" );
    }

    network::daemon_worker::Request_Encode getOriginatingWorkerRequest( boost::asio::yield_context& yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_daemon.m_workerServer.getConnection( getOriginatingEndPointID().value() ) )
        {
            return network::daemon_worker::Request_Encode( *this, pConnection->getSocket(), yield_ctx );
        }
        THROW_RTE( "Could not locate originating connection" );
    }

    network::daemon_worker::Request_Encode getWorkerRequest( network::Server::Connection::Ptr pWorker,
                                                             boost::asio::yield_context&      yield_ctx )
    {
        return network::daemon_worker::Request_Encode( *this, pWorker->getSocket(), yield_ctx );
    }

    network::daemon_host::Request_Encode getHostRequest( network::Server::Connection::Ptr pHost,
                                                         boost::asio::yield_context&      yield_ctx )
    {
        return network::daemon_host::Request_Encode( *this, pHost->getSocket(), yield_ctx );
    }

    // network::host_daemon::Impl
    virtual void GetVersion( boost::asio::yield_context& yield_ctx ) override
    {
        getOriginatingHostResponse( yield_ctx ).GetVersion( getRootRequest( yield_ctx ).GetVersion() );
    }

    virtual void ListHosts( boost::asio::yield_context& yield_ctx ) override
    {
        std::vector< std::string > hosts;
        for ( auto& [ id, pConnection ] : m_daemon.m_hostServer.getConnections() )
            hosts.push_back( pConnection->getName() );
        getOriginatingHostResponse( yield_ctx ).ListHosts( hosts );
    }

    virtual void ListActivities( boost::asio::yield_context& yield_ctx ) override
    {
        auto root   = getRootRequest( yield_ctx );
        auto result = root.ListActivities();
        getOriginatingHostResponse( yield_ctx ).ListActivities( result );
    }

    virtual void PipelineRun( const mega::pipeline::Configuration& configuration,
                              boost::asio::yield_context&          yield_ctx ) override
    {
        auto root   = getRootRequest( yield_ctx );
        auto result = root.PipelineRun( configuration );
        getOriginatingHostResponse( yield_ctx ).PipelineRun( result );
    }

    virtual void Shutdown( boost::asio::yield_context& yield_ctx ) override
    {
        // swapped order - response first - then send actual request
        getOriginatingHostResponse( yield_ctx ).Shutdown();
        getRootRequest( yield_ctx ).Shutdown();
    }

    // network::worker_daemon::Impl
    virtual void PipelineReadyForWork( const network::ActivityID&  rootActivityID,
                                       boost::asio::yield_context& yield_ctx ) override
    {
        getRootRequest( yield_ctx ).PipelineReadyForWork( rootActivityID );
        getOriginatingWorkerResponse( yield_ctx ).PipelineReadyForWork();
    }

    virtual void PipelineWorkProgress( const std::string& message, boost::asio::yield_context& yield_ctx ) override
    {
        getRootRequest( yield_ctx ).PipelineWorkProgress( message );
        getOriginatingWorkerResponse( yield_ctx ).PipelineWorkProgress();
    }

    // network::worker_daemon::Impl worker stash
    virtual void getBuildHashCode( const boost::filesystem::path& filePath,
                                   boost::asio::yield_context&    yield_ctx ) override
    {
        auto                 root     = getRootRequest( yield_ctx );
        const task::FileHash hashCode = root.getBuildHashCode( filePath );
        auto                 worker   = getOriginatingWorkerResponse( yield_ctx );
        worker.getBuildHashCode( hashCode );
    }

    virtual void setBuildHashCode( const boost::filesystem::path& filePath, const task::FileHash& hashCode,
                                   boost::asio::yield_context& yield_ctx ) override
    {
        auto root = getRootRequest( yield_ctx );
        root.setBuildHashCode( filePath, hashCode );
        auto worker = getOriginatingWorkerResponse( yield_ctx );
        worker.setBuildHashCode();
    }

    virtual void stash( const boost::filesystem::path& filePath, const task::DeterminantHash& determinant,
                        boost::asio::yield_context& yield_ctx ) override
    {
        auto root = getRootRequest( yield_ctx );
        root.stash( filePath, determinant );
        auto worker = getOriginatingWorkerResponse( yield_ctx );
        worker.stash();
    }

    virtual void restore( const boost::filesystem::path& filePath, const task::DeterminantHash& determinant,
                          boost::asio::yield_context& yield_ctx ) override
    {
        auto       root      = getRootRequest( yield_ctx );
        const bool bRestored = root.restore( filePath, determinant );
        auto       worker    = getOriginatingWorkerResponse( yield_ctx );
        worker.restore( bRestored );
    }

    // network::root_daemon::Impl
    virtual void ReportActivities( boost::asio::yield_context& yield_ctx ) override
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
            std::copy( hostActivities.begin(), hostActivities.end(), std::back_inserter( activities ) );
        }

        for ( const auto& [ id, pDeamon ] : m_daemon.m_workerServer.getConnections() )
        {
            auto worker           = getWorkerRequest( pDeamon, yield_ctx );
            auto workerActivities = worker.ReportActivities();
            std::copy( workerActivities.begin(), workerActivities.end(), std::back_inserter( activities ) );
        }

        getRootResponse( yield_ctx ).ReportActivities( activities );
    }

    virtual void PipelineStartJobs( const mega::pipeline::Configuration& configuration,
                                    const network::ActivityID&           rootActivityID,
                                    boost::asio::yield_context&          yield_ctx ) override
    {
        std::vector< network::ActivityID > allJobs;
        for ( auto& [ id, pWorker ] : m_daemon.m_workerServer.getConnections() )
        {
            auto worker = getWorkerRequest( pWorker, yield_ctx );
            auto jobs   = worker.PipelineStartJobs( configuration, rootActivityID );
            std::copy( jobs.begin(), jobs.end(), std::back_inserter( allJobs ) );
        }
        getRootResponse( yield_ctx ).PipelineStartJobs( allJobs );
    }

    virtual void PipelineStartTask( const mega::pipeline::TaskDescriptor& task,
                                    boost::asio::yield_context&           yield_ctx ) override
    {
        getRootResponse( yield_ctx )
            .PipelineStartTask( getOriginatingWorkerRequest( yield_ctx ).PipelineStartTask( task ) );
    }

    virtual void ExecuteShutdown( boost::asio::yield_context& yield_ctx ) override
    {
        for ( const auto& [ id, pDeamon ] : m_daemon.m_hostServer.getConnections() )
        {
            getHostRequest( pDeamon, yield_ctx ).ExecuteShutdown();
        }
        for ( const auto& [ id, pDeamon ] : m_daemon.m_workerServer.getConnections() )
        {
            getWorkerRequest( pDeamon, yield_ctx ).ExecuteShutdown();
        }
        getRootResponse( yield_ctx ).ExecuteShutdown();

        boost::asio::post( [ &daemon = m_daemon ]() { daemon.shutdown(); } );
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
    : m_ioContext( ioContext )
    , m_requestActivityFactory( *this )
    , m_activityManager( "daemon", ioContext )
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
    SPDLOG_INFO( "Daemon shutdown" );
}

void Daemon::shutdown()
{
    m_rootClient.stop();
    m_hostServer.stop();
    m_workerServer.stop();
}

} // namespace service
} // namespace mega