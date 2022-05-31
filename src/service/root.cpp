
#include "service/root.hpp"

#include "pipeline/task.hpp"
#include "service/network/activity.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/messages.hxx"
#include "service/protocol/model/root_daemon.hxx"
#include "service/protocol/model/daemon_root.hxx"

#include "pipeline/pipeline.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/system/detail/error_code.hpp>
#include <iostream>
#include <memory>

namespace mega
{
namespace service
{

    

class RootRequestActivity : public network::Activity, public network::daemon_root::Impl
{
protected:
    Root& m_root;

public:
    RootRequestActivity( Root&                        root,
                         const network::ActivityID&   activityID,
                         const network::ConnectionID& originatingConnectionID )
        : Activity( root.m_activityManager, activityID, originatingConnectionID )
        , m_root( root )
    {
    }

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context yield_ctx ) override
    {
        return network::Activity::dispatchRequest( msg, yield_ctx )
               || network::daemon_root::Impl::dispatchRequest( msg, *this, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connectionID,
                        const std::string&           strErrorMsg,
                        boost::asio::yield_context   yield_ctx ) override
    {
        if ( network::Server::Connection::Ptr pHostConnection = m_root.m_server.getConnection( connectionID ) )
        {
            network::sendErrorResponse( getActivityID(), pHostConnection->getSocket(), strErrorMsg, yield_ctx );
        }
        else
        {
            THROW_RTE( "Connection to daemon lost" );
        }
    }

    network::daemon_root::Response_Encode getOriginatingDaemonResponse( boost::asio::yield_context yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_root.m_server.getConnection( getOriginatingEndPointID().value() ) )
        {
            return network::daemon_root::Response_Encode( *this, pConnection->getSocket(), yield_ctx );
        }
        THROW_RTE( "Connection to daemon lost" );
    }

    network::root_daemon::Request_Encode getDaemonRequest( network::Server::Connection::Ptr pConnection,
                                                           boost::asio::yield_context       yield_ctx )
    {
        return network::root_daemon::Request_Encode( *this, pConnection->getSocket(), yield_ctx );
    }

    // network::daemon_root::Impl
    virtual void GetVersion( boost::asio::yield_context yield_ctx ) override
    {
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.GetVersion( network::getVersion() );
    }

    virtual void ListActivities( boost::asio::yield_context yield_ctx ) override
    {
        std::vector< network::ActivityID > activities;

        for ( const auto& id : m_activityManager.reportActivities() )
        {
            activities.push_back( id );
        }

        for ( const auto& [ id, pDeamon ] : m_root.m_server.getConnections() )
        {
            auto daemon           = getDaemonRequest( pDeamon, yield_ctx );
            auto daemonActivities = daemon.ReportActivities();
            daemon.Complete();
            std::copy( daemonActivities.begin(), daemonActivities.end(), std::back_inserter( activities ) );
        }

        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.ListActivities( activities );
    }

    virtual void getBuildHashCode( const boost::filesystem::path& filePath,
                                   boost::asio::yield_context     yield_ctx ) override
    {
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.getBuildHashCode( m_root.m_stash.getBuildHashCode( filePath ) );
    }

    virtual void setBuildHashCode( const boost::filesystem::path& filePath, const task::FileHash& hashCode,
                                   boost::asio::yield_context yield_ctx ) override
    {
        m_root.m_stash.setBuildHashCode( filePath, hashCode );
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.setBuildHashCode();
    }

    virtual void stash( const boost::filesystem::path& filePath, const task::DeterminantHash& determinant,
                        boost::asio::yield_context yield_ctx ) override
    {
        m_root.m_stash.stash( filePath, determinant );
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.stash();
    }

    virtual void restore( const boost::filesystem::path& filePath, const task::DeterminantHash& determinant,
                          boost::asio::yield_context yield_ctx ) override
    {
        const bool bRestored = m_root.m_stash.restore( filePath, determinant );
        auto       daemon    = getOriginatingDaemonResponse( yield_ctx );
        daemon.restore( bRestored );
    }
};

class RootPipelineActivity : public RootRequestActivity
{
    std::set< network::ActivityID > m_jobs;
    using TaskChannel = boost::asio::experimental::concurrent_channel< void(
        boost::system::error_code, mega::pipeline::TaskDescriptor ) >;
    TaskChannel m_taskReady;
    struct TaskCompletion
    {
        network::ActivityID            jobID;
        mega::pipeline::TaskDescriptor task;
        bool                           bSuccess;
    };
    using TaskCompletionChannel
        = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, TaskCompletion ) >;
    TaskCompletionChannel m_taskComplete;

public:
    RootPipelineActivity( Root&                        root,
                          const network::ActivityID&   activityID,
                          const network::ConnectionID& originatingConnectionID )
        : RootRequestActivity( root, activityID, originatingConnectionID )
        , m_taskReady( root.m_io_context, 16 )
        , m_taskComplete( root.m_io_context, 16 )
    {
    }

