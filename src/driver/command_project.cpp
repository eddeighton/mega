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

#include "environment/environment.hpp"

#include "log/log.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

#include <vector>
#include <string>
#include <iostream>

namespace driver::project
{
namespace
{
mega::MP toMP( const std::string& strMP )
{
    mega::MP           mp;
    std::istringstream is( strMP );
    is >> mp;
    return mp;
}
} // namespace

void command( mega::network::Log& log, bool bHelp, const std::vector< std::string >& args )
{
    std::string projectName;
    I32         projectVersion = -1;

    std::string strLoad, strUnload, strGet;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Project Commands" );
    {
        // clang-format off
        commandOptions.add_options()
        ( "name",       po::value< std::string >( &projectName ),   "Project Name" )
        ( "version",    po::value< I32 >( &projectVersion ),        "Program Version" )
        ( "load",       po::value( &strLoad ),                      "Load program to MP" )
        ( "unload",     po::value( &strUnload ),                    "Unload any existing program on MP" )
        ( "get",        po::value( &strGet ),                       "Get current program running on MP" )
        
        ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "name", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        using namespace mega::service;

        if( !strLoad.empty() )
        {
            VERIFY_RTE_MSG( !projectName.empty(), "Invalid project name" );
            const Project project( projectName );
            Program       program;
            {
                if( projectVersion == -1 )
                {
                    auto programOpt = Program::latest( project, Environment::workBin() );
                    VERIFY_RTE_MSG( programOpt.has_value(), "No latest version found for project: " << project );
                    program = programOpt.value();
                }
                else
                {
                    program = Program( project, Program::Version( projectVersion ) );
                }
            }

            const auto programFile = Environment::prog( program );
            VERIFY_RTE_MSG(
                boost::filesystem::exists( programFile ), "Failed to locate program at: " << programFile.string() );

            const mega::MP mp = toMP( strLoad );

            mega::service::Terminal terminal( log );
            terminal.ProgramLoad( program, mp );
        }
        else if( !strUnload.empty() )
        {
            const mega::MP mp = toMP( strUnload );
            mega::service::Terminal terminal( log );
            terminal.ProgramUnload( mp );
        }
        else if( !strGet.empty() )
        {
            const mega::MP mp = toMP( strGet );
            mega::service::Terminal terminal( log );
            auto prog = terminal.ProgramGet( mp );
            std::cout << "MP: " << mp << " reports: " << prog << std::endl;
        }
        else
        {
            std::cout << "Current User:       " << Environment::user() << "\n";
            std::cout << "CFG Type:           " << Environment::cfgType() << "\n";
            std::cout << "Work Dir:           " << Environment::work().string() << "\n";
            std::cout << std::endl;

            if( !projectName.empty() )
            {
                const Project project( projectName );
                const auto    lastestProjectOpt = Program::latest( project, Environment::workBin() );

                std::cout << "Project:            " << project << "\n";
                if( lastestProjectOpt.has_value() )
                {
                    std::cout << "Latest Program:     " << lastestProjectOpt.value() << "\n";
                }
                else
                {
                    std::cout << "Latest Program:     NONE\n";
                }
            }

            std::cout << std::endl;

            mega::service::Terminal          terminal( log );
            const auto                       result    = terminal.GetMegastructureInstallation();
            const mega::utilities::ToolChain toolChain = result.getToolchain();

            // clang-format off
            std::cout   << "Installation:       " << result.getInstallationPath().string() << "\n"

                        << "Clang Version:      " << toolChain.strClangCompilerVersion << "\n"

                        << "parserPath:         " << toolChain.parserPath.string() << "\n"
                        << "megaCompilerPath:   " << toolChain.megaCompilerPath.string() << "\n"
                        << "megaExecutorPath:   " << toolChain.megaExecutorPath.string() << "\n"
                        << "clangCompilerPath:  " << toolChain.clangCompilerPath.string() << "\n"
                        << "clangPluginPath:    " << toolChain.clangPluginPath.string() << "\n"
                        << "databasePath:       " << toolChain.databasePath.string() << "\n"

                        << "megaManglePath:     " << toolChain.megaManglePath.string() << "\n"
                        << "leafPath:           " << toolChain.leafPath.string() << "\n"

                        << "parserDllHash:      " << toolChain.parserHash.toHexString() << "\n"
                        << "megaCompilerHash:   " << toolChain.megaCompilerHash.toHexString() << "\n"
                        << "clangPluginHash:    " << toolChain.clangPluginHash.toHexString() << "\n"
                        << "databaseVersion:    " << toolChain.databaseVersion.toHexString() << "\n"
                        << "clangCompilerHash:  " << toolChain.clangCompilerHash.toHexString() << "\n"
                        << "toolChainHash:      " << toolChain.toolChainHash.toHexString() << "\n"

                        << std::endl;
            // clang-format on
        }
    }
}
} // namespace driver::project
