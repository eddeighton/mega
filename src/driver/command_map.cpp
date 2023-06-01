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

#include "database/common/environment_archive.hpp"
#include "database/model/FinalStage.hxx"

#include "mega/defaults.hpp"
#include "mega/type_id_io.hpp"
#include "mega/reference_io.hpp"

#include "service/network/log.hpp"

#include "utilities/project.hpp"

#ifndef _WIN32
#include "schematic/factory.hpp"
#include "schematic/schematic.hpp"
#include "schematic/compilation.hpp"
#include "schematic/visibility.hpp"
#endif

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/dll/import.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/serialization.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace driver::map
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path schematicFilePath, projectPath, outputFilePath;

    namespace po = boost::program_options;

#ifdef _WIN32
    THROW_RTE( "Map commands not supported on windows" );
#else

    po::options_description commandOptions( " Process schematic files" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "input",             po::value< boost::filesystem::path >( &schematicFilePath ),  "Schematic Input File" )
            ( "project_install",   po::value< boost::filesystem::path >( &projectPath ),        "Path to Megastructure Project" )
            ( "output",            po::value< boost::filesystem::path >( &outputFilePath ),     "Output svg file" )
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
        VERIFY_RTE_MSG( boost::filesystem::exists( schematicFilePath ),
                        "Failed to locate input schematic file: " << schematicFilePath.string() );

        schematic::File::Ptr pFile = schematic::load( schematicFilePath );

        if( schematic::Schematic::Ptr pSchematic = boost::dynamic_pointer_cast< schematic::Schematic >( pFile ) )
        {
            using namespace schematic;

            pSchematic->task_contours();
            pSchematic->task_extrusions();
            pSchematic->task_compilation();

            analysis::Compilation::Ptr pCompilation = pSchematic->getCompilation();
            VERIFY_RTE_MSG( pCompilation, "Failed to acquire schematic compilation" );

            analysis::FloorAnalysis floorAnalysis( *pCompilation, pSchematic );

            floorAnalysis.render( outputFilePath );

            // pCompilation->getFaces(int &floorFaces, int &fillerFaces)
        }

        // const mega::Project project( projectPath );

        // VERIFY_RTE_MSG( boost::filesystem::exists( project.getProjectDatabase() ),
        //                 "Failed to locate project database at: " << project.getProjectDatabase().string() );

        // mega::io::ArchiveEnvironment environment( project.getProjectDatabase() );

        // using namespace FinalStage;
        // Database database( environment, environment.project_manifest() );
    }
#endif
}

} // namespace driver::map
