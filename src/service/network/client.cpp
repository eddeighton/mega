
#include "service/network/client.hpp"


#include <exception>

namespace mega
{
namespace network
{

Client::~Client()
{
    try
    {
        m_socket.cancel();
        m_socket.close();
    }
    catch ( std::exception& )
    {
    }
}

} // namespace network
} // namespace mega
