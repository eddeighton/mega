

#include "service/terminal.hpp"

#include "service/network/log.hpp"
#include "service/protocol/common/status.hpp"

#include "boost/program_options.hpp"
#include "boost/filesystem/path.hpp"

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
