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
#include "log/log.hpp"

#include "service/network/network.hpp"
#include "service/network/end_point.hpp"

#include <boost/bind/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/strand.hpp>

#include <iostream>

namespace mega::network
{

Server::Connection::Connection( Server& server, boost::asio::io_context& ioContext,
                                LogicalThreadManager& logicalthreadManager )
    : m_server( server )
    , m_strand( boost::asio::make_strand( ioContext ) )
    , m_socket( m_strand )
    , m_receiver( logicalthreadManager, m_socket, [ this ] { disconnected(); } )
{
}

void Server::Connection::start()
{
    VERIFY_RTE_MSG( !weak_from_this().expired(), "Server::Connection bad weak_ptr" );

    m_pSender = make_socket_sender( m_socket );
    m_receiver.run( m_strand, m_pSender );
    SPDLOG_TRACE( "Server::Connection::start connection started" );
}

void Server::Connection::stop()
{
    m_socket.shutdown( Traits::Socket::shutdown_both );
    m_receiver.stop();
}

void Server::Connection::disconnected()
{
    SPDLOG_TRACE( "Server::Connection::disconnected" );

    if( m_socket.is_open() )
    {
        boost::system::error_code ec;
        m_socket.close( ec );
    }
    if( m_disconnectCallback.has_value() )
    {
        ( *m_disconnectCallback )();
    }
    m_server.onDisconnected( shared_from_this() );
}

Server::Server( boost::asio::io_context& ioContext, LogicalThreadManager& logicalThreadManager, short port )
    : m_ioContext( ioContext )
    , m_logicalThreadManager( logicalThreadManager )
    , m_acceptor( m_ioContext, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), port ) )
{
}

void Server::stop()
{
    m_acceptor.close();
    {
        ConnectionMap temp = m_connections;
        for( auto& pConnection : temp )
        {
            pConnection->stop();
        }
    }
}

void Server::waitForConnection()
{
    Connection::Ptr pNewConnection = std::make_shared< Connection >( *this, m_ioContext, m_logicalThreadManager );
    m_acceptor.async_accept( pNewConnection->getSocket(),
                             boost::asio::bind_executor( pNewConnection->getStrand(),
                                                         boost::bind( &Server::onConnect, this, pNewConnection,
                                                                      boost::asio::placeholders::error ) ) );
}

void Server::onConnect( Connection::Ptr pNewConnection, const boost::system::error_code& ec )
{
    if( !ec )
    {
        pNewConnection->start();
        m_connections.insert( pNewConnection );
    }
    if( m_acceptor.is_open() )
        waitForConnection();
}

void Server::onDisconnected( Connection::Ptr pConnection )
{
    m_connections.erase( pConnection );

    for( auto i = m_connectionLabels.begin(), iEnd = m_connectionLabels.end(); i != iEnd; ++i )
    {
        if( i->second == pConnection )
        {
            m_connectionLabels.erase( i );
            break;
        }
    }
}

} // namespace mega::network
