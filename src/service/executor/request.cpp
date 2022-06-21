
#include "service/executor/request.hpp"

#include "service/executor.hpp"
#include "job.hpp"
#include "simulation.hpp"

namespace mega
{
namespace service
{

ExecutorRequestConversation::ExecutorRequestConversation( Executor&                      executor,
                                                          const network::ConversationID& conversationID,
                                                          const network::ConnectionID&   originatingConnectionID )
    : ConcurrentConversation( executor, conversationID, originatingConnectionID )
    , m_executor( executor )
{
}

bool ExecutorRequestConversation::dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    return network::leaf_worker::Impl::dispatchRequest( msg, yield_ctx );
}

void ExecutorRequestConversation::error( const network::ConnectionID& connectionID, const std::string& strErrorMsg,
                                         boost::asio::yield_context& yield_ctx )
{
    if ( m_executor.getLeafSender().getConnectionID() == connectionID )
    {
        m_executor.getLeafSender().sendErrorResponse( getID(), strErrorMsg, yield_ctx );
    }
    else
    {
        SPDLOG_ERROR( "Executor: Cannot resolve connection in error handler" );
        THROW_RTE( "Executor: Executor Critical error in error handler" );
    }
}

network::worker_leaf::Request_Encode
ExecutorRequestConversation::getLeafRequest( boost::asio::yield_context& yield_ctx )
{
    return network::worker_leaf::Request_Encode( *this, m_executor.getLeafSender(), yield_ctx );
}
network::leaf_worker::Response_Encode
ExecutorRequestConversation::getLeafResponse( boost::asio::yield_context& yield_ctx )
{
    return network::leaf_worker::Response_Encode( *this, m_executor.getLeafSender(), yield_ctx );
}

void ExecutorRequestConversation::RootListNetworkNodes( boost::asio::yield_context& yield_ctx )
{
    std::vector< std::string > result = { m_executor.getProcessName() };
    getLeafResponse( yield_ctx ).RootListNetworkNodes( result );
}

void ExecutorRequestConversation::RootPipelineStartJobs( const pipeline::Configuration& configuration,
                                                         const network::ConversationID& rootConversationID,
                                                         boost::asio::yield_context&    yield_ctx )
{
    mega::pipeline::Pipeline::Ptr pPipeline;
    {
        std::ostringstream osLog;
        pPipeline = pipeline::Registry::getPipeline( configuration, osLog );
        if ( !pPipeline )
        {
            SPDLOG_ERROR( "Executor: Failed to load pipeline: {}", configuration.getPipelineID() );
            THROW_RTE( "Executor: Failed to load pipeline: " << configuration.get() );
        }
        else
        {
            SPDLOG_INFO( "{}", osLog.str() );
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
        m_executor.conversationStarted( pJob );
    }
}

void ExecutorRequestConversation::RootProjectUpdated( const mega::network::Project& project,
                                                      boost::asio::yield_context&   yield_ctx )
{
    mega::runtime::initialiseRuntime( project );
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
    Simulation::Ptr pSim = std::make_shared< Simulation >(
        m_executor, m_executor.createConversationID( m_executor.getLeafSender().getConnectionID() ) );

    m_executor.m_simulations.insert( { pSim->getID(), pSim } );
    m_executor.conversationStarted( pSim );

    getLeafResponse( yield_ctx ).RootSimCreate( pSim->getID() );
}

void ExecutorRequestConversation::RootSimReadLock( const mega::network::ConversationID& simulationID,
                                                   boost::asio::yield_context&          yield_ctx )
{
    Executor::SimulationMap::const_iterator iFind = m_executor.m_simulations.find( simulationID );
    VERIFY_RTE_MSG( iFind != m_executor.m_simulations.end(), "Failed to find simulation: " << simulationID );
    Simulation::Ptr pSimulation = iFind->second;

    // pSimulation->send(const ReceivedMsg &msg)

    getLeafResponse( yield_ctx ).RootSimReadLock( 0 );
}

void ExecutorRequestConversation::RootSimWriteLock( const mega::network::ConversationID& simulationID,
                                                    boost::asio::yield_context&          yield_ctx )
{
    Executor::SimulationMap::const_iterator iFind = m_executor.m_simulations.find( simulationID );
    VERIFY_RTE_MSG( iFind != m_executor.m_simulations.end(), "Failed to find simulation: " << simulationID );
    Simulation::Ptr pSimulation = iFind->second;

    getLeafResponse( yield_ctx ).RootSimReadLock( 0 );
}
} // namespace service
} // namespace mega
