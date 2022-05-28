

#include "service/daemon/daemon.hpp"
#include "service/network/log.hpp"

#include "common/assert_verify.hpp"

#include "boost/program_options.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/filesystem/operations.hpp>

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

int main( int argc, const char* argv[] )
{
    std::string strIP = "localhost";

    using NumThreadsType        = decltype( std::thread::hardware_concurrency() );
    NumThreadsType uiNumThreads = 1U;
    boost::filesystem::path logFolder = boost::filesystem::current_path() / "log";
    {
        bool bShowHelp = false;

        namespace po = boost::program_options;
        po::options_description options;

        // clang-format off
        options.add_options()
        ( "help",    po::bool_switch( &bShowHelp ),                     "Show Command Line Help" )
        ( "ip",      po::value< std::string >( &strIP ),                "Root IP Address" )
        ( "threads", po::value< NumThreadsType >( &uiNumThreads ),      "Max number of threads" )
        ( "log",    po::value< boost::filesystem::path >( &logFolder ), "Logging folder" )
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

        uiNumThreads = std::min( std::max( 1U, uiNumThreads ), std::thread::hardware_concurrency() );
    }

    std::cout << "Connecting to: " << strIP << std::endl;

    try
    {
        auto logThreads = mega::network::configureLog( logFolder, "daemon" );

        boost::asio::io_context ioContext;

        mega::service::Daemon daemon( ioContext, strIP );

        std::vector< std::thread > threads;
        for ( NumThreadsType i = 0; i < uiNumThreads; ++i )
        {
            threads.emplace_back( std::move( std::thread( [ &ioContext ]() { ioContext.run(); } ) ) );
        }
        for ( std::thread& thread : threads )
        {
            thread.join();
        }
    }
    catch ( std::exception& ex )
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
