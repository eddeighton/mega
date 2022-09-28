

#include "pipeline/configuration.hpp"

#include "pipeline/stash.hpp"
#include "service/terminal.hpp"

#include "pipeline/pipeline.hpp"

#include "utilities/cmake.hpp"

#include "parser/parser.hpp"

#include "boost/program_options.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/program_options/value_semantic.hpp"
#include "boost/dll.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"
#include "common/stash.hpp"

#include <iostream>
#include <spdlog/spdlog.h>

namespace driver
{
namespace new_install
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path projectInstallDir;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Notify Megastructure of a new project installation" );
    {
        // clang-format off
        commandOptions.add_options()
        ( "mega_project",      po::value< boost::filesystem::path >( &projectInstallDir ),   "Project Installation Directory" )
        ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "dir", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    if ( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        try
        {
            const mega::network::Project project( projectInstallDir );
            THROW_RTE( "TODO" );
            // mega::service::Terminal      terminal;
            // if ( !terminal.NewInstallation( project ) )
            // {
            //     THROW_RTE( "Installation failed" );
            // }
        }
        catch ( std::exception& ex )
        {
            THROW_RTE( "Exception executing pipeline: " << ex.what() );
        }
    }
}
} // namespace new_install
} // namespace driver
