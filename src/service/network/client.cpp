//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.


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

    SPDLOG_TRACE( "Client connected to: {}", m_connectionID );

    m_receiver.run( ioContext, m_connectionID );

    m_pSender = make_socket_sender( m_socket, m_connectionID );
}

void Client::stop()
{
    boost::system::error_code ec;
    m_socket.shutdown( m_socket.shutdown_receive, ec );
}

void Client::disconnected() { SPDLOG_TRACE( "Client disconnected from: {}", m_connectionID ); }

Client::~Client() {}

} // namespace network
} // namespace mega
