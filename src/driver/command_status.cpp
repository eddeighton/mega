

#include "service/terminal.hpp"

#include "service/network/log.hpp"
#include "service/protocol/common/status.hpp"

#include "mega/reference_io.hpp"

#include "boost/program_options.hpp"
#include "boost/filesystem/path.hpp"

#include <vector>
#include <string>
#include <iostream>

namespace driver
{
namespace status
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    bool        bList = false, bPing = false;
    std::string strMP, strMPO;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Project Commands" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "list",    po::bool_switch( &bList ),             "List all conversations" )
            ( "ping",    po::bool_switch( &bPing ),             "Ping an mp or mpo" )
            ( "mp",      po::value< std::string >( &strMP ),    "Machine Process ( 0 or 0.1 )" )
            ( "mpo",     po::value< std::string >( &strMPO ),   "Machine Process Owner ( 0.1.2 )" )
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

        if ( bPing )
        {
            if ( !strMP.empty() )
            {
                mega::MP mp;
                {
                    std::istringstream is( strMP );
                    is >> mp;
                }
                std::cout << terminal.PingMP( mp ) << std::endl;
            }
            else if ( !strMPO.empty() )
            {
                mega::MPO mpo;
                {
                    std::istringstream is( strMPO );
                    is >> mpo;
                }
                std::cout << terminal.PingMPO( mpo ) << std::endl;
            }
            else
            {
                THROW_RTE( "Missing mp or mpo" );
            }
        }
        else
        {
            std::cout << terminal.GetNetworkStatus() << std::endl;
        }
    }
}
} // namespace status
} // namespace driver
