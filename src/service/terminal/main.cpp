
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

    namespace po = boost::program_options;

    bool                    bShowHelp  = false;
    bool                    bListNodes = false;
    boost::filesystem::path pipelinePath;
    boost::filesystem::path projectPath;
    bool                    bGetProject = false;
    bool                    bLoop       = false;
    bool                    bQuit       = false;

    po::options_description commands( "Commands" );

    // clang-format off
    commands.add_options()
    ( "help,?",         po::bool_switch( &bShowHelp ),                          "Show Command Line Help" )
    ( "nodes,n",        po::bool_switch( &bListNodes ),                         "List network nodes"     )
    ( "pipeline,p",     po::value< boost::filesystem::path >( &pipelinePath ),  "Run a pipeline"         )
    ( "setProject,s",   po::value< boost::filesystem::path >( &projectPath ),   "Select a project"       )
    ( "getProject,g",   po::bool_switch( &bGetProject ),                        "Get current project"    )
    ( "loop,l",         po::bool_switch( &bLoop ),                              "Run interactively"      )
    ( "quit,q",         po::bool_switch( &bQuit ),                              "Quit this host"         )
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
    }

    bool bRunLoop = bLoop; // capture bLoop as will be reset

    try
    {
        auto logThreads
            = mega::network::configureLog( logFolder, "terminal", mega::network::fromStr( strConsoleLogLevel ),
                                           mega::network::fromStr( strLogFileLevel ) );

        mega::service::Terminal terminal( optionalHostName );

        // clang-format on
        while ( terminal.running() )
        {
            if ( bShowHelp )
            {
                std::cout << commands << std::endl;
            }
            else if ( bListNodes )
            {
                auto result = terminal.listNetworkNodes();
                for ( const std::string& str : result )
                {
                    std::cout << str << std::endl;
                }
            }
            else if ( !pipelinePath.empty() )
            {
                mega::pipeline::Configuration pipelineConfig;
                boost::filesystem::loadAsciiFile( pipelinePath, pipelineConfig.data() );
                const mega::network::PipelineResult result = terminal.PipelineRun( pipelineConfig );
                std::cout << "Pipeline result:\n" << result.getSuccess() << " : " << result.getMessage() << std::endl;
            }
            else if ( bGetProject )
            {
                mega::network::Project project = terminal.GetProject();
                if ( project.getProjectInstallPath().empty() )
                    std::cout << "No active project" << std::endl;
                else
                    std::cout << "Active project: " << project.getProjectInstallPath() << std::endl;
            }
            else if ( !projectPath.empty() )
            {
                mega::network::Project project( projectPath );
                if ( terminal.SetProject( project ) )
                    std::cout << "Project set to: " << project.getProjectInstallPath() << std::endl;
                else
                    std::cout << "Set project failed";
            }
            else if ( bQuit )
            {
                break;
            }
            else
            {
                std::cout << commands << std::endl;
                bRunLoop = true;
            }

            if ( !bRunLoop )
                break;

            projectPath.clear();
            pipelinePath.clear();
            
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
        }
    }
    catch ( std::exception& ex )
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
