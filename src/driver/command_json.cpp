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


#include "database/model/ComponentListing.hxx"
#include "database/model/ParserStage.hxx"
#include "database/model/InterfaceStage.hxx"
#include "database/model/DependencyAnalysis.hxx"
#include "database/model/DependencyAnalysisView.hxx"
#include "database/model/SymbolAnalysis.hxx"
#include "database/model/SymbolAnalysisView.hxx"
#include "database/model/SymbolRollout.hxx"
#include "database/model/InterfaceAnalysisStage.hxx"
#include "database/model/FinalStage.hxx"

#include "database/common/component_info.hpp"
#include "database/common/serialisation.hpp"
#include "database/common/environments.hpp"

#include "utilities/cmake.hpp"

#include "common/scheduler.hpp"
#include "common/assert_verify.hpp"
#include "common/stash.hpp"

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <string>
#include <vector>
#include <iostream>

#define STAGE_DUMP( stageName )\
if( strStage == #stageName )\
{\
    spdlog::info( "Dumping stage: {} to file: {}", #stageName, outputFilePath.string() );\
    using namespace stageName;\
    Database database( environment, environment.project_manifest() );\
    database.load();\
    {\
        nlohmann::json data;\
        database.to_json( data );\
        std::ofstream os( outputFilePath.native(), std::ios_base::trunc | std::ios_base::out );\
        os << data;\
    }\
}

namespace driver
{
    namespace json
    {
        void command( bool bHelp, const std::vector< std::string >& args )
        {
            boost::filesystem::path rootSourceDir, rootBuildDir, outputFilePath;
            std::string strStage = "FinalStage";

            namespace po = boost::program_options;
            po::options_description commandOptions( " Generate database json file" );
            {
                // clang-format off
            commandOptions.add_options()
                ( "src_dir",    po::value< boost::filesystem::path >( &rootSourceDir ),     "Source directory" )
                ( "build_dir",  po::value< boost::filesystem::path >( &rootBuildDir ),      "Build directory" )
                ( "output",     po::value< boost::filesystem::path >( &outputFilePath ),    "JSON file to generate" )
                ( "stage",      po::value< std::string >( &strStage ),                      "Stage to dump" )
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
                mega::io::BuildEnvironment environment( rootSourceDir, rootBuildDir );

                STAGE_DUMP( ComponentListing )
                STAGE_DUMP( ParserStage )
                STAGE_DUMP( InterfaceStage )
                STAGE_DUMP( DependencyAnalysis )
                STAGE_DUMP( DependencyAnalysisView )
                STAGE_DUMP( SymbolAnalysis )
                STAGE_DUMP( SymbolAnalysisView )
                STAGE_DUMP( SymbolRollout )
                STAGE_DUMP( InterfaceAnalysisStage )
                STAGE_DUMP( FinalStage )
            }
        }
    } // namespace json
} // namespace driver
