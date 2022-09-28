

// copy from mega/src/cmake/database/database/CMakeLists.txt
#include "database/model/ComponentListing.hxx"
#include "database/model/ComponentListingView.hxx"
#include "database/model/ConcreteTypeAnalysis.hxx"
#include "database/model/ConcreteTypeAnalysisView.hxx"
#include "database/model/ConcreteTypeRollout.hxx"
#include "database/model/ConcreteStage.hxx"
#include "database/model/DependencyAnalysis.hxx"
#include "database/model/DependencyAnalysisView.hxx"
#include "database/model/DerivationAnalysis.hxx"
#include "database/model/DerivationAnalysisView.hxx"
#include "database/model/FinalStage.hxx"    
#include "database/model/HyperGraphAnalysis.hxx"
#include "database/model/HyperGraphAnalysisView.hxx"
#include "database/model/InterfaceAnalysisStage.hxx"
#include "database/model/InterfaceStage.hxx"
#include "database/model/MemoryStage.hxx"
#include "database/model/OperationsStage.hxx"
#include "database/model/ParserStage.hxx"
#include "database/model/SymbolAnalysis.hxx"
#include "database/model/SymbolAnalysisView.hxx"
#include "database/model/SymbolRollout.hxx"
    
#include "database/common/component_info.hpp"
#include "database/common/serialisation.hpp"
#include "database/common/environment_build.hpp"

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

#define STAGE_DUMP( stageName )                                                                     \
    if ( strStage == #stageName )                                                                   \
    {                                                                                               \
        spdlog::info( "Dumping stage: {} to file: {}", #stageName, outputFilePath.string() );       \
        bMatched = true;                                                                            \
        using namespace stageName;                                                                  \
        Database database( environment );                                                           \
        {                                                                                           \
            nlohmann::json data;                                                                    \
            database.to_json( data );                                                               \
            std::ofstream os( outputFilePath.native(), std::ios_base::trunc | std::ios_base::out ); \
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
        "DerivationAnalysis\n" << 
        "DerivationAnalysisView\n" << 
        "ConcreteTypeAnalysis\n" << 
        "ConcreteTypeAnalysisView\n" << 
        "ConcreteTypeRollout\n" << 
        "MemoryStage\n" << 
        "OperationsStage\n" << 
        "FinalStage\n";
    }
    else
    {
        mega::compiler::Directories directories{ srcDir, buildDir, "", "" };
        mega::io::BuildEnvironment  environment( directories );

        bool bMatched = false;

        // copy from /workspace/root/src/mega/src/api/database/model/file_info.hxx
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
        STAGE_DUMP( DerivationAnalysis )
        STAGE_DUMP( DerivationAnalysisView )
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
