
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
#include "mega/type_id_io.hpp"
#include "mega/reference_io.hpp"

#include "common/file.hpp"

#include "mega/boost_serialization_xml_workaround.hpp"

#include <list>
#include <string>
#include <sstream>
#include <unordered_map>

namespace boost::archive
{
static constexpr auto boostXMLArchiveFlags = boost::archive::no_header | boost::archive::no_codecvt
                                             | boost::archive::no_xml_tag_checking | boost::archive::no_tracking;

class XMLIArchive : public xml_iarchive_impl< XMLIArchive >
{
    using base = boost::archive::xml_iarchive_impl< XMLIArchive >;

    friend class detail::common_iarchive< XMLIArchive >;
    friend class basic_xml_iarchive< XMLIArchive >;
    friend class boost::archive::load_access;

    using IndexRefMap = std::unordered_map< mega::AddressTable::Index, mega::reference >;

public:
    XMLIArchive()                     = delete;
    XMLIArchive( const XMLIArchive& ) = delete;
    inline XMLIArchive( std::istream& bsb )
        : base( bsb, boostXMLArchiveFlags )
    {
    }

    template < typename T >
    inline void load( T& value )
    {
        base::load( value );
    }

    inline void load( mega::reference& ref )
    {
        std::string strEncoding;
        base::load( strEncoding );
        std::istringstream is( strEncoding );

        if( strEncoding.find( '.' ) == std::string::npos )
        {
            mega::AddressTable::Index index;
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

    void mapIndex( mega::AddressTable::Index index, const mega::reference& ref )
    {
        m_indexToRefMap.insert( { index, ref } );
    }

private:
    IndexRefMap m_indexToRefMap;
};

class XMLOArchive : public xml_oarchive_impl< XMLOArchive >
{
    using base = boost::archive::xml_oarchive_impl< XMLOArchive >;

    friend class detail::common_oarchive< XMLOArchive >;
    friend class basic_xml_oarchive< XMLOArchive >;
    friend class boost::archive::save_access;

public:
    XMLOArchive()                     = delete;
    XMLOArchive( const XMLOArchive& ) = delete;
    inline XMLOArchive( std::ostream& bsb )
        : base( bsb, boostXMLArchiveFlags )
    {
    }

    // Anything not an attribute and not a name-value pair is an
    // error and should be trapped here.
    template < class T >
    void save_override( T& t )
    {
        // If your program fails to compile here, its most likely due to
        // not specifying an nvp wrapper around the variable to
        // be serialized.
        BOOST_MPL_ASSERT( ( serialization::is_wrapper< T > ));
        this->detail_common_oarchive::save_override( t );
    }

    // special treatment for name-value pairs.
    template < class T >
    void save_override( const ::boost::serialization::nvp< T >& t )
    {
        this->This()->save_start( t.name() );
        this->detail_common_oarchive::save_override( t.const_value() );
        this->This()->save_end( t.name() );
    }

    // specific overrides for attributes - not name value pairs so we
    // want to trap them before the above "fall through"
    void save_override( const class_id_type& t ) {}
    void save_override( const class_id_optional_type& t ) {}
    void save_override( const class_id_reference_type& t ) {}
    void save_override( const object_id_type& t ) {}
    void save_override( const object_reference_type& t ) {}
    void save_override( const version_type& t ) {}
    void save_override( const class_name_type& t ) {}
    void save_override( const tracking_type& t ) {}

    template < typename T >
    inline void save( const T& value )
    {
        base::save( value );
    }

    inline void save( const float& number )
    {
        std::string strEncode;
        {
            std::ostringstream os;
            os << std::defaultfloat << number;
            strEncode = os.str();
        }
        base::save( strEncode );
    }
    inline void save( const double& number )
    {
        std::string strEncode;
        {
            std::ostringstream os;
            os << std::defaultfloat << number;
            strEncode = os.str();
        }
        base::save( strEncode );
    }

    inline void save( mega::reference& ref )
    {
        this->end_preamble();

        std::string strEncode;
        {
            std::ostringstream os;
            if( auto indexOpt = refToIndexIfExist( ref ) )
            {
                os << indexOpt.value();
            }
            else
            {
                using ::operator<<;
                os << ref;
            }
            strEncode = os.str();
        }
        base::save( strEncode );
    }

    inline std::optional< mega::AddressTable::Index > refToIndexIfExist( const mega::reference& maybeNetAddress ) const
    {
        return m_table.refToIndexIfExist( maybeNetAddress );
    }
    inline const mega::AddressTable::Index& refToIndex( const mega::reference& maybeNetAddress )
    {
        return m_table.refToIndex( maybeNetAddress );
    }

private:
    mega::AddressTable m_table;
};

} // namespace boost::archive

namespace boost::serialization
{
inline void serialize( boost::archive::XMLIArchive& ar, mega::reference& value, const unsigned int )
{
    ar.load( value );
}

inline void serialize( boost::archive::XMLOArchive& ar, mega::reference& value, const unsigned int )
{
    ar.save( value );
}
} // namespace boost::serialization

namespace mega
{

class XMLSaveArchive
{
public:
    inline XMLSaveArchive( const boost::filesystem::path& filePath )
        : m_pFileStream( boost::filesystem::createNewFileStream( filePath ) )
        , m_archive( *m_pFileStream )
    {
    }

