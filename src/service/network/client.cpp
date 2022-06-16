
#include "service/network/client.hpp"
#include "service/network/network.hpp"
#include "service/network/log.hpp"
#include "service/network/end_point.hpp"

#include "boost/bind/bind.hpp"
#include "boost/asio/connect.hpp"
#include "service/network/sender.hpp"

#include <exception>
#include <future>
#include <iostream>

namespace mega
{
namespace network
{

Client::Client( boost::asio::io_context& ioContext, ConversationManager& conversationManager,
                const std::string& strServiceIP, const std::string& strServiceName )
    : m_ioContext( ioContext )
    , m_resolver( ioContext )
    , m_strand( boost::asio::make_strand( ioContext ) )
    , m_socket( m_strand )
    , m_receiver( conversationManager, m_socket, boost::bind( &Client::disconnected, this ) )
{
    boost::asio::ip::tcp::resolver::results_type endpoints = m_resolver.resolve( strServiceIP, strServiceName );

    if ( endpoints.empty() )
    {
        SPDLOG_ERROR( "Failed to resolve: {} on ip: {}", strServiceName, strServiceIP );
        THROW_RTE( "Failed to resolve " << strServiceName << " service on ip: " << strServiceIP );
    }

    m_endPoint     = boost::asio::connect( m_socket, endpoints );
    m_connectionID = makeConnectionID( m_socket );

    SPDLOG_INFO( "Client connected to: {}", m_connectionID );

    m_receiver.run( ioContext, m_connectionID );

    m_pSender = make_socket_sender( m_socket, m_connectionID );
}

void Client::stop()
{
    boost::system::error_code ec;
    m_socket.close( ec );
}

void Client::disconnected() { SPDLOG_INFO( "Client disconnected from: {}", m_connectionID ); }

Client::~Client() {}

} // namespace network
} // namespace mega
