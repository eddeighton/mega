
#include "service/network/network.hpp"
#include "service/network/log.hpp"
#include "service/host/host.hpp"

#include "common/assert_verify.hpp"
#include "common/string.hpp"

#include "boost/program_options.hpp"
#include <boost/program_options/parsers.hpp>
#include <boost/filesystem/operations.hpp>

#include <iostream>
#include <optional>
#include <string>

int main( int argc, const char* argv[] )
{
    std::optional< std::string > optionalHostName;
    boost::filesystem::path      logFolder   = boost::filesystem::current_path() / "log";
    std::string                  strLogLevel = "warn";
    {
        bool bShowHelp = false;

        namespace po = boost::program_options;
        po::options_description options;

        // clang-format off
        std::string strHostName;
        options.add_options()
        ( "help",   po::bool_switch( &bShowHelp ),                      "Show Command Line Help" )
        ( "name",   po::value< std::string >( &strHostName ),           "Host name" )
        ( "log",    po::value< boost::filesystem::path >( &logFolder ), "Logging folder" )
        ( "level", po::value< std::string >( &strLogLevel ),            "Logging level" )
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
        auto logThreads = mega::network::configureLog( logFolder, "host", mega::network::fromStr( strLogLevel ) );

        mega::service::Host host( optionalHostName );

        bool bShowHelp        = false;
        bool bShowVersion     = false;
        bool bListHosts       = false;
        bool bRunTestPipeline = false;
        bool bQuit            = false;

        namespace po = boost::program_options;
        po::options_description options;

        // clang-format off
        options.add_options()
        ( "help,?",    po::bool_switch( &bShowHelp ),        "Show Command Line Help"   )
        ( "version,v", po::bool_switch( &bShowVersion ),     "Get Version"              )
        ( "hosts,h",   po::bool_switch( &bListHosts ),       "List hosts"               )
        ( "test,t",    po::bool_switch( &bRunTestPipeline ), "Run test pipeline"        )
        ( "quit,q",    po::bool_switch( &bQuit ),            "Quit this host"           )
        ;
        // clang-format on

        while ( true )
        {
            bShowHelp        = false;
            bShowVersion     = false;
            bListHosts       = false;
            bRunTestPipeline = false;
            bQuit            = false;

            {
                std::ostringstream os;
                {
                    std::string strLine;
                    while ( strLine.empty() )
                    {
                        std::cout << "megahost:";
                        std::getline( std::cin, strLine );
                    }
                    if ( strLine.front() != '-' )
                    {
                        if ( strLine.length() == 1 )
                            os << '-' << strLine;
                        else
                            os << "--" << strLine;
                    }
                    else
                        os << strLine;
                }
                {
                    po::parsed_options parsedOptions
                        = po::command_line_parser( common::simpleTokenise( os.str(), " " ) ).options( options ).run();
                    po::variables_map vm;
                    po::store( parsedOptions, vm );
                    po::notify( vm );
                }
            }

            if ( bShowHelp )
            {
                std::cout << options << std::endl;
            }
            else if ( bShowVersion )
            {
                std::cout << "Version is: " << host.GetVersion() << std::endl;
            }
            else if ( bListHosts )
            {
                for ( const std::string& strHost : host.ListHosts() )
                {
                    std::cout << strHost << std::endl;
                }
            }
            else if ( bRunTestPipeline )
            {
                std::cout << "Version is: " << host.runTestPipeline() << std::endl;
            }
            else if ( bQuit )
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
