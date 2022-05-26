
#include "service/network/network.hpp"
#include "service/host/host.hpp"

#include "common/assert_verify.hpp"

#include "boost/program_options.hpp"

#include <iostream>
#include <optional>
#include <string>

int main( int argc, const char* argv[] )
{
    std::optional< std::string > optionalHostName;
    {
        bool bShowHelp = false;

        namespace po = boost::program_options;
        po::options_description options;

        // clang-format off
        std::string strHostName;
        options.add_options()
        ( "help",   po::bool_switch( &bShowHelp ),      "Show Command Line Help" )
        ( "name",   po::value< std::string >( &strHostName ), "Host name" )
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
        if ( !strHostName.empty() )
        {
            optionalHostName.emplace( std::move( strHostName ) );
        }
    }

    try
    {
        mega::service::Host host( optionalHostName );

        while ( true )
        {
            std::string strLine;
            std::getline( std::cin, strLine );
            if ( strLine == "help" )
            {
                std::cout << "help      - show this" << std::endl;
                std::cout << "version   - show daemon version" << std::endl;
                std::cout << "hosts     - list daemon hosts" << std::endl;
                std::cout << "quit      - shutdown this host" << std::endl;
            }
            if ( strLine == "version" )
            {
                std::cout << "Version is: " << host.GetVersion() << std::endl;
            }
            else if ( strLine == "hosts" )
            {
                for ( const std::string& strHost : host.ListHosts() )
                {
                    std::cout << strHost << std::endl;
                }
            }
            else if ( strLine == "quit" )
            {
                break;
            }
            else
            {
                std::cout << "Unrecognised cmd" << std::endl;
            }
        }
    }
    catch ( std::exception& ex )
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
