
#include "service/network/server.hpp"

#include "service/network/network.hpp"

#include "boost/bind/bind.hpp"

namespace mega
{
namespace network
{

Server::Connection::Connection( Server& server, Server::ExecutionContextType& execution_context )
    : m_server( server )
    , m_strand( boost::asio::make_strand( execution_context ) )
    , m_socket( execution_context )
    , m_receiver( m_socket )
    , m_watchDogTimer( execution_context )
{
}

void Server::Connection::start()
{
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

Server::Server( ExecutionContextType& execution_context )
    : m_execution_context( execution_context )
    , m_acceptor( m_execution_context,
                  boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), mega::network::MegaRootPort() ) )
{
}

void Server::waitForConnection()
{
    using tcp                      = boost::asio::ip::tcp;
    Connection::Ptr pNewConnection = std::make_shared< Connection >( *this, m_execution_context );
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
        m_connections.insert( pNewConnection );
        // std::cout << "New connection from: " << pNewConnection->getName() << std::endl;
    }
    else
    {
        // std::cout << "Connection Error: " << ec.what() << std::endl;
    }
    waitForConnection();
}

void Server::onDisconnected( Connection::Ptr pConnection )
{
    m_connections.erase( pConnection );
    // std::cout << "Connection lost from: " << pConnection->getName() << std::endl;
}
} // namespace network
} // namespace mega
