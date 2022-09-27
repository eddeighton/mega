
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
mega::MPO ExecutorRequestConversation::SimCreate( boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE_MSG( mega::runtime::isRuntimeInitialised(), "Megastructure Project not initialised" );
    SPDLOG_TRACE( "ExecutorRequestConversation::SimCreate {}", getID() );
    return m_executor.createSimulation( *this, yield_ctx );
}

} // namespace service
} // namespace mega
