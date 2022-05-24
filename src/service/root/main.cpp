
#include "service/network/network.hpp"

#include "common/assert_verify.hpp"

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/executor.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "boost/asio.hpp"
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/spawn.hpp>

#include "boost/program_options.hpp"
#include "boost/array.hpp"
#include "boost/bind/bind.hpp"
#include <boost/core/ref.hpp>
#include <boost/system/detail/error_code.hpp>

#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <vector>
#include <set>

/*
class RootServer;

class Connection : public mega::network::Receiver::MessageHandler, public std::enable_shared_from_this< Connection >
{
public:
    using Ptr = std::shared_ptr< Connection >;

    template < typename TExecutorType >
    Connection( RootServer& server, TExecutorType& io_context )
        : m_server( server )
        //, m_strand( boost::asio::make_strand( io_context ) )
        , m_socket( io_context )
        , m_pReceiver( std::make_shared< mega::network::Receiver >( io_context, m_socket, *this ) )
        , m_pSender( std::make_shared< mega::network::Sender >( m_socket ) )
        , m_watchDogTimer( io_context )
    {
    }

    ~Connection() {}
    
    void OnWatchDog();
    
    void start()
    {
        std::ostringstream os;
        os << m_socket.local_endpoint() << " : " << m_socket.remote_endpoint();
        m_strName = os.str();

        m_pReceiver->updateLastActivityTime();

        auto pReceiver = m_pReceiver;
        boost::asio::spawn( m_socket.get_executor(),
                            [ pReceiver ]( boost::asio::yield_context yield ) { pReceiver->receive( yield ); } );

        //boost::asio::spawn( m_socket.get_executor(),
        //                    [ pReceiver ]( boost::asio::yield_context yield ) { pReceiver->receive( yield ); } );

        // start the watch dog
        // OnWatchDog();
    }

    virtual void onHelloRequest( const mega::network::HelloMsg& rq )
    {
        static int iCounter = 0;
        if ( ( ++iCounter % 1000 ) == 0 )
        {
            std::cout << ".";
            std::cout.flush();
        }

        // std::cout << "Received Hello request msg: " << rq.msg() << std::endl;
        // m_pSender->send_hello_response( rq.msg() );
    }

    // virtual void onHelloResponse( const mega::HelloResponse& rq ) {}
    
        virtual void onHeartbeatRequest( const mega::HeartBeatRequest& rq )
        {
            m_pSender->send_heartbeat_response();
        }

        virtual void onHeartbeatResponse( const mega::HeartBeatResponse& rq )
        {
            //
        }

    const std::string&            getName() const { return m_strName; }
    boost::asio::ip::tcp::socket& getSocket() { return m_socket; }

    // boost::asio::strand< ASIOQueueType::executor_type >& getStrand() { return m_strand; }

private:
    RootServer& m_server;
    // boost::asio::strand< ASIOQueueType::executor_type > m_strand;
    boost::asio::ip::tcp::socket m_socket;
    mega::network::Receiver::Ptr m_pReceiver;
    mega::network::Sender::Ptr   m_pSender;
    boost::asio::steady_timer    m_watchDogTimer;
    std::string                  m_strName;
};

class RootServer
{
public:
    RootServer( mega::network::ASIOQueueType& io_context )
        : m_io_context( io_context )
        , m_acceptor(
              io_context, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), mega::network::MegaRootPort() ) )
    {
    }

    void waitForConnection()
    {
        using tcp                      = boost::asio::ip::tcp;
        Connection::Ptr pNewConnection = std::make_shared< Connection >( *this, m_io_context );
        m_acceptor.async_accept( pNewConnection->getSocket(),
                                 // boost::asio::bind_executor( pNewConnection->getStrand(),
                                 boost::bind( &RootServer::onConnect, this, pNewConnection,
                                              boost::asio::placeholders::error ) ); //);
    }

    void onConnect( Connection::Ptr pNewConnection, const boost::system::error_code& ec )
    {
        if ( !ec )
        {
            pNewConnection->start();
            m_connections.insert( pNewConnection );
            std::cout << "New connection from: " << pNewConnection->getName() << std::endl;
        }
        else
        {
            std::cout << "Connection Error: " << ec.what() << std::endl;
        }
        waitForConnection();
    }

    void onDisconnected( Connection::Ptr pConnection )
    {
        m_connections.erase( pConnection );
        std::cout << "Connection lost from: " << pConnection->getName() << std::endl;
    }

    mega::network::ASIOQueueType&  m_io_context;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::set< Connection::Ptr >    m_connections;
};

void Connection::OnWatchDog()
{
    // boost::asio::
    using namespace std::chrono_literals;
    if ( m_pReceiver->getLastActivityTime() + 5s < std::chrono::steady_clock::now() )
    {
        std::cout << "Inactivity detected by watchdog" << std::endl;
        m_watchDogTimer.cancel();
        m_socket.cancel();
        m_socket.close();
        m_server.onDisconnected( shared_from_this() );
    }
    else
    {
        using namespace std::chrono_literals;
        m_watchDogTimer.expires_after( 1s );
        m_watchDogTimer.async_wait( boost::bind( &Connection::OnWatchDog, shared_from_this() ) );
        m_pSender->send_heartbeat_request();
    }
}
*/

int main( int argc, const char* argv[] )
{
    {
        bool bShowHelp = false;

        namespace po = boost::program_options;
        po::options_description options;

        // clang-format off
        options.add_options()
        ( "help",   po::bool_switch( &bShowHelp ),      "Show Command Line Help" )
        ;
        // clang-format on

        po::parsed_options parsedOptions = po::command_line_parser( argc, argv ).options( options ).run();

        po::variables_map vm;
        po::store( parsedOptions, vm );
        po::notify( vm );

        if ( bShowHelp )
        {
            std::cout << options << "\n";
            return 0;
        }
    }

    /*try
    {
        mega::network::ASIOQueueType thread_pool;
        RootServer                   server( thread_pool );
        server.waitForConnection();

        thread_pool.run();
    }
    catch ( std::exception& ex )
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return -1;
    }*/

    return 0;
}
