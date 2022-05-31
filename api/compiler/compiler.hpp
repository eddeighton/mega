
#ifndef COMPILER_27_MAY_2022
#define COMPILER_27_MAY_2022

#include "pipeline/configuration.hpp"

#include "common/hash.hpp"
#include "common/stash.hpp"

#include "boost/filesystem/path.hpp"

#include <vector>
#include <string>

namespace mega
{
namespace compiler
{

struct ToolChain
{
    boost::filesystem::path parserDllPath, megaCompilerPath, clangCompilerPath, clangPluginPath, databasePath;
    task::FileHash          parserDllHash, megaCompilerHash, clangCompilerHash, clangPluginHash, databaseHash;
    task::DeterminantHash   toolChainHash;

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
    std::string                            projectName;
    std::vector< boost::filesystem::path > componentInfoPaths;
    Directories                            directories;
    ToolChain                              toolChain;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& projectName;
        archive& componentInfoPaths;
        archive& directories;
        archive& toolChain;
    }
};

pipeline::Configuration makePipelineConfiguration( const Configuration& config );

} // namespace compiler
} // namespace mega

#endif // COMPILER_27_MAY_2022