    template < typename T >
    inline void save( const char* name, T& value )
    {
        m_archive& boost::serialization::make_nvp( name, value );
    }

    inline void beginStructure( const reference& ref )
    {
        //
        m_archive.refToIndex( ref );
    }
    inline void endStructure( const reference& ref ) {}

    inline void beginData( const char* name, bool bIsObject, const reference& ref )
    {
        auto indexOpt = m_archive.refToIndexIfExist( ref );
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
        auto indexOpt = m_archive.refToIndexIfExist( ref );
        VERIFY_RTE( indexOpt.has_value() );

        std::ostringstream os;
        os << "</" << name << " >";
        m_archive.put( os.str().c_str() );
    }

private:
    std::unique_ptr< std::ostream > m_pFileStream;
    boost::archive::XMLOArchive     m_archive;
};

class XMLLoadArchive
{
    struct Frame
    {
        mega::XMLTag::Vector::const_iterator iter, iterEnd;
        U64                                  size;
        using Stack = std::list< Frame >;
    };

    static inline mega::XMLTag parseTagsAndResetStream( std::istream& is )
    {
        auto result = mega::parse( is );
        is.seekg( 0 );
        return result;
    }

public:
    inline XMLLoadArchive( const boost::filesystem::path& filePath )
        : m_pFileStream( boost::filesystem::loadFileStream( filePath ) )
        , m_rootTag( parseTagsAndResetStream( *m_pFileStream ) )
        , m_archive( *m_pFileStream )
    {
    }

    template < typename T >
    inline void load( const char* name, T& value )
    {
        m_archive& boost::serialization::make_nvp( name, value );
    }

    inline void beginStructure( const char* name, bool bIsObject, const reference& ref )
    {
        if( !m_stack.empty() )
        {
            Frame& frame = m_stack.back();

            while( ( frame.iter != frame.iterEnd ) && ( name != frame.iter->key ) )
            {
                ++frame.iter;
            }
            VERIFY_RTE_MSG( name == frame.iter->key, "Expected name: " << name << " but have: " << frame.iter->key );
            m_stack.push_back(
                Frame{ frame.iter->children.begin(), frame.iter->children.end(), frame.iter->children.size() } );
            ++frame.iter;
        }
        else
        {
            VERIFY_RTE_MSG( name == m_rootTag.key, "Expected name: " << name << " but have: " << m_rootTag.key );
            m_stack.push_back(
                Frame{ m_rootTag.children.begin(), m_rootTag.children.end(), m_rootTag.children.size() } );
        }
    }

    inline void endStructure( const char* name, bool bIsObject, const reference& ref )
    {
        ASSERT( !m_stack.empty() );
        m_stack.pop_back();
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
        m_archive.mapIndex( tag.indexOpt.value(), ref );
    }

    inline U64 tag_count()
    {
        ASSERT( !m_stack.empty() );
        return m_stack.back().size;
    }

private:
    inline const XMLTag& getCurrentTag() const
    {
        if( !m_stack.empty() )
        {
            return *m_stack.back().iter;
        }
        else
        {
            return m_rootTag;
        }
    }

    std::unique_ptr< std::istream > m_pFileStream;
    mega::XMLTag                    m_rootTag;
    boost::archive::XMLIArchive     m_archive;
    Frame::Stack                    m_stack;
};

} // namespace mega

#endif // GUARD_2022_October_31_archive
