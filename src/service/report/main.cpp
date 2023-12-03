
#include "http_server.hpp"
#include "report.hpp"
#include "http_logical_thread.hpp"

#include "service/network/network.hpp"
#include "service/network/log.hpp"
#include "service/protocol/model/report.hxx"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/config.hpp>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <functional>

int main( int argc, char* argv[] )
{
    using Port = boost::asio::ip::port_type;

    int                     uiNumThreads       = 1; // std::thread::hardware_concurrency();
    std::string             strIP              = "0.0.0.0";
    Port                    port               = 8080;
    Port                    daemonPort         = mega::network::MegaDaemonPort();
    boost::filesystem::path logFolder          = boost::filesystem::current_path() / "log";
    std::string             strConsoleLogLevel = "warn", strLogFileLevel = "warn";
    int                     timeoutSeconds = 20;
    {
        bool bShowHelp = false;

        namespace po = boost::program_options;
        po::options_description options;

        // clang-format off
        options.add_options()
        ( "help",    po::bool_switch( &bShowHelp ),                                         "Show Command Line Help" )
        ( "ip",      po::value< std::string >( &strIP ),                                    "Root IP Address" )
        ( "threads", po::value< int >( &uiNumThreads ),                                     "Max number of threads" )
        ( "log",     po::value< boost::filesystem::path >( &logFolder ),                    "Logging folder" )
        ( "console", po::value< std::string >( &strConsoleLogLevel ),                       "Console logging level" )
        ( "level",   po::value< std::string >( &strLogFileLevel ),                          "Log file logging level" )
        ( "port",    po::value< Port >( &port )->default_value( port ),                     "Report server http port number" )
        ( "daemon",  po::value< Port >( &daemonPort )->default_value( daemonPort ),         "Megastructure Daemon port number" )
        ( "timeout", po::value< int >( &timeoutSeconds )->default_value( timeoutSeconds ),  "HTTP Timeout in seconds" )
        ;
        // clang-format on

        po::parsed_options parsedOptions = po::command_line_parser( argc, argv ).options( options ).run();

        po::variables_map vm;
        po::store( parsedOptions, vm );
        po::notify( vm );

        if( bShowHelp )
        {
            std::cout << options << "\n";
            return 0;
        }

        if( strIP.empty() )
        {
            std::cerr << "Missing IP Address" << std::endl;
            return -1;
        }
        uiNumThreads
            = std::min( std::max( 1, uiNumThreads ), static_cast< int >( std::thread::hardware_concurrency() ) );
    }

    SPDLOG_INFO( "Starting report server on: {} : {}", strIP, port );

    auto log = mega::network::configureLog( mega::network::Log::Config{ logFolder, "report",
                                                                        mega::network::fromStr( strConsoleLogLevel ),
                                                                        mega::network::fromStr( strLogFileLevel ) } );

    try
    {
        auto const                     address = boost::asio::ip::make_address( strIP );
        boost::asio::ip::tcp::endpoint endPoint{ address, port };
        boost::asio::io_context        ioService{ uiNumThreads };
        mega::service::report::Report  reportService( ioService, log, daemonPort, timeoutSeconds, endPoint );

        {
            // THROW_TODO;
            // const auto& projectOpt = reportService.getProject();
            // if( projectOpt.has_value() )
            // {
            //     SPDLOG_INFO( "Reporting node started with MP: {} Project: {}",
            //                  reportService.getMP(),
            //                  projectOpt.value().getProjectInstallPath().string() );
            // }
            // else
            // {
            //     SPDLOG_INFO( "Reporting node started with MPO: {} Project: {}", reportService.getMP(), "none" );
            // }
        }

        mega::httpserver::ReportFactory reportFactory
            = [ &reportService ]( boost::asio::ip::tcp::socket& socket ) { reportService.createReport( socket ); };

        boost::asio::spawn(
            ioService,

            [ &ioService, endPoint, &reportFactory ]( boost::asio::yield_context yield_ctx )
            { return mega::httpserver::runHTPPServer( ioService, endPoint, reportFactory, yield_ctx ); },

            []( std::exception_ptr ex )
            {
                if( ex )
                {
                    std::rethrow_exception( ex );
                }
            } );

        std::vector< std::thread > v;
        v.reserve( uiNumThreads - 1 );
        for( auto i = uiNumThreads - 1; i > 0; --i )
        {
            v.emplace_back( [ &ioService ] { ioService.run(); } );
        }
        ioService.run();
    }
    catch( std::exception& ex )
    {
        SPDLOG_ERROR( "Encountered exception: {}", ex.what() );
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
