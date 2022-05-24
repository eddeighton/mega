

#include "mega/coroutine.hpp"

#include "service/network/network.hpp"

#include "common/assert_verify.hpp"

#include "boost/asio/io_context.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/ts/netfwd.hpp"
#include "boost/asio.hpp"

#include "boost/bind/bind.hpp"
#include "boost/array.hpp"
#include "boost/program_options.hpp"

#include <boost/asio/detached.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/system/detail/error_code.hpp>
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
/*
class Client : public mega::network::Receiver::MessageHandler
{
public:
    Client( boost::asio::io_context& io_context, const std::string& strServerIP, mega::network::ChannelType& channel )
        : m_io_context( io_context )
        , m_work_guard( io_context.get_executor() )
        , m_resolver( m_io_context )
        , m_socket( m_io_context )
        , m_watchDogTimer( m_io_context )
        , m_pReceiver( std::make_shared< mega::network::Receiver >( io_context, m_socket, *this ) )
        , m_pSender( std::make_shared< mega::network::Sender >( m_socket ) )
        , m_channel( channel )
    {
        boost::asio::ip::tcp::resolver::results_type endpoints
            = m_resolver.resolve( strServerIP, mega::network::MegaRootServiceName() );

        VERIFY_RTE_MSG(
            !endpoints.empty(),
            "Failed to resolve " << mega::network::MegaRootServiceName() << " service on ip: " << strServerIP );

        m_endPoint = boost::asio::connect( m_socket, endpoints );

        m_pReceiver->updateLastActivityTime();

        auto pReceiver = m_pReceiver;
        boost::asio::spawn( m_socket.get_executor(),
                            [ pReceiver ]( boost::asio::yield_context yield ) { pReceiver->receive( yield ); } );

        boost::asio::spawn( m_socket.get_executor(),
                            [ &channel ]( boost::asio::yield_context yield )
                            {
                                while ( true )
                                {
                                    int i = channel.async_receive( yield );

                                    std::cout << "Got: " << i << std::endl;
                                }
                            } );
        // start the watch dog
        // OnWatchDog();
    }
    
        void OnWatchDog()
        {
            // boost::asio::
            using namespace std::chrono_literals;
            if ( m_pReceiver->getLastActivityTime() + 5s < std::chrono::steady_clock::now() )
            {
                std::cout << "Inactivity detected by watchdog" << std::endl;
                m_socket.close();
                m_work_guard.reset();
            }
            else
            {
                using namespace std::chrono_literals;
                m_watchDogTimer.expires_after( 1s );
                m_watchDogTimer.async_wait( boost::bind( &Client::OnWatchDog, this ) );
                m_pSender->send_heartbeat_request();
            }
        }

    virtual void onHelloRequest( const mega::network::HelloMsg& rq )
    {
        // std::cout << "Received Hello request msg: " << rq.msg() << std::endl;
    }

    virtual void onHelloResponse( const mega::HelloResponse& rq )
    {
        // std::cout << "Received Hello response msg: " << rq.msg() << std::endl;
    }
    
        virtual void onHeartbeatRequest( const mega::HeartBeatRequest& rq )
        {
            // std::cout << "Heartbeat request: " << rq.time() << std::endl;
            m_pSender->send_heartbeat_response();
        }

        virtual void onHeartbeatResponse( const mega::HeartBeatResponse& rq )
        {
            // std::cout << "Heartbeat response: " << rq.time() << std::endl;
        }

    void send( const std::string& strMsg ) { m_pSender->send_hello_request( strMsg ); }

    boost::asio::io_context&                                                    m_io_context;
    boost::asio::executor_work_guard< decltype( m_io_context.get_executor() ) > m_work_guard;
    boost::asio::ip::tcp::resolver                                              m_resolver;
    boost::asio::ip::tcp::socket                                                m_socket;
    boost::asio::ip::tcp::endpoint                                              m_endPoint;

    boost::asio::steady_timer m_watchDogTimer;

    mega::network::Receiver::Ptr m_pReceiver;
    mega::network::Sender::Ptr   m_pSender;
    mega::network::ChannelType&  m_channel;
};

mega::ActionCoroutine test( boost::asio::yield_context yield )
{
    co_yield mega::ReturnReason();

    std::cout << "Coroutine works!" << std::endl;
    boost::asio::post( yield );
    std::cout << "Coroutine yield works!" << std::endl;

    co_return mega::ReturnReason();
}

mega::ActionCoroutine test2()
{
    co_await test();
}
*/

int main( int argc, const char* argv[] )
{
    std::string strIP = "localhost";
    {
        bool bShowHelp = false;

        namespace po = boost::program_options;
        po::options_description options;

        // clang-format off
        options.add_options()
        ( "help",   po::bool_switch( &bShowHelp ),          "Show Command Line Help" )
        ( "ip",     po::value< std::string >( &strIP ),     "Root IP Address" )
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

        if ( strIP.empty() )
        {
            std::cerr << "Missing IP Address" << std::endl;
            return -1;
        }
    }

    std::cout << "Connecting to: " << strIP << std::endl;

    try
    {
        /*boost::asio::io_context networkQueue;
        boost::asio::io_context simQueue;

        mega::network::ChannelType channel( networkQueue );

        Client client( networkQueue, strIP, channel );

        boost::asio::spawn( simQueue,
                            [ &channel, &client ]( boost::asio::yield_context yield )
                            {
                                int iCounter = 0;
                                while ( true )
                                {
                                    //for ( int i = 0; i < 100; ++i )
                                     //   client.send( mega::network::TEST_MESSAGE );

                                    channel.async_send( boost::system::error_code(), iCounter++, yield );
                                    // std::cout << "Channel returned: " << result << std::endl;

                                    auto t = test( yield );
                                    t.resume();
                                    t.resume();

                                    boost::asio::post( yield );

                                    //boost::asio::steady_timer::async_wait()
                                }
                            } );*/

        /*
                std::string strLine;
                std::getline( std::cin, strLine );
                std::istringstream is( strLine );
                int iRate = 1;
                is >> iRate;

                int iCounter = 0;
                while ( true )
                {

                    for ( int i = 0; i < iRate; ++i )
                        client.send( mega::network::TEST_MESSAGE );

                    channel.async_send(boost::system::error_code(), iCounter++, boost::asio::detached );

                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for( 100ms );
                }

        std::thread networkThread( [ &networkQueue ]() { networkQueue.run(); } );
        std::thread simThread( [ &simQueue ]() { simQueue.run(); } );

        int iCounter = 0;
        while ( true )
        {
            auto f =
                channel.async_send( boost::system::error_code(), ++iCounter, boost::asio::use_future );
            
            using namespace std::chrono_literals;
            if( std::future_status::ready == f.wait_for( 10ms ) )
            {
                std::cout << "Future wait success" << std::endl;
            }


            if ( channel.try_send( boost::system::error_code(), ++iCounter ) )
            {
                std::cout << "try_send success" << std::endl;
            }
            else
            {
                std::cout << "try_send failure" << std::endl;
            }
            using namespace std::chrono_literals;
            std::this_thread::sleep_for( 100ms );
        }

        networkThread.join();
        simThread.join();*/
    }
    catch ( std::exception& ex )
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
