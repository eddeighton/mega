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

#include "service/terminal.hpp"

#include "log/file_log.hpp"

#include "service/network/log.hpp"

#include "compiler/build_report.hpp"
#include "compiler/base_task.hpp"

#include "common/stl.hpp"
#include "common/compose.hpp"
#include "common/terminal.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

namespace driver::trace
{

void simpleTrace( std::atomic< bool >& bContinue, bool bShowMsgs, bool bShowStart )
{
    while( bContinue )
    {
        std::string strLine;
        if( std::getline( std::cin, strLine ) )
        {
            std::istringstream         is( strLine );
            mega::compiler::TaskReport report;
            bool                       bReadReport = false;

            try
            {
                is >> report;
                bReadReport = true;
            }
            catch( std::exception& )
            {
                // ignore bad lines
            }
            if( bReadReport )
            {
                const char* pDrawColour = nullptr;
                std::string sepLine     = "";
                switch( report.type )
                {
                    case mega::compiler::TaskReport::eMSG:
                        if( bShowMsgs )
                        {
                            pDrawColour = common::COLOUR_BLUE_BEGIN;
                            sepLine     = "\n";
                        }
                        break;
                    case mega::compiler::TaskReport::eCMD:
                        if( bShowMsgs )
                        {
                            pDrawColour = common::COLOUR_CYAN_BEGIN;
                            sepLine     = "\n";
                        }
                        break;
                    case mega::compiler::TaskReport::eOUT:
                        if( bShowMsgs )
                        {
                            pDrawColour = common::COLOUR_CYAN_BEGIN;
                        }
                        break;
                    case mega::compiler::TaskReport::eSTARTED:
                        if( bShowStart )
                        {
                            pDrawColour = common::COLOUR_WHITE_BEGIN;
                        }
                        break;
                    case mega::compiler::TaskReport::eFAILED:
                        pDrawColour = common::COLOUR_RED_BEGIN;
                        break;
                    case mega::compiler::TaskReport::eERROR:
                        pDrawColour = common::COLOUR_RED_BEGIN;
                        break;
                    case mega::compiler::TaskReport::eCACHED:
                        pDrawColour = common::COLOUR_YELLOW_BEGIN;
                        break;
                    case mega::compiler::TaskReport::eSUCCESS:
                        pDrawColour = common::COLOUR_GREEN_BEGIN;
                        break;
                    case mega::compiler::TaskReport::TOTAL_REPORT_TYPES:
                        break;
                }
                if( pDrawColour )
                {
                    std::cout << pDrawColour <<

                        std::setw( 9 ) << std::setfill( ' ' ) << report.type <<

                        std::setw( 35 ) << std::setfill( ' ' ) << report.name.task <<

                        std::setw( 30 ) << std::right << std::setfill( ' ' ) << report.name.source <<

                        " -> " <<

                        std::setw( 80 ) << std::left << std::setfill( ' ' ) << report.name.target <<

                        sepLine << report.info <<

                        common::COLOUR_END << std::endl;
                }
            }
        }
    }
}

void simpleReport( std::atomic< bool >& bContinue, bool bShowMsgs, bool bShowStart )
{
    mega::compiler::BuildStatus buildState;

    auto printTask = [ bShowMsgs, bShowStart ]( const mega::compiler::TaskStatus& taskStatus, const char* pDrawColour )
    {
        if( bShowMsgs )
        {
            std::cout << pDrawColour <<

                std::setw( 35 ) << std::setfill( ' ' ) << taskStatus.getName().task <<

                std::setw( 30 ) << std::right << std::setfill( ' ' ) << taskStatus.getName().source <<

                " -> " <<

                std::setw( 80 ) << std::left << std::setfill( ' ' ) << taskStatus.getName().target <<

                common::COLOUR_END << "\n";

            if( bShowStart )
            {
                std::cout << taskStatus.getCmd() << "\n";
            }

            for( const auto& str : taskStatus.getOutput() )
            {
                if( !str.empty() )
                {
                    std::cout << str << "\n";
                }
            }
            for( const auto& str : taskStatus.getError() )
            {
                if( !str.empty() )
                {
                    std::cout << common::COLOUR_RED_BEGIN << str << common::COLOUR_END << "\n";
                }
            }
        }
        else
        {
            std::cout << pDrawColour <<

                std::setw( 35 ) << std::setfill( ' ' ) << taskStatus.getName().task <<

                std::setw( 30 ) << std::right << std::setfill( ' ' ) << taskStatus.getName().source <<

                " -> " <<

                std::setw( 80 ) << std::left << std::setfill( ' ' ) << taskStatus.getName().target <<

                common::COLOUR_END << "\n";
        }
    };

    while( bContinue )
    {
        std::string strLine;
        if( std::getline( std::cin, strLine ) )
        {
            std::istringstream         is( strLine );
            mega::compiler::TaskReport report;
            bool                       bReadReport = false;

            try
            {
                is >> report;
                bReadReport = true;
            }
            catch( std::exception& )
            {
                // ignore bad lines
            }
            if( bReadReport )
            {
                buildState.progress( report );

                mega::compiler::BuildStatus::TaskStatusMap status = buildState.get();

                // clear the screen
                // std::cout << "\x1B[2J\x1B[H";
                system( "clear" );

                std::cout << "Active:\n\n";
                for( const auto& [ taskName, taskStatus ] : status )
                {
                    if( taskStatus.getState() == mega::compiler::eTask_Started )
                    {
                        printTask( taskStatus, common::COLOUR_WHITE_BEGIN );
                    }
                }

                std::cout << "\nFailed:\n\n";
                for( const auto& [ taskName, taskStatus ] : status )
                {
                    if( taskStatus.getState() == mega::compiler::eTask_Failed )
                    {
                        printTask( taskStatus, common::COLOUR_RED_BEGIN );
                    }
                }
                /*
                                std::cout << "\nSuccess:\n\n";
                                for( const auto& [ taskName, taskStatus ] : status )
                                {
                                    if( taskStatus.getState() == mega::compiler::eTask_Success )
                                    {
                                        printTask( taskStatus, common::COLOUR_GREEN_BEGIN );
                                    }
                                }

                                std::cout << "\nCached:\n\n";
                                for( const auto& [ taskName, taskStatus ] : status )
                                {
                                    if( taskStatus.getState() == mega::compiler::eTask_Cached )
                                    {
                                        printTask( taskStatus, common::COLOUR_YELLOW_BEGIN );
                                    }
                                }
                */
                std::cout << std::endl;
            }
        }
    }
}
void command( bool bHelp, const std::vector< std::string >& args )
{
    namespace po    = boost::program_options;
    bool bShowUI    = true;
    bool bShowMsgs  = false;
    bool bShowStart = false;

    po::options_description commandOptions( " Simulation Commands" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "ui",     po::bool_switch( &bShowUI ),    "Show FTXUI user interface for progress report" )
            ( "msg",    po::bool_switch( &bShowMsgs ),  "Show messages" )
            ( "start",  po::bool_switch( &bShowStart ), "Show starts" )

            ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "file", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        std::atomic< bool > bContinue = true;

        if( bShowUI )
        {
            std::thread inputThread( [ &bContinue, bShowMsgs, bShowStart ]()
                                     { simpleReport( bContinue, bShowMsgs, bShowStart ); } );
            inputThread.join();
        }
        else
        {
            std::thread inputThread( [ &bContinue, bShowMsgs, bShowStart ]()
                                     { simpleTrace( bContinue, bShowMsgs, bShowStart ); } );
            inputThread.join();
        }
    }
}

} // namespace driver::trace
