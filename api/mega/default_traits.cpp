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

#include "runtime/context.hpp"

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
void store_( void* p, void* pArchive )
{
}

template < typename T >
void load_( void* p, void* pArchive )
{
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
    const T* pT = reinterpret_cast< const T* >( p );
    mega::MPOContext::get()->write( ref, bShared, sizeof( T ), pT );
}

} // namespace

namespace mega
{

#define SIMPLETYPE( manged_name, type )                                               \
    void new_##manged_name( void* p, void* pMemory )                                  \
    {                                                                                 \
        new_< type >( p );                                                            \
    }                                                                                 \
    void delete_##manged_name( void* p )                                              \
    {                                                                                 \
        delete_< type >( p );                                                         \
    }                                                                                 \
    void store_##manged_name( void* p, void* pArchive )                               \
    {                                                                                 \
        store_< type >( p, pArchive );                                                \
    }                                                                                 \
    void load_##manged_name( void* p, void* pArchive )                                \
    {                                                                                 \
        load_< type >( p, pArchive );                                                 \
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

// std::vector< int >
void new_classstd00vector3int4( void* p, void* pMemory ) { new_< std::vector< int > >( p ); }
void delete_classstd00vector3int4( void* p ) { delete_< std::vector< int > >( p ); }
void store_classstd00vector3int4( void* p, void* pArchive ) { store_< std::vector< int > >( p, pArchive ); }
void load_classstd00vector3int4( void* p, void* pArchive ) { load_< std::vector< int > >( p, pArchive ); }
void copy_classstd00vector3int4( const void* pFrom, void* pTo ) { copy_< std::vector< int > >( pFrom, pTo ); }

// mega::ReferenceVector
void new_mega00ReferenceVector( void* p, void* pMemory )
{
    typename mega::runtime::ManagedSharedMemory::segment_manager* pSegmentMgr
        = reinterpret_cast< typename mega::runtime::ManagedSharedMemory::segment_manager* >( pMemory );
    using Allocator
        = boost::interprocess::allocator< mega::reference, mega::runtime::ManagedSharedMemory::segment_manager >;
    new ( p ) ReferenceVector( Allocator( pSegmentMgr ) );
}
void delete_mega00ReferenceVector( void* p ) { delete_< mega::ReferenceVector >( p ); }
void store_mega00ReferenceVector( void* p, void* pArchive ) { store_< mega::ReferenceVector >( p, pArchive ); }
void load_mega00ReferenceVector( void* p, void* pArchive ) { load_< mega::ReferenceVector >( p, pArchive ); }
void copy_mega00ReferenceVector( const void* pFrom, void* pTo ) { copy_< mega::ReferenceVector >( pFrom, pTo ); }

} // namespace mega
