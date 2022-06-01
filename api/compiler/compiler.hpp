
#ifndef COMPILER_27_MAY_2022
#define COMPILER_27_MAY_2022

#include "pipeline/configuration.hpp"
#include "pipeline/pipeline.hpp"

#include "common/hash.hpp"
#include "common/stash.hpp"

#include "boost/filesystem/path.hpp"
#include "boost/serialization/nvp.hpp"
#include "boost/serialization/split_member.hpp"

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

    template < typename Archive, typename HashCodeType >
    void saveHashCode( Archive& archive, const char* pszName, const HashCodeType& hashcode ) const
    {
        const std::string strHex = hashcode.toHexString();
        archive&          boost::serialization::make_nvp( pszName, strHex );
    }

    template < typename Archive, typename HashCodeType >
    void loadHashCode( Archive& archive, const char* pszName, HashCodeType& hashcode )
    {
        std::string strHex;
        archive&    boost::serialization::make_nvp( pszName, strHex );
        hashcode.set( common::Hash::fromHexString( strHex ) );
    }

    template < typename Archive >
    void save( Archive& archive, const unsigned int version ) const
    {
        archive& boost::serialization::make_nvp( "parserDllPath", parserDllPath );
        archive& boost::serialization::make_nvp( "megaCompilerPath", megaCompilerPath );
        archive& boost::serialization::make_nvp( "clangCompilerPath", clangCompilerPath );
        archive& boost::serialization::make_nvp( "clangPluginPath", clangPluginPath );
        archive& boost::serialization::make_nvp( "databasePath", databasePath );

        saveHashCode( archive, "parserDllHash", parserDllHash );
        saveHashCode( archive, "megaCompilerHash", megaCompilerHash );
        saveHashCode( archive, "clangCompilerHash", clangCompilerHash );
        saveHashCode( archive, "clangPluginHash", clangPluginHash );
        saveHashCode( archive, "databaseHash", databaseHash );

        saveHashCode( archive, "toolChainHash", toolChainHash );
    }

    template < typename Archive >
    void load( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "parserDllPath", parserDllPath );
        archive& boost::serialization::make_nvp( "megaCompilerPath", megaCompilerPath );
        archive& boost::serialization::make_nvp( "clangCompilerPath", clangCompilerPath );
        archive& boost::serialization::make_nvp( "clangPluginPath", clangPluginPath );
        archive& boost::serialization::make_nvp( "databasePath", databasePath );

        loadHashCode( archive, "parserDllHash", parserDllHash );
        loadHashCode( archive, "megaCompilerHash", megaCompilerHash );
        loadHashCode( archive, "clangCompilerHash", clangCompilerHash );
        loadHashCode( archive, "clangPluginHash", clangPluginHash );
        loadHashCode( archive, "databaseHash", databaseHash );

        loadHashCode( archive, "toolChainHash", toolChainHash );
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

struct Directories
{
    boost::filesystem::path rootSourceDir, rootBuildDir, templatesDir;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "rootSourceDir", rootSourceDir );
        archive& boost::serialization::make_nvp( "rootBuildDir", rootBuildDir );
        archive& boost::serialization::make_nvp( "templatesDir", templatesDir );
    }
};

struct Configuration
{
    pipeline::PipelineID                   pipelineID;
    std::string                            projectName;
    std::vector< boost::filesystem::path > componentInfoPaths;
    Directories                            directories;
    ToolChain                              toolChain;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        // ensure pipelineID is first thing to serialize
        // use ConfigurationHeader for pipelineID
        //archive& boost::serialization::make_nvp( "id", pipelineID );
        archive& boost::serialization::make_nvp( "projectName", projectName );
        archive& boost::serialization::make_nvp( "componentInfoPaths", componentInfoPaths );
        archive& boost::serialization::make_nvp( "directories", directories );
        archive& boost::serialization::make_nvp( "toolChain", toolChain );
    }
};

pipeline::Configuration makePipelineConfiguration( const Configuration& config );

} // namespace compiler
} // namespace mega

#endif // COMPILER_27_MAY_2022
