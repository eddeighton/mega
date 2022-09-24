
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
