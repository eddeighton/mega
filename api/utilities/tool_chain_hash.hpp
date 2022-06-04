
#ifndef TOOLCHAIN_HASH_03_JUNE_20221
#define TOOLCHAIN_HASH_03_JUNE_20221

#include "utilities/serialization_helpers.hpp"

#include "boost/serialization/nvp.hpp"
#include "boost/serialization/split_member.hpp"
#include "boost/filesystem/path.hpp"

#include <common/stash.hpp>

namespace mega
{
namespace utilities
{

struct ToolChain
{
    ToolChain();
    ToolChain( const boost::filesystem::path& parserDll,
               const boost::filesystem::path& megaCompiler,
               const boost::filesystem::path& clangCompiler,
               const boost::filesystem::path& clangPlugin,
               const boost::filesystem::path& databaseDll );

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
    void save( Archive& archive, const unsigned int v ) const
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
    void load( Archive& archive, const unsigned int v )
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

} // namespace utilities
} // namespace mega

#endif // TOOLCHAIN_HASH_03_JUNE_20221