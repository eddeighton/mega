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

#include "mega/reference_io.hpp"

#include "boost/program_options.hpp"
#include "boost/filesystem/path.hpp"

#include <vector>
#include <string>
#include <iostream>

namespace driver
{
namespace sim
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    bool        bList = false;
    std::string strCreate, strList, strDestroy, strID, strRead, strWrite, strRelease, strSuspend, strResume, strStop,
        strStart;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Simulation Commands" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "create",     po::value( &strCreate ),        "Create using Executor MPO" )
            ( "list",       po::bool_switch( &bList ) ,     "List" )
            ( "destroy",    po::value( &strDestroy ) ,      "Destroy" )
            ( "id",         po::value( &strID ) ,           "ID to use with lock commands" )
            ( "read",       po::value( &strRead ) ,         "Obtain read lock" )
            ( "write",      po::value( &strWrite ) ,        "Obtain write lock" )
            ( "release",    po::value( &strRelease ) ,      "Release locks" )

            ( "suspend",    po::value( &strSuspend ) ,      "Suspend scheduler" )
            ( "resume",     po::value( &strResume ) ,       "Resume scheduler" )
            ( "stop",       po::value( &strStop ) ,         "Stop scheduler" )
            ( "start",      po::value( &strStart ) ,        "Start scheduler" )
            ;
        // clang-format on
    }

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).run(), vm );
    po::notify( vm );

    if ( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        if ( !strCreate.empty() )
        {
            mega::MP executorMP;
            {
                std::istringstream is( strCreate );
                is >> executorMP;
            }
            mega::service::Terminal terminal;
            const mega::MPO         simMPO = terminal.SimCreate( executorMP );
            std::cout << simMPO << std::endl;
        }
        else
        {
            std::cout << "Unrecognised project command" << std::endl;
        }
        // else if ( bList )
        // {
        //     mega::service::Terminal terminal;
        //     const auto              result = terminal.SimList();
        //     for ( auto simID : result )
        //     {
        //         std::cout << simID << std::endl;
        //     }
        // }
        // else if ( !strDestroy.empty() )
        // {
        //     mega::network::ConversationID simID;
        //     {
        //         std::istringstream is( strDestroy );
        //         is >> simID;
        //     }
        //     mega::service::Terminal terminal;
        //     terminal.SimDestroy( simID );
        // }
        // else if ( !strRead.empty() )
        // {
        //     mega::network::ConversationID owningID;
        //     {
        //         std::istringstream is( strID );
        //         is >> owningID;
        //     }
        //     mega::network::ConversationID simID;
        //     {
        //         std::istringstream is( strRead );
        //         is >> simID;
        //     }
        //     mega::service::Terminal terminal;
        //     const bool              bResult = terminal.SimRead( owningID, simID );
        //     std::cout << std::boolalpha << bResult << std::endl;
        // }
        // else if ( !strWrite.empty() )
        // {
        //     mega::network::ConversationID owningID;
        //     {
        //         std::istringstream is( strID );
        //         is >> owningID;
        //     }
        //     mega::network::ConversationID simID;
        //     {
        //         std::istringstream is( strWrite );
        //         is >> simID;
        //     }
        //     mega::service::Terminal terminal;
        //     const bool              bResult = terminal.SimWrite( owningID, simID );
        //     std::cout << std::boolalpha << bResult << std::endl;
        // }
        // else if ( !strRelease.empty() )
        // {
        //     mega::network::ConversationID owningID;
        //     {
        //         std::istringstream is( strID );
        //         is >> owningID;
        //     }
        //     mega::network::ConversationID simID;
        //     {
        //         std::istringstream is( strRelease );
        //         is >> simID;
        //     }
        //     mega::service::Terminal terminal;
        //     terminal.SimRelease( owningID, simID );
        // }
        // else if ( !strSuspend.empty() )
        // {
        // }
        // else if ( !strResume.empty() )
        // {
        // }
        // else if ( !strStop.empty() )
        // {
        // }
        // else if ( !strStart.empty() )
        // {
        // }
        // else
        // {
        //     std::cout << "Unrecognised project command" << std::endl;
        // }
    }
}
} // namespace sim
} // namespace driver
