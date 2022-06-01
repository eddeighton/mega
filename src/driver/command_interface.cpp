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

//#include "database/model/interface.hpp"
/*
#include "command_interface/base_task.hpp"
#include "command_interface/task_parse_ast.hpp"
#include "command_interface/task_includes.hpp"
#include "command_interface/task_interface.hpp"
#include "command_interface/task_dependencies.hpp"
#include "command_interface/task_symbols.hpp"
#include "command_interface/task_interface_analysis.hpp"
#include "command_interface/task_concrete.hpp"

#include "database/model/manifest.hxx"
*/

#include "pipeline/configuration.hpp"

#include "service/host.hpp"

#include "service/network/log.hpp"

#include "pipeline/pipeline.hpp"

#include "compiler/compiler.hpp"

#include "utilities/cmake.hpp"

#include "boost/program_options.hpp"
#include "boost/filesystem/path.hpp"

#include <common/file.hpp>
#include <common/stash.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

namespace driver
{
namespace interface
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    std::string             projectName, strComponentInfoPaths;
    boost::filesystem::path rootSourceDir, rootBuildDir, parserDll, megaCompiler, clangCompiler, clangPlugin,
        databaseDll, templatesDir, pipelineXML;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Compile Mega Project Interface" );
    {
        // clang-format off
        commandOptions.add_options()
        ( "project",        po::value< std::string >( &projectName ),                "Mega Project Name" )
        ( "components",     po::value< std::string >( &strComponentInfoPaths ),      "Component info files" )
        ( "root_src_dir",   po::value< boost::filesystem::path >( &rootSourceDir ),  "Root source directory" )
        ( "root_build_dir", po::value< boost::filesystem::path >( &rootBuildDir ),   "Root build directory" )
        ( "mega_compiler",  po::value< boost::filesystem::path >( &megaCompiler ),   "Megastructure compiler pipeline path" )
        ( "clang_compiler", po::value< boost::filesystem::path >( &clangCompiler ),  "Clang Compiler path" )
        ( "parser_dll",     po::value< boost::filesystem::path >( &parserDll ),      "Parser DLL Path" )
        ( "clang_plugin",   po::value< boost::filesystem::path >( &clangPlugin ),    "Clang Plugin path" )
        ( "database_dll",   po::value< boost::filesystem::path >( &databaseDll ),    "Database DLL Path" )
        ( "templates",      po::value< boost::filesystem::path >( &templatesDir ),   "Inja Templates directory" )
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

        const boost::filesystem::path    compilerPath = megaCompiler.parent_path() / "compiler";
        const mega::pipeline::PipelineID pipelineID   = compilerPath.native();

        // calculate toolchain hash codes
        // clang-format off

        // TODO - calculate these hashcodes as part of the mega build
        const task::FileHash fileHashparserDll     ( parserDll     );
        const task::FileHash fileHashmegaCompiler  ( megaCompiler  );
        const task::FileHash fileHashclangCompiler ( clangCompiler );
        const task::FileHash fileHashclangPlugin   ( clangPlugin   );
        const task::FileHash fileHashdatabaseDll   ( databaseDll   );


        const task::DeterminantHash toolChainHash( 
            { 
                fileHashparserDll    ,
                fileHashmegaCompiler ,
                fileHashclangCompiler,
                fileHashclangPlugin  ,
                fileHashdatabaseDll  
            } );

        mega::compiler::Configuration config =
        {
            pipelineID,
            projectName,
            componentInfoPaths,

            mega::compiler::Directories
            {
                rootSourceDir,
                rootBuildDir,
                templatesDir
            },

            mega::compiler::ToolChain
            {
                parserDll       ,
                megaCompiler    ,
                clangCompiler   ,
                clangPlugin     ,
                databaseDll     ,

                fileHashparserDll    ,
                fileHashmegaCompiler ,
                fileHashclangCompiler,
                fileHashclangPlugin  ,
                fileHashdatabaseDll  ,

                toolChainHash
            }
        };
        // clang-format on

        const mega::pipeline::Configuration pipelineConfig = mega::compiler::makePipelineConfiguration( config );
        auto                                pOutFile       = boost::filesystem::createNewFileStream( pipelineXML );
        *pOutFile << pipelineConfig.get();
    }
}
} // namespace interface
} // namespace driver
