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

#include "utilities/tool_chain_hash.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <spdlog/spdlog.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace driver
{
namespace tool_chain
{
void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path clangPlugin, parser, megaCompiler, clangCompiler, database, jit, megaMangle, leaf;
    boost::filesystem::path outputFilePath;

    {
        namespace po = boost::program_options;
        // clang-format off
        po::options_description commandOptions( " Generate database json file" );
        {
            commandOptions.add_options()
                ( "clang_compiler", po::value< boost::filesystem::path >( &clangCompiler ), "Clang Compiler path" )
                ( "parser",         po::value< boost::filesystem::path >( &parser ),        "Parser Path" )
                ( "mega_compiler",  po::value< boost::filesystem::path >( &megaCompiler ),  "Megastructure compiler pipeline path" )
                ( "clang_plugin",   po::value< boost::filesystem::path >( &clangPlugin ),   "Clang Plugin path" )
                ( "database",       po::value< boost::filesystem::path >( &database ),      "Database Path" )

                ( "jit",            po::value< boost::filesystem::path >( &jit ),           "Jit Path" )
                ( "mega_mangle",    po::value< boost::filesystem::path >( &megaMangle ),    "Mega Mangle Path" )
                ( "leaf",           po::value< boost::filesystem::path >( &leaf ),          "Leaf Path" )

                ( "output_xml",     po::value< boost::filesystem::path >( &outputFilePath ),  "Output XML File" )
                ;
        }
        // clang-format on
        po::variables_map vm;
        po::store( po::command_line_parser( args ).options( commandOptions ).run(), vm );
        po::notify( vm );
        if( bHelp )
        {
            std::cout << commandOptions << "\n";
            return;
        }
    }

    // clang-format off
    VERIFY_RTE_MSG( boost::filesystem::exists( clangCompiler ), "File not found clangCompiler at : " << clangCompiler.string() );
    VERIFY_RTE_MSG( boost::filesystem::exists( parser ), "File not found parser at : " << parser.string() );
    VERIFY_RTE_MSG( boost::filesystem::exists( megaCompiler ), "File not found megaCompiler at : " << megaCompiler.string() );
    VERIFY_RTE_MSG( boost::filesystem::exists( clangPlugin ), "File not found clangPlugin at : " << clangPlugin.string() );
    VERIFY_RTE_MSG( boost::filesystem::exists( database ), "File not found database at : " << database.string() );
    VERIFY_RTE_MSG( boost::filesystem::exists( jit ), "File not found jit at : " << jit.string() );
    VERIFY_RTE_MSG( boost::filesystem::exists( megaMangle ), "File not found megaMangle at : " << megaMangle.string() );
    VERIFY_RTE_MSG( boost::filesystem::exists( leaf ), "File not found leaf at : " << leaf.string() );
    // clang-format on

    {
        std::ostringstream os;
        {
            const std::string strClangVersion   = mega::utilities::ToolChain::getClangVersion( clangCompiler );
            const mega::U64   szDatabaseVersion = mega::utilities::ToolChain::getDatabaseVersion( database );

            const mega::utilities::ToolChain toolChain( strClangVersion, szDatabaseVersion, parser, megaCompiler,
                                                        clangCompiler, clangPlugin, database, jit, megaMangle, leaf );

            boost::archive::xml_oarchive oa( os );
            oa&                          boost::serialization::make_nvp( "toolchain", toolChain );
        }
        boost::filesystem::ensureFoldersExist( outputFilePath );
        boost::filesystem::updateFileIfChanged( outputFilePath, os.str() );
    }
}
} // namespace tool_chain
} // namespace driver
