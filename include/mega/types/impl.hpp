
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

#ifndef GUARD_2023_January_12_impl
#define GUARD_2023_January_12_impl

#include "traits.hpp"
#include "rtti.hxx"

#include "mega/bin_archive.hpp"
#include "mega/xml_archive.hpp"
#include "mega/record_archive.hpp"
#include "mega/any.hpp"
#include "mega/logical_address_space.hpp"

#include "mega/maths_types_io.hpp"
#include "mega/native_types_io.hpp"
#include "mega/allocator_io.hpp"

#include "service/protocol/common/context.hpp"

#include "log/records.hxx"
#include "log/file_log.hpp"

#include <boost/interprocess/streams/bufferstream.hpp>

namespace mega
{

template < typename T >
struct SimpleDimension
{
    using Traits = DimensionTraits< T >;

    static inline void construct( void* pAddress )
    {
        //
        new( pAddress ) T{};
    }
    static inline void destruct( T& object )
    {
        //
        object.~T();
    }

    static inline void save_xml( mega::XMLSaveArchive& archive, const char* pszName, const T& value )
    {
        //
        archive.save( pszName, value );
    }
    static inline void load_xml( mega::XMLLoadArchive& archive, const char* pszName, T& value )
    {
        //
        archive.load( pszName, value );
    }
    static inline void save_bin( mega::BinSaveArchive& archive, const T& value )
    {
        //
        archive.save( value );
    }
    static inline void load_bin( mega::BinLoadArchive& archive, T& value )
    {
        //
        archive.load( value );
    }

    static inline void copy( const T& from, T& to ) { to = from; }

    static inline void save_memory_record( const mega::reference& ref, const T& value )
    {
        VERIFY_RTE_MSG( ref.isHeapAddress(), "save_memory_record passed network address " );
        mega::log::FileStorage& log = mega::Context::get()->getLog();
        log.record( mega::log::Memory::Write{
            ref, std::string_view( reinterpret_cast< const char* >( &value ), sizeof( T ) ) } );
    }
    static inline void load_memory_record( const void* pData, mega::U64 size, T& value )
    {
        ASSERT( size == sizeof( value ) );
        copy( *reinterpret_cast< const T* >( pData ), value );
    }

    static inline Any read_any( const T& from )
    {
        return { reinterpret_cast< const void* >( &from ), rtti< T >::ID };
    }

    static inline void write_any( const T& from, Any value )
    {
    }

    static inline void print( std::ostream& os, const T& value )
    {
        using ::operator<<;
        os << value;
    }
};

template < typename T >
struct NonSimpleDimension
{
    using Traits = DimensionTraits< T >;

    static inline void construct( void* pAddress )
    {
        //
        new( pAddress ) T{};
    }
    static inline void destruct( T& object )
    {
        //
        object.~T();
    }

    static inline void save_xml( mega::XMLSaveArchive& archive, const char* pszName, const T& value )
    {
        archive.save( pszName, value );
    }
    static inline void load_xml( mega::XMLLoadArchive& archive, const char* pszName, T& value )
    {
        //
        archive.load( pszName, value );
    }
    static inline void save_bin( mega::BinSaveArchive& archive, const T& value )
    {
        //
        archive.save( value );
    }
    static inline void load_bin( mega::BinLoadArchive& archive, T& value )
    {
        //
        archive.load( value );
    }

    static inline void copy( const T& from, T& to )
    {
        //
        to = from;
    }

    static inline void save_memory_record( const mega::reference& ref, const T& value )
    {
        VERIFY_RTE_MSG( ref.isHeapAddress(), "save_memory_record passed network address " );
        RecordSaveArchive recordSaveArchive;
        recordSaveArchive.save( value );

        mega::log::FileStorage& log = mega::Context::get()->getLog();
        log.record( mega::log::Memory::Write{ ref, recordSaveArchive.get() } );
    }
    static inline void load_memory_record( const void* pData, mega::U64 size, T& value )
    {
        RecordLoadArchive recordLoadArchive( { reinterpret_cast< const char* >( pData ), size } );
        recordLoadArchive.load( value );
    }

    static inline Any read_any( const T& from )
    {
        return { reinterpret_cast< const void* >( &from ), rtti< T >::ID };
    }

    static inline void write_any( const T& from, Any value )
    {
    }

    static inline void print( std::ostream& os, const T& value )
    {
        using ::operator<<;
        os << value;
    }
};
} // namespace mega

#endif // GUARD_2023_January_12_impl
