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



// copy from mega/src/cmake/database/database/CMakeLists.txt
#include "database/ComponentListing.hxx"
#include "database/ComponentListingView.hxx"
#include "database/ConcreteTypeAnalysis.hxx"
#include "database/ConcreteTypeAnalysisView.hxx"
#include "database/ConcreteTypeRollout.hxx"
#include "database/ConcreteStage.hxx"
#include "database/DependencyAnalysis.hxx"
#include "database/DependencyAnalysisView.hxx"
#include "database/InheritanceAnalysis.hxx"
#include "database/InheritanceAnalysisView.hxx"
#include "database/InheritanceAnalysisRollout.hxx"
#include "database/FinalStage.hxx"    
#include "database/HyperGraphAnalysis.hxx"
#include "database/HyperGraphAnalysisView.hxx"
#include "database/InterfaceAnalysisStage.hxx"
#include "database/InterfaceStage.hxx"
#include "database/MemoryStage.hxx"
#include "database/OperationsStage.hxx"
#include "database/ParserStage.hxx"
#include "database/SymbolAnalysis.hxx"
#include "database/SymbolAnalysisView.hxx"
#include "database/SymbolRollout.hxx"
    
#include "database/component_info.hpp"

#include "database/serialisation.hpp"
#include "environment/environment_build.hpp"

#include "common/assert_verify.hpp"
#include "common/stash.hpp"

#include "nlohmann/json.hpp"
#include <spdlog/spdlog.h>

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <string>
#include <vector>
#include <iostream>

#define STAGE_DUMP( stageName )                                                                     \
    if ( strStage == #stageName )                                                                   \
    {                                                                                               \
        spdlog::info( "Dumping stage: {} to file: {}", #stageName, outputFilePath.string() );       \
        bMatched = true;                                                                            \
        using namespace stageName;                                                                  \
        Database database( environment );                                                           \
        {                                                                                           \
            nlohmann::json data;                                                                    \
            std::ofstream os( outputFilePath.string(), std::ios_base::trunc | std::ios_base::out ); \
            os << data;                                                                             \
        }                                                                                           \
    }

namespace driver
{
namespace json
{
void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path srcDir, buildDir, outputFilePath, sourceFilePath;
    std::string             strStage = "FinalStage";

    namespace po = boost::program_options;
    po::options_description commandOptions( " Generate database json file" );
    {
        // clang-format off
            commandOptions.add_options()
                ( "src_dir",    po::value< boost::filesystem::path >( &srcDir ),            "Source directory" )
                ( "build_dir",  po::value< boost::filesystem::path >( &buildDir ),          "Build directory" )
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
        std::cout << "Valid stages:\n" <<
        "ComponentListing\n" << 
        "ComponentListingView\n" << 
        "ParserStage\n" << 
        "InterfaceStage\n" << 
        "DependencyAnalysis\n" << 
        "DependencyAnalysisView\n" << 
        "SymbolAnalysis\n" << 
        "SymbolAnalysisView\n" << 
        "SymbolRollout\n" << 
        "InterfaceAnalysisStage\n" << 
        "ConcreteStage\n" << 
        "HyperGraphAnalysis\n" << 
        "HyperGraphAnalysisView\n" << 
        "InheritanceAnalysis\n" << 
        "InheritanceAnalysisView\n" << 
        "InheritanceAnalysisRollout\n" << 
        "ConcreteTypeAnalysis\n" << 
        "ConcreteTypeAnalysisView\n" << 
        "ConcreteTypeRollout\n" << 
        "MemoryStage\n" << 
        "OperationsStage\n" << 
        "FinalStage\n";
    }
    else
    {
        mega::io::Directories directories{ srcDir, buildDir, "", "" };
        mega::io::BuildEnvironment  environment( directories );

        bool bMatched = false;

        // copy from /workspace/root/src/mega/src/api/database/file_info.hxx
        STAGE_DUMP( ComponentListing )
        STAGE_DUMP( ComponentListingView )
        STAGE_DUMP( ParserStage )
        STAGE_DUMP( InterfaceStage )
        STAGE_DUMP( DependencyAnalysis )
        STAGE_DUMP( DependencyAnalysisView )
        STAGE_DUMP( SymbolAnalysis )
        STAGE_DUMP( SymbolAnalysisView )
        STAGE_DUMP( SymbolRollout )
        STAGE_DUMP( InterfaceAnalysisStage )
        STAGE_DUMP( ConcreteStage )
        STAGE_DUMP( HyperGraphAnalysis )
        STAGE_DUMP( HyperGraphAnalysisView )
        STAGE_DUMP( InheritanceAnalysis )
        STAGE_DUMP( InheritanceAnalysisView )
        STAGE_DUMP( InheritanceAnalysisRollout )
        STAGE_DUMP( ConcreteTypeAnalysis )
        STAGE_DUMP( ConcreteTypeAnalysisView )
        STAGE_DUMP( ConcreteTypeRollout )
        STAGE_DUMP( MemoryStage )
        STAGE_DUMP( OperationsStage )
        STAGE_DUMP( FinalStage )

        VERIFY_RTE_MSG( bMatched, "Failed to match stage: " << strStage );
    }
}
} // namespace json
} // namespace driver
