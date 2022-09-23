
#include "service/executor/request.hpp"

#include "service/executor.hpp"

#include "job.hpp"
#include "service/protocol/common/header.hpp"
#include "simulation.hpp"

#include "service/protocol/model/exe_sim.hxx"

namespace mega
{
namespace service
{

ExecutorRequestConversation::ExecutorRequestConversation( Executor&                      executor,
                                                          const network::ConversationID& conversationID,
                                                          std::optional< network::ConnectionID >
                                                              originatingConnectionID )
    : ConcurrentConversation( executor, conversationID, originatingConnectionID )
    , m_executor( executor )
{
}

network::Message ExecutorRequestConversation::dispatchRequest( const network::Message&     msg,
                                                               boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if ( result = network::leaf_exe::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "ExecutorRequestConversation::dispatchRequest failed" );
}

void ExecutorRequestConversation::dispatchResponse( const network::ConnectionID& connectionID,
                                                    const network::Message&      msg,
                                                    boost::asio::yield_context&  yield_ctx )
{
    if ( ( m_executor.getLeafSender().getConnectionID() == connectionID )
         || ( m_executor.m_receiverChannel.getSender()->getConnectionID() == connectionID ) )
    {
        m_executor.getLeafSender().send( getID(), msg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "ExecutorRequestConversation: Cannot resolve connection for response: {}", connectionID );
        THROW_RTE( "ExecutorRequestConversation: Executor Critical error in response handler: " << connectionID );
    }
}

void ExecutorRequestConversation::error( const network::ConnectionID& connectionID, const std::string& strErrorMsg,
                                         boost::asio::yield_context& yield_ctx )
{
    if ( ( m_executor.getLeafSender().getConnectionID() == connectionID )
         || ( m_executor.m_receiverChannel.getSender()->getConnectionID() == connectionID ) )
    {
        m_executor.getLeafSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "ExecutorRequestConversation: Cannot resolve connection in error handler: {} for error: {}",
                      connectionID, strErrorMsg );
        THROW_RTE( "ExecutorRequestConversation: Executor Critical error in error handler: " << connectionID << " : "
                                                                                             << strErrorMsg );
    }
}

network::exe_leaf::Request_Sender ExecutorRequestConversation::getLeafRequest( boost::asio::yield_context& yield_ctx )
{
    return network::exe_leaf::Request_Sender( *this, m_executor.getLeafSender(), yield_ctx );
}

/*network::leaf_exe::Response_Encode ExecutorRequestConversation::getLeafResponse( boost::asio::yield_context& yield_ctx
)
{
    return network::leaf_exe::Response_Encode( *this, m_executor.getLeafSender(), yield_ctx );
}*/
/*
void ExecutorRequestConversation::RootListNetworkNodes( boost::asio::yield_context& yield_ctx )
{
    std::vector< std::string > result = { m_executor.getProcessName() };
    getLeafResponse( yield_ctx ).RootListNetworkNodes( result );
}

void ExecutorRequestConversation::RootPipelineStartJobs( const mega::utilities::ToolChain& toolChain,
                                                         const pipeline::Configuration&    configuration,
                                                         const network::ConversationID&    rootConversationID,
                                                         boost::asio::yield_context&       yield_ctx )
{
    mega::pipeline::Pipeline::Ptr pPipeline;
    {
        std::ostringstream osLog;
        pPipeline = pipeline::Registry::getPipeline( toolChain, configuration, osLog );
        if ( !pPipeline )
        {
            SPDLOG_ERROR( "PIPELINE: Executor: Failed to load pipeline: {}", configuration.getPipelineID() );
            THROW_RTE( "Executor: Failed to load pipeline: " << configuration.get() );
        }
        else
        {
            SPDLOG_TRACE( "PIPELINE: {}", osLog.str() );
        }
    }

    std::vector< network::ConversationID > jobIDs;
    std::vector< JobConversation::Ptr >    jobs;
    for ( int i = 0; i < m_executor.getNumThreads(); ++i )
    {
        JobConversation::Ptr pJob = std::make_shared< JobConversation >(
            m_executor, m_executor.createConversationID( m_executor.getLeafSender().getConnectionID() ), pPipeline,
            rootConversationID );
        jobIDs.push_back( pJob->getID() );
        jobs.push_back( pJob );
    }
    getLeafResponse( yield_ctx ).RootPipelineStartJobs( jobIDs );

    for ( JobConversation::Ptr pJob : jobs )
    {
        m_executor.conversationInitiated( pJob, m_executor.getLeafSender() );
    }
}

void ExecutorRequestConversation::RootProjectUpdated( const mega::network::Project& project,
                                                      boost::asio::yield_context&   yield_ctx )
{
    mega::runtime::initialiseRuntime( m_executor.m_megastructureInstallation, project );
    getLeafResponse( yield_ctx ).RootProjectUpdated();
}

void ExecutorRequestConversation::RootSimList( boost::asio::yield_context& yield_ctx )
{
    std::vector< network::ConversationID > simulationIDs;
    for ( auto& [ id, pSim ] : m_executor.m_simulations )
    {
        simulationIDs.push_back( id );
    }
    getLeafResponse( yield_ctx ).RootSimList( simulationIDs );
}

void ExecutorRequestConversation::RootSimCreate( boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE_MSG( mega::runtime::isRuntimeInitialised(), "Megastructure Project not initialised" );
    const network::ConversationID id = m_executor.createConversationID( m_executor.getLeafSender().getConnectionID() );
    Simulation::Ptr               pSim = std::make_shared< Simulation >( m_executor, id );
    m_executor.simulationInitiated( pSim );
    getLeafResponse( yield_ctx ).RootSimCreate( pSim->getID() );
}

void ExecutorRequestConversation::RootSimDestroy( const mega::network::ConversationID& simulationID,
                                                  boost::asio::yield_context&          yield_ctx )
{
    VERIFY_RTE_MSG( mega::runtime::isRuntimeInitialised(), "Megastructure Project not initialised" );
    if ( Simulation::Ptr pSim = m_executor.getSimulation( simulationID ) )
    {
        network::exe_sim::Request_Encode rq( *this, pSim->getRequestSender(), yield_ctx );
        SPDLOG_TRACE( "SIM: ExecutorRequestConversation::RootSimDestroy {}", simulationID );
        rq.ExeSimDestroy( getID() );
        SPDLOG_TRACE( "SIM: ExecutorRequestConversation::RootSimDestroy responded" );
    }
    getLeafResponse( yield_ctx ).RootSimDestroy();
}

void ExecutorRequestConversation::RootSimReadLock( const mega::network::ConversationID& owningID,
                                                   const mega::network::ConversationID& simulationID,
                                                   boost::asio::yield_context&          yield_ctx )
{
    VERIFY_RTE_MSG( mega::runtime::isRuntimeInitialised(), "Megastructure Project not initialised" );
    if ( Simulation::Ptr pSim = m_executor.getSimulation( simulationID ) )
    {
        network::exe_sim::Request_Encode rq( *this, pSim->getRequestSender(), yield_ctx );
        const bool                       bResult = rq.ExeSimReadLockAcquire( owningID, getID() );
        SPDLOG_TRACE( "SIM: ExecutorRequestConversation::RootSimReadLock request {} {}", owningID, simulationID );
        getLeafResponse( yield_ctx ).RootSimReadLock( bResult );
        SPDLOG_TRACE( "SIM: ExecutorRequestConversation::RootSimReadLock returned {} {}", owningID, simulationID );
    }
    else
    {
        getLeafResponse( yield_ctx ).RootSimReadLock( false );
    }
}

void ExecutorRequestConversation::RootSimWriteLock( const mega::network::ConversationID& owningID,
                                                    const mega::network::ConversationID& simulationID,
                                                    boost::asio::yield_context&          yield_ctx )
{
    VERIFY_RTE_MSG( mega::runtime::isRuntimeInitialised(), "Megastructure Project not initialised" );
    if ( Simulation::Ptr pSim = m_executor.getSimulation( simulationID ) )
    {
        network::exe_sim::Request_Encode rq( *this, pSim->getRequestSender(), yield_ctx );
        const bool                       bResult = rq.ExeSimWriteLockAcquire( owningID, getID() );
        getLeafResponse( yield_ctx ).RootSimWriteLock( bResult );
    }
    else
    {
        getLeafResponse( yield_ctx ).RootSimWriteLock( false );
    }
}

void ExecutorRequestConversation::RootSimReleaseLock( const mega::network::ConversationID& owningID,
                                                      const mega::network::ConversationID& simulationID,
                                                      boost::asio::yield_context&          yield_ctx )
{
    VERIFY_RTE_MSG( mega::runtime::isRuntimeInitialised(), "Megastructure Project not initialised" );
    if ( Simulation::Ptr pSim = m_executor.getSimulation( simulationID ) )
    {
        network::exe_sim::Request_Encode rq( *this, pSim->getRequestSender(), yield_ctx );
        SPDLOG_TRACE( "SIM: ExecutorRequestConversation::RootSimReleaseLock request {} {}", owningID, simulationID );
        rq.ExeSimLockRelease( owningID, getID() );
        SPDLOG_TRACE( "SIM: ExecutorRequestConversation::RootSimReleaseLock returned {} {}", owningID, simulationID );
    }
    getLeafResponse( yield_ctx ).RootSimReleaseLock();
}

void ExecutorRequestConversation::RootShutdown( boost::asio::yield_context& yield_ctx )
{
    getLeafResponse( yield_ctx ).RootShutdown();
    boost::asio::post( [ &executor = m_executor ]() { executor.shutdown(); } );
}
*/
} // namespace service
} // namespace mega
