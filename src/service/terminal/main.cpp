
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
#include <bitset>

namespace terminal
{
#define COMMAND( cmd, desc )                                                   \
    namespace cmd                                                              \
    {                                                                          \
    extern void command( bool bHelp, const std::vector< std::string >& args ); \
    }
#include "commands.hxx"
#undef COMMAND

} // namespace terminal

enum MainCommand
{
#define COMMAND( cmd, desc ) eCmd_##cmd,
#include "commands.hxx"
#undef COMMAND
    TOTAL_MAIN_COMMANDS
};

int main( int argc, const char* argv[] )
{
    boost::filesystem::path logDir             = boost::filesystem::current_path();
    std::string             strConsoleLogLevel = "info";
    std::string             strLogFileLevel    = "debug";
    bool                    bWait              = false;

    std::bitset< TOTAL_MAIN_COMMANDS > cmds;
    MainCommand                        mainCmd = TOTAL_MAIN_COMMANDS;

    {
        std::vector< std::string > commandArgs;

        namespace po = boost::program_options;
        po::variables_map vm;
        bool              bGeneralWait = false;

#define COMMAND( cmd, desc ) bool bCmd_##cmd = false;
#include "commands.hxx"
#undef COMMAND

        po::options_description genericOptions( " General" );
        {
            // clang-format off
            genericOptions.add_options()
            ( "help",                                                           "Produce general or command help message" )
            ( "log_dir",    po::value< boost::filesystem::path >( &logDir ),    "Log directory" )
            ( "console",    po::value< std::string >( &strConsoleLogLevel ),    "Console logging level" )
            ( "level",      po::value< std::string >( &strLogFileLevel ),       "Log file logging level" )
            ( "wait",       po::bool_switch( &bGeneralWait ),                   "Wait at startup for attaching a debugger" );
            // clang-format on
        }

        po::options_description commandOptions( " Commands" );
        {
            commandOptions.add_options()
#define COMMAND( cmd, desc ) ( #cmd, po::bool_switch( &bCmd_##cmd ), desc )
#include "commands.hxx"
#undef COMMAND
                ;
        }

        if ( cmds.count() > 1 )
        {
            spdlog::info( "Invalid command combination. Type '--help' for options" );
            return 1;
        }

        po::options_description commandHiddenOptions( "" );
        {
            commandHiddenOptions.add_options()( "args", po::value< std::vector< std::string > >( &commandArgs ) );
        }

        po::options_description visibleOptions( "Allowed options" );
        visibleOptions.add( genericOptions ).add( commandOptions );

        po::options_description allOptions( "all" );
        allOptions.add( genericOptions ).add( commandOptions ).add( commandHiddenOptions );

        po::positional_options_description p;
        p.add( "args", -1 );

        po::parsed_options parsedOptions
            = po::command_line_parser( argc, argv ).options( allOptions ).positional( p ).allow_unregistered().run();
        po::store( parsedOptions, vm );
        po::notify( vm );

        auto logThreads = mega::network::configureLog( logDir, "terminal", mega::network::fromStr( strConsoleLogLevel ),
                                                       mega::network::fromStr( strLogFileLevel ) );

        try
        {
            if ( bGeneralWait )
            {
                spdlog::info( "Waiting for input..." );
                char c;
                std::cin >> c;
            }

#define COMMAND( cmd, desc )                                                                   \
    if ( bCmd_##cmd )                                                                          \
    {                                                                                          \
        cmds.set( eCmd_##cmd );                                                                \
        VERIFY_RTE_MSG( mainCmd == TOTAL_MAIN_COMMANDS, "Duplicate main commands specified" ); \
        mainCmd = eCmd_##cmd;                                                                  \
    }
#include "commands.hxx"
#undef COMMAND

            const bool bShowHelp = vm.count( "help" );

            std::vector< std::string > commandArguments
                = po::collect_unrecognized( parsedOptions.options, po::include_positional );

            switch ( mainCmd )
            {
#define COMMAND( cmd, desc )                                   \
    case eCmd_##cmd:                                           \
        terminal::cmd::command( bShowHelp, commandArguments ); \
        break;
#include "commands.hxx"
#undef COMMAND

                case TOTAL_MAIN_COMMANDS:
                default:
                    if ( vm.count( "help" ) )
                    {
                        std::cout << visibleOptions << "\n";
                    }
                    else
                    {
                        spdlog::info( "Invalid command. Type '--help' for options" );
                        return 1;
                    }
            }
            return 0;
        }
        catch ( boost::program_options::error& e )
        {
            spdlog::error( "Invalid input. {}. Type '--help' for options", e.what() );
            return 1;
        }
        catch ( boost::archive::archive_exception& ex )
        {
            spdlog::error( "Archive Exception: {} {}", ex.code, ex.what() );
            return 1;
        }
        catch ( std::exception& e )
        {
            spdlog::error( "Exception: {}", e.what() );
            return 1;
        }
        catch ( ... )
        {
            spdlog::error( "Unknown error" );
            return 1;
        }
    }

    // ensure standard output is flushed
    std::cout.flush();

    return 0;
}
