
#include "service/worker.hpp"

#include "service/network/activity_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/worker_daemon.hxx"
#include "service/protocol/model/daemon_worker.hxx"

#include "pipeline/pipeline.hpp"
#include "pipeline/stash.hpp"

#include "common/requireSemicolon.hpp"

#include "boost/dll.hpp"

#include <boost/dll/shared_library_load_mode.hpp>
#include <optional>
#include <future>
#include <thread>

#include "parser/parser.hpp"

namespace mega
{
namespace service
{

class WorkerRequestActivity : public network::Activity, public network::daemon_worker::Impl
{
protected:
    Worker& m_worker;

public:
    WorkerRequestActivity( Worker& worker, const network::ActivityID& activityID,
                           const network::ConnectionID& originatingConnectionID )
        : Activity( worker.m_activityManager, activityID, originatingConnectionID )
        , m_worker( worker )
    {
    }

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context& yield_ctx )
    {
        return network::daemon_worker::Impl::dispatchRequest( msg, *this, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connectionID, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx )
    {
        if ( network::getConnectionID( m_worker.m_client.getSocket() ) == connectionID )
        {
            network::sendErrorResponse( getActivityID(), m_worker.m_client.getSocket(), strErrorMsg, yield_ctx );
        }
        else
        {
            SPDLOG_ERROR( "Cannot resolve connection in error handler" );
            THROW_RTE( "Worker Critical error in error handler" );
        }
    }

    network::worker_daemon::Request_Encode getDaemonRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::worker_daemon::Request_Encode( *this, m_worker.m_client.getSocket(), yield_ctx );
    }
    network::daemon_worker::Response_Encode getDaemonResponse( boost::asio::yield_context& yield_ctx )
    {
        return network::daemon_worker::Response_Encode( *this, m_worker.m_client.getSocket(), yield_ctx );
    }

    virtual void ReportActivities( boost::asio::yield_context& yield_ctx )
    {
        std::vector< network::ActivityID > activities;

        for ( const auto& id : m_activityManager.reportActivities() )
        {
            activities.push_back( id );
        }

        getDaemonResponse( yield_ctx ).ReportActivities( activities );
    }

    virtual void ListThreads( boost::asio::yield_context& yield_ctx )
    {
        getDaemonResponse( yield_ctx ).ListThreads( m_worker.getNumThreads() );
    }

    virtual void ExecuteShutdown( boost::asio::yield_context& yield_ctx )
    {
        getDaemonResponse( yield_ctx ).ExecuteShutdown();

        boost::asio::post( [ &worker = m_worker ]() { worker.shutdown(); } );
    }

    virtual void PipelineStartJobs( const pipeline::Configuration& configuration,
                                    const network::ActivityID&     rootActivityID,
                                    boost::asio::yield_context&    yield_ctx );
};

class JobActivity : public WorkerRequestActivity,
                    public pipeline::Progress,
                    public pipeline::Stash,
                    public pipeline::DependencyProvider
{
    const network::ActivityID     m_rootActivityID;
    mega::pipeline::Pipeline::Ptr m_pPipeline;
    boost::asio::yield_context*   m_pYieldCtx = nullptr;

public:
    using Ptr = std::shared_ptr< JobActivity >;
    JobActivity( Worker& worker, const network::ActivityID& activityID, mega::pipeline::Pipeline::Ptr pPipeline,
                 const network::ActivityID& rootActivityID )
        : WorkerRequestActivity( worker, activityID, activityID.getConnectionID() )
        , m_pPipeline( pPipeline )
        , m_rootActivityID( rootActivityID )
    {
        VERIFY_RTE( m_pPipeline );
    }

    virtual void PipelineStartTask( const mega::pipeline::TaskDescriptor& task,
                                    boost::asio::yield_context&           yield_ctx ) override
    {
        m_pYieldCtx = &yield_ctx;
        m_pPipeline->execute( task, *this, *this, *this );
    }

    // pipeline::DependencyProvider
    virtual EG_PARSER_INTERFACE* getParser() override { return m_worker.m_pParser.get(); }

    // pipeline::Progress
    virtual void onStarted( const std::string& strMsg ) override
    {
        getDaemonRequest( *m_pYieldCtx ).PipelineWorkProgress( strMsg );
    }

    virtual void onProgress( const std::string& strMsg ) override
    {
        getDaemonRequest( *m_pYieldCtx ).PipelineWorkProgress( strMsg );
    }

    virtual void onFailed( const std::string& strMsg ) override
    {
        getDaemonResponse( *m_pYieldCtx ).PipelineStartTask( network::PipelineResult( false, strMsg ) );
    }

    virtual void onCompleted( const std::string& strMsg ) override
    {
        getDaemonResponse( *m_pYieldCtx ).PipelineStartTask( network::PipelineResult( true, strMsg ) );
    }

    // pipeline::Stash
    virtual task::FileHash getBuildHashCode( const boost::filesystem::path& filePath ) override
    {
        return getDaemonRequest( *m_pYieldCtx ).getBuildHashCode( filePath );
    }
    virtual void setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode ) override
    {
        getDaemonRequest( *m_pYieldCtx ).setBuildHashCode( filePath, hashCode );
    }
    virtual void stash( const boost::filesystem::path& file, task::DeterminantHash code ) override
    {
        getDaemonRequest( *m_pYieldCtx ).stash( file, code );
    }
    virtual bool restore( const boost::filesystem::path& file, task::DeterminantHash code ) override
    {
        return getDaemonRequest( *m_pYieldCtx ).restore( file, code );
    }

    void run( boost::asio::yield_context& yield_ctx ) override
    {
        getDaemonRequest( yield_ctx ).PipelineReadyForWork( m_rootActivityID );
    }
};

void WorkerRequestActivity::PipelineStartJobs( const pipeline::Configuration& configuration,
                                               const network::ActivityID&     rootActivityID,
                                               boost::asio::yield_context&    yield_ctx )
{
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
    getDaemonResponse( yield_ctx ).PipelineStartJobs( jobIDs );

    for ( JobActivity::Ptr pJob : jobs )
    {
        m_worker.m_activityManager.activityStarted( pJob );
    }
}

network::Activity::Ptr
Worker::HostActivityFactory::createRequestActivity( const network::Header&       msgHeader,
                                                    const network::ConnectionID& originatingConnectionID ) const
{
    return network::Activity::Ptr(
        new WorkerRequestActivity( m_worker, msgHeader.getActivityID(), originatingConnectionID ) );
}

Worker::Worker( boost::asio::io_context& io_context, int numThreads )
    : m_activityFactory( *this )
    , m_io_context( io_context )
    , m_numThreads( numThreads )
    , m_activityManager( "worker", m_io_context )
    , m_client(
          m_io_context, m_activityManager, m_activityFactory, "localhost", mega::network::MegaWorkerServiceName() )
{
    m_pParser = boost::dll::import_symbol< EG_PARSER_INTERFACE >(
        "parser", "g_parserSymbol", boost::dll::load_mode::append_decorations );
}

Worker::~Worker() { SPDLOG_INFO( "Worker shutdown" ); }

void Worker::shutdown() { m_client.stop(); }

} // namespace service
} // namespace mega
