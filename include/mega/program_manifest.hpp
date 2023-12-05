
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

#ifndef GUARD_2023_December_04_program_manifest
#define GUARD_2023_December_04_program_manifest

#include "mega/values/service/program.hpp"

#include "common/serialisation.hpp"
#include "common/assert_verify.hpp"
#include "common/stash.hpp"

namespace mega::service
{
class ProgramManifest
{
public:
    struct ComponentHash
    {
        std::string    name;
        task::FileHash hash;

        template < typename Archive, typename HashCodeType >
        inline void saveHashCode( Archive& archive, const char* pszName, const HashCodeType& hashcode ) const
        {
            const std::string strHex = hashcode.toHexString();
            archive&          boost::serialization::make_nvp( pszName, strHex );
        }

        template < typename Archive, typename HashCodeType >
        inline void loadHashCode( Archive& archive, const char* pszName, HashCodeType& hashcode )
        {
            std::string strHex;
            archive&    boost::serialization::make_nvp( pszName, strHex );
            hashcode.set( common::Hash::fromHexString( strHex ) );
        }

        template < typename Archive >
        inline void save( Archive& archive, const unsigned int /*version*/ ) const
        {
            archive& boost::serialization::make_nvp( "name", name );
            saveHashCode( archive, "hash", hash );
        }

        template < typename Archive >
        inline void load( Archive& archive, const unsigned int v )
        {
            archive& boost::serialization::make_nvp( "name", name );
            loadHashCode( archive, "hash", hash );
        }

        inline bool operator==( const ComponentHash& ) const = default;

        BOOST_SERIALIZATION_SPLIT_MEMBER()
    };

    inline ProgramManifest() = default;

    inline ProgramManifest( Program program, boost::filesystem::path database,
                     std::vector< ComponentHash > components )
        : m_program( std::move( program ) )
        , m_database( std::move( database ) )
        , m_components( std::move( components ) )
    {
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "program", m_program );
            archive& boost::serialization::make_nvp( "database", m_database );
            archive& boost::serialization::make_nvp( "components", m_components );
        }
        else
        {
            archive& m_program;
            archive& m_database;
            archive& m_components;
        }
    }

    inline const Program&                      getProgram() const { return m_program; }
    inline const boost::filesystem::path&      getDatabase() const { return m_database; }
    inline const std::vector< ComponentHash >& getComponents() const { return m_components; }

    inline bool operator==( const ProgramManifest& cmp ) const = default;

private:
    Program                      m_program;
    boost::filesystem::path      m_database;
    std::vector< ComponentHash > m_components;
};
} // namespace mega::service

#endif // GUARD_2023_December_04_program_manifest
