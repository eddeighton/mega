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
#include "command_interface/base_task.hpp"
#include "command_interface/task_parse_ast.hpp"
#include "command_interface/task_includes.hpp"
#include "command_interface/task_interface.hpp"
#include "command_interface/task_dependencies.hpp"
#include "command_interface/task_symbols.hpp"
#include "command_interface/task_interface_analysis.hpp"
#include "command_interface/task_concrete.hpp"

#include "database/model/manifest.hxx"

namespace driver
{
namespace interface
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path rootSourceDir, rootBuildDir, parserDll, megaCompiler, clangCompiler, clangPlugin,
        databaseDll, templatesDir;
    std::string projectName;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Compile Mega Project Interface" );
    {
        // clang-format off
        commandOptions.add_options()
        ( "root_src_dir",   po::value< boost::filesystem::path >( &rootSourceDir ),  "Root source directory" )
        ( "root_build_dir", po::value< boost::filesystem::path >( &rootBuildDir ),   "Root build directory" )
        ( "project",        po::value< std::string >( &projectName ),                "Mega Project Name" )
        ( "mega_compiler",  po::value< boost::filesystem::path >( &megaCompiler ),   "Mega Structure Compiler path" )
        ( "clang_compiler", po::value< boost::filesystem::path >( &clangCompiler ),  "Clang Compiler path" )
        ( "parser_dll",     po::value< boost::filesystem::path >( &parserDll ),      "Parser DLL Path" )
        ( "clang_plugin",   po::value< boost::filesystem::path >( &clangPlugin ),    "Clang Plugin path" )
        ( "database_dll",   po::value< boost::filesystem::path >( &databaseDll ),    "Database DLL Path" )
        ( "templates",      po::value< boost::filesystem::path >( &templatesDir ),   "Inja Templates directory" )
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
        mega::io::BuildEnvironment environment( rootSourceDir, rootBuildDir, templatesDir );

        VERIFY_RTE_MSG( boost::filesystem::exists( parserDll ), "Failed to locate parser DLL: " << parserDll );

        // load the manifest
        const mega::io::manifestFilePath projectManifestPath = environment.project_manifest();
        mega::io::Manifest               manifest( environment, projectManifestPath );

        const ToolChain toolchain( parserDll, megaCompiler, clangCompiler, clangPlugin, databaseDll );

        task::Task::PtrVector                                  tasks;
        std::map< mega::io::megaFilePath, task::Task::RawPtr > includePCHTasks;
        task::Task::RawPtrSet                                  interfaceGenTasks;
        {
            int iCounter = 1;
            for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
            {
                Task_ParseAST* pTask
                    = new Task_ParseAST( TaskArguments{ {}, environment, toolchain, iCounter }, sourceFilePath );
                tasks.push_back( task::Task::Ptr( pTask ) );

                Task_Include* pTaskInclude
                    = new Task_Include( TaskArguments{ { pTask }, environment, toolchain, iCounter }, sourceFilePath );
                tasks.push_back( task::Task::Ptr( pTaskInclude ) );

                Task_IncludePCH* pTaskIncludePCH = new Task_IncludePCH(
                    TaskArguments{ { pTaskInclude }, environment, toolchain, iCounter }, sourceFilePath );
                includePCHTasks.insert( std::make_pair( sourceFilePath, pTaskIncludePCH ) );
                tasks.push_back( task::Task::Ptr( pTaskIncludePCH ) );

                Task_ObjectInterfaceGen* pInterfaceGenTask = new Task_ObjectInterfaceGen(
                    TaskArguments{ { pTask }, environment, toolchain, iCounter }, sourceFilePath );
                interfaceGenTasks.insert( pInterfaceGenTask );
                tasks.push_back( task::Task::Ptr( pInterfaceGenTask ) );

                ++iCounter;
            }
        }

        VERIFY_RTE_MSG( !tasks.empty(), "No input source code found" );

        Task_DependencyAnalysis* pDependencyAnalysisTask
            = new Task_DependencyAnalysis( TaskArguments{ interfaceGenTasks, environment, toolchain, 0 }, manifest );
        tasks.push_back( task::Task::Ptr( pDependencyAnalysisTask ) );

        Task_SymbolAnalysis* pSymbolAnalysisTask = new Task_SymbolAnalysis(
            TaskArguments{ { pDependencyAnalysisTask }, environment, toolchain, 0 }, manifest );
        tasks.push_back( task::Task::Ptr( pSymbolAnalysisTask ) );

        {
            int iCounter = 1;
            for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
            {
                Task_SymbolRollout* pSymbolRollout = new Task_SymbolRollout(
                    TaskArguments{ { pSymbolAnalysisTask }, environment, toolchain, iCounter }, sourceFilePath );
                tasks.push_back( task::Task::Ptr( pSymbolRollout ) );

                Task_ObjectInterfaceGeneration* pInterfaceGen = new Task_ObjectInterfaceGeneration(
                    TaskArguments{ { pSymbolRollout }, environment, toolchain, iCounter }, sourceFilePath );
                tasks.push_back( task::Task::Ptr( pInterfaceGen ) );

                Task_ObjectInterfaceAnalysis* pObjectInterfaceAnalysis = new Task_ObjectInterfaceAnalysis(
                    TaskArguments{
                        { pInterfaceGen, includePCHTasks[ sourceFilePath ] }, environment, toolchain, iCounter },
                    sourceFilePath );
                tasks.push_back( task::Task::Ptr( pObjectInterfaceAnalysis ) );

                Task_ConcreteTree* pConcreteTree = new Task_ConcreteTree(
                    TaskArguments{ { pObjectInterfaceAnalysis }, environment, toolchain, iCounter }, sourceFilePath );
                tasks.push_back( task::Task::Ptr( pConcreteTree ) );

                ++iCounter;
            }
        }

        task::Schedule::Ptr pSchedule( new task::Schedule( tasks ) );
        task::run( pSchedule, std::cout );
    }
}
} // namespace interface
} // namespace driver
