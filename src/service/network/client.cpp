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
#include "log/log.hpp"
#include "service/network/end_point.hpp"
#include "service/network/sender_factory.hpp"

#include <boost/bind/bind.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/strand.hpp>

#include <exception>
#include <future>
#include <iostream>

namespace mega::network
{

Client::Client( boost::asio::io_context& ioContext, LogicalThreadManager& logicalthreadManager,
                const std::string& strServiceIP, short portNumber )
    : m_ioContext( ioContext )
    , m_resolver( ioContext )
    , m_strand( boost::asio::make_strand( ioContext ) )
    , m_socket( m_strand )
    , m_receiver( logicalthreadManager, m_socket, [ this ] { disconnected(); } )
{
    std::string strPort;
    {
        std::ostringstream os;
        os << portNumber;
        strPort = os.str();
    }
    Traits::Resolver::query        query( strServiceIP, strPort );
    Traits::Resolver::results_type endpoints = m_resolver.resolve( query );

    if( endpoints.empty() )
    {
        SPDLOG_ERROR( "Failed to resolve ip: {} port: {}", strServiceIP, portNumber );
        THROW_RTE( "Failed to resolve ip: " << strServiceIP << " port: " << portNumber );
    }

    m_endPoint = boost::asio::connect( m_socket, endpoints );
    m_pSender  = make_socket_sender( m_socket );
    m_receiver.run( ioContext, m_pSender );
}

void Client::stop()
{
    boost::system::error_code ec;
    m_socket.shutdown( m_socket.shutdown_both, ec );
}

void Client::disconnected()
{
    SPDLOG_TRACE( "Client disconnected" );
}

Client::~Client() = default;

} // namespace mega::network
