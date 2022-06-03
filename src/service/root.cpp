
#include "service/root.hpp"

#include "pipeline/task.hpp"
#include "service/network/activity.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/common/pipeline_result.hpp"
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

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context& yield_ctx ) override
    {
        return network::daemon_root::Impl::dispatchRequest( msg, *this, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connectionID,
                        const std::string&           strErrorMsg,
                        boost::asio::yield_context&  yield_ctx ) override
    {
        if ( network::Server::Connection::Ptr pHostConnection = m_root.m_server.getConnection( connectionID ) )
        {
            network::sendErrorResponse( getActivityID(), pHostConnection->getSocket(), strErrorMsg, yield_ctx );
        }
        else
        {
            SPDLOG_ERROR( "Cannot resolve connection in error handler" );
            THROW_RTE( "Root Critical error in error handler" );
        }
    }

    network::daemon_root::Response_Encode getOriginatingDaemonResponse( boost::asio::yield_context& yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_root.m_server.getConnection( getOriginatingEndPointID().value() ) )
        {
            return network::daemon_root::Response_Encode( *this, pConnection->getSocket(), yield_ctx );
        }
        THROW_RTE( "Connection to daemon lost" );
    }

    network::root_daemon::Request_Encode getOriginatingDaemonRequest( boost::asio::yield_context& yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_root.m_server.getConnection( getOriginatingEndPointID().value() ) )
        {
            return network::root_daemon::Request_Encode( *this, pConnection->getSocket(), yield_ctx );
        }
        THROW_RTE( "Connection to daemon lost" );
    }

    network::root_daemon::Request_Encode getDaemonRequest( network::Server::Connection::Ptr pConnection,
                                                           boost::asio::yield_context&      yield_ctx )
    {
        return network::root_daemon::Request_Encode( *this, pConnection->getSocket(), yield_ctx );
    }

    // network::daemon_root::Impl
    virtual void GetVersion( boost::asio::yield_context& yield_ctx ) override
    {
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.GetVersion( network::getVersion() );
    }

    virtual void ListActivities( boost::asio::yield_context& yield_ctx ) override
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
            std::copy( daemonActivities.begin(), daemonActivities.end(), std::back_inserter( activities ) );
        }

        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.ListActivities( activities );
    }

    virtual void Shutdown( boost::asio::yield_context& yield_ctx ) override
    {
        // response straight away - then execute shutdown
        getOriginatingDaemonResponse( yield_ctx ).Shutdown();
        for ( const auto& [ id, pDeamon ] : m_root.m_server.getConnections() )
        {
            getDaemonRequest( pDeamon, yield_ctx ).ExecuteShutdown();
        }
        m_root.shutdown();
    }

    // pipeline::Stash

    virtual void getBuildHashCode( const boost::filesystem::path& filePath,
                                   boost::asio::yield_context&    yield_ctx ) override
    {
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.getBuildHashCode( m_root.m_stash.getBuildHashCode( filePath ) );
    }

    virtual void setBuildHashCode( const boost::filesystem::path& filePath, const task::FileHash& hashCode,
                                   boost::asio::yield_context& yield_ctx ) override
    {
        m_root.m_stash.setBuildHashCode( filePath, hashCode );
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.setBuildHashCode();
    }

    virtual void stash( const boost::filesystem::path& filePath, const task::DeterminantHash& determinant,
                        boost::asio::yield_context& yield_ctx ) override
    {
        m_root.m_stash.stash( filePath, determinant );
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.stash();
    }

    virtual void restore( const boost::filesystem::path& filePath, const task::DeterminantHash& determinant,
                          boost::asio::yield_context& yield_ctx ) override
    {
        const bool bRestored = m_root.m_stash.restore( filePath, determinant );
        auto       daemon    = getOriginatingDaemonResponse( yield_ctx );
        daemon.restore( bRestored );
    }
};

class RootPipelineActivity : public RootRequestActivity, public pipeline::Progress, public pipeline::Stash
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

    static constexpr std::uint32_t CHANNEL_SIZE = 256;
