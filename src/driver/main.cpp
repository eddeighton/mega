//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include "service/network/log.hpp"

#include <spdlog/spdlog.h>

#include <boost/archive/archive_exception.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>

#pragma warning( push )
#pragma warning( disable : 4996 ) // iterator thing
#pragma warning( disable : 4244 ) // conversion to DWORD from system_clock::rep
#include <boost/process.hpp>
#pragma warning( pop )

#include <cstdlib>

#include <iostream>
#include <iomanip>
#include <bitset>

// dummy implementation of MPOContext
namespace mega
{
class MPOContext
{
};
MPOContext* getMPOContext() { return nullptr; }
void        resetMPOContext() {}
void        setMPOContext( MPOContext* pMPOContext ) {}
} // namespace mega

namespace driver
{
#define COMMAND( cmd, input, desc )                                                   \
    namespace cmd                                                              \
    {                                                                          \
    extern void command( bool bHelp, const std::vector< std::string >& args ); \
    }
#include "commands.hxx"
#undef COMMAND

} // namespace driver

enum MainCommand
{
#define COMMAND( cmd, input, desc ) eCmd_##cmd,
#include "commands.hxx"
#undef COMMAND
    TOTAL_MAIN_COMMANDS
};

int main( int argc, const char* argv[] )
{
    //boost::timer::cpu_timer timer;

    boost::filesystem::path logDir             = boost::filesystem::current_path();
    std::string             strConsoleLogLevel = "info";
    std::string             strLogFileLevel    = "off";
    bool                    bWait              = false;

    std::bitset< TOTAL_MAIN_COMMANDS > cmds;
    MainCommand                        mainCmd = TOTAL_MAIN_COMMANDS;

    // commands
    std::string strBuildCommand;

    {
        std::vector< std::string > commandArgs;

        namespace po = boost::program_options;
        po::variables_map vm;
        bool              bGeneralWait = false;

#define COMMAND( cmd, input, desc ) bool bCmd_##cmd = false;
#include "commands.hxx"
#undef COMMAND

        po::options_description genericOptions( " General" );
        {
            // clang-format off
            genericOptions.add_options()
            ( "help,?",                                                         "Produce general or command help message" )
            ( "log_dir",    po::value< boost::filesystem::path >( &logDir ),    "Build log directory" )
            ( "console",    po::value< std::string >( &strConsoleLogLevel ),    "Console logging level" )
            ( "level",      po::value< std::string >( &strLogFileLevel ),       "Log file logging level" )
            ( "wait",       po::bool_switch( &bGeneralWait ),                   "Wait at startup for attaching a debugger" );
            // clang-format on
        }

        po::options_description commandOptions( " Commands" );
        {
            commandOptions.add_options()
#define COMMAND( cmd, input, desc ) ( input, po::bool_switch( &bCmd_##cmd ), desc )
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

        mega::network::configureLog(
            logDir, "driver", mega::network::fromStr( strConsoleLogLevel ), mega::network::fromStr( strLogFileLevel ) );

        try
        {
            if ( bGeneralWait )
            {
                spdlog::info( "Waiting for input..." );
                char c;
                std::cin >> c;
            }

#define COMMAND( cmd, input, desc )                                                                   \
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
#define COMMAND( cmd, input, desc )                                 \
    case eCmd_##cmd:                                         \
        driver::cmd::command( bShowHelp, commandArguments ); \
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
