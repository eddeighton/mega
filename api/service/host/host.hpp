
#ifndef SERVICE_24_MAY_2022
#define SERVICE_24_MAY_2022

#include "service/network/client.hpp"

#include <boost/asio/io_service.hpp>

#include <memory>
#include <optional>
#include <string>
#include <optional>
#include <vector>
#include <thread>

namespace mega
{
namespace service
{
class Simulation
{
public:
    using Ptr       = std::shared_ptr< Simulation >;
    using PtrVector = std::vector< Ptr >;

    void* getRoot();
};

class Lock
{
public:
    using Ptr = std::shared_ptr< Lock >;
};

class Host
{
public:
    Host( std::optional< const std::string > optName = std::nullopt );

    Lock::Ptr getLock();

    Simulation::PtrVector getSimulations();
    Simulation::Ptr       createSimulation();

private:
    boost::asio::io_context m_io_context;
    network::Client         m_client;
    std::thread             m_io_thread;
    Simulation::PtrVector   m_simulations;
};

} // namespace service
} // namespace mega

#endif // SERVICE_24_MAY_2022
