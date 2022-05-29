
#include "service/root/root.hpp"

#include "service/network/activity.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/model/messages.hxx"
#include "service/protocol/model/root_daemon.hxx"
#include "service/protocol/model/daemon_root.hxx"

#include "pipeline/pipeline.hpp"

#include <boost/system/detail/error_code.hpp>
#include <iostream>
#include <memory>

namespace mega
{
namespace service
{

class RequestActivity : public network::Activity, public network::daemon_root::Impl
{
protected:
    Root& m_root;

public:
    RequestActivity( Root&                        root,
                     const network::ActivityID&   activityID,
                     const network::ConnectionID& originatingConnectionID )
        : Activity( root.m_activityManager, activityID, originatingConnectionID )
        , m_root( root )
    {
    }

    virtual bool dispatchRequest( const network::MessageVariant& msg, boost::asio::yield_context yield_ctx )
    {
        return network::Activity::dispatchRequest( msg, yield_ctx )
               || network::daemon_root::Impl::dispatchRequest( msg, *this, yield_ctx );
    }

    virtual void error( const network::ConnectionID& connectionID,
                        const std::string&           strErrorMsg,
                        boost::asio::yield_context   yield_ctx )
    {
        if ( network::Server::Connection::Ptr pHostConnection = m_root.m_server.getConnection( connectionID ) )
        {
            network::sendErrorResponse( getActivityID(), pHostConnection->getSocket(), strErrorMsg, yield_ctx );
        }
        else
        {
            // ?
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
    virtual void GetVersion( boost::asio::yield_context yield_ctx )
    {
        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.GetVersion( network::getVersion() );
    }

    virtual void runTestPipeline( boost::asio::yield_context yield_ctx ) { THROW_RTE( "Unreachable" ); }
    virtual void PipelineReadyForWork( const network::ActivityID& rootActivityID, boost::asio::yield_context yield_ctx )
    {
        THROW_RTE( "Unreachable" );
    }
    virtual void PipelineWorkProgress( const network::ActivityID& rootActivityID, const std::string& strTask,
                                       const std::string& strProgress, boost::asio::yield_context yield_ctx )
    {
        THROW_RTE( "Unreachable" );
    }
    virtual void PipelineWorkCompleted( const network::ActivityID& rootActivityID, const std::string& strTask,
                                        boost::asio::yield_context yield_ctx )
    {
        THROW_RTE( "Unreachable" );
    }
};

class PipelineActivity : public RequestActivity
{
    std::vector< network::ActivityID > m_jobs;
    using TaskChannel = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, std::string ) >;
    TaskChannel m_taskReady, m_taskComplete;

public:
    PipelineActivity( Root&                        root,
                      const network::ActivityID&   activityID,
                      const network::ConnectionID& originatingConnectionID )
        : RequestActivity( root, activityID, originatingConnectionID )
        , m_taskReady( root.m_io_context )
        , m_taskComplete( root.m_io_context )
    {
    }

    virtual void runTestPipeline( boost::asio::yield_context yield_ctx )
    {
        SPDLOG_INFO( "Started pipeline: compiler" );

        for ( auto& [ id, pDaemon ] : m_root.m_server.getConnections() )
        {
            auto                                     daemon = getDaemonRequest( pDaemon, yield_ctx );
            const std::vector< network::ActivityID > jobs   = daemon.StartPipeline( getActivityID() );
            daemon.Complete();
            std::copy( jobs.begin(), jobs.end(), std::back_inserter( m_jobs ) );
        }
        if ( m_jobs.empty() )
        {
            THROW_RTE( "Failed to find workers for pipeline" );
        }

        {
            pipeline::Registry            pipelineRegistry;
            mega::pipeline::Pipeline::Ptr pPipeline = pipelineRegistry.getPipeline( "compiler" );
            mega::pipeline::Schedule      schedule  = pPipeline->getSchedule();

            std::set< std::string > scheduledTasks;
            int                     iAvailableTasks = 0;
            while ( !schedule.isComplete() )
            {
                for ( const mega::pipeline::Task& task : schedule.getReady() )
                {
                    if ( !scheduledTasks.count( task.str() ) )
                    {
                        ++iAvailableTasks;
                        m_taskReady.async_send( boost::system::error_code(), task.str(), yield_ctx );
                        scheduledTasks.insert( task.str() );
                    }
                    if ( iAvailableTasks == m_jobs.size() )
                        break;
                }

                do
                {
                    std::string strCompleted = m_taskComplete.async_receive( yield_ctx );
                    schedule.complete( strCompleted );
                    --iAvailableTasks;
                } while ( m_taskComplete.ready() );
            }
        }

        auto               daemon = getOriginatingDaemonResponse( yield_ctx );
        std::ostringstream os;
        for ( const network::ActivityID& jobID : m_jobs )
        {
            m_taskReady.async_send( boost::system::error_code(), "", yield_ctx );
            m_taskComplete.async_receive( yield_ctx );
            os << jobID.getID() << ": " << jobID.getConnectionID() << " completed\n";
        }
        daemon.runTestPipeline( os.str() );
    }

    std::string getTask( boost::asio::yield_context yield_ctx )
    {
        //
        return m_taskReady.async_receive( yield_ctx );
    }

    void completeTask( const std::string& strTask, boost::asio::yield_context yield_ctx )
    {
        m_taskComplete.async_send( boost::system::error_code(), strTask, yield_ctx );
    }

    virtual void PipelineReadyForWork( const network::ActivityID& rootActivityID, boost::asio::yield_context yield_ctx )
    {
        std::shared_ptr< PipelineActivity > pCoordinator = std::dynamic_pointer_cast< PipelineActivity >(
            m_root.m_activityManager.findExistingActivity( rootActivityID ) );

        auto daemon = getOriginatingDaemonResponse( yield_ctx );

        const std::string strTask = pCoordinator->getTask( yield_ctx );
        if ( strTask == "" )
        {
            SPDLOG_INFO( "PipelineReadyForWork: {} completed", getActivityID() );
            pCoordinator->completeTask( "", yield_ctx );
            daemon.PipelineReadyForWork( "" );
        }
        else
        {
            SPDLOG_INFO( "PipelineReadyForWork: {} task: {}", getActivityID(), strTask );
            daemon.PipelineReadyForWork( strTask );
        }
    }
    virtual void PipelineWorkProgress( const network::ActivityID& rootActivityID, const std::string& strTask,
                                       const std::string& strProgress, boost::asio::yield_context yield_ctx )
    {
        std::shared_ptr< PipelineActivity > pCoordinator = std::dynamic_pointer_cast< PipelineActivity >(
            m_root.m_activityManager.findExistingActivity( rootActivityID ) );

        auto daemon = getOriginatingDaemonResponse( yield_ctx );
        daemon.PipelineWorkProgress();
    }
    virtual void PipelineWorkCompleted( const network::ActivityID& rootActivityID, const std::string& strTask,
                                        boost::asio::yield_context yield_ctx )
    {
        std::shared_ptr< PipelineActivity > pCoordinator = std::dynamic_pointer_cast< PipelineActivity >(
            m_root.m_activityManager.findExistingActivity( rootActivityID ) );

        SPDLOG_INFO( "Pipeline task completed: {}", strTask );

        pCoordinator->completeTask( strTask, yield_ctx );

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
        case network::daemon_root::MSG_PipelineWorkCompleted_Request::ID:
        case network::daemon_root::MSG_runTestPipeline_Request::ID:
            return network::Activity::Ptr(
                new PipelineActivity( m_root, msgHeader.getActivityID(), originatingConnectionID ) );
        default:
            return network::Activity::Ptr(
                new RequestActivity( m_root, msgHeader.getActivityID(), originatingConnectionID ) );
    }
}

Root::Root( boost::asio::io_context& ioContext )
    : m_io_context( ioContext )
    , m_activityFactory( *this )
    , m_activityManager( ioContext )
    , m_server( ioContext, m_activityManager, m_activityFactory, network::MegaRootPort() )
{
    m_server.waitForConnection();
}

} // namespace service
} // namespace mega