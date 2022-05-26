
#include "service/network/network.hpp"

#include "service/root/root.hpp"

#include "common/assert_verify.hpp"

#include "boost/program_options.hpp"

#include <boost/asio/io_context.hpp>

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

int main( int argc, const char* argv[] )
{
    {
        bool bShowHelp = false;

        namespace po = boost::program_options;
        po::options_description options;

        // clang-format off
        options.add_options()
        ( "help",   po::bool_switch( &bShowHelp ), "Show Command Line Help" )
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

    try
    {
        boost::asio::io_context ioContext;

        mega::service::Root root( ioContext );

        std::vector< std::thread > threads;
        for ( int i = 0; i < std::thread::hardware_concurrency() - 1; ++i )
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
