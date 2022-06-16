
#include "service/root.hpp"

#include "pipeline/task.hpp"

#include "service/network/conversation.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/common/pipeline_result.hpp"

#include "service/protocol/model/root_daemon.hxx"
#include "service/protocol/model/daemon_root.hxx"

#include "version/version.hpp"

#include "pipeline/pipeline.hpp"

#include "spdlog/fmt/chrono.h"
#include "spdlog/stopwatch.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/detail/error_code.hpp>
#include "boost/asio/experimental/concurrent_channel.hpp"

#include <iostream>
#include <memory>

namespace mega
{
namespace service
{

class RootRequestConversation : public network::Conversation, public network::daemon_root::Impl
{
protected:
    Root& m_root;

public:
    RootRequestConversation( Root&                          root,
                             const network::ConversationID& conversationID,
                             const network::ConnectionID&   originatingConnectionID )
        : Conversation( root, conversationID, originatingConnectionID )
        , m_root( root )
    {
    }

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context& yield_ctx ) override
    {
        return network::daemon_root::Impl::dispatchRequest( msg, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connectionID,
                        const std::string&           strErrorMsg,
                        boost::asio::yield_context&  yield_ctx ) override
    {
        if ( network::Server::Connection::Ptr pHostConnection = m_root.m_server.getConnection( connectionID ) )
        {
            pHostConnection->sendErrorResponse( getID(), strErrorMsg, yield_ctx );
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
            return network::daemon_root::Response_Encode( *this, *pConnection, yield_ctx );
        }
        THROW_RTE( "Connection to daemon lost" );
    }

    network::root_daemon::Request_Encode getOriginatingDaemonRequest( boost::asio::yield_context& yield_ctx )
    {
        if ( network::Server::Connection::Ptr pConnection
             = m_root.m_server.getConnection( getOriginatingEndPointID().value() ) )
        {
            return network::root_daemon::Request_Encode( *this, *pConnection, yield_ctx );
        }
        THROW_RTE( "Connection to daemon lost" );
    }

    network::root_daemon::Request_Encode getDaemonRequest( network::Server::Connection::Ptr pConnection,
                                                           boost::asio::yield_context&      yield_ctx )
    {
        return network::root_daemon::Request_Encode( *this, *pConnection, yield_ctx );
    }

    // network::daemon_root::Impl
    virtual void TermListNetworkNodes( boost::asio::yield_context& yield_ctx ) override
    {
        std::vector< std::string > nodes;
        nodes.push_back( getProcessName() );
        {
            for ( auto& [ id, pConnection ] : m_root.m_server.getConnections() )
            {
                network::root_daemon::Request_Encode rq( *this, *pConnection, yield_ctx );
                auto                                 r = rq.RootListNetworkNodes();
                std::copy( r.begin(), r.end(), std::back_inserter( nodes ) );
            }
        }
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.TermListNetworkNodes( nodes );
    }

    // network::daemon_root::Impl
    /*
    virtual void GetVersion( boost::asio::yield_context& yield_ctx ) override
    {
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.GetVersion( "0.0.0.0" );
    }

    virtual void ListActivities( boost::asio::yield_context& yield_ctx ) override
    {
        std::vector< network::ConversationID > activities;

        for ( const auto& id : m_conversationManager.reportActivities() )
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

        boost::asio::post( [ &root = m_root ]() { root.shutdown(); } );
    }*/

    // pipeline::Stash
    virtual void ExeGetBuildHashCode( const boost::filesystem::path& filePath,
                                      boost::asio::yield_context&    yield_ctx ) override
    {
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.ExeGetBuildHashCode( m_root.m_stash.getBuildHashCode( filePath ) );
    }

    virtual void ExeSetBuildHashCode( const boost::filesystem::path& filePath, const task::FileHash& hashCode,
                                      boost::asio::yield_context& yield_ctx ) override
    {
        m_root.m_stash.setBuildHashCode( filePath, hashCode );
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.ExeSetBuildHashCode();
    }

    virtual void ExeStash( const boost::filesystem::path& filePath, const task::DeterminantHash& determinant,
                           boost::asio::yield_context& yield_ctx ) override
    {
        m_root.m_stash.stash( filePath, determinant );
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.ExeStash();
    }

    virtual void ExeRestore( const boost::filesystem::path& filePath, const task::DeterminantHash& determinant,
                             boost::asio::yield_context& yield_ctx ) override
    {
        const bool bRestored = m_root.m_stash.restore( filePath, determinant );
        auto       daemon    = getOriginatingDaemonResponse( yield_ctx );
        daemon.ExeRestore( bRestored );
    }
};

class RootPipelineConversation : public RootRequestConversation, public pipeline::Progress, public pipeline::Stash
{
    std::set< network::ConversationID > m_jobs;

