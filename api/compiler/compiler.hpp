
#ifndef COMPILER_27_MAY_2022
#define COMPILER_27_MAY_2022

#include "pipeline/configuration.hpp"

#include "common/hash.hpp"
#include "common/stash.hpp"

#include "boost/filesystem/path.hpp"

namespace mega
{
namespace compiler
{

struct ToolChain
{
    boost::filesystem::path parserDllPath, megaCompilerPath, clangCompilerPath, clangPluginPath, databasePath;
    task::FileHash parserDllHash, megaCompilerHash, clangCompilerHash, clangPluginHash, databaseHash;
    task::DeterminantHash toolChainHash;

    /*ToolChain( const boost::filesystem::path& parserDllPath, const boost::filesystem::path& megaCompilerPath,
               const boost::filesystem::path& clangCompilerPath, const boost::filesystem::path& clangPluginPath,
               const boost::filesystem::path& databasePath )
        : parserDllPath( parserDllPath )
        , megaCompilerPath( megaCompilerPath )
        , clangCompilerPath( clangCompilerPath )
        , clangPluginPath( clangPluginPath )
        , databasePath( databasePath )
     , parserDllHash( parserDllPath )
     , megaCompilerHash( megaCompilerPath )
     , clangCompilerHash( clangCompilerPath )
     , clangPluginHash( clangPluginPath )
     , databaseHash( databasePath )
     , toolChainHash( { parserDllHash, megaCompilerHash, clangCompilerHash, clangPluginHash, databaseHash } )*/

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& parserDllPath;
        archive& megaCompilerPath;
        archive& clangCompilerPath;
        archive& clangPluginPath;
        archive& databasePath;
        
        archive& parserDllHash;
        archive& megaCompilerHash;
        archive& clangCompilerHash;
        archive& clangPluginHash;
        archive& databaseHash;

        archive& toolChainHash;
    }

};

struct Directories
{
    boost::filesystem::path rootSourceDir, rootBuildDir, templatesDir;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& rootSourceDir;
        archive& rootBuildDir;
        archive& templatesDir;
    }
};

struct Configuration
{
    std::string projectName;
    Directories directories;
    ToolChain   toolChain;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& projectName;
        archive& directories;
        archive& toolChain;
    }
};

pipeline::Configuration makePipelineConfiguration( const Configuration& config );

} // namespace compiler
} // namespace mega

#endif // COMPILER_27_MAY_2022
