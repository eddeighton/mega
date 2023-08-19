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
        std::atomic< bool >         bContinue = true;
        std::mutex                  mut;
        //mega::compiler::BuildStatus buildState;

        std::thread inputThread(
            [ &mut, &bContinue, &bShowUI, &bShowMsgs, &bShowStart ]()
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
                            std::lock_guard< std::mutex > lock( mut );
                            if( !bShowUI )
                            {
                                const char* pDrawColour = nullptr;
                                std::string sepLine = "";
                                switch( report.type )
                                {
                                    case mega::compiler::TaskReport::eMSG:
                                        if( bShowMsgs )
                                        {
                                            pDrawColour = common::COLOUR_BLUE_BEGIN;
                                            sepLine = "\n";
                                        }
                                        break;
                                    case mega::compiler::TaskReport::eCMD:
                                        if( bShowMsgs )
                                        {
                                            pDrawColour = common::COLOUR_CYAN_BEGIN;
                                            sepLine = "\n";
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
            } );

        inputThread.join();
    }
}

} // namespace driver::trace
