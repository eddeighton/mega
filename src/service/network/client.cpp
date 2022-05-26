
#include "service/network/client.hpp"
#include "service/network/network.hpp"

#include "boost/bind/bind.hpp"
#include "boost/asio/connect.hpp"

#include <exception>
#include <future>
#include <iostream>

namespace mega
{
namespace network
{

Client::Client( boost::asio::io_context& ioContext, ActivityFactory& activityFactory, const std::string& strServiceIP )
    : ActivityManager( ioContext, activityFactory )
    , m_resolver( ioContext )
    , m_socket( ioContext )
    , m_watchDogTimer( ioContext )
    , m_receiver( *this, m_decoder, m_socket, boost::bind( &Client::disconnected, this ) )
{
    boost::asio::ip::tcp::resolver::results_type endpoints
        = m_resolver.resolve( strServiceIP, mega::network::MegaRootServiceName() );

    VERIFY_RTE_MSG(
        !endpoints.empty(),
        "Failed to resolve " << mega::network::MegaRootServiceName() << " service on ip: " << strServiceIP );

    m_endPoint = boost::asio::connect( m_socket, endpoints );

    std::cout << "Connected from: " << m_socket.local_endpoint() << " to " << m_socket.remote_endpoint() << std::endl;

    m_receiver.run( ioContext );
}

void Client::disconnected()
{
    std::cout << "Client disconnected" << std::endl;
    m_socket.cancel();
    m_socket.close();
}

void Client::spawnActivity( Activity::Ptr pActivity )
{
    activityStarted( pActivity );
    boost::asio::spawn(
        m_ioContext, [ pActivity ]( boost::asio::yield_context yield_ctx ) { pActivity->run( yield_ctx ); } );
}

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
