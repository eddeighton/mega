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
#include "environment/jit_database.hpp"

#include "runtime/functor_dispatch.hpp"
#include "runtime/functor_id.hxx"

#include "il/backend/backend.hpp"

#include "log/log.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

#include <vector>
#include <string>
#include <iostream>

namespace driver::function
{

void command( mega::network::Log& log, bool bHelp, const std::vector< std::string >& args )
{
    std::string projectName;
    I32         projectVersion = -1;
    std::string strFunction;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Project Commands" );
    {
        // clang-format off
        commandOptions.add_options()
        ( "name",       po::value< std::string >( &projectName ),   "Project Name" )
        ( "version",    po::value< I32 >( &projectVersion ),        "Program Version" )
        ( "id",         po::value( &strFunction ),                  "Function ID" )
        
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
        using namespace mega;
        using namespace mega::service;
        VERIFY_RTE_MSG( !strFunction.empty(), "No function specified" );

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
        const auto programManifest = Environment::load( program );

        mega::runtime::JITDatabase database( programManifest.getDatabase() );
        il::Factory factory;

        // const mega::TypeID       typeID = mega::ROOT_TYPE_ID;
        {
            const mega::TypeID typeID{ 0x00040000 };
            const runtime::FunctorID functorID{ runtime::FunctorID::e_Object_constructor, runtime::Object_id{ typeID } };
            const auto funcDef = runtime::dispatchFactory( database, factory, functorID );
            std::cout << il::generateCPP( funcDef ) << std::endl;
        }
        {
            const mega::TypeID typeID{ 0x00040000 };
            const runtime::FunctorID functorID{ runtime::FunctorID::e_Object_destructor, runtime::Object_id{ typeID } };
            const auto funcDef = runtime::dispatchFactory( database, factory, functorID );
            std::cout << il::generateCPP( funcDef ) << std::endl;
        }
    }
}
} // namespace driver::project
