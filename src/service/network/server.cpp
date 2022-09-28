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


#include "service/network/server.hpp"
#include "service/network/log.hpp"

#include "service/network/network.hpp"
#include "service/network/end_point.hpp"

#include "boost/bind/bind.hpp"
#include "boost/asio/placeholders.hpp"

#include <iostream>

namespace mega
{
namespace network
{

Server::Connection::Connection( Server& server, boost::asio::io_context& ioContext,
                                ConversationManager& conversationManager )
    : m_server( server )
    , m_strand( boost::asio::make_strand( ioContext ) )
    , m_socket( m_strand )
    , m_receiver( conversationManager, m_socket, boost::bind( &Connection::disconnected, this ) )
{
}

Server::Connection::~Connection() {}

void Server::Connection::start()
{
    m_connectionID = makeConnectionID( m_socket );
    m_pSender      = make_socket_sender( m_socket, m_connectionID.value() );

    // calculate name
    {
        std::ostringstream os;
        os << m_connectionID.value();
        m_strName = os.str();
    }

    m_receiver.run( m_strand, m_connectionID.value() );

    // SPDLOG_TRACE( "New connection started: {}", m_strName );
}

void Server::Connection::stop()
{
    m_socket.shutdown( boost::asio::ip::tcp::socket::shutdown_both );
    m_receiver.stop();
}

void Server::Connection::disconnected()
{
    if ( m_socket.is_open() )
    {
        boost::system::error_code ec;
        m_socket.close( ec );
    }
    if ( m_disconnectCallback.has_value() )
    {
        ( *m_disconnectCallback )( m_connectionID.value() );
    }
    m_server.onDisconnected( shared_from_this() );
}

Server::Server( boost::asio::io_context& ioContext, ConversationManager& conversationManager, short port )
    : m_ioContext( ioContext )
    , m_conversationManager( conversationManager )
    , m_acceptor( m_ioContext, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), port ) )
{
}

Server::~Server() {}

void Server::stop()
{
    m_acceptor.close();
    {
        ConnectionMap temp = m_connections;
        for ( auto& [ id, pConnection ] : temp )
        {
            pConnection->stop();
        }
    }
}

void Server::waitForConnection()
{
    using tcp                      = boost::asio::ip::tcp;
    Connection::Ptr pNewConnection = std::make_shared< Connection >( *this, m_ioContext, m_conversationManager );
    m_acceptor.async_accept( pNewConnection->getSocket(),
                             boost::asio::bind_executor( pNewConnection->getStrand(),
                                                         boost::bind( &Server::onConnect, this, pNewConnection,
                                                                      boost::asio::placeholders::error ) ) );
}

void Server::onConnect( Connection::Ptr pNewConnection, const boost::system::error_code& ec )
{
    if ( !ec )
    {
        pNewConnection->start();
        m_connections.insert( std::make_pair( pNewConnection->getSocketConnectionID(), pNewConnection ) );
    }
    if ( m_acceptor.is_open() )
        waitForConnection();
}

void Server::onDisconnected( Connection::Ptr pConnection )
{
    m_connections.erase( pConnection->getSocketConnectionID() );

    for ( auto i = m_mpoMap.begin(), iEnd = m_mpoMap.end(); i != iEnd; ++i )
    {
        if ( i->second == pConnection )
        {
            m_mpoMap.erase( i );
            break;
        }
    }
}

Server::Connection::Ptr Server::getConnection( const ConnectionID& connectionID )
{
    ConnectionMap::iterator iFind = m_connections.find( connectionID );
    if ( iFind != m_connections.end() )
    {
        return iFind->second;
    }
    else
    {
        return Connection::Ptr();
    }
}

Server::Connection::Ptr Server::findConnection( const mega::MPO& mpo ) const
{
    const auto iFind = m_mpoMap.find( mpo );
    if ( iFind != m_mpoMap.end() )
    {
        return iFind->second;
    }
    else
    {
        return Connection::Ptr{};
    }
}

Server::Connection::Ptr Server::findConnection( const mega::MP& mp ) const
{
    const auto iFind = m_mpMap.find( mp );
    if ( iFind != m_mpMap.end() )
    {
        return iFind->second;
    }
    else
    {
        return Connection::Ptr{};
    }
}
void Server::mapConnection( const mega::MPO& mpo, Connection::Ptr pConnection )
{
    m_mpoMap.insert( { mpo, pConnection } );
}
void Server::unmapConnection( const mega::MPO& mpo ) { m_mpoMap.erase( mpo ); }
void Server::mapConnection( const mega::MP& mp, Connection::Ptr pConnection ) { m_mpMap.insert( { mp, pConnection } ); }
void Server::unmapConnection( const mega::MP& mp ) { m_mpMap.erase( mp ); }

} // namespace network
} // namespace mega
