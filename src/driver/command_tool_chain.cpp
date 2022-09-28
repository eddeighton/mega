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

#include "spdlog/spdlog.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/dll.hpp>
#include <boost/shared_ptr.hpp>

#pragma warning( push )
#pragma warning( disable : 4996 ) // iterator thing
#pragma warning( disable : 4244 ) // conversion to DWORD from system_clock::rep
#include <boost/process.hpp>
#pragma warning( pop )

#include <string>
#include <vector>
#include <iostream>

namespace driver
{
namespace tool_chain
{
namespace
{

std::string runCmd( const std::string& strCmd )
{
    namespace bp = boost::process;

    std::ostringstream osResult;

    bp::ipstream errStream, outStream; // reading pipe-stream
    bp::child    c( strCmd, bp::std_out > outStream, bp::std_err > errStream );

    std::string strOutputLine;
    while ( c.running() && std::getline( outStream, strOutputLine ) )
    {
        if ( !strOutputLine.empty() )
        {
            osResult << strOutputLine;
        }
    }

    c.wait();

    return osResult.str();
}

std::string getClangVersion( const boost::filesystem::path& path_clangCompiler )
{
    std::ostringstream osCmd;
    osCmd << path_clangCompiler.native() << " --version";
    return runCmd( osCmd.str() );
}

mega::U64 getDatabaseVersion( const boost::filesystem::path& path_database )
{
    boost::shared_ptr< const mega::U64 > pSymbolDirect
        = boost::dll::import_alias< const mega::U64 >( path_database, "MEGA_DATABASE_VERSION" );
    return *pSymbolDirect;
}

} // namespace
void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path clangPlugin, parserDll, megaCompiler, clangCompiler, databaseDll;
    boost::filesystem::path outputFilePath;

    {
        namespace po = boost::program_options;
        // clang-format off
        po::options_description commandOptions( " Generate database json file" );
        {
            commandOptions.add_options()
                ( "clang_compiler", po::value< boost::filesystem::path >( &clangCompiler ),  "Clang Compiler path" )
                ( "parser_dll",     po::value< boost::filesystem::path >( &parserDll ),      "Parser DLL Path" )
                ( "mega_compiler",  po::value< boost::filesystem::path >( &megaCompiler ),   "Megastructure compiler pipeline path" )
                ( "clang_plugin",   po::value< boost::filesystem::path >( &clangPlugin ),    "Clang Plugin path" )
                ( "database_dll",   po::value< boost::filesystem::path >( &databaseDll ),    "Database DLL Path" )

                ( "output_xml",     po::value< boost::filesystem::path >( &outputFilePath ),  "Output XML File" )
                ;
        }
        // clang-format on
        po::variables_map vm;
        po::store( po::command_line_parser( args ).options( commandOptions ).run(), vm );
        po::notify( vm );
        if ( bHelp )
        {
            std::cout << commandOptions << "\n";
            return;
        }
    }

    // clang-format off
    VERIFY_RTE_MSG( boost::filesystem::exists( clangCompiler ), "File not found clangCompiler at : " << clangCompiler.string() );
    VERIFY_RTE_MSG( boost::filesystem::exists( parserDll ), "File not found parserDll at : " << parserDll.string() );
    VERIFY_RTE_MSG( boost::filesystem::exists( megaCompiler ), "File not found megaCompiler at : " << megaCompiler.string() );
    VERIFY_RTE_MSG( boost::filesystem::exists( clangPlugin ), "File not found clangPlugin at : " << clangPlugin.string() );
    VERIFY_RTE_MSG( boost::filesystem::exists( databaseDll ), "File not found databaseDll at : " << databaseDll.string() );
    // clang-format on

    {
        std::ostringstream os;
        {
            const std::string strClangVersion   = getClangVersion( clangCompiler );
            const mega::U64   szDatabaseVersion = getDatabaseVersion( databaseDll );

            const mega::utilities::ToolChain toolChain(
                strClangVersion, szDatabaseVersion, parserDll, megaCompiler, clangCompiler, clangPlugin, databaseDll );

            boost::archive::xml_oarchive oa( os );
            oa&                          boost::serialization::make_nvp( "toolchain", toolChain );
        }
        boost::filesystem::updateFileIfChanged( outputFilePath, os.str() );
    }
}
} // namespace tool_chain
} // namespace driver