public:
    RootPipelineActivity( Root&                        root,
                          const network::ActivityID&   activityID,
                          const network::ConnectionID& originatingConnectionID )
        : RootRequestActivity( root, activityID, originatingConnectionID )
        , m_taskReady( root.m_io_context, CHANNEL_SIZE )
        , m_taskComplete( root.m_io_context, CHANNEL_SIZE )
    {
    }

    virtual task::FileHash getBuildHashCode( const boost::filesystem::path& filePath ) override
    {
        THROW_RTE( "Never used" );
        //return m_root.m_stash.getBuildHashCode( filePath );
    }
    virtual void setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode ) override
    {
        THROW_RTE( "Never used" );
        //m_root.m_stash.setBuildHashCode( filePath, hashCode );
    }
    virtual void stash( const boost::filesystem::path& filePath, task::DeterminantHash determinant ) override
    {
        THROW_RTE( "Never used" );
        //m_root.m_stash.stash( filePath, determinant );
    }
    virtual bool restore( const boost::filesystem::path& filePath, task::DeterminantHash determinant ) override
    {
        THROW_RTE( "Never used" );
        //return m_root.m_stash.restore( filePath, determinant );
    }

    virtual void onStarted( const std::string& strMsg ) override
    {
        onProgress( strMsg );
    }

    virtual void onProgress( const std::string& strMsg ) override
    {
        std::istringstream is( strMsg );
        while ( is )
        {
            std::string strLine;
            std::getline( is, strLine );
            if ( !strLine.empty() )
            {
                SPDLOG_INFO( "{}", strLine );
            }
        }
    }

    virtual void onFailed( const std::string& strMsg ) override
    {
        onProgress( strMsg );
    }

    virtual void onCompleted( const std::string& strMsg ) override
    {
        onProgress( strMsg );
    }


    virtual void PipelineRun( const pipeline::Configuration& configuration,
                              boost::asio::yield_context&    yield_ctx ) override
    {
        SPDLOG_INFO( "Started pipeline: {}", configuration.getPipelineID() );

        m_root.m_stash.resetBuildHashCodes();

        for ( auto& [ id, pDaemon ] : m_root.m_server.getConnections() )
        {
            auto                                     daemon = getDaemonRequest( pDaemon, yield_ctx );
            const std::vector< network::ActivityID > jobs = daemon.PipelineStartJobs( configuration, getActivityID() );
            for ( const network::ActivityID& id : jobs )
                m_jobs.insert( id );
        }
        if ( m_jobs.empty() )
        {
            THROW_RTE( "Failed to find workers for pipeline" );
        }

        mega::pipeline::Pipeline::Ptr pPipeline = pipeline::Registry::getPipeline( configuration );
        if ( !pPipeline )
        {
            THROW_RTE( "Failed to load pipeline: " << configuration.get() );
        }
        mega::pipeline::Schedule                   schedule = pPipeline->getSchedule( *this, *this );
        std::set< mega::pipeline::TaskDescriptor > scheduledTasks, activeTasks;
        bool                                       bScheduleFailed = false;
        {
            while ( !schedule.isComplete() && !bScheduleFailed )
            {
                for ( const mega::pipeline::TaskDescriptor& task : schedule.getReady() )
                {
                    VERIFY_RTE( task != mega::pipeline::TaskDescriptor() );
                    if ( activeTasks.size() < CHANNEL_SIZE )
                    {
                        if ( !scheduledTasks.count( task ) )
                        {
                            scheduledTasks.insert( task );
                            VERIFY_RTE( activeTasks.insert( task ).second );
                            m_taskReady.async_send( boost::system::error_code(), task, yield_ctx );
                        }
                    }
                    else
                        break;
                }

                while ( !activeTasks.empty() )
                {
                    const TaskCompletion taskCompletion = m_taskComplete.async_receive( yield_ctx );
                    VERIFY_RTE( activeTasks.erase( taskCompletion.task ) == 1U );
                    if ( taskCompletion.bSuccess )
                    {
                        schedule.complete( taskCompletion.task );
                    }
                    else
                    {
                        SPDLOG_WARN( "Pipeline failed at task: {}", taskCompletion.task.getName() );
                        bScheduleFailed = true;
                        break;
                    }
                    if( !m_taskComplete.ready() )
                        break;  
                }
            }
        }

        // close out remaining active tasks
        while ( !activeTasks.empty() )
        {
            TaskCompletion taskCompletion = m_taskComplete.async_receive( yield_ctx );
            VERIFY_RTE( activeTasks.erase( taskCompletion.task ) == 1U );
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
        }
        for ( const network::ActivityID& jobID : m_jobs )
        {
            m_taskComplete.async_receive( yield_ctx );
        }

        {
            std::ostringstream os;
            auto               daemon = getOriginatingDaemonResponse( yield_ctx );
            if ( bScheduleFailed )
            {
                os << "Pipeline: " << configuration.getPipelineID() << " failed";
                daemon.PipelineRun( network::PipelineResult( false, os.str() ) );
            }
            else
            {
                os << "Pipeline: " << configuration.getPipelineID() << " succeeded";
                daemon.PipelineRun( network::PipelineResult( true, os.str() ) );
            }
        }
    }

    mega::pipeline::TaskDescriptor getTask( boost::asio::yield_context& yield_ctx )
    {
        return m_taskReady.async_receive( yield_ctx );
    }

    void completeTask( const mega::pipeline::TaskDescriptor& task, bool bSuccess,
                       boost::asio::yield_context& yield_ctx )
    {
        m_taskComplete.async_send(
            boost::system::error_code(), TaskCompletion{ getActivityID(), task, bSuccess }, yield_ctx );
    }

    virtual void PipelineReadyForWork( const network::ActivityID&  rootActivityID,
                                       boost::asio::yield_context& yield_ctx ) override
    {
        std::shared_ptr< RootPipelineActivity > pCoordinator = std::dynamic_pointer_cast< RootPipelineActivity >(
            m_root.m_activityManager.findExistingActivity( rootActivityID ) );
        VERIFY_RTE( pCoordinator );
        
        {
            auto daemonRequest = getOriginatingDaemonRequest( yield_ctx );
            while ( true )
            {
                const mega::pipeline::TaskDescriptor task = pCoordinator->getTask( yield_ctx );
                if ( task == mega::pipeline::TaskDescriptor() )
                {
                    pCoordinator->completeTask( mega::pipeline::TaskDescriptor(), true, yield_ctx );
                    break;
                }
                else
                {
                    try
                    {
                        network::PipelineResult result = daemonRequest.PipelineStartTask( task );
                        {
                            std::istringstream is( result.getMessage() );
                            while ( is )
                            {
                                std::string strLine;
                                std::getline( is, strLine );
                                if ( !strLine.empty() )
                                {
                                    if ( result.getSuccess() )
                                        SPDLOG_INFO( "{}", strLine );
                                    else
                                        SPDLOG_WARN( "{}", strLine );
                                }
                            }
                        }
                        pCoordinator->completeTask( task, result.getSuccess(), yield_ctx );
                    }
                    catch ( std::exception& ex )
                    {
                        std::istringstream is( ex.what() );
                        while ( is )
                        {
                            std::string strLine;
                            std::getline( is, strLine );
                            if ( !strLine.empty() )
                            {
                                SPDLOG_WARN( "EXCEPTION: {} {}", task.getName(), strLine );
                            }
                        }
                        pCoordinator->completeTask( task, false, yield_ctx );
                    }
                }
            }
        }

        getOriginatingDaemonResponse( yield_ctx ).PipelineReadyForWork();
    }

    virtual void PipelineWorkProgress( const std::string& strMessage, boost::asio::yield_context& yield_ctx ) override
    {
        std::istringstream is( strMessage );
        while ( is )
        {
            std::string strLine;
            std::getline( is, strLine );
            if ( !strLine.empty() )
            {
                SPDLOG_INFO( "{}", strLine );
            }
        }
        getOriginatingDaemonResponse( yield_ctx ).PipelineWorkProgress();
    }
};

network::Activity::Ptr
Root::RootActivityFactory::createRequestActivity( const network::Header&       msgHeader,
                                                  const network::ConnectionID& originatingConnectionID ) const
{
    switch ( msgHeader.getMessageID() )
    {
        case network::daemon_root::MSG_PipelineReadyForWork_Request::ID:
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
    , m_activityManager( "root", ioContext )
    , m_server( ioContext, m_activityManager, m_activityFactory, network::MegaRootPort() )
    , m_stash( boost::filesystem::current_path() / "stash" )
{
    m_server.waitForConnection();
}

void Root::shutdown()
{
    m_server.stop();
    m_io_context.stop();

    SPDLOG_INFO( "Root shutdown" );
}

} // namespace service
} // namespace mega