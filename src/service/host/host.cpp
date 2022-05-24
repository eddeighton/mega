
#include "service/host/host.hpp"

#include "service/network/network.hpp"
#include <boost/asio/thread_pool.hpp>

namespace mega
{
namespace service
{

void* Simulation::getRoot() { return nullptr; }

Host::Host( std::optional< const std::string > optName /* = std::nullopt*/ )
    : m_client( m_io_context, "localhost" )
    , m_io_thread( [ &io_context = m_io_context ]() { io_context.run(); } )
{
}

Lock::Ptr Host::getLock()
{
    Lock::Ptr pLock;
    return pLock;
}

Simulation::PtrVector Host::getSimulations() { return m_simulations; }

Simulation::Ptr Host::createSimulation()
{
    Simulation::Ptr pSim;
    return pSim;
}

} // namespace service
} // namespace mega