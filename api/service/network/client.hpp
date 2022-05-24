#ifndef CLIENT_24_MAY_2022
#define CLIENT_24_MAY_2022

#include "service/network/receiver.hpp"
#include "service/network/network.hpp"

#include "common/assert_verify.hpp"

#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/steady_timer.hpp"

#include <string>
#include <thread>

namespace mega
{
namespace network
{

class Client
{
public:
    using ExecutionContextType = boost::asio::io_context;

    template< typename TExecutionContextType >
    Client( TExecutionContextType& ioContext, const std::string& strServiceIP )
        : m_resolver( ioContext )
        , m_socket( ioContext )
        , m_watchDogTimer( ioContext )
        , m_receiver( m_socket )
    {
        boost::asio::ip::tcp::resolver::results_type endpoints
            = m_resolver.resolve( strServiceIP, mega::network::MegaRootServiceName() );

        VERIFY_RTE_MSG(
            !endpoints.empty(),
            "Failed to resolve " << mega::network::MegaRootServiceName() << " service on ip: " << strServiceIP );

        m_endPoint = boost::asio::connect( m_socket, endpoints );

        m_receiver.run( ioContext );
    }

    ~Client();

private:
    boost::asio::ip::tcp::resolver m_resolver;
    boost::asio::ip::tcp::socket   m_socket;
    boost::asio::ip::tcp::endpoint m_endPoint;
    boost::asio::steady_timer      m_watchDogTimer;
    Receiver                       m_receiver;
};

} // namespace network
} // namespace mega

#endif // CLIENT_24_MAY_2022
