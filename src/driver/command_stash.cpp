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

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

#include <vector>
#include <string>
#include <iostream>

namespace driver
{
namespace stash
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    bool bClear = false;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Stash Commands" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "clear",    po::bool_switch( &bClear ),    "Clear the stash" )
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
        if ( bClear )
        {
            mega::service::Terminal terminal;
            terminal.ClearStash();
            std::cout << "Stash cleared" << std::endl;
        }
        else
        {
            THROW_RTE( "No command" );
        }
    }
}
} // namespace stash
} // namespace driver
