
#include "service/network/server.hpp"

#include "service/network/network.hpp"
#include "service/network/end_point.hpp"

#include "boost/bind/bind.hpp"
#include "boost/asio/placeholders.hpp"

namespace mega
{
namespace network
{

Server::Connection::Connection( Server& server, boost::asio::io_context& ioContext )
    : m_server( server )
    , m_strand( boost::asio::make_strand( ioContext ) )
    , m_socket( m_strand )
    , m_receiver( server, m_decoder, m_socket ) // would be 2 decoders here
    , m_watchDogTimer( m_strand )
{
}

void Server::Connection::start()
{
    boost::asio::ip::tcp::endpoint t;

    // calculate name
    {
        std::ostringstream os;
        os << m_socket.local_endpoint() << " : " << m_socket.remote_endpoint();
        m_strName = os.str();
    }

    m_receiver.run( m_strand );
    // start the watch dog
    // OnWatchDog();
}

Server::Server( boost::asio::io_context& ioContext, ActivityFactory& activityFactory )
    : ActivityManager( ioContext, activityFactory )
    , m_acceptor(
          m_ioContext, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), mega::network::MegaRootPort() ) )
{
}

void Server::waitForConnection()
{
    using tcp                      = boost::asio::ip::tcp;
    Connection::Ptr pNewConnection = std::make_shared< Connection >( *this, m_ioContext );
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
        m_connections.insert( std::make_pair( getConnectionID( pNewConnection->getSocket() ), pNewConnection ) );
    }
    waitForConnection();
}

void Server::onDisconnected( Connection::Ptr pConnection )
{
    m_connections.erase( getConnectionID( pConnection->getSocket() ) );
}

Server::Connection::Ptr Server::getConnection( const ConnectionID& connectionID )
{
    ConnectionMap::iterator iFind = m_connections.find( connectionID );
    if( iFind != m_connections.end() )
    {
        return iFind->second;
    }
    else
    {
        return Connection::Ptr();
    }
}
} // namespace network
} // namespace mega
