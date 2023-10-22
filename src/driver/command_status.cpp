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

#include "service/network/log.hpp"
#include "mega/values/service/status.hpp"

#include "service/network/status_printer.hpp"

#include "mega/values/runtime/reference_io.hpp"

#include "service/network/log.hpp"
#include "spdlog/stopwatch.h"

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

#include <vector>
#include <string>
#include <iostream>

namespace driver::status
{

void command( mega::network::Log& log, bool bHelp, const std::vector< std::string >& args )
{
    bool                                   bTime = false;
    std::string                            strMPO, strMsg;
    int                                    msgSize = 0;
    mega::utilities::StatusPrinter::Config m_config;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Project Commands" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "ping",    po::value< std::string >( &strMPO ),   "Ping an mp or mpo" )
            ( "time",    po::bool_switch( &bTime ),             "Calculate performance timing" )
            ( "msg",     po::value< std::string >( &strMsg ),   "Message to send in ping" )
            ( "size",    po::value< int >( &msgSize ),          "Message size to generate ( instead of input msg )" )

            ( "conv",    po::bool_switch( &m_config.m_bLogicalThreads ), "Response logicalthreads status" )
            ( "mem",     po::bool_switch( &m_config.m_bMemory        ), "Response memory status" )
            ( "locks",   po::value< bool >( &m_config.m_bLocks )->default_value( true ), "Response locks status ( true by default )" )
            ( "logs",    po::bool_switch( &m_config.m_bLog           ), "Response log status" )
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
        mega::service::Terminal terminal( log );

        if( msgSize )
        {
            VERIFY_RTE_MSG( strMsg.empty(), "Cannot specify both size and msg" );
            strMsg.reserve( msgSize );
            for( int i = 0; i < msgSize; ++i )
            {
                strMsg.push_back( 'a' + ( i % 26 ) );
            }
        }

        // bTime
        std::optional< spdlog::stopwatch > sw;
        if( bTime )
        {
            sw = spdlog::stopwatch{};
            sw->reset();
        }

        if( !strMPO.empty() )
        {
            auto dotCount = std::count_if( strMPO.begin(), strMPO.end(), []( char c ) { return c == '.'; } );
            if( dotCount == 2 )
            {
                mega::MPO mpo;
                {
                    std::istringstream is( strMPO );
                    is >> mpo;
                }
                std::cout << terminal.PingMPO( mpo, strMsg ) << std::endl;
            }
            else if( dotCount < 2 )
            {
                mega::MP mp;
                {
                    std::istringstream is( strMPO );
                    is >> mp;
                }
                std::cout << terminal.PingMP( mp, strMsg ) << std::endl;
            }
            else
            {
                THROW_RTE( "Invalid MPO specified: " << strMPO );
            }
        }
        else
        {
            mega::utilities::StatusPrinter statusPrinter( m_config );
            mega::network::Status          status = terminal.GetNetworkStatus();
            status.sort();
            statusPrinter.print( status, std::cout );
        }

        if( bTime )
        {
            SPDLOG_INFO(
                "Total time: {}", std::chrono::duration_cast< mega::network::LogTime >( sw.value().elapsed() ) );
        }
    }
}
} // namespace driver::status
