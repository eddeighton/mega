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
#include "service/protocol/common/status.hpp"

#include "mega/reference_io.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

#include <vector>
#include <string>
#include <iostream>

namespace driver::status
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    bool        bList = false;
    std::string strMPO;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Project Commands" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "list",    po::bool_switch( &bList ),             "List all conversations" )
            ( "ping",    po::value< std::string >( &strMPO ),   "Ping an mp or mpo" )
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
        mega::service::Terminal terminal;

        if ( !strMPO.empty() )
        {
            auto dotCount = std::count_if( strMPO.begin(), strMPO.end(), []( char c ) { return c == '.'; } );
            if ( dotCount == 2 )
            {
                mega::MPO mpo;
                {
                    std::istringstream is( strMPO );
                    is >> mpo;
                }
                std::cout << terminal.PingMPO( mpo ) << std::endl;
            }
            else if ( dotCount < 2 )
            {
                mega::MP mp;
                {
                    std::istringstream is( strMPO );
                    is >> mp;
                }
                std::cout << terminal.PingMP( mp ) << std::endl;
            }
            else
            {
                THROW_RTE( "Invalid MPO specified: " << strMPO );
            }
        }
        else
        {
            std::cout << terminal.GetNetworkStatus() << std::endl;
        }
    }
}
} // namespace driver::status
