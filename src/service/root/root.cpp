
#include "service/root.hpp"

#include "mega/common.hpp"
#include "mega/execution_context.hpp"

#include "service/network/conversation.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/common/megastructure_installation.hpp"
#include "service/protocol/common/pipeline_result.hpp"

#include "service/protocol/model/root_daemon.hxx"
#include "service/protocol/model/daemon_root.hxx"

#include "pipeline/task.hpp"
#include "pipeline/pipeline.hpp"

#include "utilities/serialization_helpers.hpp"

#include "version/version.hpp"

#include <common/file.hpp>

#include "spdlog/fmt/chrono.h"
#include "spdlog/stopwatch.h"

#include "boost/archive/xml_iarchive.hpp"
#include "boost/archive/xml_oarchive.hpp"
#include "boost/dll/runtime_symbol_info.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/process/environment.hpp"
#include "boost/system/detail/error_code.hpp"
#include "boost/asio/experimental/concurrent_channel.hpp"

#include <iostream>
#include <limits>
#include <memory>

namespace mega
{
namespace service
{

class RootRequestConversation : public network::InThreadConversation, public network::daemon_root::Impl
{
protected:
    Root& m_root;

public:
    RootRequestConversation( Root&                          root,
                             const network::ConversationID& conversationID,
                             const network::ConnectionID&   originatingConnectionID )
        : InThreadConversation( root, conversationID, originatingConnectionID )
        , m_root( root )
    {
    }

