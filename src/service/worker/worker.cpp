
#include "service/worker/worker.hpp"

#include "service/network/activity_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/worker_daemon.hxx"
#include "service/protocol/model/daemon_worker.hxx"

#include "pipeline/pipeline.hpp"

#include "common/requireSemicolon.hpp"

#include <optional>
#include <future>

namespace mega
{
namespace service
{

class RequestActivity : public network::Activity, public network::daemon_worker::Impl
{
protected:
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

    virtual void error( const network::ConnectionID& connectionID, const std::string& strErrorMsg,
                        boost::asio::yield_context yield_ctx )
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

    network::worker_daemon::Request_Encode getDaemonRequest( boost::asio::yield_context yield_ctx )
    {
        return network::worker_daemon::Request_Encode( *this, m_worker.m_client.getSocket(), yield_ctx );
    }
    network::daemon_worker::Response_Encode getDaemonResponse( boost::asio::yield_context yield_ctx )
    {
        return network::daemon_worker::Response_Encode( *this, m_worker.m_client.getSocket(), yield_ctx );
    }


    virtual void ReportActivities( boost::asio::yield_context yield_ctx ) 
    {
        std::vector< network::ActivityID > activities;

        for( const auto& id : m_activityManager.reportActivities() )
        {
            activities.push_back( id );
        }

        getDaemonResponse( yield_ctx ).ReportActivities( activities );
    }
    
    virtual void ListThreads( boost::asio::yield_context yield_ctx )
    {
        getDaemonResponse( yield_ctx ).ListThreads( m_worker.getNumThreads() );
    }

    virtual void PipelineStartJobs( const mega::pipeline::Pipeline::ID& pipelineID,
                                    const network::ActivityID&          rootActivityID,
                                    boost::asio::yield_context          yield_ctx );
};

class JobActivity : public RequestActivity, public pipeline::Progress
{
    const network::ActivityID      m_rootActivityID;
    mega::pipeline::Pipeline::Ptr  m_pPipeline;
    boost::asio::yield_context*    m_pYieldCtx = nullptr;
    mega::pipeline::TaskDescriptor m_currentTask;

public:
    using Ptr = std::shared_ptr< JobActivity >;
    JobActivity( Worker& worker, const network::ActivityID& activityID, mega::pipeline::Pipeline::Ptr pPipeline,
                 const network::ActivityID& rootActivityID )
        : RequestActivity( worker, activityID, activityID.getConnectionID() )
        , m_pPipeline( pPipeline )
        , m_rootActivityID( rootActivityID )
    {
        VERIFY_RTE( m_pPipeline );
    }

    virtual void onStarted()
    {
        auto daemon = getDaemonRequest( *m_pYieldCtx );
        daemon.PipelineWorkProgress( m_rootActivityID, m_currentTask, "Started" );
        daemon.Complete();
    }

    virtual void onProgress()
    {
        auto daemon = getDaemonRequest( *m_pYieldCtx );
        daemon.PipelineWorkProgress( m_rootActivityID, m_currentTask, "Progress" );
        daemon.Complete();
    }

    virtual void onCompleted()
    {
        auto daemon = getDaemonRequest( *m_pYieldCtx );
        daemon.PipelineWorkCompleted( m_rootActivityID, m_currentTask );
        daemon.Complete();
    }

    void run( boost::asio::yield_context yield_ctx )
    {
        requestStarted( network::getConnectionID( m_worker.m_client.getSocket() ) );

        try
        {
            SPDLOG_INFO( "JobActivity: {}", getActivityID() );

            m_pYieldCtx = &yield_ctx;

            auto daemonRequest = getDaemonRequest( yield_ctx );
            while ( true )
            {
                m_currentTask = daemonRequest.PipelineReadyForWork( m_rootActivityID );
                daemonRequest.Complete();
                if ( m_currentTask != mega::pipeline::TaskDescriptor() )
                {
                    try
                    {
                        SPDLOG_INFO( "JobActivity got task: {}", m_currentTask.get() );
                        m_pPipeline->execute( m_currentTask, *this );
                    }
                    catch ( std::exception& ex )
                    {
                        SPDLOG_WARN( "Pipeline task failed: {}", m_currentTask.get() );
                        daemonRequest.PipelineWorkFailed( m_rootActivityID, m_currentTask );
                        daemonRequest.Complete();
                    }
                }
                else
                {
                    break;
                }
            }
        }
        catch ( std::exception& ex )
        {
            SPDLOG_ERROR( "JobActivity exception: {}", ex.what() );
        }
        requestCompleted();
    }
};

void RequestActivity::PipelineStartJobs( const mega::pipeline::Pipeline::ID& pipelineID,
                                         const network::ActivityID&          rootActivityID,
                                         boost::asio::yield_context          yield_ctx )
{
    pipeline::Registry            pipelineRegistry;
    mega::pipeline::Pipeline::Ptr pPipeline = pipelineRegistry.getPipeline( pipelineID );

    std::vector< network::ActivityID > jobIDs;
    std::vector< JobActivity::Ptr >    jobs;
    for ( int i = 0; i < m_worker.getNumThreads(); ++i )
    {
        JobActivity::Ptr pJob = std::make_shared< JobActivity >(
            m_worker,
            m_worker.m_activityManager.createActivityID( network::getConnectionID( m_worker.m_client.getSocket() ) ),
            pPipeline, rootActivityID );
        jobIDs.push_back( pJob->getActivityID() );
        jobs.push_back( pJob );
    }

    for ( JobActivity::Ptr pJob : jobs )
    {
        m_worker.m_activityManager.activityStarted( pJob );
    }
    getDaemonResponse( yield_ctx ).PipelineStartJobs( jobIDs );
}

network::Activity::Ptr
Worker::HostActivityFactory::createRequestActivity( const network::Header&       msgHeader,
                                                    const network::ConnectionID& originatingConnectionID ) const
{
    return network::Activity::Ptr(
        new RequestActivity( m_worker, msgHeader.getActivityID(), originatingConnectionID ) );
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

} // namespace service
} // namespace mega
