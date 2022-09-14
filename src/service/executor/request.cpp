
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

bool ExecutorRequestConversation::dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    return network::leaf_exe::Impl::dispatchRequest( msg, yield_ctx );
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

network::exe_leaf::Request_Encode ExecutorRequestConversation::getLeafRequest( boost::asio::yield_context& yield_ctx )
{
    return network::exe_leaf::Request_Encode( *this, m_executor.getLeafSender(), yield_ctx );
}
network::leaf_exe::Response_Encode ExecutorRequestConversation::getLeafResponse( boost::asio::yield_context& yield_ctx )
{
    return network::leaf_exe::Response_Encode( *this, m_executor.getLeafSender(), yield_ctx );
}

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
            SPDLOG_ERROR( "Executor: Failed to load pipeline: {}", configuration.getPipelineID() );
            THROW_RTE( "Executor: Failed to load pipeline: " << configuration.get() );
        }
        else
        {
            SPDLOG_TRACE( "{}", osLog.str() );
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
    // terminate all simulations
    
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

    Simulation::Ptr pSim = std::make_shared< Simulation >( m_executor, id );

    m_executor.m_simulations.insert( { pSim->getID(), pSim } );
    m_executor.conversationInitiated( pSim, m_executor.getLeafSender() );

    getLeafResponse( yield_ctx ).RootSimCreate( pSim->getID() );
}

void ExecutorRequestConversation::RootSimReadLock( const mega::network::ConversationID& simulationID,
                                                   boost::asio::yield_context&          yield_ctx )
{
    VERIFY_RTE_MSG( mega::runtime::isRuntimeInitialised(), "Megastructure Project not initialised" );
    SPDLOG_TRACE( "ExecutorRequestConversation::RootSimReadLock: {}", simulationID );

    Executor::SimulationMap::const_iterator iFind = m_executor.m_simulations.find( simulationID );
    VERIFY_RTE_MSG( iFind != m_executor.m_simulations.end(), "Failed to find simulation: " << simulationID );
    Simulation::Ptr pSimulation = iFind->second;

    network::exe_sim::Request_Encode rq( *this, pSimulation->getRequestSender(), yield_ctx );
    const mega::TimeStamp            timeStamp = rq.ExeSimReadLockAcquire( getID() );

    getLeafResponse( yield_ctx ).RootSimReadLock();
}

void ExecutorRequestConversation::RootSimWriteLock( const mega::network::ConversationID& simulationID,
                                                    boost::asio::yield_context&          yield_ctx )
{
    VERIFY_RTE_MSG( mega::runtime::isRuntimeInitialised(), "Megastructure Project not initialised" );
    Executor::SimulationMap::const_iterator iFind = m_executor.m_simulations.find( simulationID );
    VERIFY_RTE_MSG( iFind != m_executor.m_simulations.end(), "Failed to find simulation: " << simulationID );
    Simulation::Ptr pSimulation = iFind->second;

    network::exe_sim::Request_Encode rq( *this, pSimulation->getRequestSender(), yield_ctx );
    const mega::TimeStamp            timeStamp = rq.ExeSimWriteLockAcquire( getID() );

    getLeafResponse( yield_ctx ).RootSimWriteLock();
}

void ExecutorRequestConversation::RootSimReleaseLock( const mega::network::ConversationID& simulationID,
                                                      boost::asio::yield_context&          yield_ctx )
{
    VERIFY_RTE_MSG( mega::runtime::isRuntimeInitialised(), "Megastructure Project not initialised" );
    Executor::SimulationMap::const_iterator iFind = m_executor.m_simulations.find( simulationID );
    VERIFY_RTE_MSG( iFind != m_executor.m_simulations.end(), "Failed to find simulation: " << simulationID );
    Simulation::Ptr pSimulation = iFind->second;

    network::exe_sim::Request_Encode rq( *this, pSimulation->getRequestSender(), yield_ctx );
    rq.ExeSimLockRelease( getID() );

    getLeafResponse( yield_ctx ).RootSimReleaseLock();
}

void ExecutorRequestConversation::RootShutdown( boost::asio::yield_context& yield_ctx )
{
    getLeafResponse( yield_ctx ).RootShutdown();
    boost::asio::post( [ &executor = m_executor ]() { executor.shutdown(); } );
}

} // namespace service
} // namespace mega
