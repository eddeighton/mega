
#include "service/network/client.hpp"
#include "service/network/network.hpp"
#include "service/network/log.hpp"
#include "service/network/end_point.hpp"

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

    if ( endpoints.empty() )
    {
        SPDLOG_ERROR( "Failed to resolve: {} on ip: {}", strServiceName, strServiceIP );
        THROW_RTE( "Failed to resolve " << strServiceName << " service on ip: " << strServiceIP );
    }

    m_endPoint     = boost::asio::connect( m_socket, endpoints );
    m_connectionID = getConnectionID( m_socket );

    SPDLOG_INFO( "Client connected to: {}", m_connectionID );

    m_receiver.run( ioContext );
}

void Client::stop()
{
    //m_socket.shutdown( boost::asio::ip::tcp::socket::shutdown_both );
    //m_receiver.stop();
    boost::system::error_code ec;
    m_socket.close( ec );
}

void Client::disconnected()
{
    SPDLOG_INFO( "Client disconnected from: {}", m_connectionID );
    //boost::system::error_code ec;
    //m_socket.close( ec );
}

Client::~Client() {}

} // namespace network
} // namespace mega
