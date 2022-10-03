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

#include <boost/process.hpp>

namespace mega::utilities
{

ToolChain::ToolChain() {}

ToolChain::ToolChain( const std::string& strClangCompilerVersion,
                      mega::U64          szDatabaseVersion,

                      const boost::filesystem::path& path_parserDll,
                      const boost::filesystem::path& path_megaCompiler,
                      const boost::filesystem::path& path_clangCompiler,
                      const boost::filesystem::path& path_clangPlugin,
                      const boost::filesystem::path& path_databaseDll )

    : parserDllPath( path_parserDll )
    , megaCompilerPath( path_megaCompiler )
    , clangCompilerPath( path_clangCompiler )
    , clangPluginPath( path_clangPlugin )
    , databasePath( path_databaseDll )

    , parserDllHash( path_parserDll )
    , megaCompilerHash( path_megaCompiler )
    , clangPluginHash( path_clangPlugin )

    , strClangCompilerVersion( strClangCompilerVersion )
    , clangCompilerHash( strClangCompilerVersion )

    , databaseVersion( szDatabaseVersion )

    , toolChainHash( parserDllHash, megaCompilerHash, clangCompilerHash, clangPluginHash, databaseVersion )
{
}

} // namespace mega::utilities
