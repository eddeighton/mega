#ifndef PIPELINE_22_SEPT_2022
#define PIPELINE_22_SEPT_2022

#include "request.hpp"

#include "service/network/log.hpp"

namespace mega
{
namespace service
{
class Root;

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

    static constexpr mega::U32 CHANNEL_SIZE = 256;

public:
    RootPipelineConversation( Root&                          root,
                              const network::ConversationID& conversationID,
                              const network::ConnectionID&   originatingConnectionID );

    // implement pipeline::Stash so that can create schedule - not actually used
    virtual task::FileHash getBuildHashCode( const boost::filesystem::path& filePath ) override
    {
        THROW_RTE( "Root: Unreachable" );
    }
    virtual void setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode ) override
    {
        THROW_RTE( "Root: Unreachable" );
    }
    virtual void stash( const boost::filesystem::path& filePath, task::DeterminantHash determinant ) override
    {
        THROW_RTE( "Root: Unreachable" );
    }
    virtual bool restore( const boost::filesystem::path& filePath, task::DeterminantHash determinant ) override
    {
        THROW_RTE( "Root: Unreachable" );
    }

    virtual void onStarted( const std::string& strMsg ) override { onProgress( strMsg ); }
    virtual void onProgress( const std::string& strMsg ) override { network::logLinesInfo( strMsg ); }
    virtual void onFailed( const std::string& strMsg ) override { onProgress( strMsg ); }
    virtual void onCompleted( const std::string& strMsg ) override { onProgress( strMsg ); }

    /*virtual void TermPipelineRun( const pipeline::Configuration& configuration,
                                  boost::asio::yield_context&    yield_ctx ) override
    {
        if ( !m_root.m_megastructureInstallationOpt.has_value() )
            m_root.m_megastructureInstallationOpt = m_root.getMegastructureInstallation();
        VERIFY_RTE_MSG(
            m_root.m_megastructureInstallationOpt.has_value(), "Megastructure Installation Toolchain unspecified" );
        const auto toolChain = m_root.m_megastructureInstallationOpt.value().getToolchainXML();

        spdlog::stopwatch             sw;
        mega::pipeline::Pipeline::Ptr pPipeline;
        {
            std::ostringstream osLog;
            pPipeline = pipeline::Registry::getPipeline( toolChain, configuration, osLog );
            if ( !pPipeline )
            {
                SPDLOG_ERROR( "Failed to load pipeline: {}", configuration.getPipelineID() );
                THROW_RTE( "Root: Failed to load pipeline: " << configuration.get() );
            }
            else
            {
                SPDLOG_INFO( "{}", osLog.str() );
            }
        }

        m_root.m_buildHashCodes.reset();

        for ( auto& [ id, pDaemon ] : m_root.m_server.getConnections() )
        {
            auto                                         daemon = getDaemonRequest( pDaemon, yield_ctx );
            const std::vector< network::ConversationID > jobs
                = daemon.RootPipelineStartJobs( toolChain, configuration, getID() );
            for ( const network::ConversationID& id : jobs )
                m_jobs.insert( id );
        }
        if ( m_jobs.empty() )
        {
            SPDLOG_WARN( "Failed to find executors for pipeline: {}", configuration.getPipelineID() );
            THROW_RTE( "Root: Failed to find executors for pipeline" );
        }
        SPDLOG_TRACE( "Found {} jobs for pipeline {}", m_jobs.size(), configuration.getPipelineID() );

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
            auto               daemon = getStackTopDaemonResponse( yield_ctx );
            if ( bScheduleFailed )
            {
                SPDLOG_WARN( "FAILURE: Pipeline {} failed in: {}ms", configuration.getPipelineID(),
                             std::chrono::duration_cast< network::LogTime >( sw.elapsed() ) );
                os << "Pipeline: " << configuration.getPipelineID() << " failed";
                daemon.TermPipelineRun( network::PipelineResult( false, os.str(), m_root.m_buildHashCodes.get() ) );
            }
            else
            {
                SPDLOG_INFO( "SUCCESS: Pipeline {} succeeded in: {}", configuration.getPipelineID(),
                             std::chrono::duration_cast< network::LogTime >( sw.elapsed() ) );
                os << "Pipeline: " << configuration.getPipelineID() << " succeeded";
                daemon.TermPipelineRun( network::PipelineResult( true, os.str(), m_root.m_buildHashCodes.get() ) );
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
            auto daemonRequest = getDaemonRequestByInitiatedCon( getID(), yield_ctx );
            VERIFY_RTE( daemonRequest.has_value() );
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
                        network::PipelineResult result = daemonRequest->RootPipelineStartTask( task );
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

        getStackTopDaemonResponse( yield_ctx ).ExePipelineReadyForWork();
    }

    virtual void ExePipelineWorkProgress( const std::string&          strMessage,
                                          boost::asio::yield_context& yield_ctx ) override
    {
        network::logLinesInfo( strMessage );
        getStackTopDaemonResponse( yield_ctx ).ExePipelineWorkProgress();
    }*/
};

} // namespace service
} // namespace mega

#endif // PIPELINE_22_SEPT_2022