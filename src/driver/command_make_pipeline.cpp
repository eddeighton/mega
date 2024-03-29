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

#include "pipeline/version.hpp"
#include "pipeline/configuration.hpp"
#include "pipeline/pipeline.hpp"

#include "log/log.hpp"

#include "environment/environment_build.hpp"

#include "database/manifest.hxx"
#include "database/component_info.hpp"

#include "compiler/configuration.hpp"
#include "compiler/cmake.hpp"

#include "mega/values/compilation/tool_chain_hash.hpp"

#include "log/log.hpp"
#include "common/file.hpp"
#include "common/stash.hpp"

#include <spdlog/spdlog.h>

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include <iostream>

namespace driver::make_pipeline
{

void command( mega::network::Log& , bool bHelp, const std::vector< std::string >& args )
{
    std::string             projectName, strComponentInfoPaths;
    boost::filesystem::path srcDir, buildDir, installDir, templatesDir, toolchainXML, unityProjectDir, unityEditor,
        pipelineXML;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Make a MegaStructure Pipeline Configuration XML File" );
    {
        // clang-format off
        commandOptions.add_options()
        ( "mega_project",      po::value< std::string >( &projectName ),                    "Mega Project Name" )
        ( "components",        po::value< std::string >( &strComponentInfoPaths ),          "Component info files" )
        ( "src_dir",           po::value< boost::filesystem::path >( &srcDir ),             "Root source directory" )
        ( "build_dir",         po::value< boost::filesystem::path >( &buildDir ),           "Root build directory" )
        ( "install_dir",       po::value< boost::filesystem::path >( &installDir ),         "Installation directory" )
        ( "templates",         po::value< boost::filesystem::path >( &templatesDir ),       "Inja Templates directory" )
        ( "toolchain_xml",     po::value< boost::filesystem::path >( &toolchainXML ),       "Toolchain XML File" )
        ( "unity_project_dir", po::value< boost::filesystem::path >( &unityProjectDir ),    "Unity Project Directory" )
        ( "unity_editor",      po::value< boost::filesystem::path >( &unityEditor ),        "Unity Editor Program" )
        ( "pipeline_xml",      po::value< boost::filesystem::path >( &pipelineXML ),        "Pipeline Configuration XML File to generate" )
        ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "dir", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    if( bHelp )
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
        const mega::pipeline::PipelineID pipelineID   = compilerPath.string();
        const mega::Version              version; //      = mega::Version::getVersion();

        const mega::io::Directories directories{ srcDir, buildDir, installDir, templatesDir };

        std::vector< mega::io::ComponentInfo > componentInfos;
        {
            for( const boost::filesystem::path& componentInfoPath : componentInfoPaths )
            {
                componentInfos.emplace_back( mega::io::ComponentInfo::load( componentInfoPath ) );
            }
        }

        // create the manifest
        const mega::io::BuildEnvironment environment( directories );
        const mega::io::Manifest         manifest( environment, srcDir, componentInfos );

        {
            // save the manifest
            const mega::io::manifestFilePath projectManifestPath = environment.project_manifest();
            manifest.save_temp( environment, projectManifestPath );
            environment.temp_to_real( projectManifestPath );
        }

        // clang-format off
        const mega::compiler::Configuration config =
        {
            pipelineID,
            version,

            projectName,
            componentInfos,

            directories,
            manifest,
            
            unityProjectDir,
            unityEditor
        };
        // clang-format on

        const mega::pipeline::Configuration pipelineConfig = mega::compiler::makePipelineConfiguration( config );
        auto                                pOutFile       = boost::filesystem::createNewFileStream( pipelineXML );
        *pOutFile << pipelineConfig.get();
    }
}

} // namespace driver::make_pipeline
