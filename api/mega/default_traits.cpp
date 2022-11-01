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

#include "mega/default_traits.hpp"
#include "mega/allocator.hpp"
#include "mega/archive.hpp"

#include "service/mpo_context.hpp"

#include "service/network/log.hpp"

#include "log/log.hpp"

#include <vector>

namespace
{

template < typename T >
void new_( void* p )
{
    new ( p ) T{};
}

template < typename T >
void delete_( void* p )
{
    T* pT = reinterpret_cast< T* >( p );
    pT->~T();
}

template < typename T >
void save_( void* p, const char* name, void* pArchive )
{
    T*                 pData = reinterpret_cast< T* >( p );
    mega::SaveArchive& ar    = *reinterpret_cast< mega::SaveArchive* >( pArchive );
    ar.save( *pData, name );
}

template < typename T >
void load_( void* p, const char* name, void* pArchive )
{
    T*                 pData = reinterpret_cast< T* >( p );
    mega::LoadArchive& ar    = *reinterpret_cast< mega::LoadArchive* >( pArchive );
    ar.load( *pData, name );
}

template < typename T >
void copy_( const void* pFrom, void* pTo )
{
    const T* pFromT = reinterpret_cast< const T* >( pFrom );
    T*       pToT   = reinterpret_cast< T* >( pTo );
    *pToT           = *pFromT;
}

template < typename T >
void event_( const mega::reference& ref, bool bShared, const void* p )
{
    mega::log::Storage& log = mega::getMPOContext()->getLog();

    log.record( mega::log::MemoryTrackType::Simulation,
                mega::log::MemoryRecord(
                    ref, bShared, std::string_view( reinterpret_cast< const char* >( p ), sizeof( T ) ) ) );
}

/*
template < typename T >
void event_nonSimple( const mega::reference& ref, bool bShared, const T& value )
{
    using Buffer = std::vector< char >;
    boost::interprocess::basic_vectorbuf< Buffer > os;
    {
        boost::archive::binary_oarchive oa( os );
        oa&                             value;
    }
}

template < typename T >
void read_nonSimple( std::vector< char >& buffer )
{
    using Buffer = std::vector< char >;
    T                                              data;
    boost::interprocess::basic_vectorbuf< Buffer > is( buffer );
    boost::archive::binary_iarchive                ia( is );
    ia&                                            data;
}
*/

} // namespace

