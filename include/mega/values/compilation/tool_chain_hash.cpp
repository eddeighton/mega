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

#include "mega/values/compilation/tool_chain_hash.hpp"

#include "common/process.hpp"

#include <boost/process.hpp>
#include <boost/dll.hpp>

namespace mega::utilities
{

ToolChain::ToolChain() = default;

ToolChain::ToolChain( const std::string& _strClangCompilerVersion,
                      mega::U64          _szDatabaseVersion,

                      const boost::filesystem::path& _parser,
                      const boost::filesystem::path& _path_megaCompiler,
                      const boost::filesystem::path& _path_clangCompiler,
                      const boost::filesystem::path& _path_clangPlugin,
                      const boost::filesystem::path& _database )

    : parserPath( _parser )
    , megaCompilerPath( _path_megaCompiler )
    , clangCompilerPath( _path_clangCompiler )
    , clangPluginPath( _path_clangPlugin )
    , databasePath( _database )

    , parserHash( _parser )
    , megaCompilerHash( _path_megaCompiler )
    , clangPluginHash( _path_clangPlugin )

    , strClangCompilerVersion( _strClangCompilerVersion )

    , databaseVersion( _szDatabaseVersion )
    , clangCompilerHash( _strClangCompilerVersion )

    , toolChainHash( parserHash, megaCompilerHash, clangCompilerHash, clangPluginHash, databaseVersion )
{
}

namespace
{
std::string runCmd( const std::string& strCmd )
{
    std::string strOutput, strError;
    common::runProcess( strCmd, strOutput, strError );
    return strOutput;
}
} // namespace

std::string ToolChain::getClangVersion( const boost::filesystem::path& path_clangCompiler )
{
    std::ostringstream osCmd;
    osCmd << path_clangCompiler.string() << " --version";
    return runCmd( osCmd.str() );
}

mega::U64 ToolChain::getDatabaseVersion( const boost::filesystem::path& path_database )
{
    boost::shared_ptr< const mega::U64 > pSymbolDirect
        = boost::dll::import_alias< const mega::U64 >( path_database, "MEGA_DATABASE_VERSION" );
    return *pSymbolDirect;
}

} // namespace mega::utilities
