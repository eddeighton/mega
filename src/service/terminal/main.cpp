
#include "service/terminal.hpp"

#include "service/network/network.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"

#include "common/assert_verify.hpp"
#include "common/string.hpp"
#include "common/file.hpp"

#include "boost/program_options.hpp"
#include <boost/program_options/parsers.hpp>
#include <boost/filesystem/operations.hpp>

#include <iostream>
#include <optional>
#include <string>

int main( int argc, const char* argv[] )
{
    std::optional< std::string > optionalHostName;
    boost::filesystem::path      logFolder          = boost::filesystem::current_path() / "log";
    std::string                  strConsoleLogLevel = "warn", strLogFileLevel = "warn";
    bool bLoop = false;

    /*namespace po = boost::program_options;

    bool        bShowHelp       = false;
    bool        bShowVersion    = false;
    bool        bListHosts      = false;
    bool        bListActivities = false;
    std::string strPipeline;
    bool        bShutdown = false;
    bool        bQuit     = false;

    po::options_description commands( "Commands" );

    // clang-format off
    commands.add_options()
    ( "help,?",         po::bool_switch( &bShowHelp ),              "Show Command Line Help"    )
    ( "version,v",      po::bool_switch( &bShowVersion ),           "Get Version"               )
    ( "hosts,h",        po::bool_switch( &bListHosts ),             "List hosts"                )
    ( "activities,a",   po::bool_switch( &bListActivities ),        "List activiies"            )
    ( "pipeline,p",     po::value< std::string >( &strPipeline ),   "Run a pipeline"            )
    ( "shutdown,s",     po::bool_switch( &bShutdown ),              "Shutdown service"          )
    ( "loop,l",         po::bool_switch( &bLoop ),                  "Run interactively"         )
    ( "quit,q",         po::bool_switch( &bQuit ),                  "Quit this host"            )
    ;

    {
        po::options_description options( "General" );

        // clang-format off
        std::string strHostName;
        options.add_options()
        ( "name",       po::value< std::string >( &strHostName ),               "Host name" )
        ( "log",        po::value< boost::filesystem::path >( &logFolder ),     "Logging folder" )
        ( "console",    po::value< std::string >( &strConsoleLogLevel ),        "Console logging level" )
        ( "level",      po::value< std::string >( &strLogFileLevel ),           "Log file logging level" )
        ;
        // clang-format on

        po::parsed_options parsedOptions
            = po::command_line_parser( argc, argv ).options( options ).options( commands ).run();

        po::variables_map vm;
        po::store( parsedOptions, vm );
        po::notify( vm );

        if ( bShowHelp )
        {
            std::cout << options << commands << "\n";
            return 0;
        }
        if ( !strHostName.empty() )
        {
            optionalHostName.emplace( std::move( strHostName ) );
        }
    }*/

    const bool bRunLoop = bLoop; // capture bLoop as will be reset

    try
    {
        auto logThreads = mega::network::configureLog( logFolder, "terminal", mega::network::fromStr( strConsoleLogLevel ),
                                                       mega::network::fromStr( strLogFileLevel ) );

        mega::service::Terminal terminal( optionalHostName );

        auto result = terminal.listNetworkNodes();
        for( const std::string& str : result )
        {
            std::cout << str << std::endl;
        }

        // clang-format on
        /*while ( terminal.running() )
        {
            if ( bShowHelp )
            {
                std::cout << commands << std::endl;
            }
            else if ( bShowVersion )
            {
                auto version = terminal.GetVersion();
                std::cout << "Version is: " << version << std::endl;
            }
            else if ( bListHosts )
            {
                for ( const std::string& strHost : terminal.ListHosts() )
                {
                    std::cout << strHost << std::endl;
                }
            }
            else if ( bListActivities )
            {
                for ( const mega::network::ConversationID& conversationID : terminal.listActivities() )
                {
                    std::cout << "Conversation: " << conversationID.getID() << " "
                              << "connectionID: " << conversationID.getConnectionID() << std::endl;
                }
            }
            else if ( !strPipeline.empty() )
            {
                mega::pipeline::Configuration pipelineConfig;
                boost::filesystem::loadAsciiFile( boost::filesystem::path( strPipeline ), pipelineConfig.data() );
                const mega::network::PipelineResult result = terminal.PipelineRun( pipelineConfig );
                std::cout << "Pipeline result:\n" << result.getSuccess() << " : " << result.getMessage() << std::endl;
            }
            else if ( bShutdown )
            {
                terminal.Shutdown();
                break;
            }
            else if ( bQuit )
            {
                break;
            }
            else
            {
                std::cout << commands << std::endl;
            }

            if( !bRunLoop )
                break;

            {
                std::ostringstream os;
                {
                    std::string strLine;
                    while ( strLine.empty() )
                    {
                        std::cout << "megaterminal:";
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
                        = po::command_line_parser( common::simpleTokenise( os.str(), " " ) ).options( commands ).run();
                    po::variables_map vm;
                    po::store( parsedOptions, vm );
                    po::notify( vm );
                }
            }
        }*/
    }
    catch ( std::exception& ex )
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
