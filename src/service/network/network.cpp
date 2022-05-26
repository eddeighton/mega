
#include "service/network/network.hpp"

namespace mega
{
namespace network
{

const char* getVersion() { return "0.0.0.0"; }

short MegaDaemonPort() { return 4237; }

const char* MegaDaemonServiceName() { return "megadaemon"; }

short MegaRootPort() { return 4238; }

const char* MegaRootServiceName() { return "megaroot"; }

} // namespace network
} // namespace mega