namespace mega
{

void save_begin_part( const char* partName, void* pArchive )
{
    mega::SaveArchive& ar = *reinterpret_cast< mega::SaveArchive* >( pArchive );
    ar.beginPart( partName );
}
void save_end_part( const char* partName, void* pArchive )
{
    mega::SaveArchive& ar = *reinterpret_cast< mega::SaveArchive* >( pArchive );
    ar.endPart( partName );
}
void load_begin_part( const char* partName, void* pArchive )
{
    mega::LoadArchive& ar = *reinterpret_cast< mega::LoadArchive* >( pArchive );
    ar.beginPart( partName );
}
void load_end_part( const char* partName, void* pArchive )
{
    mega::LoadArchive& ar = *reinterpret_cast< mega::LoadArchive* >( pArchive );
    ar.endPart( partName );
}

#define SIMPLETYPE( manged_name, type )                                               \
    void new_##manged_name( void* p, void* pMemory )                                  \
    {                                                                                 \
        new_< type >( p );                                                            \
    }                                                                                 \
    void delete_##manged_name( void* p )                                              \
    {                                                                                 \
        delete_< type >( p );                                                         \
    }                                                                                 \
    void save_##manged_name( void* p, const char* name, void* pArchive )              \
    {                                                                                 \
        save_< type >( p, name, pArchive );                                           \
    }                                                                                 \
    void load_##manged_name( void* p, const char* name, void* pArchive )              \
    {                                                                                 \
        load_< type >( p, name, pArchive );                                           \
    }                                                                                 \
    void copy_##manged_name( const void* pFrom, void* pTo )                           \
    {                                                                                 \
        copy_< type >( pFrom, pTo );                                                  \
    }                                                                                 \
    void event_##manged_name( const reference& ref, bool bShared, const void* pData ) \
    {                                                                                 \
        event_< type >( ref, bShared, pData );                                        \
    }

#include "default_traits.hxx"
#undef SIMPLETYPE

#define ALLOCATOR( manged_name, type )                                                \
    void new_##manged_name( void* p, void* pMemory )                                  \
    {                                                                                 \
        new_< type >( p );                                                            \
    }                                                                                 \
    void delete_##manged_name( void* p )                                              \
    {                                                                                 \
        delete_< type >( p );                                                         \
    }                                                                                 \
    void save_##manged_name( void* p, const char* name, void* pArchive )              \
    {                                                                                 \
        save_< type >( p, name, pArchive );                                           \
    }                                                                                 \
    void load_##manged_name( void* p, const char* name, void* pArchive )              \
    {                                                                                 \
        load_< type >( p, name, pArchive );                                           \
    }                                                                                 \
    void copy_##manged_name( const void* pFrom, void* pTo )                           \
    {                                                                                 \
        copy_< type >( pFrom, pTo );                                                  \
    }                                                                                 \
    void event_##manged_name( const reference& ref, bool bShared, const void* pData ) \
    {                                                                                 \
        event_< type >( ref, bShared, pData );                                        \
    }

#include "allocator_traits.hxx"
#undef ALLOCATOR

// std::vector< int >
void new_classstd00vector3int4( void* p, void* pMemory ) { new_< std::vector< int > >( p ); }
void delete_classstd00vector3int4( void* p ) { delete_< std::vector< int > >( p ); }
void save_classstd00vector3int4( void* p, const char* name, void* pArchive )
{
    save_< std::vector< int > >( p, name, pArchive );
}
void load_classstd00vector3int4( void* p, const char* name, void* pArchive )
{
    load_< std::vector< int > >( p, name, pArchive );
}
void copy_classstd00vector3int4( const void* pFrom, void* pTo ) { copy_< std::vector< int > >( pFrom, pTo ); }

// mega::ReferenceVector
void new_mega00ReferenceVector( void* p, void* pMemory )
{
    // SPDLOG_TRACE( "new_mega00ReferenceVector - start" );
    typename mega::runtime::ManagedSharedMemory::segment_manager* pSegmentMgr
        = reinterpret_cast< typename mega::runtime::ManagedSharedMemory::segment_manager* >( pMemory );
    using Allocator
        = boost::interprocess::allocator< mega::reference, mega::runtime::ManagedSharedMemory::segment_manager >;
    new ( p ) ReferenceVector( Allocator( pSegmentMgr ) );
    // SPDLOG_TRACE( "new_mega00ReferenceVector - done" );
}
void delete_mega00ReferenceVector( void* p ) { delete_< mega::ReferenceVector >( p ); }
void save_mega00ReferenceVector( void* p, const char* name, void* pArchive )
{
    // save_< mega::ReferenceVector >( p, name, pArchive );
}
void load_mega00ReferenceVector( void* p, const char* name, void* pArchive )
{
    // load_< mega::ReferenceVector >( p, name, pArchive );
}
void copy_mega00ReferenceVector( const void* pFrom, void* pTo ) { copy_< mega::ReferenceVector >( pFrom, pTo ); }

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

#define ALLOCATOR( manged_name, type )                               \
    Instance allocate_##manged_name( void* p )                       \
    {                                                                \
        return allocate_< type >( *reinterpret_cast< type* >( p ) ); \
    }                                                                \
    void free_##manged_name( void* p, Instance instance )            \
    {                                                                \
        free_< type >( *reinterpret_cast< type* >( p ), instance );  \
    }

#include "allocator_traits.hxx"
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

} // namespace mega
