
#include "service/network/network.hpp"

#include "common/assert_verify.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "boost/program_options.hpp"

#include "boost/asio.hpp"
#include "boost/array.hpp"

#include <iostream>

int main( int argc, const char* argv[] )
{
    std::string strIP;
    {
        bool bShowHelp = false;

        namespace po = boost::program_options;
        po::options_description options;

        // clang-format off
        options.add_options()
        ( "help",   po::bool_switch( &bShowHelp ),      "Show Command Line Help" )
        ( "ip",     po::value< std::string >( &strIP ), "Daemon IP Address" )
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
        boost::asio::io_context io_context;

        boost::asio::ip::tcp::resolver resolver( io_context );

        boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve( strIP, "megadaemon" );

        VERIFY_RTE_MSG( !endpoints.empty(), "Failed to resolve megadaemon service on ip: " << strIP );
    }
    catch ( std::exception& ex )
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
