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

#include "mega/types/traits.hpp"

#include "mega/allocator.hpp"

#include <vector>

namespace mega
{
/*
template < typename T >
void new_( void* p )
{
    new( p ) T{};
}

template < typename T >
void delete_( void* p )
{
    T* pT = reinterpret_cast< T* >( p );
    pT->~T();
}

template < typename T >
void save_xml_( void* p, const char* name, void* pArchive )
{
    T*                    pData = reinterpret_cast< T* >( p );
    mega::XMLSaveArchive& ar    = *reinterpret_cast< mega::XMLSaveArchive* >( pArchive );
    ar.save( *pData, name );
}

template < typename T >
void load_xml_( void* p, const char* name, void* pArchive )
{
    T*                    pData = reinterpret_cast< T* >( p );
    mega::XMLLoadArchive& ar    = *reinterpret_cast< mega::XMLLoadArchive* >( pArchive );
    ar.load( *pData, name );
}

template < typename T >
void save_bin_( void* p, void* pArchive )
{
    // SPDLOG_TRACE( "save_bin_ {} {}", typeid( T ).name(), p );

    T*                    pData = reinterpret_cast< T* >( p );
    mega::BinSaveArchive& ar    = *reinterpret_cast< mega::BinSaveArchive* >( pArchive );
    ar.save( *pData );
}

template < typename T >
void load_bin_( void* p, void* pArchive )
{
    T*                    pData = reinterpret_cast< T* >( p );
    mega::BinLoadArchive& ar    = *reinterpret_cast< mega::BinLoadArchive* >( pArchive );
    ar.load( *pData );
}

template < typename T >
void copy_( const void* pFrom, void* pTo )
{
    const T* pFromT = reinterpret_cast< const T* >( pFrom );
    T*       pToT   = reinterpret_cast< T* >( pTo );
    *pToT           = *pFromT;
}

template < typename T >
void event_( const mega::reference& ref, const void* p )
{
    mega::log::Storage& log = mega::getMPOContext()->getLog();
    log.record( mega::log::MemoryTrackType::Simulation,
                mega::log::MemoryRecord( ref, std::string_view( reinterpret_cast< const char* >( p ), sizeof( T ) ) ) );
}

template < typename T >
void event_nonSimple_( const mega::reference& ref, const void* p )
{
    const T& value = *reinterpret_cast< const T* >( p );
    using Buffer   = std::vector< char >;
    boost::interprocess::basic_vectorbuf< Buffer > os;
    {
        boost::archive::binary_oarchive oa( os );
        oa&                             value;
    }
}*/

/*
template < typename T >
void read_nonSimple_( std::vector< char >& buffer )
{
    using Buffer = std::vector< char >;
    T                                              data;
    boost::interprocess::basic_vectorbuf< Buffer > is( buffer );
    boost::archive::binary_iarchive                ia( is );
    ia&                                            data;
}
*/


/*
void save_begin_object( const reference& ref, void* pArchive )
{
    SPDLOG_TRACE( "save_begin_object {}", ref );
    mega::BinSaveArchive& ar = *reinterpret_cast< mega::BinSaveArchive* >( pArchive );
    ar.beginObject( ref );
}*/
/*
void xml_save_begin_part( const char* partName, void* pArchive )
{
    mega::XMLSaveArchive& ar = *reinterpret_cast< mega::XMLSaveArchive* >( pArchive );
    ar.beginPart( partName );
}
void xml_save_end_part( const char* partName, void* pArchive )
{
    mega::XMLSaveArchive& ar = *reinterpret_cast< mega::XMLSaveArchive* >( pArchive );
    ar.endPart( partName );
}
void xml_load_begin_part( const char* partName, void* pArchive )
{
    mega::XMLLoadArchive& ar = *reinterpret_cast< mega::XMLLoadArchive* >( pArchive );
    ar.beginPart( partName );
}
void xml_load_end_part( const char* partName, void* pArchive )
{
    mega::XMLLoadArchive& ar = *reinterpret_cast< mega::XMLLoadArchive* >( pArchive );
    ar.endPart( partName );
}*/
/*
#define SIMPLETYPE( manged_name, type )                                      \
    void new_##manged_name( void* p )                                        \
    {                                                                        \
        new_< type >( p );                                                   \
        DimensionImpl< type >::construct( p );
    }                                                                        \
    void delete_##manged_name( void* p )                                     \
    {                                                                        \
        delete_< type >( p );                                                \
    }                                                                        \
    void save_xml_##manged_name( void* p, const char* name, void* pArchive ) \
    {                                                                        \
        save_xml_< type >( p, name, pArchive );                              \
    }                                                                        \
    void load_xml_##manged_name( void* p, const char* name, void* pArchive ) \
    {                                                                        \
        load_xml_< type >( p, name, pArchive );                              \
    }                                                                        \
    void save_bin_##manged_name( void* p, void* pArchive )                   \
    {                                                                        \
        save_bin_< type >( p, pArchive );                                    \
    }                                                                        \
    void load_bin_##manged_name( void* p, void* pArchive )                   \
    {                                                                        \
        load_bin_< type >( p, pArchive );                                    \
    }                                                                        \
    void copy_##manged_name( const void* pFrom, void* pTo )                  \
    {                                                                        \
        copy_< type >( pFrom, pTo );                                         \
    }                                                                        \
    void event_##manged_name( const reference& ref, const void* pData )      \
    {                                                                        \
        event_< type >( ref, pData );                                        \
    }

#include "mega/types/simple.xmc"
#undef SIMPLETYPE

#define NONSIMPLETYPE( manged_name, type )                                   \
    void new_##manged_name( void* p )                                        \
    {                                                                        \
        new_< type >( p );                                                   \
    }                                                                        \
    void delete_##manged_name( void* p )                                     \
    {                                                                        \
        delete_< type >( p );                                                \
    }                                                                        \
    void save_xml_##manged_name( void* p, const char* name, void* pArchive ) \
    {                                                                        \
        save_xml_< type >( p, name, pArchive );                              \
    }                                                                        \
    void load_xml_##manged_name( void* p, const char* name, void* pArchive ) \
    {                                                                        \
        load_xml_< type >( p, name, pArchive );                              \
    }                                                                        \
    void save_bin_##manged_name( void* p, void* pArchive )                   \
    {                                                                        \
        save_bin_< type >( p, pArchive );                                    \
    }                                                                        \
    void load_bin_##manged_name( void* p, void* pArchive )                   \
    {                                                                        \
        load_bin_< type >( p, pArchive );                                    \
    }                                                                        \
    void copy_##manged_name( const void* pFrom, void* pTo )                  \
    {                                                                        \
        copy_< type >( pFrom, pTo );                                         \
    }                                                                        \
    void event_##manged_name( const reference& ref, const void* pData )      \
    {                                                                        \
        event_nonSimple_< type >( ref, pData );                              \
    }

#include "mega/types/non_simple.xmc"
#undef NONSIMPLETYPE

#define ALLOCATOR( manged_name, type, size )                                 \
    void new_##manged_name( void* p )                                        \
    {                                                                        \
        new_< type >( p );                                                   \
    }                                                                        \
    void delete_##manged_name( void* p )                                     \
    {                                                                        \
        delete_< type >( p );                                                \
    }                                                                        \
    void save_xml_##manged_name( void* p, const char* name, void* pArchive ) \
    {                                                                        \
        save_xml_< type >( p, name, pArchive );                              \
    }                                                                        \
    void load_xml_##manged_name( void* p, const char* name, void* pArchive ) \
    {                                                                        \
        load_xml_< type >( p, name, pArchive );                              \
    }                                                                        \
    void save_bin_##manged_name( void* p, void* pArchive )                   \
    {                                                                        \
        save_bin_< type >( p, pArchive );                                    \
    }                                                                        \
    void load_bin_##manged_name( void* p, void* pArchive )                   \
    {                                                                        \
        load_bin_< type >( p, pArchive );                                    \
    }                                                                        \
    void copy_##manged_name( const void* pFrom, void* pTo )                  \
    {                                                                        \
        copy_< type >( pFrom, pTo );                                         \
    }                                                                        \
    void event_##manged_name( const reference& ref, const void* pData )      \
    {                                                                        \
        event_< type >( ref, pData );                                        \
    }

#include "mega/types/allocators_32.xmc"
#include "mega/types/allocators_64.xmc"
#include "mega/types/allocators_128.xmc"
#undef ALLOCATOR

// std::vector< int >
void new_classstd00vector3int4( void* p )
{
    new_< std::vector< int > >( p );
}
void delete_classstd00vector3int4( void* p )
{
    delete_< std::vector< int > >( p );
}
void save_xml_classstd00vector3int4( void* p, const char* name, void* pArchive )
{
    save_xml_< std::vector< int > >( p, name, pArchive );
}
void load_xml_classstd00vector3int4( void* p, const char* name, void* pArchive )
{
    load_xml_< std::vector< int > >( p, name, pArchive );
}
void save_bin_classstd00vector3int4( void* p, void* pArchive )
{
    save_bin_< std::vector< int > >( p, pArchive );
}
void load_bin_classstd00vector3int4( void* p, void* pArchive )
{
    load_bin_< std::vector< int > >( p, pArchive );
}
void copy_classstd00vector3int4( const void* pFrom, void* pTo )
{
    copy_< std::vector< int > >( pFrom, pTo );
}

// mega::ReferenceVector
void new_mega00ReferenceVector( void* p )
{
    new_< ReferenceVector >( p );
}
void delete_mega00ReferenceVector( void* p )
{
    delete_< mega::ReferenceVector >( p );
}
void save_xml_mega00ReferenceVector( void* p, const char* name, void* pArchive )
{
    save_xml_< mega::ReferenceVector >( p, name, pArchive );
}
void load_xml_mega00ReferenceVector( void* p, const char* name, void* pArchive )
{
    load_xml_< mega::ReferenceVector >( p, name, pArchive );
}
void save_bin_mega00ReferenceVector( void* p, void* pArchive )
{
    save_bin_< mega::ReferenceVector >( p, pArchive );
}
void load_bin_mega00ReferenceVector( void* p, void* pArchive )
{
    load_bin_< mega::ReferenceVector >( p, pArchive );
}
void copy_mega00ReferenceVector( const void* pFrom, void* pTo )
{
    copy_< mega::ReferenceVector >( pFrom, pTo );
}

// allocator routines
Instance allocate_bool( void* p )
{
    bool* pBool = reinterpret_cast< bool* >( p );
    *pBool      = true;
    return 0;
}
void free_bool( void* p, Instance )
{
    bool* pBool = reinterpret_cast< bool* >( p );
    *pBool      = false;
}

template < typename TAllocator >
inline Instance allocate_( TAllocator& allocator )
{
    return allocator.allocate();
}

template < typename TAllocator >
inline void free_( TAllocator& allocator, Instance instance )
{
    allocator.free( instance );
}

#define ALLOCATOR( manged_name, type, size )                         \
    Instance allocate_##manged_name( void* p )                       \
    {                                                                \
        return allocate_< type >( *reinterpret_cast< type* >( p ) ); \
    }                                                                \
    void free_##manged_name( void* p, Instance instance )            \
    {                                                                \
        free_< type >( *reinterpret_cast< type* >( p ), instance );  \
    }

#include "mega/types/allocators_32.xmc"
#include "mega/types/allocators_64.xmc"
#include "mega/types/allocators_128.xmc"
#undef ALLOCATOR

void schedule_start( const reference& ref )
{
    mega::log::Storage& log = mega::getMPOContext()->getLog();
    log.record( log::SchedulerRecord( ref, log::SchedulerRecord::Start ) );
}

void schedule_stop( const reference& ref )
{
    mega::log::Storage& log = mega::getMPOContext()->getLog();
    log.record( log::SchedulerRecord( ref, log::SchedulerRecord::Stop ) );
}
*/
} // namespace mega
