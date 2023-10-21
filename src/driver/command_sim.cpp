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

#include "mega/values/runtime/reference_io.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

#include <vector>
#include <string>
#include <iostream>

namespace driver
{
namespace sim
{

mega::MachineID toMachineID( const std::string& strMachineID )
{
    mega::MachineID    machineID;
    std::istringstream is( strMachineID );
    is >> machineID;
    return machineID;
}

mega::MP toMP( const std::string& strMP )
{
    mega::MP           mp;
    std::istringstream is( strMP );
    is >> mp;
    return mp;
}

mega::MPO toMPO( const std::string& strMPO )
{
    mega::MPO          mpo;
    std::istringstream is( strMPO );
    is >> mpo;
    return mpo;
}

void command( bool bHelp, const std::vector< std::string >& args )
{
    bool        bList = false;
    std::string strCreate, strList, strDestroy, strID, strRead, strWrite, strRelease, strSuspend, strResume, strStop,
        strStart, strErrorCheck;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Simulation Commands" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "create",     po::value( &strCreate ),        "Create executor MP or simulation MPO" )
            ( "destroy",    po::value( &strDestroy ) ,      "Destroy executor or simulation MPO" )

            ( "id",         po::value( &strID ) ,           "Source MPO for lock Commands" )
            ( "read",       po::value( &strRead ) ,         "Request Read Lock on specified MPO ( use id to specify who from )" )
            ( "write",      po::value( &strWrite ) ,        "Request Write Lock on specified MPO ( use id to specify who from )" )
            ( "release",    po::value( &strRelease ) ,      "Request Release Lock on specified MPO ( use id to specify who from )" )

            ( "error",      po::value( &strErrorCheck ),    "Send test request to MPO that will generate exception error in simulation" )

            // ( "suspend",    po::value( &strSuspend ) ,      "Suspend scheduler" )
            // ( "resume",     po::value( &strResume ) ,       "Resume scheduler" )
            // ( "stop",       po::value( &strStop ) ,         "Stop scheduler" )
            // ( "start",      po::value( &strStart ) ,        "Start scheduler" )
            ;
        // clang-format on
    }

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).run(), vm );
    po::notify( vm );

    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        if( !strCreate.empty() )
        {
            if( std::count( strCreate.begin(), strCreate.end(), '.' ) == 0 )
            {
                const mega::MachineID   daemonMachineID = toMachineID( strCreate );
                mega::service::Terminal terminal;
                const mega::MP          executorMP = terminal.ExecutorCreate( daemonMachineID );
                std::cout << executorMP << std::endl;
            }
            else if( std::count( strCreate.begin(), strCreate.end(), '.' ) == 1 )
            {
                const mega::MP          executorMP = toMP( strCreate );
                mega::service::Terminal terminal;
                const mega::MPO         simMPO = terminal.SimCreate( executorMP );
                std::cout << simMPO << std::endl;
            }
            else
            {
                THROW_RTE( "Invalid machineID or MP specified to create: " << strCreate );
            }
        }
        else if( !strDestroy.empty() )
        {
            if( std::count( strDestroy.begin(), strDestroy.end(), '.' ) == 1 )
            {
                const mega::MP          executorMP = toMP( strDestroy );
                mega::service::Terminal terminal;
                terminal.ExecutorDestroy( executorMP );
            }
            else if( std::count( strDestroy.begin(), strDestroy.end(), '.' ) == 2 )
            {
                const mega::MPO         simMPO = toMPO( strDestroy );
                mega::service::Terminal terminal;
                terminal.SimDestroy( simMPO );
            }
            else
            {
                THROW_RTE( "Invalid MP or MPO specified to destroy: " << strDestroy );
            }
        }
        else if( !strRead.empty() )
        {
            VERIFY_RTE_MSG( !strID.empty(), "Missing source mpo" );
            const mega::MPO         sourceMPO = toMPO( strID );
            const mega::MPO         targetMPO = toMPO( strRead );
            mega::service::Terminal terminal;
            const mega::TimeStamp   lockCycle = terminal.SimRead( sourceMPO, targetMPO );
            std::cout << lockCycle << std::endl;
        }
        else if( !strWrite.empty() )
        {
            VERIFY_RTE_MSG( !strID.empty(), "Missing source mpo" );
            const mega::MPO         sourceMPO = toMPO( strID );
            const mega::MPO         targetMPO = toMPO( strWrite );
            mega::service::Terminal terminal;
            const mega::TimeStamp   lockCycle = terminal.SimWrite( sourceMPO, targetMPO );
            std::cout << lockCycle << std::endl;
        }
        else if( !strRelease.empty() )
        {
            VERIFY_RTE_MSG( !strID.empty(), "Missing source mpo" );
            const mega::MPO         sourceMPO = toMPO( strID );
            const mega::MPO         targetMPO = toMPO( strRelease );
            mega::service::Terminal terminal;
            terminal.SimRelease( sourceMPO, targetMPO );
        }
        else if( !strErrorCheck.empty() )
        {
            const mega::MPO         targetMPO = toMPO( strErrorCheck );
            mega::service::Terminal terminal;
            try
            {
                terminal.SimErrorCheck( targetMPO );
            }
            catch( std::runtime_error& ex )
            {
                std::cout << "Got returned error: " << ex.what() << std::endl;
            }
        }
        else
        {
            std::cout << "Unrecognised project command" << std::endl;
        }
    }
}
} // namespace sim
} // namespace driver