    virtual void PipelineRun( const mega::pipeline::Pipeline::ID& pipelineID,
                              const pipeline::Configuration&      configuration,
                              boost::asio::yield_context          yield_ctx ) override
    {
        SPDLOG_INFO( "Started pipeline: compiler" );

        m_root.m_stash.resetBuildHashCodes();

        for ( auto& [ id, pDaemon ] : m_root.m_server.getConnections() )
        {
            auto                                     daemon = getDaemonRequest( pDaemon, yield_ctx );
            const std::vector< network::ActivityID > jobs
                = daemon.PipelineStartJobs( pipelineID, configuration, getActivityID() );
            daemon.Complete();
            for ( const network::ActivityID& id : jobs )
                m_jobs.insert( id );
        }
        if ( m_jobs.empty() )
        {
            THROW_RTE( "Failed to find workers for pipeline" );
        }

        mega::pipeline::Pipeline::Ptr pPipeline = pipeline::Registry::getPipeline( pipelineID, configuration );
        if ( !pPipeline )
        {
            THROW_RTE( "Failed to load pipeline: " << pipelineID );
        }
        mega::pipeline::Schedule                   schedule = pPipeline->getSchedule();
        std::set< mega::pipeline::TaskDescriptor > scheduledTasks, activeTasks;
        bool                                       bScheduleFailed = false;
        {
            while ( !schedule.isComplete() && !bScheduleFailed )
            {
                if ( !bScheduleFailed )
                {
                    for ( const mega::pipeline::TaskDescriptor& task : schedule.getReady() )
                    {
                        if ( !scheduledTasks.count( task ) )
                        {
                            activeTasks.insert( task );
                            m_taskReady.async_send( boost::system::error_code(), task, yield_ctx );
                            scheduledTasks.insert( task );
                        }
                        if ( activeTasks.size() == m_jobs.size() )
                            break;
                    }
                }

                do
                {
                    if ( !activeTasks.empty() )
                    {
                        const TaskCompletion taskCompletion = m_taskComplete.async_receive( yield_ctx );
                        activeTasks.erase( taskCompletion.task );
                        if ( taskCompletion.bSuccess )
                        {
                            schedule.complete( taskCompletion.task );
                        }
                        else
                        {
                            SPDLOG_WARN( "Pipeline failed at task: {}", taskCompletion.task.getName() );
                            bScheduleFailed = true;
                        }
                    }
                } while ( m_taskComplete.ready() );
            }
        }

        // close out remaining active tasks
        while ( !activeTasks.empty() )
        {
            TaskCompletion taskCompletion = m_taskComplete.async_receive( yield_ctx );
            activeTasks.erase( taskCompletion.task );
            if ( taskCompletion.bSuccess )
            {
                schedule.complete( taskCompletion.task );
            }
            else
            {
                SPDLOG_WARN( "Pipeline failed at task: {}", taskCompletion.task.getName() );
            }
        }

        // send termination task to each job
        for ( const network::ActivityID& jobID : m_jobs )
        {
            m_taskReady.async_send( boost::system::error_code(), mega::pipeline::TaskDescriptor(), yield_ctx );
            m_taskComplete.async_receive( yield_ctx );
        }

        {
            std::ostringstream os;
            if ( bScheduleFailed )
                os << "Pipeline: " << pipelineID << " failed";
            else
                os << "Pipeline: " << pipelineID << " succeeded";
            auto daemon = getOriginatingDaemonResponse( yield_ctx );
            daemon.PipelineRun( os.str() );
        }
    }

    mega::pipeline::TaskDescriptor getTask( boost::asio::yield_context yield_ctx )
    {
        return m_taskReady.async_receive( yield_ctx );
    }