    using TaskChannel = boost::asio::experimental::concurrent_channel< void(
        boost::system::error_code, mega::pipeline::TaskDescriptor ) >;
    TaskChannel m_taskReady;

    struct TaskCompletion
    {
        network::ConversationID        jobID;
        mega::pipeline::TaskDescriptor task;
        bool                           bSuccess;
    };
    using TaskCompletionChannel
        = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, TaskCompletion ) >;
    TaskCompletionChannel m_taskComplete;

    static constexpr std::uint32_t CHANNEL_SIZE = 256;

public:
    RootPipelineConversation( Root&                          root,
                              const network::ConversationID& conversationID,
                              const network::ConnectionID&   originatingConnectionID )
        : RootRequestConversation( root, conversationID, originatingConnectionID )
        , m_taskReady( root.getIOContext(), CHANNEL_SIZE )
        , m_taskComplete( root.getIOContext(), CHANNEL_SIZE )
    {
    }

    // implement pipeline::Stash so that can create schedule - not actually used
    virtual task::FileHash getBuildHashCode( const boost::filesystem::path& filePath ) override
    {
        THROW_RTE( "Unreachable" );
    }
    virtual void setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode ) override
    {
        THROW_RTE( "Unreachable" );
    }
    virtual void stash( const boost::filesystem::path& filePath, task::DeterminantHash determinant ) override
    {
        THROW_RTE( "Unreachable" );
    }
    virtual bool restore( const boost::filesystem::path& filePath, task::DeterminantHash determinant ) override
    {
        THROW_RTE( "Unreachable" );
    }

    virtual void onStarted( const std::string& strMsg ) override { onProgress( strMsg ); }

    virtual void onProgress( const std::string& strMsg ) override { network::logLinesInfo( strMsg ); }

    virtual void onFailed( const std::string& strMsg ) override { onProgress( strMsg ); }

    virtual void onCompleted( const std::string& strMsg ) override { onProgress( strMsg ); }

