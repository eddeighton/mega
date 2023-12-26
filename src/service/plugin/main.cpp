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

#include "service/plugin/api.hpp"

#include "service/network/network.hpp"

#include "mega/values/native_types.hpp"
#include "mega/values/runtime/pointer.hpp"

#include "event/range.hpp"

#include "common/assert_verify.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/config.hpp>

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <iomanip>

#pragma pack(1)
struct MemoryRecordHeader
{
    mega::U16       size;
    mega::Pointer ref;
    mega::U16       dataSize;
};
#pragma pack()

void update()
{
    while( true )
    {
        const mega::event::Range* pRange = reinterpret_cast< const mega::event::Range* >( mp_downstream() );
        if( !pRange )
        {
            break;
        }
        for( mega::U64 iter = pRange->m_memory.m_begin; iter != pRange->m_memory.m_end; )
        {
            const MemoryRecordHeader* pRecord = reinterpret_cast< const MemoryRecordHeader* >( iter );
            std::cout << "Found memory record: " << pRecord->ref << std::endl;
            iter += pRecord->size;
        }
    }
    mp_upstream( 0.1f, nullptr );
}

int main( int argc, const char* argv[] )
{
    using NumThreadsType                       = decltype( std::thread::hardware_concurrency() );
    NumThreadsType          uiNumThreads       = std::thread::hardware_concurrency();
    std::string             strIP              = "localhost";
    short                   daemonPortNumber   = mega::network::MegaDaemonPort();
    boost::filesystem::path logFolder          = boost::filesystem::current_path() / "log";
    std::string             strConsoleLogLevel = "warn", strLogFileLevel = "warn";
    {
        bool bShowHelp = false;

        namespace po = boost::program_options;
        po::options_description options;

        // clang-format off
        options.add_options()
        ( "help",    po::bool_switch( &bShowHelp ),                                                 "Show Command Line Help" )
        ( "ip",      po::value< std::string >( &strIP ),                                            "Root IP Address" )
        ( "threads", po::value< NumThreadsType >( &uiNumThreads ),                                  "Max number of threads" )
        ( "log",     po::value< boost::filesystem::path >( &logFolder ),                            "Logging folder" )
        ( "console", po::value< std::string >( &strConsoleLogLevel ),                               "Console logging level" )
        ( "level",   po::value< std::string >( &strLogFileLevel ),                                  "Log file logging level" )
        ( "port",    po::value< short >( &daemonPortNumber )->default_value( daemonPortNumber ),    "Daemon port number" )
        ;
        // clang-format on

        po::parsed_options parsedOptions = po::command_line_parser( argc, argv ).options( options ).run();

        po::variables_map vm;
        po::store( parsedOptions, vm );
        po::notify( vm );

        if( bShowHelp )
        {
            std::cout << options << "\n";
            return 0;
        }

        if( strIP.empty() )
        {
            std::cerr << "Missing IP Address" << std::endl;
            return -1;
        }
        uiNumThreads = std::min( std::max( 1U, uiNumThreads ), std::thread::hardware_concurrency() );
    }

    // std::cout << "Connecting to: " << strIP << std::endl;
    // auto  start       = std::chrono::steady_clock::now();

    try
    {
        std::cout << "PLUGIN_TEST: Initialising plugin" << std::endl;
        mp_initialise( strConsoleLogLevel.c_str(), strLogFileLevel.c_str() );

        for( int i = 0; i < 100; ++i )
        {
            mp_upstream( 0.1f, nullptr );
        }
        
        std::cout << "PLUGIN_TEST: Shutting down" << std::endl;
        mp_shutdown();

        std::cout << "PLUGIN_TEST: Shutdown complete" << std::endl;
    }
    catch( std::exception& ex )
    {
        std::cerr << "PLUGIN_TEST: Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
