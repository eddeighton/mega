
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

Client::Client( boost::asio::io_context& ioContext, ActivityManager& activityManager, ActivityFactory& activityFactory,
                const std::string& strServiceIP, const std::string& strServiceName )
    : m_ioContext( ioContext )
    , m_resolver( ioContext )
    , m_strand( boost::asio::make_strand( ioContext ) )
    , m_socket( m_strand )
    , m_receiver( activityManager, activityFactory, m_socket, boost::bind( &Client::disconnected, this ) )
{
    boost::asio::ip::tcp::resolver::results_type endpoints = m_resolver.resolve( strServiceIP, strServiceName );

    VERIFY_RTE_MSG( !endpoints.empty(), "Failed to resolve " << strServiceName << " service on ip: " << strServiceIP );

    m_endPoint = boost::asio::connect( m_socket, endpoints );

    std::cout << "Connected from: " << m_socket.local_endpoint() << " to " << m_socket.remote_endpoint() << std::endl;

    m_receiver.run( ioContext );
}

void Client::stop()
{
    m_socket.cancel();
    m_socket.close();
    m_ioContext.stop();
}

void Client::disconnected()
{
    std::cout << "Client disconnected" << std::endl;
    stop();
}

Client::~Client()
{
    try
    {
        stop();
    }
    catch ( std::exception& )
    {
    }
}

} // namespace network
} // namespace mega