    void completeTask( const mega::pipeline::TaskDescriptor& task, bool bSuccess, boost::asio::yield_context yield_ctx )
    {
        m_taskComplete.async_send(
            boost::system::error_code(), TaskCompletion{ getActivityID(), task, bSuccess }, yield_ctx );
    }

    virtual void PipelineReadyForWork( const network::ActivityID& rootActivityID,
                                       boost::asio::yield_context yield_ctx ) override
    {
        std::shared_ptr< RootPipelineActivity > pCoordinator = std::dynamic_pointer_cast< RootPipelineActivity >(
            m_root.m_activityManager.findExistingActivity( rootActivityID ) );

        auto daemon = getOriginatingDaemonResponse( yield_ctx );

        const mega::pipeline::TaskDescriptor task = pCoordinator->getTask( yield_ctx );
        if ( task == mega::pipeline::TaskDescriptor() )
        {
            SPDLOG_TRACE( "PipelineReadyForWork: {} completed", getActivityID() );
            pCoordinator->completeTask( mega::pipeline::TaskDescriptor(), true, yield_ctx );
            daemon.PipelineReadyForWork( mega::pipeline::TaskDescriptor() );
        }
        else
        {
            SPDLOG_TRACE( "PipelineReadyForWork: {} task: {}", getActivityID(), task.getName() );
            daemon.PipelineReadyForWork( task );
        }
    }

    virtual void PipelineWorkProgress( const network::ActivityID&            rootActivityID,
                                       const mega::pipeline::TaskDescriptor& task, const std::string& strMessage,
                                       boost::asio::yield_context yield_ctx ) override
    {
        std::shared_ptr< RootPipelineActivity > pCoordinator = std::dynamic_pointer_cast< RootPipelineActivity >(
            m_root.m_activityManager.findExistingActivity( rootActivityID ) );
        SPDLOG_TRACE( "{}", strMessage );
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.PipelineWorkProgress();
    }

    virtual void PipelineWorkFailed( const network::ActivityID&            rootActivityID,
                                     const mega::pipeline::TaskDescriptor& task, const std::string& strMessage,
                                     boost::asio::yield_context yield_ctx ) override
    {
        std::shared_ptr< RootPipelineActivity > pCoordinator = std::dynamic_pointer_cast< RootPipelineActivity >(
            m_root.m_activityManager.findExistingActivity( rootActivityID ) );
        SPDLOG_INFO( "{}", strMessage );
        pCoordinator->completeTask( task, false, yield_ctx );
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.PipelineWorkFailed();
    }

    virtual void PipelineWorkCompleted( const network::ActivityID&            rootActivityID,
                                        const mega::pipeline::TaskDescriptor& task, const std::string& strMessage,
                                        boost::asio::yield_context yield_ctx ) override
    {
        std::shared_ptr< RootPipelineActivity > pCoordinator = std::dynamic_pointer_cast< RootPipelineActivity >(
            m_root.m_activityManager.findExistingActivity( rootActivityID ) );
        SPDLOG_INFO( "{}", strMessage );
        pCoordinator->completeTask( task, true, yield_ctx );
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.PipelineWorkCompleted();
    }
};

network::Activity::Ptr
Root::RootActivityFactory::createRequestActivity( const network::Header&       msgHeader,
                                                  const network::ConnectionID& originatingConnectionID ) const
{
    switch ( msgHeader.getMessageID() )
    {
        case network::daemon_root::MSG_PipelineReadyForWork_Request::ID:
        case network::daemon_root::MSG_PipelineWorkProgress_Request::ID:
        case network::daemon_root::MSG_PipelineWorkFailed_Request::ID:
        case network::daemon_root::MSG_PipelineWorkCompleted_Request::ID:
        case network::daemon_root::MSG_PipelineRun_Request::ID:
            return network::Activity::Ptr(
                new RootPipelineActivity( m_root, msgHeader.getActivityID(), originatingConnectionID ) );
        default:
            return network::Activity::Ptr(
                new RootRequestActivity( m_root, msgHeader.getActivityID(), originatingConnectionID ) );
    }
}

Root::Root( boost::asio::io_context& ioContext )
    : m_io_context( ioContext )
    , m_activityFactory( *this )
    , m_activityManager( ioContext )
    , m_server( ioContext, m_activityManager, m_activityFactory, network::MegaRootPort() )
    , m_stash( boost::filesystem::current_path() / "stash" )
{
    m_server.waitForConnection();
}

} // namespace service
} // namespace mega