    virtual bool dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx ) override
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
            THROW_RTE( "Root: Critical error in error handler" );
        }
    }

    network::daemon_root::Response_Encode getStackTopDaemonResponse( boost::asio::yield_context& yield_ctx )
    {
        VERIFY_RTE( getStackConnectionID().has_value() );
        if ( network::Server::Connection::Ptr pConnection
             = m_root.m_server.getConnection( getStackConnectionID().value() ) )
        {
            return network::daemon_root::Response_Encode( *this, *pConnection, yield_ctx );
        }
        THROW_RTE( "Root: Connection to daemon lost" );
    }

    std::optional< network::root_daemon::Request_Encode >
    getDaemonRequestByCon( const mega::network::ConversationID& conversationID, boost::asio::yield_context& yield_ctx )
    {
        for ( auto& [ id, pConnection ] : m_root.m_server.getConnections() )
        {
            if ( pConnection->getConversations().count( conversationID ) )
            {
                return network::root_daemon::Request_Encode( *this, *pConnection, yield_ctx );
            }
        }
        return std::optional< network::root_daemon::Request_Encode >();
    }

    network::root_daemon::Request_Encode getDaemonRequest( network::Server::Connection::Ptr pConnection,
                                                           boost::asio::yield_context&      yield_ctx )
    {
        return network::root_daemon::Request_Encode( *this, *pConnection, yield_ctx );
    }

    // network::daemon_root::Impl
    virtual void DaemonEnrole( boost::asio::yield_context& yield_ctx ) override
    {
        mega::MachineProcessExecutor mpe = m_root.m_executionContextManager.newDaemon();
        getStackTopDaemonResponse( yield_ctx ).DaemonEnrole( mpe.mpe_storage );
    }

    virtual void DaemonLeafEnrole( const mega::MPEStorage& storage, boost::asio::yield_context& yield_ctx ) override
    {
        const mega::MachineProcessExecutor mpe = m_root.m_executionContextManager.newLeaf( storage );
        getStackTopDaemonResponse( yield_ctx ).DaemonLeafEnrole( mpe.mpe_storage );
    }

    virtual void DaemonGetExecutionContextID( const mega::MPEStorage&     mpe,
                                              boost::asio::yield_context& yield_ctx ) override
    {
        const network::ConversationID& conversationID = m_root.m_executionContextManager.get( mpe );
        getStackTopDaemonResponse( yield_ctx ).DaemonGetExecutionContextID( conversationID );
    }

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
        auto daemon = getStackTopDaemonResponse( yield_ctx );
        daemon.TermListNetworkNodes( nodes );
    }

    virtual void TermGetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override
    {
        auto daemon = getStackTopDaemonResponse( yield_ctx );
        daemon.TermGetMegastructureInstallation( m_root.getMegastructureInstallation() );
    }

    virtual void TermGetProject( boost::asio::yield_context& yield_ctx ) override
    {
        auto daemon = getStackTopDaemonResponse( yield_ctx );
        daemon.TermGetProject( m_root.getProject() );
    }

    virtual void TermSetProject( const mega::network::Project& project, boost::asio::yield_context& yield_ctx ) override
    {
        auto daemon = getStackTopDaemonResponse( yield_ctx );

        if ( project.getProjectInstallPath() != m_root.getProject().getProjectInstallPath() )
        {
            m_root.setProject( project );
            m_root.saveConfig();

            // notify all executors of the project change
            for ( auto& [ id, pConnection ] : m_root.m_server.getConnections() )
            {
                network::root_daemon::Request_Encode rq( *this, *pConnection, yield_ctx );
                rq.RootProjectUpdated( project );
            }

            daemon.TermSetProject( true );
        }
        else
        {
            daemon.TermSetProject( false );
        }
    }

    virtual void TermNewInstallation( const mega::network::Project& project,
                                      boost::asio::yield_context&   yield_ctx ) override
    {
        bool bSuccess = true;
        try
        {
            // notify all executors of the project change
            for ( auto& [ id, pConnection ] : m_root.m_server.getConnections() )
            {
                network::root_daemon::Request_Encode rq( *this, *pConnection, yield_ctx );
                rq.RootProjectUpdated( project );
            }
        }
        catch ( std::exception& ex )
        {
            SPDLOG_ERROR( "Exception attempting to roll out project installation: {} : {}",
                          project.getProjectInstallPath().string(), ex.what() );
            bSuccess = false;
        }

        auto daemon = getStackTopDaemonResponse( yield_ctx );
        daemon.TermNewInstallation( bSuccess );
    }

    virtual void TermSimNew( boost::asio::yield_context& yield_ctx ) override
    {
        network::ConversationID simID;
        {
            network::Server::Connection::Ptr pLowestDaemon;
            {
                std::size_t szLowest = std::numeric_limits< std::size_t >::max();
                for ( const auto& [ id, pDaemon ] : m_root.m_server.getConnections() )
                {
                    auto simIDs = getDaemonRequest( pDaemon, yield_ctx ).RootSimList();
                    if ( simIDs.size() < szLowest )
                    {
                        szLowest      = simIDs.size();
                        pLowestDaemon = pDaemon;
                    }
                }
            }
            if ( pLowestDaemon )
            {
                simID = getDaemonRequest( pLowestDaemon, yield_ctx ).RootSimCreate();
            }
            else
            {
                THROW_RTE( "Root: Failed to locate daemon to run simulation on" );
            }
        }

        auto daemon = getStackTopDaemonResponse( yield_ctx );
        daemon.TermSimNew( simID );
    }

    virtual void TermSimList( boost::asio::yield_context& yield_ctx ) override
    {
        std::vector< network::ConversationID > simulationIDs;

        for ( const auto& [ id, pDeamon ] : m_root.m_server.getConnections() )
        {
            auto simIDs = getDaemonRequest( pDeamon, yield_ctx ).RootSimList();
            std::copy( simIDs.begin(), simIDs.end(), std::back_inserter( simulationIDs ) );
        }

        auto daemon = getStackTopDaemonResponse( yield_ctx );
        daemon.TermSimList( simulationIDs );
    }

    virtual void TermSimReadLock( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto pDaemon = getDaemonRequestByCon( simulationID, yield_ctx );
        VERIFY_RTE( pDaemon.has_value() );
        pDaemon->RootSimReadLock( simulationID );
        getStackTopDaemonResponse( yield_ctx ).TermSimReadLock();
    }

    virtual void TermSimWriteLock( const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override
    {
        auto pDaemon = getDaemonRequestByCon( simulationID, yield_ctx );
        VERIFY_RTE( pDaemon.has_value() );
        pDaemon->RootSimWriteLock( simulationID );
        getStackTopDaemonResponse( yield_ctx ).TermSimWriteLock();
    }

    virtual void TermClearStash( boost::asio::yield_context& yield_ctx ) override
    {
        m_root.m_stash.clear();
        getStackTopDaemonResponse( yield_ctx ).TermClearStash();
    }

    virtual void TermCapacity( boost::asio::yield_context& yield_ctx ) override
    {
        auto capacity = m_root.m_logicalAddressSpace.getCapacity();
        getStackTopDaemonResponse( yield_ctx ).TermCapacity( capacity );
    }

    virtual void TermShutdown( boost::asio::yield_context& yield_ctx ) override
    {
        getStackTopDaemonResponse( yield_ctx ).TermShutdown();

        for ( const auto& [ id, pDeamon ] : m_root.m_server.getConnections() )
        {
            getDaemonRequest( pDeamon, yield_ctx ).RootShutdown();
        }

        boost::asio::post( [ &root = m_root ]() { root.shutdown(); } );
    }

    // pipeline::Stash
    virtual void ExeGetBuildHashCode( const boost::filesystem::path& filePath,
                                      boost::asio::yield_context&    yield_ctx ) override
    {
        auto daemon = getStackTopDaemonResponse( yield_ctx );
        daemon.ExeGetBuildHashCode( m_root.m_buildHashCodes.get( filePath ) );
    }

    virtual void ExeSetBuildHashCode( const boost::filesystem::path& filePath, const task::FileHash& hashCode,
                                      boost::asio::yield_context& yield_ctx ) override
    {
        m_root.m_buildHashCodes.set( filePath, hashCode );
        auto daemon = getStackTopDaemonResponse( yield_ctx );
        daemon.ExeSetBuildHashCode();
    }

    virtual void ExeStash( const boost::filesystem::path& filePath, const task::DeterminantHash& determinant,
                           boost::asio::yield_context& yield_ctx ) override
    {
        m_root.m_stash.stash( filePath, determinant );
        auto daemon = getStackTopDaemonResponse( yield_ctx );
        daemon.ExeStash();
    }

    virtual void ExeRestore( const boost::filesystem::path& filePath, const task::DeterminantHash& determinant,
                             boost::asio::yield_context& yield_ctx ) override
    {
        const bool bRestored = m_root.m_stash.restore( filePath, determinant );
        auto       daemon    = getStackTopDaemonResponse( yield_ctx );
        daemon.ExeRestore( bRestored );
    }

    virtual void ExeGetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override
    {
        auto daemon = getStackTopDaemonResponse( yield_ctx );
        daemon.ExeGetMegastructureInstallation( m_root.getMegastructureInstallation() );
    }

    virtual void ExeGetProject( boost::asio::yield_context& yield_ctx ) override
    {
        auto daemon = getStackTopDaemonResponse( yield_ctx );
        daemon.ExeGetProject( m_root.getProject() );
    }

    virtual void ExeCreateExecutionContext( const mega::MPEStorage&     mpe,
                                            boost::asio::yield_context& yield_ctx ) override
    {
        auto result = m_root.m_executionContextManager.newExecutor( mpe, getID() );
        getStackTopDaemonResponse( yield_ctx ).ExeCreateExecutionContext( result.mpe_storage );
    }

    virtual void ExeReleaseExecutionContext( const mega::MPEStorage&     index,
                                             boost::asio::yield_context& yield_ctx ) override
    {
        m_root.m_executionContextManager.release( index );
        getStackTopDaemonResponse( yield_ctx ).ExeReleaseExecutionContext();
    }

    virtual void ToolGetMegastructureInstallation( boost::asio::yield_context& yield_ctx ) override
    {
        auto daemon = getStackTopDaemonResponse( yield_ctx );
        daemon.ToolGetMegastructureInstallation( m_root.getMegastructureInstallation() );
    }
    virtual void ExeAllocateNetworkAddress( const mega::MPEStorage&     mpe,
                                            const mega::TypeID&         objectTypeID,
                                            boost::asio::yield_context& yield_ctx ) override
    {
        const NetworkAddress result = m_root.m_logicalAddressSpace.allocateNetworkAddress( mpe, objectTypeID );
        getStackTopDaemonResponse( yield_ctx ).ExeAllocateNetworkAddress( result );
    }

    virtual void ExeDeAllocateNetworkAddress( const mega::MPEStorage&     mpe,
                                              const mega::AddressStorage& networkAddress,
                                              boost::asio::yield_context& yield_ctx ) override
    {
        m_root.m_logicalAddressSpace.deAllocateNetworkAddress( mpe, networkAddress );
        getStackTopDaemonResponse( yield_ctx ).ExeDeAllocateNetworkAddress();
    }
    virtual void ExeSimReadLock( const mega::network::ConversationID& simulationID,
                                 boost::asio::yield_context&          yield_ctx ) override
    {
        auto pDaemon = getDaemonRequestByCon( simulationID, yield_ctx );
        VERIFY_RTE( pDaemon.has_value() );
        pDaemon->RootSimReadLock( simulationID );
        getStackTopDaemonResponse( yield_ctx ).ExeSimReadLock();
    }

    virtual void ExeSimWriteLock( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto pDaemon = getDaemonRequestByCon( simulationID, yield_ctx );
        VERIFY_RTE( pDaemon.has_value() );
        pDaemon->RootSimWriteLock( simulationID );
        getStackTopDaemonResponse( yield_ctx ).ExeSimWriteLock();
    }

    virtual void ExeSimReleaseLock( const mega::network::ConversationID& simulationID,
                                    boost::asio::yield_context&          yield_ctx ) override
    {
        auto pDaemon = getDaemonRequestByCon( simulationID, yield_ctx );
        VERIFY_RTE( pDaemon.has_value() );
        pDaemon->RootSimReleaseLock( simulationID );
        getStackTopDaemonResponse( yield_ctx ).ExeSimReleaseLock();
    }

    virtual void ToolCreateExecutionContext( const mega::MPEStorage&     mpe,
                                             boost::asio::yield_context& yield_ctx ) override
    {
        auto result = m_root.m_executionContextManager.newExecutor( mpe, getID() );
        getStackTopDaemonResponse( yield_ctx ).ToolCreateExecutionContext( result.mpe_storage );
    }

    virtual void ToolReleaseExecutionContext( const mega::MPEStorage&     index,
                                              boost::asio::yield_context& yield_ctx ) override
    {
        m_root.m_executionContextManager.release( index );
        getStackTopDaemonResponse( yield_ctx ).ToolReleaseExecutionContext();
    }
    virtual void ToolAllocateNetworkAddress( const mega::MPEStorage&     mpe,
                                             const mega::TypeID&         objectTypeID,
                                             boost::asio::yield_context& yield_ctx ) override
    {
        const NetworkAddress result = m_root.m_logicalAddressSpace.allocateNetworkAddress( mpe, objectTypeID );
        getStackTopDaemonResponse( yield_ctx ).ToolAllocateNetworkAddress( result );
    }

    virtual void ToolDeAllocateNetworkAddress( const mega::MPEStorage&     mpe,
                                               const mega::AddressStorage& networkAddress,
                                               boost::asio::yield_context& yield_ctx ) override
    {
        m_root.m_logicalAddressSpace.deAllocateNetworkAddress( mpe, networkAddress );
        getStackTopDaemonResponse( yield_ctx ).ToolDeAllocateNetworkAddress();
    }
    virtual void ToolStash( const boost::filesystem::path& filePath,
                            const task::DeterminantHash&   determinant,
                            boost::asio::yield_context&    yield_ctx ) override
    {
        m_root.m_stash.stash( filePath, determinant );
        getStackTopDaemonResponse( yield_ctx ).ToolStash();
    }
    virtual void ToolRestore( const boost::filesystem::path& filePath,
                              const task::DeterminantHash&   determinant,
                              boost::asio::yield_context&    yield_ctx ) override
    {
        const bool bRestored = m_root.m_stash.restore( filePath, determinant );
        getStackTopDaemonResponse( yield_ctx ).ToolRestore( bRestored );
    }
    virtual void ToolSimReadLock( const mega::network::ConversationID& simulationID,
                                  boost::asio::yield_context&          yield_ctx ) override
    {
        auto pDaemon = getDaemonRequestByCon( simulationID, yield_ctx );
        VERIFY_RTE( pDaemon.has_value() );
        pDaemon->RootSimReadLock( simulationID );
        getStackTopDaemonResponse( yield_ctx ).ToolSimReadLock();
    }

    virtual void ToolSimWriteLock( const mega::network::ConversationID& simulationID,
                                   boost::asio::yield_context&          yield_ctx ) override
    {
        auto pDaemon = getDaemonRequestByCon( simulationID, yield_ctx );
        VERIFY_RTE( pDaemon.has_value() );
        pDaemon->RootSimWriteLock( simulationID );
        getStackTopDaemonResponse( yield_ctx ).ToolSimWriteLock();
    }

    virtual void ToolSimReleaseLock( const mega::network::ConversationID& simulationID,
                                     boost::asio::yield_context&          yield_ctx ) override
    {
        auto pDaemon = getDaemonRequestByCon( simulationID, yield_ctx );
        VERIFY_RTE( pDaemon.has_value() );
        pDaemon->RootSimReleaseLock( simulationID );
        getStackTopDaemonResponse( yield_ctx ).ToolSimReleaseLock();
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

    virtual void TermPipelineRun( const pipeline::Configuration& configuration,
                                  boost::asio::yield_context&    yield_ctx ) override
    {
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
            auto daemonRequest = getDaemonRequestByCon( getID(), yield_ctx );
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
    }
};

Root::Root( boost::asio::io_context& ioContext )
    : network::ConversationManager( network::makeProcessName( network::Node::Root ), ioContext )
    , m_server( ioContext, *this, network::MegaRootPort() )
    , m_stash( boost::filesystem::current_path() / "stash" )
{
    loadConfig();
    m_server.waitForConnection();
}

void Root::loadConfig()
{
    const boost::filesystem::path configFile = boost::filesystem::current_path() / "config.xml";
    if ( boost::filesystem::exists( configFile ) )
    {
        std::unique_ptr< boost::filesystem::ifstream > pFileStream
            = boost::filesystem::createBinaryInputFileStream( configFile );
        {
            boost::archive::xml_iarchive xml( *pFileStream );
            xml&                         boost::serialization::make_nvp( "config", m_config );
        }
    }
}

void Root::saveConfig()
{
    const boost::filesystem::path configFile = boost::filesystem::current_path() / "config.xml";

    std::unique_ptr< boost::filesystem::ofstream > pFileStream
        = boost::filesystem::createBinaryOutputFileStream( configFile );
    {
        boost::archive::xml_oarchive xml( *pFileStream );
        xml&                         boost::serialization::make_nvp( "config", m_config );
    }
}

void Root::shutdown()
{
    m_server.stop();
    SPDLOG_TRACE( "Root shutdown" );
}

network::MegastructureInstallation Root::getMegastructureInstallation()
{
    if ( !m_megastructureInstallationOpt.has_value() )
    {
        const boost::filesystem::path currentProcessPath = boost::dll::program_location();
        VERIFY_RTE( !currentProcessPath.empty() );

        const boost::filesystem::path binPath = currentProcessPath.parent_path();
        VERIFY_RTE( binPath.has_parent_path() );

        const boost::filesystem::path installationPath = binPath.parent_path();

        m_megastructureInstallationOpt = network::MegastructureInstallation{ installationPath };
    }
    return m_megastructureInstallationOpt.value();
}

network::ConversationBase::Ptr Root::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                       const network::Header&       header,
                                                       const network::Message&      msg )
{
    switch ( header.getMessageID() )
    {
        case network::daemon_root::MSG_ExePipelineReadyForWork_Request::ID:
        case network::daemon_root::MSG_TermPipelineRun_Request::ID:
            return network::ConversationBase::Ptr(
                new RootPipelineConversation( *this, header.getConversationID(), originatingConnectionID ) );
        default:
            return network::ConversationBase::Ptr(
                new RootRequestConversation( *this, header.getConversationID(), originatingConnectionID ) );
    }
}
void Root::conversationNew( const network::Header& header, const network::ReceivedMsg& msg )
{
    auto pCon = m_server.getConnection( msg.connectionID );
    VERIFY_RTE( pCon );
    pCon->conversationNew( header.getConversationID() );
    SPDLOG_TRACE( "Root::conversationNew: {} {}", header.getConversationID(), msg.msg );
}

void Root::conversationEnd( const network::Header& header, const network::ReceivedMsg& msg )
{
    auto pCon = m_server.getConnection( msg.connectionID );
    VERIFY_RTE( pCon );
    pCon->conversationEnd( header.getConversationID() );
    SPDLOG_TRACE( "Root::conversationEnd: {} {}", header.getConversationID(), msg.msg );

    m_executionContextManager.release( header.getConversationID() );
}

} // namespace service
} // namespace mega
