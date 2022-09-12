
#include "utilities/tool_chain_hash.hpp"

#include <boost/process.hpp>

namespace mega
{
namespace utilities
{

ToolChain::ToolChain() {}

ToolChain::ToolChain( const std::string& strClangCompilerVersion,
                      std::size_t        szDatabaseVersion,

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

} // namespace utilities
} // namespace mega