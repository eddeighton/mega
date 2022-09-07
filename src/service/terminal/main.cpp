
#include "mega/common.hpp"
#include "service/protocol/common/megastructure_installation.hpp"
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
    std::string                  strConsoleLogLevel = "info", strLogFileLevel = "debug";

    namespace po = boost::program_options;

    bool                    bShowHelp  = false;
    bool                    bListNodes = false;
    boost::filesystem::path pipelinePath;
    boost::filesystem::path projectPath;
    bool                    bGetMegastructureInstallation = false;
    bool                    bGetProject                   = false;
    bool                    bNewSimulation                = false;
    bool                    bListSimulations              = false;
    std::string             strSimulationID;
    bool                    bClearStash = false;
    bool                    bCapacity = false;
    bool                    bShutdown = false;
    bool                    bQuit       = false;

    po::options_description commands( "Commands" );

    // clang-format off
    commands.add_options()
    ( "help,?",         po::bool_switch( &bShowHelp ),                          "Show Command Line Help"    )
    ( "nodes,n",        po::bool_switch( &bListNodes ),                         "List network nodes"        )
    ( "pipeline,p",     po::value< boost::filesystem::path >( &pipelinePath ),  "Run a pipeline"            )
    ( "getInstall,i",   po::bool_switch( &bGetMegastructureInstallation ),      "Get the Mega Structure Installation" )
    ( "setProject,s",   po::value< boost::filesystem::path >( &projectPath ),   "Select a project"          )
    ( "getProject,g",   po::bool_switch( &bGetProject ),                        "Get current project"       )
    ( "exe,e",          po::bool_switch( &bNewSimulation ),                     "Start a new executor"      )
    ( "list,l",         po::bool_switch( &bListSimulations ),                   "List simulations"          )
    ( "test,t",         po::value< std::string >( &strSimulationID ),           "Test simulation read lock" )
    ( "capacity,c",     po::bool_switch( &bCapacity ),                          "Report logical address space capacity" )
    ( "clear,r",        po::bool_switch( &bClearStash ),                        "Clear stash"               )
    ( "shutdown,!",       po::bool_switch( &bShutdown ),                        "Shutdown mega structure"   )
    ( "quit,q",         po::bool_switch( &bQuit ),                              "Quit this host"            )
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
            = po::command_line_parser( argc, argv ).options( options.add( commands ) ).run();

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

    bool bRunLoop = false;

    auto logThreads = mega::network::configureLog( logFolder, "terminal", mega::network::fromStr( strConsoleLogLevel ),
                                                   mega::network::fromStr( strLogFileLevel ) );

    try
    {
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
                auto result = terminal.ListNetworkNodes();
                for ( const std::string& str : result )
                {
                    SPDLOG_INFO( "{}", str );
                }
            }
            else if ( !pipelinePath.empty() )
            {
                mega::pipeline::Configuration pipelineConfig;
                boost::filesystem::loadAsciiFile( pipelinePath, pipelineConfig.data() );
                const mega::network::PipelineResult result = terminal.PipelineRun( pipelineConfig );
                SPDLOG_INFO( "Pipeline result:\n {} \n {}", result.getSuccess(), result.getMessage() );
            }
            else if ( bGetMegastructureInstallation )
            {
                mega::network::MegastructureInstallation install = terminal.GetMegastructureInstallation();
                if ( install.isEmpty() )
                    SPDLOG_INFO( "No Mega Structure Installation" );
                else
                    SPDLOG_INFO( "Megastructure Installation: {}", install.getInstallationPath().string() );
            }
            else if ( bGetProject )
            {
                mega::network::Project project = terminal.GetProject();
                if ( project.isEmpty() )
                    SPDLOG_INFO( "No active project" );
                else
                    SPDLOG_INFO( "Active project: {}", project.getProjectInstallPath().string() );
            }
            else if ( !projectPath.empty() )
            {
                mega::network::Project project( projectPath );
                if ( terminal.SetProject( project ) )
                    SPDLOG_INFO( "Project set to: {}", project.getProjectInstallPath().string() );
                else
                    SPDLOG_INFO( "Set project failed" );
            }
            else if ( bNewSimulation )
            {
                const mega::network::ConversationID simulationID = terminal.SimNew();
                SPDLOG_INFO( "New simulation: {}", simulationID );
            }
            else if ( bListSimulations )
            {
                const std::vector< mega::network::ConversationID > simulationIds = terminal.SimList();
                for ( const mega::network::ConversationID& simID : simulationIds )
                {
                    SPDLOG_INFO( "{}", simID );
                }
            }
            else if ( !strSimulationID.empty() )
            {
                mega::network::ConversationID simID;
                {
                    std::istringstream is( strSimulationID );
                    is >> simID;
                }
                SPDLOG_INFO( "Attempting to read lock on {}", simID );
                terminal.testReadLock( simID );
                // SPDLOG_INFO( "{}", timeStamp );
            }
            else if( bClearStash )
            {
                terminal.ClearStash();
                SPDLOG_INFO( "Cleared stash" );
            }
            else if( bCapacity )
            {
                auto result = terminal.Capacity();
                SPDLOG_INFO( "Logical Address Space Capacity: {}", result );
            }
            else if( bShutdown )
            {
                SPDLOG_INFO( "Shutting down" );
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
                bRunLoop = true;
            }

            if ( !bRunLoop )
                break;

            projectPath.clear();
            pipelinePath.clear();
            strSimulationID.clear();

            {
                std::ostringstream os;
                {
                    std::string strLine;
                    while ( strLine.empty() )
                    {
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
        SPDLOG_ERROR( "Exception: {}", ex.what() );
        return -1;
    }
    catch ( ... )
    {
        SPDLOG_ERROR( "Unknown exception" );
        return -1;
    }

    return 0;
}
