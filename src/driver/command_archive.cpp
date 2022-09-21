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

#include "compiler/configuration.hpp"
#include "database/common/component_info.hpp"
#include "database/common/serialisation.hpp"
#include "database/common/environment_build.hpp"
#include "database/common/archive.hpp"

#include "common/scheduler.hpp"
#include "common/assert_verify.hpp"
#include "common/stash.hpp"

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace driver
{
    namespace archive
    {
        void command( bool bHelp, const std::vector< std::string >& args )
        {
            boost::filesystem::path srcDir, buildDir, outputFilePath;

            namespace po = boost::program_options;
            po::options_description commandOptions( " Generate retail archive" );
            {
                // clang-format off
            commandOptions.add_options()
                ( "src_dir",    po::value< boost::filesystem::path >( &srcDir ),            "Source directory" )
                ( "build_dir",  po::value< boost::filesystem::path >( &buildDir ),          "Build directory" )
                ( "output",     po::value< boost::filesystem::path >( &outputFilePath ),    "Archive file to generate" )
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
                mega::compiler::Directories directories{ srcDir, buildDir, "", "" };
                mega::io::BuildEnvironment environment( directories );
                const mega::io::Manifest manifest( environment, environment.project_manifest() );
                mega::io::ReadArchive::compile_archive( outputFilePath, manifest, srcDir, buildDir );
            }
        }
    } // namespace archive
} // namespace driver
