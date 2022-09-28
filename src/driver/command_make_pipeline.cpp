

#include "version/version.hpp"
#include "pipeline/configuration.hpp"

#include "service/network/log.hpp"

#include "pipeline/pipeline.hpp"

#include "compiler/configuration.hpp"

#include "utilities/cmake.hpp"
#include "utilities/tool_chain_hash.hpp"

#include "common/file.hpp"
#include "common/stash.hpp"

#include "spdlog/spdlog.h"

#include "boost/program_options.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/archive/xml_iarchive.hpp"

#include <iostream>

namespace driver
{
namespace make_pipeline
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    std::string             projectName, strComponentInfoPaths;
    boost::filesystem::path srcDir, buildDir, installDir, templatesDir, toolchainXML, pipelineXML;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Make a MegaStructure Pipeline Configuration XML File" );
    {
        // clang-format off
        commandOptions.add_options()
        ( "mega_project",   po::value< std::string >( &projectName ),                "Mega Project Name" )
        ( "components",     po::value< std::string >( &strComponentInfoPaths ),      "Component info files" )
        ( "src_dir",        po::value< boost::filesystem::path >( &srcDir ),         "Root source directory" )
        ( "build_dir",      po::value< boost::filesystem::path >( &buildDir ),       "Root build directory" )
        ( "install_dir",    po::value< boost::filesystem::path >( &installDir ),     "Installation directory" )
        ( "templates",      po::value< boost::filesystem::path >( &templatesDir ),   "Inja Templates directory" )
        ( "toolchain_xml",  po::value< boost::filesystem::path >( &toolchainXML ),   "Toolchain XML File" )
        ( "pipeline_xml",   po::value< boost::filesystem::path >( &pipelineXML ),    "Pipeline Configuration XML File to generate" )
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
        const std::vector< boost::filesystem::path > componentInfoPaths
            = mega::utilities::pathListToFiles( mega::utilities::parseCMakeStringList( strComponentInfoPaths, ";" ) );

        mega::utilities::ToolChain toolChain;
        {
            VERIFY_RTE_MSG( boost::filesystem::exists( toolchainXML ),
                            "Failed to locate toolchain file: " << toolchainXML.string() );
            auto                         pInStream = boost::filesystem::createBinaryInputFileStream( toolchainXML );
            boost::archive::xml_iarchive ia( *pInStream );
            ia&                          boost::serialization::make_nvp( "toolchain", toolChain );
        }

        const boost::filesystem::path    compilerPath = toolChain.megaCompilerPath;
        const mega::pipeline::PipelineID pipelineID   = compilerPath.native();
        const mega::Version              version      = mega::Version::getVersion();

        // clang-format off
        mega::compiler::Configuration config =
        {
            pipelineID,
            version,

            projectName,
            componentInfoPaths,

            mega::compiler::Directories
            {
                srcDir,
                buildDir,
                installDir,
                templatesDir
            }
        };
        // clang-format on

        const mega::pipeline::Configuration pipelineConfig = mega::compiler::makePipelineConfiguration( config );
        auto                                pOutFile       = boost::filesystem::createNewFileStream( pipelineXML );
        *pOutFile << pipelineConfig.get();
    }
}
} // namespace make_pipeline
} // namespace driver
