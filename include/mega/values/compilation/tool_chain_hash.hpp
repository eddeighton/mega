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

#ifndef TOOLCHAIN_HASH_03_JUNE_20221
#define TOOLCHAIN_HASH_03_JUNE_20221

#include "mega/values/native_types.hpp"

#include "common/serialisation.hpp"

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/filesystem/path.hpp>

#include <common/stash.hpp>

namespace mega::utilities
{
struct ToolChain
{
    ToolChain();

    ToolChain( const std::string&             strClangCompilerVersion,
               mega::U64                      databaseVersion,
               const boost::filesystem::path& parser,
               const boost::filesystem::path& megaCompiler,
               const boost::filesystem::path& clangCompiler,
               const boost::filesystem::path& clangPlugin,
               const boost::filesystem::path& database );

    boost::filesystem::path parserPath, megaCompilerPath, clangCompilerPath, clangPluginPath, databasePath;
    task::FileHash        parserHash, megaCompilerHash, clangPluginHash;
    std::string           strClangCompilerVersion;
    common::Hash          databaseVersion;
    common::Hash          clangCompilerHash;
    task::DeterminantHash toolChainHash;

    static std::string getClangVersion( const boost::filesystem::path& path_clangCompiler );
    static mega::U64   getDatabaseVersion( const boost::filesystem::path& path_database );

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
    void save( Archive& archive, const unsigned int /*version*/ ) const
    {
        archive& boost::serialization::make_nvp( "parserPath", parserPath );
        archive& boost::serialization::make_nvp( "megaCompilerPath", megaCompilerPath );
        archive& boost::serialization::make_nvp( "clangCompilerPath", clangCompilerPath );
        archive& boost::serialization::make_nvp( "clangPluginPath", clangPluginPath );
        archive& boost::serialization::make_nvp( "databasePath", databasePath );

        archive& boost::serialization::make_nvp( "clangVersion", strClangCompilerVersion );

        saveHashCode( archive, "databaseVersion", databaseVersion );
        saveHashCode( archive, "parserHash", parserHash );
        saveHashCode( archive, "megaCompilerHash", megaCompilerHash );
        saveHashCode( archive, "clangCompilerHash", clangCompilerHash );
        saveHashCode( archive, "clangPluginHash", clangPluginHash );

        saveHashCode( archive, "toolChainHash", toolChainHash );
    }

    template < typename Archive >
    void load( Archive& archive, const unsigned int v )
    {
        archive& boost::serialization::make_nvp( "parserPath", parserPath );
        archive& boost::serialization::make_nvp( "megaCompilerPath", megaCompilerPath );
        archive& boost::serialization::make_nvp( "clangCompilerPath", clangCompilerPath );
        archive& boost::serialization::make_nvp( "clangPluginPath", clangPluginPath );
        archive& boost::serialization::make_nvp( "databasePath", databasePath );

        archive& boost::serialization::make_nvp( "clangVersion", strClangCompilerVersion );

        loadHashCode( archive, "databaseVersion", databaseVersion );
        loadHashCode( archive, "parserHash", parserHash );
        loadHashCode( archive, "megaCompilerHash", megaCompilerHash );
        loadHashCode( archive, "clangCompilerHash", clangCompilerHash );
        loadHashCode( archive, "clangPluginHash", clangPluginHash );

        loadHashCode( archive, "toolChainHash", toolChainHash );
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

} // namespace mega::utilities

#endif // TOOLCHAIN_HASH_03_JUNE_20221