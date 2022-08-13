
#include "utilities/tool_chain_hash.hpp"

#include <boost/process.hpp>

namespace mega
{
namespace utilities
{

namespace
{
std::string getClangVersion( const boost::filesystem::path& path_clangCompiler )
{
    namespace bp = boost::process;

    std::ostringstream osResult;

    std::ostringstream osCmd;
    osCmd << path_clangCompiler.native() << " --version";

    bp::ipstream errStream, outStream; // reading pipe-stream
    bp::child    c( osCmd.str(), bp::std_out > outStream, bp::std_err > errStream );

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
} // namespace

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
    , clangPluginHash( path_clangPlugin )
    , databaseHash( path_databaseDll )

    , strClangCompilerVersion( getClangVersion( path_clangCompiler ) )
    , clangCompilerHash( strClangCompilerVersion )

    , toolChainHash( parserDllHash, megaCompilerHash, clangCompilerHash, clangPluginHash, databaseHash )
{
}

} // namespace utilities
} // namespace mega