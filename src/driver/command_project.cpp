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

#include "service/terminal.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

#include <vector>
#include <string>
#include <iostream>

namespace driver::project
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path projectPath;
    bool                    bGetProject   = false;
    bool                    bResetProject = false;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Project Commands" );
    {
        // clang-format off
        commandOptions.add_options()
        ( "set",    po::value< boost::filesystem::path >( &projectPath ), "Set project" )
        ( "get",    po::bool_switch( &bGetProject ),                      "Get current project" )
        ( "reset",  po::bool_switch( &bResetProject ),                    "Unload any active project" )
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
        if ( !projectPath.empty() )
        {
            mega::service::Terminal      terminal;
            const mega::network::Project project( projectPath );
            terminal.SetProject( project );
        }
        else if ( bGetProject )
        {
            mega::service::Terminal      terminal;
            const mega::network::Project project = terminal.GetProject();
            std::cout << project.getProjectInstallPath().string() << std::endl;
        }
        else if( bResetProject )
        {
            mega::service::Terminal      terminal;
            const mega::network::Project project;
            terminal.SetProject( project );
        }
        else // if ( bGetInstallInfo )
        {
            mega::service::Terminal          terminal;
            const mega::network::Project     project   = terminal.GetProject();
            const auto                       result    = terminal.GetMegastructureInstallation();
            const mega::utilities::ToolChain toolChain = result.getToolchainXML();

            std::ostringstream osProject;
            {
                if ( project.isEmpty() )
                {
                    osProject << "NO PROJECT";
                }
                else
                {
                    osProject << "Project Install:    " << project.getProjectInstallPath().string() << "\n";
                    osProject << "Project Bin:        " << project.getProjectBin().string() << "\n";
                    osProject << "Project Database:   " << project.getProjectDatabase().string() << "\n";
                    osProject << "Project Temp Dir:   " << project.getProjectTempDir().string() << "\n";
                }
            }

            // clang-format off
            std::cout   << "Installation:       " << result.getInstallationPath().string() << "\n"

                        << "Clang Version:      " << toolChain.strClangCompilerVersion << "\n"

                        << "parserDllPath:      " << toolChain.parserDllPath.string() << "\n"
                        << "megaCompilerPath:   " << toolChain.megaCompilerPath.string() << "\n"
                        << "clangCompilerPath:  " << toolChain.clangCompilerPath.string() << "\n"
                        << "clangPluginPath:    " << toolChain.clangPluginPath.string() << "\n"
                        << "databasePath:       " << toolChain.databasePath.string() << "\n"

                        << "parserDllHash:      " << toolChain.parserDllHash.toHexString() << "\n"
                        << "megaCompilerHash:   " << toolChain.megaCompilerHash.toHexString() << "\n"
                        << "clangPluginHash:    " << toolChain.clangPluginHash.toHexString() << "\n"
                        << "databaseVersion:    " << toolChain.databaseVersion.toHexString() << "\n"
                        << "clangCompilerHash:  " << toolChain.clangCompilerHash.toHexString() << "\n"
                        << "toolChainHash:      " << toolChain.toolChainHash.toHexString() << "\n"

                        << osProject.str()

                        << std::endl;
            // clang-format on
        }
    }
}
} // namespace driver::project
