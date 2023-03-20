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

#include "service/plugin/plugin.hpp"

#include "service/network/network.hpp"

#include "common/assert_verify.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/config.hpp>

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

auto elapsed( std::chrono::steady_clock::time_point& last )
{
    const auto timeNow = std::chrono::steady_clock::now();
    const auto delta = std::chrono::duration_cast< std::chrono::steady_clock::duration >( timeNow - last );
    last = timeNow;
    return delta;
}

void printCurrentNetworkConnection()
{
    auto netID = mp_network_current();
    if( netID != -1 )
    {
        const char* pszNet = mp_network_name( netID );
        if( pszNet )
        {
            std::cout << "PLUGIN_TEST: Current network: '" << pszNet << "'" << std::endl;
        }
        else
        {
            std::cout << "PLUGIN_TEST: Current network: 'unknown'" << std::endl;
        }
    }
    else
    {
        std::cout << "PLUGIN_TEST: Current network: 'disconnected'" << std::endl;
    }
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
    //auto  start       = std::chrono::steady_clock::now();
    float fUpdateRate = 0.1f;

    try
    {
        std::cout << "PLUGIN_TEST: Initialising plugin" << std::endl;
        mp_initialise( strConsoleLogLevel.c_str(), strLogFileLevel.c_str() );

        {
            while( mp_network_count() == 0 )
            {
                mp_update( fUpdateRate );
            }
        }

        printCurrentNetworkConnection();
        std::cout << "PLUGIN_TEST: Total networks: " << mp_network_count() << std::endl;

        MEGA_64 networkID = 0;
        std::cout << "PLUGIN_TEST: Connecting to: '" << mp_network_name( networkID ) << "' network" << std::endl;

        mp_network_connect( networkID );
        {
            while( -1 == mp_network_current() )
            {
                mp_update( fUpdateRate );
            }
        }

        printCurrentNetworkConnection();
        std::cout << "PLUGIN_TEST: Current planet: " << std::boolalpha << mp_planet_current() << std::endl;

        mp_planet_create();
        {
            while( mp_planet_current() == false )
            {
                mp_update( fUpdateRate );
            }
        }
        std::cout << "PLUGIN_TEST: Current planet: " << std::boolalpha << mp_planet_current() << std::endl;

        std::cout << "PLUGIN_TEST: waiting for input..." << std::endl;
        char c;
        std::cin >> c;

        mp_planet_destroy();
        {
            while( mp_planet_current() == true )
            {
                mp_update( fUpdateRate );
            }
        }
        std::cout << "PLUGIN_TEST: Current planet: " << std::boolalpha << mp_planet_current() << std::endl;
        printCurrentNetworkConnection();

        std::cout << "PLUGIN_TEST: waiting for input..." << std::endl;
        std::cin >> c;

        mp_network_disconnect();
        {
            while( -1 != mp_network_current() )
            {
                mp_update( fUpdateRate );
            }
        }
        printCurrentNetworkConnection();
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
