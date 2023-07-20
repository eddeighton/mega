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

#include "service/executor/clock_standalone.hpp"
#include "service/executor/executor.hpp"

#include "service/network/network.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/node.hpp"

#include "pipeline/pipeline.hpp"

#include "common/assert_verify.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/config.hpp>

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

int main( int argc, const char* argv[] )
{
    using namespace std::chrono_literals;
    mega::service::ProcessClockStandalone::FloatTickDuration tickRate = 15ms;
    using NumThreadsType                                              = decltype( std::thread::hardware_concurrency() );
    NumThreadsType          uiNumThreads                              = std::thread::hardware_concurrency();
    std::string             strIP                                     = "localhost";
    short                   daemonPortNumber                          = mega::network::MegaDaemonPort();
    boost::filesystem::path logFolder                                 = boost::filesystem::current_path() / "log";
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

    std::cout << "Connecting to: " << strIP << std::endl;

    try
    {
        mega::network::configureLog( logFolder, "executor", mega::network::fromStr( strConsoleLogLevel ),
                                     mega::network::fromStr( strLogFileLevel ) );

        boost::asio::io_context ioContext;

        mega::service::ProcessClockStandalone clock( ioContext, tickRate );
        mega::service::Executor               executor(
            ioContext, uiNumThreads, daemonPortNumber, clock, mega::network::Node::Executor );

        /*auto signalHandler = [ &executor ]( const boost::system::error_code& error, int signalNumber )
            {
                if( !error )
                {
                    std::cout << "\nSignal handler shutting down executor" << std::endl;
                    executor.shutdown();
                }
            };*/

        // Construct a signal set registered for process termination.
        // boost::asio::signal_set signals( ioContext, SIGINT, SIGTERM);

        // Start an asynchronous wait for one of the signals to occur.
        // signals.async_wait( signalHandler );

        std::vector< std::thread > threads;
        for( int i = 0; i < uiNumThreads; ++i )
        {
            threads.emplace_back( std::move( std::thread( [ &ioContext ]() { ioContext.run(); } ) ) );
        }
        for( std::thread& thread : threads )
        {
            thread.join();
        }
    }
    catch( std::exception& ex )
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