    virtual void TermPipelineRun( const pipeline::Configuration& configuration,
                                  boost::asio::yield_context&    yield_ctx ) override
    {
        spdlog::stopwatch             sw;
        mega::pipeline::Pipeline::Ptr pPipeline;
        {
            std::ostringstream osLog;
            pPipeline = pipeline::Registry::getPipeline( configuration, osLog );
            if ( !pPipeline )
            {
                SPDLOG_ERROR( "Failed to load pipeline: {}", configuration.getPipelineID() );
                THROW_RTE( "Failed to load pipeline: " << configuration.get() );
            }
            else
            {
                SPDLOG_INFO( "{}", osLog.str() );
            }
        }

        m_root.m_stash.resetBuildHashCodes();

        for ( auto& [ id, pDaemon ] : m_root.m_server.getConnections() )
        {
            auto                                         daemon = getDaemonRequest( pDaemon, yield_ctx );
            const std::vector< network::ConversationID > jobs = daemon.RootPipelineStartJobs( configuration, getID() );
            for ( const network::ConversationID& id : jobs )
                m_jobs.insert( id );
        }
        if ( m_jobs.empty() )
        {
            SPDLOG_WARN( "Failed to find workers for pipeline: {}", configuration.getPipelineID() );
            THROW_RTE( "Failed to find workers for pipeline" );
        }
        SPDLOG_INFO( "Found {} jobs for pipeline {}", m_jobs.size(), configuration.getPipelineID() );

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
                    if ( !m_taskComplete.ready() )
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
        for ( const network::ConversationID& jobID : m_jobs )
        {
            m_taskReady.async_send( boost::system::error_code(), mega::pipeline::TaskDescriptor(), yield_ctx );
        }
        for ( const network::ConversationID& jobID : m_jobs )
        {
            m_taskComplete.async_receive( yield_ctx );
        }

        {
            std::ostringstream os;
            auto               daemon = getOriginatingDaemonResponse( yield_ctx );
            if ( bScheduleFailed )
            {
                SPDLOG_INFO( "FAILURE: Pipeline {} failed in: {}ms", configuration.getPipelineID(),
                             std::chrono::duration_cast< network::LogTime >( sw.elapsed() ) );
                os << "Pipeline: " << configuration.getPipelineID() << " failed";
                daemon.TermPipelineRun( network::PipelineResult( false, os.str() ) );
            }
            else
            {
                SPDLOG_INFO( "SUCCESS: Pipeline {} succeeded in: {}", configuration.getPipelineID(),
                             std::chrono::duration_cast< network::LogTime >( sw.elapsed() ) );
                os << "Pipeline: " << configuration.getPipelineID() << " succeeded";
                daemon.TermPipelineRun( network::PipelineResult( true, os.str() ) );
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
        m_taskComplete.async_send( boost::system::error_code(), TaskCompletion{ getID(), task, bSuccess }, yield_ctx );
    }

    virtual void ExePipelineReadyForWork( const network::ConversationID& rootConversationID,
                                          boost::asio::yield_context&    yield_ctx ) override
    {
        std::shared_ptr< RootPipelineConversation > pCoordinator
            = std::dynamic_pointer_cast< RootPipelineConversation >(
                m_root.findExistingConversation( rootConversationID ) );
        VERIFY_RTE( pCoordinator );

        {
            auto              daemonRequest = getOriginatingDaemonRequest( yield_ctx );
            spdlog::stopwatch sw;
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
                        sw.reset();
                        network::PipelineResult result = daemonRequest.RootPipelineStartTask( task );
                        network::logLinesSuccessFail( result.getMessage(), result.getSuccess(),
                                                      std::chrono::duration_cast< network::LogTime >( sw.elapsed() ) );
                        pCoordinator->completeTask( task, result.getSuccess(), yield_ctx );
                    }
                    catch ( std::exception& ex )
                    {
                        network::logLinesWarn( task.getName(), ex.what() );
                        pCoordinator->completeTask( task, false, yield_ctx );
                    }
                }
            }
        }

        getOriginatingDaemonResponse( yield_ctx ).ExePipelineReadyForWork();
    }

    virtual void ExePipelineWorkProgress( const std::string&          strMessage,
                                          boost::asio::yield_context& yield_ctx ) override
    {
        network::logLinesInfo( strMessage );
        getOriginatingDaemonResponse( yield_ctx ).ExePipelineWorkProgress();
    }
};

Root::Root( boost::asio::io_context& ioContext )
    : network::ConversationManager( network::Node::toStr( network::Node::Root ), ioContext )
    , m_server( ioContext, *this, network::MegaRootPort() )
    , m_stash( boost::filesystem::current_path() / "stash" )
{
    m_server.waitForConnection();
}

void Root::shutdown()
{
    m_server.stop();
    SPDLOG_INFO( "Root shutdown" );
}

network::ConversationBase::Ptr Root::joinConversation( const network::ConnectionID&   originatingConnectionID,
                                                       const network::Header&         header,
                                                       const network::MessageVariant& msg )
{
    switch ( header.getMessageID() )
    {
        case network::daemon_root::MSG_ExePipelineReadyForWork_Request::ID:
        case network::daemon_root::MSG_TermPipelineRun_Request::ID:
            return network::Conversation::Ptr(
                new RootPipelineConversation( *this, header.getConversationID(), originatingConnectionID ) );
        default:
            return network::Conversation::Ptr(
                new RootRequestConversation( *this, header.getConversationID(), originatingConnectionID ) );
    }
}

} // namespace service
} // namespace mega