
#include "utilities/tool_chain_hash.hpp"

namespace mega
{
namespace utilities
{

ToolChain::ToolChain() {}

ToolChain::ToolChain( const boost::filesystem::path& path_parserDll,
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
    , clangCompilerHash( path_clangCompiler )
    , clangPluginHash( path_clangPlugin )
    , databaseHash( path_databaseDll )

    , toolChainHash( parserDllHash, megaCompilerHash, clangCompilerHash, clangPluginHash, databaseHash )
{
}

} // namespace utilities
} // namespace mega