
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

#include "mega/values/compilation/type_id.hpp"

#include "mega/values/runtime/reference.hpp"
#include "mega/values/runtime/reference_io.hpp"
#include "mega/values/runtime/maths_types.hpp"

#include "mega/address_table.hpp"
#include "mega/tag_parser.hpp"

#include "common/file.hpp"
#include "common/string.hpp"

#include <boost/dynamic_bitset.hpp>

#include <list>
#include <string>
#include <sstream>
#include <unordered_map>

namespace mega
{

class XMLSaveArchive
{
public:
    inline XMLSaveArchive( const boost::filesystem::path& filePath )
        : m_pFileStream( boost::filesystem::createNewFileStream( filePath ) )
    {
    }

    inline void begin( const char* name )
    {
        if( name )
        {
            *m_pFileStream << "<" << name << ">";
        }
    }
    inline void end( const char* name )
    {
        if( name )
        {
            *m_pFileStream << "</" << name << ">";
        }
    }

    inline void save( const char* name, const reference& ref )
    {
        begin( name );
        std::string strEncode;
        {
            std::ostringstream os;
            if( auto indexOpt = refToIndexIfExist( ref ); indexOpt.has_value() )
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
        *m_pFileStream << strEncode;
        end( name );
    }

    template < typename T >
    inline void save( const char* name, const std::vector< T >& value )
    {
        begin( name );
        for( const auto& v : value )
        {
            *m_pFileStream << "<element>";
            save( nullptr, v );
            *m_pFileStream << "</element>";
        }
        end( name );
    }

    inline void save( const char* name, const ::F2& v )
    {
        begin( name );
        common::delimit( v.data.begin(), v.data.end(), ",", *m_pFileStream );
        end( name );
    }

    inline void save( const char* name, const ::F3& v )
    {
        begin( name );
        common::delimit( v.data.begin(), v.data.end(), ",", *m_pFileStream );
        end( name );
    }

    inline void save( const char* name, const ::F4& v )
    {
        begin( name );
        common::delimit( v.data.begin(), v.data.end(), ",", *m_pFileStream );
        end( name );
    }

    inline void save( const char* name, const ::Quat& v )
    {
        begin( name );
        common::delimit( v.data.begin(), v.data.end(), ",", *m_pFileStream );
        end( name );
    }

    inline void save( const char* name, const ::F33& v )
    {
        begin( name );
        for( const auto& row : v.data )
        {
            common::delimit( row.begin(), row.end(), ",", *m_pFileStream );
        }
        end( name );
    }

    template < typename T >
    inline void save( const char* name, const T& value )
    {
        begin( name );
        *m_pFileStream << value;
        end( name );
    }

    inline void beginStructure( const reference& ref )
    {
        //
        m_table.refToIndex( ref );
    }
    inline void endStructure( const reference& ref ) {}

    inline void beginData( const char* name, bool bIsObject, const reference& ref )
    {
        auto indexOpt = m_table.refToIndexIfObjectExist( ref );
        VERIFY_RTE( indexOpt.has_value() );
        *m_pFileStream << "<" << name << " index=\"" << indexOpt.value() << "\" >";
    }
    inline void endData( const char* name, bool bIsObject, const reference& ref )
    {
        auto indexOpt = m_table.refToIndexIfObjectExist( ref );
        VERIFY_RTE( indexOpt.has_value() );
        end( name );
    }

private:
    inline std::optional< mega::AddressTable::Index > refToIndexIfExist( const mega::reference& maybeNetAddress )
    {
        return m_table.refToIndexIfObjectExist( maybeNetAddress );
    }
    inline const mega::AddressTable::Index& refToIndex( const mega::reference& maybeNetAddress )
    {
        return m_table.refToIndex( maybeNetAddress );
    }

    mega::AddressTable              m_table;
    std::unique_ptr< std::ostream > m_pFileStream;
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
    {
    }

    template < typename T >
    inline void load( const char* name, std::vector< T >& value )
    {
        mega::consumeStart( *m_pFileStream, name );
        /*for( const auto& v : value )
        {
            mega::consumeStart( *m_pFileStream, "element" );
            load( nullptr, v );
            mega::consumeEnd( *m_pFileStream, "element" );
        }*/
        mega::consumeEnd( *m_pFileStream, name );
    }

    inline void load( const char* name, ::F2& v )
    {
        mega::consumeStart( *m_pFileStream, name );
        // common::delimit( v.data.begin(), v.data.end(), ",", *m_pFileStream );
        mega::consumeEnd( *m_pFileStream, name );
    }

    inline void load( const char* name, ::F3& v )
    {
        mega::consumeStart( *m_pFileStream, name );
        // common::delimit( v.data.begin(), v.data.end(), ",", *m_pFileStream );
        mega::consumeEnd( *m_pFileStream, name );
    }

    inline void load( const char* name, ::F4& v )
    {
        mega::consumeStart( *m_pFileStream, name );
        // common::delimit( v.data.begin(), v.data.end(), ",", *m_pFileStream );
        mega::consumeEnd( *m_pFileStream, name );
    }

    inline void load( const char* name, ::Quat& v )
    {
        mega::consumeStart( *m_pFileStream, name );
        // common::delimit( v.data.begin(), v.data.end(), ",", *m_pFileStream );
        mega::consumeEnd( *m_pFileStream, name );
    }

    inline void load( const char* name, ::F33& v )
    {
        mega::consumeStart( *m_pFileStream, name );
        for( const auto& row : v.data )
        {
            // common::delimit( row.begin(), row.end(), ",", *m_pFileStream );
        }
        mega::consumeEnd( *m_pFileStream, name );
    }

    inline void load( const char* name, mega::reference& ref )
    {
        mega::consumeStart( *m_pFileStream, name );

        std::string strEncoding;
        load( name, strEncoding );
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

        mega::consumeEnd( *m_pFileStream, name );
    }
    template < typename T >
    inline void load( const char* name, T& value )
    {
        mega::consumeStart( *m_pFileStream, name );
        *m_pFileStream >> value;
        mega::consumeEnd( *m_pFileStream, name );
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
            if( frame.iter->indexOpt.has_value() )
            {
                mapIndex( frame.iter->indexOpt.value(), ref );
            }

            m_stack.push_back(
                Frame{ frame.iter->children.begin(), frame.iter->children.end(), frame.iter->children.size() } );
            ++frame.iter;
        }
        else
        {
            VERIFY_RTE_MSG( name == m_rootTag.key, "Expected name: " << name << " but have: " << m_rootTag.key );
            if( m_rootTag.indexOpt.has_value() )
            {
                mapIndex( m_rootTag.indexOpt.value(), ref );
            }

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
        mapIndex( tag.indexOpt.value(), ref );
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

    inline void mapIndex( mega::AddressTable::Index index, const mega::reference& ref )
    {
        m_indexToRefMap.insert( { index, ref } );
    }

private:
    using IndexRefMap = std::unordered_map< mega::AddressTable::Index, mega::reference >;

    std::unique_ptr< std::istream > m_pFileStream;
    mega::XMLTag                    m_rootTag;
    IndexRefMap                     m_indexToRefMap;
    Frame::Stack                    m_stack;
};

} // namespace mega

#endif // GUARD_2022_October_31_archive
