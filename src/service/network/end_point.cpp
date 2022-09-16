
#include "service/network/end_point.hpp"

#include <sstream>

namespace mega
{
namespace network
{

ConnectionID makeConnectionID( boost::asio::ip::tcp::socket& socket )
{
    std::ostringstream os;
    os << socket.local_endpoint() << "-" << socket.remote_endpoint();
    return os.str();
}

} // namespace network
} // namespace mega