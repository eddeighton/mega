
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

#ifndef GUARD_2022_October_31_archive
#define GUARD_2022_October_31_archive

#include "mega/reference.hpp"
#include "mega/address_table.hpp"
#include "mega/tag_parser.hpp"
#include "mega/reference_io.hpp"

#include "common/file.hpp"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>

#include <string>
#include <sstream>
#include <unordered_map>

namespace mega
{
static constexpr auto boostXMLArchiveFlags = boost::archive::no_header | boost::archive::no_codecvt
                                             | boost::archive::no_xml_tag_checking | boost::archive::no_tracking;

class XMLSaveArchive
{
public:
    inline XMLSaveArchive( const boost::filesystem::path& filePath )
        : m_pFileStream( boost::filesystem::createNewFileStream( filePath ) )
        , m_archive( *m_pFileStream, boostXMLArchiveFlags )
    {
    }

    template < typename T >
    inline void save( const char* name, T& value )
    {
        m_archive& boost::serialization::make_nvp( name, value );
    }

    inline void save( const char* name, const mega::reference& ref )
    {
        std::string strEncode;
        {
            std::ostringstream os;
            if( auto indexOpt = m_table.refToIndexIfExist( ref ) )
            {
                os << indexOpt.value();
            }
            else
            {
                os << ref;
            }
            strEncode = os.str();
        }
        m_archive& boost::serialization::make_nvp( name, strEncode );
    }

    inline void save( const mega::reference& ref ) { this->save( "ref", ref ); }

    inline void beginStructure( const reference& ref )
    {
        //
        m_table.refToIndex( ref );
    }
    inline void endStructure( const reference& ref ) {}

    inline void beginData( const char* name, bool bIsObject, const reference& ref )
    {
        auto indexOpt = m_table.refToIndexIfExist( ref );
        VERIFY_RTE( indexOpt.has_value() );

        if( bIsObject )
        {
            std::ostringstream os;
            os << "<" << name << " index=\"" << indexOpt.value() << "\" >";
            m_archive.put( os.str().c_str() );
        }
        else
        {
            std::ostringstream os;
            os << "<" << name << ">";
            m_archive.put( os.str().c_str() );
        }
    }
    inline void endData( const char* name, bool bIsObject, const reference& ref )
    {
        auto indexOpt = m_table.refToIndexIfExist( ref );
        VERIFY_RTE( indexOpt.has_value() );

        std::ostringstream os;
        os << "</" << name << " >";
        m_archive.put( os.str().c_str() );
    }

private:
    std::unique_ptr< std::ostream > m_pFileStream;
    boost::archive::xml_oarchive    m_archive;
    AddressTable                    m_table;
};

class XMLLoadArchive
{
    using IndexRefMap = std::unordered_map< AddressTable::Index, reference >;

public:
    inline XMLLoadArchive( const boost::filesystem::path& filePath )
        : m_pFileStream( boost::filesystem::loadFileStream( filePath ) )
        , m_rootTag( mega::parse( *m_pFileStream ) )
    {
        m_pFileStream->seekg( 0 );
        m_pArchive = std::make_unique< boost::archive::xml_iarchive >( *m_pFileStream, boostXMLArchiveFlags );
    }

    template < typename T >
    inline void load( const char* name, T& value )
    {
        *m_pArchive& boost::serialization::make_nvp( name, value );
    }

    inline void load( const char* name, mega::reference& ref )
    {
        std::string        strEncoding;
        *m_pArchive&       boost::serialization::make_nvp( name, strEncoding );
        std::istringstream is( strEncoding );

        if( strEncoding.find( '.' ) == std::string::npos )
        {
            AddressTable::Index index;
            is >> index;
            auto iFind = m_indexToRefMap.find( index );
            VERIFY_RTE_MSG( iFind != m_indexToRefMap.end(),
                            "Failed to locate reference index in archive for reference: " << strEncoding );
            ref = iFind->second;
        }
        else
        {
            using ::operator>>;
            is >> ref;
        }
    }

    inline void load( mega::reference& ref )
    {
        //
        this->load( "ref", ref );
    }

    inline void beginStructure( const char* name, bool bIsObject, const reference& ref )
    {
        if( !m_tagStack.empty() )
        {
            mega::XMLTag::Vector::const_iterator& i = m_tagStack.back();

            while( name != i->key ) ++i;

            // VERIFY_RTE_MSG( name == i->key, "Expected name: " << name << " but have: " << i->key );
            m_tagStack.push_back( i->children.begin() );
            m_tagCountStack.push_back( i->children.size() );
            ++i;
        }
        else
        {
            VERIFY_RTE_MSG( name == m_rootTag.key, "Expected name: " << name << " but have: " << m_rootTag.key );
            m_tagStack.push_back( m_rootTag.children.begin() );
            m_tagCountStack.push_back( m_rootTag.children.size() );
        }
    }

    inline void endStructure( const char* name, bool bIsObject, const reference& ref )
    {
        ASSERT( !m_tagStack.empty() );
        m_tagStack.pop_back();
        m_tagCountStack.pop_back();
    }

    inline void beginData( const char* name, bool bIsObject, const reference& ref )
    {
        //
        mega::consumeStart( *m_pFileStream, name );
    }

    inline void endData( const char* name, bool bIsObject, const reference& ref )
    {
        //
        mega::consumeEnd( *m_pFileStream, name );
    }

    inline bool is_tag( const char* name ) { return getCurrentTag().key == name; }

    inline void allocation( const reference& ref )
    {
        const XMLTag& tag = getCurrentTag();
        ASSERT( tag.indexOpt.has_value() );
        m_indexToRefMap.insert( { tag.indexOpt.value(), ref } );
    }

    inline U64 tag_count() 
    { 
        ASSERT( !m_tagCountStack.empty() );
        return m_tagCountStack.back();
    }

private:
    inline const XMLTag& getCurrentTag() const
    {
        if( !m_tagStack.empty() )
        {
            return *m_tagStack.back();
        }
        else
        {
            return m_rootTag;
        }
    }

    std::unique_ptr< std::istream >                 m_pFileStream;
    std::unique_ptr< boost::archive::xml_iarchive > m_pArchive;
    mega::XMLTag                                    m_rootTag;
    mega::XMLTag::Stack                             m_tagStack;
    std::vector< U64 >                              m_tagCountStack;
    IndexRefMap                                     m_indexToRefMap;
};

} // namespace mega

#endif // GUARD_2022_October_31_archive
