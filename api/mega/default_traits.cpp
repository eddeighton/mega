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
void copy_( void* pFrom, void* pTo )
{
}

} // namespace

namespace mega
{

// U8 = unsigned_char;
void new_unsigned_char( void* p, void* pMemory ) { new_< unsigned char >( p ); }
void delete_unsigned_char( void* p ) { delete_< unsigned char >( p ); }
void store_unsigned_char( void* p, void* pArchive ) { store_< unsigned char >( p, pArchive ); }
void load_unsigned_char( void* p, void* pArchive ) { load_< unsigned char >( p, pArchive ); }
void copy_unsigned_char( void* pFrom, void* pTo ) { copy_< unsigned char >( pFrom, pTo ); }

// I8 = signed char;
void new_signed_char( void* p, void* pMemory ) { new_< signed char >( p ); }
void delete_signed_char( void* p ) { delete_< signed char >( p ); }
void store_signed_char( void* p, void* pArchive ) { store_< signed char >( p, pArchive ); }
void load_signed_char( void* p, void* pArchive ) { load_< signed char >( p, pArchive ); }
void copy_signed_char( void* pFrom, void* pTo ) { copy_< signed char >( pFrom, pTo ); }

// U16 = unsigned short;
void new_unsigned_short( void* p, void* pMemory ) { new_< unsigned short >( p ); }
void delete_unsigned_short( void* p ) { delete_< unsigned short >( p ); }
void store_unsigned_short( void* p, void* pArchive ) { store_< unsigned short >( p, pArchive ); }
void load_unsigned_short( void* p, void* pArchive ) { load_< unsigned short >( p, pArchive ); }
void copy_unsigned_short( void* pFrom, void* pTo ) { copy_< unsigned short >( pFrom, pTo ); }

// I16 = signed short;
void new_signed_short( void* p, void* pMemory ) { new_< signed short >( p ); }
void delete_signed_short( void* p ) { delete_< signed short >( p ); }
void store_signed_short( void* p, void* pArchive ) { store_< signed short >( p, pArchive ); }
void load_signed_short( void* p, void* pArchive ) { load_< signed short >( p, pArchive ); }
void copy_signed_short( void* pFrom, void* pTo ) { copy_< signed short >( pFrom, pTo ); }

// U32 = unsigned int;
void new_unsigned_int( void* p, void* pMemory ) { new_< unsigned int >( p ); }
void delete_unsigned_int( void* p ) { delete_< unsigned int >( p ); }
void store_unsigned_int( void* p, void* pArchive ) { store_< unsigned int >( p, pArchive ); }
void load_unsigned_int( void* p, void* pArchive ) { load_< unsigned int >( p, pArchive ); }
void copy_unsigned_int( void* pFrom, void* pTo ) { copy_< unsigned int >( pFrom, pTo ); }

// I32 = signed int;
void new_signed_int( void* p, void* pMemory ) { new_< signed int >( p ); }
void delete_signed_int( void* p ) { delete_< signed int >( p ); }
void store_signed_int( void* p, void* pArchive ) { store_< signed int >( p, pArchive ); }
void load_signed_int( void* p, void* pArchive ) { load_< signed int >( p, pArchive ); }
void copy_signed_int( void* pFrom, void* pTo ) { copy_< signed int >( pFrom, pTo ); }

void new_int( void* p, void* pMemory ) { new_< int >( p ); }
void delete_int( void* p ) { delete_< int >( p ); }
void store_int( void* p, void* pArchive ) { store_< int >( p, pArchive ); }
void load_int( void* p, void* pArchive ) { load_< int >( p, pArchive ); }
void copy_int( void* pFrom, void* pTo ) { copy_< int >( pFrom, pTo ); }

// U64 = unsigned long int;
void new_unsigned_long_int( void* p, void* pMemory ) { new_< unsigned long int >( p ); }
void delete_unsigned_long_int( void* p ) { delete_< unsigned long int >( p ); }
void store_unsigned_long_int( void* p, void* pArchive ) { store_< unsigned long int >( p, pArchive ); }
void load_unsigned_long_int( void* p, void* pArchive ) { load_< unsigned long int >( p, pArchive ); }
void copy_unsigned_long_int( void* pFrom, void* pTo ) { copy_< unsigned long int >( pFrom, pTo ); }

// I64 = signed long int;
void new_signed_long_int( void* p, void* pMemory ) { new_< signed long int >( p ); }
void delete_signed_long_int( void* p ) { delete_< signed long int >( p ); }
void store_signed_long_int( void* p, void* pArchive ) { store_< signed long int >( p, pArchive ); }
void load_signed_long_int( void* p, void* pArchive ) { load_< signed long int >( p, pArchive ); }
void copy_signed_long_int( void* pFrom, void* pTo ) { copy_< signed long int >( pFrom, pTo ); }

// F32 = float;
void new_float( void* p, void* pMemory ) { new_< float >( p ); }
void delete_float( void* p ) { delete_< float >( p ); }
void store_float( void* p, void* pArchive ) { store_< float >( p, pArchive ); }
void load_float( void* p, void* pArchive ) { load_< float >( p, pArchive ); }
void copy_float( void* pFrom, void* pTo ) { copy_< float >( pFrom, pTo ); }

// F64 = double;
void new_double( void* p, void* pMemory ) { new_< double >( p ); }
void delete_double( void* p ) { delete_< double >( p ); }
void store_double( void* p, void* pArchive ) { store_< double >( p, pArchive ); }
void load_double( void* p, void* pArchive ) { load_< double >( p, pArchive ); }
void copy_double( void* pFrom, void* pTo ) { copy_< double >( pFrom, pTo ); }

// mega::Bitmask32Allocator< 4 >
void new_mega00Bitmask32Allocator344( void* p, void* pMemory ) { new_< mega::Bitmask32Allocator< 4 > >( p ); }
void delete_mega00Bitmask32Allocator344( void* p ) { delete_< mega::Bitmask32Allocator< 4 > >( p ); }
void store_mega00Bitmask32Allocator344( void* p, void* pArchive )
{
    store_< mega::Bitmask32Allocator< 4 > >( p, pArchive );
}
void load_mega00Bitmask32Allocator344( void* p, void* pArchive )
{
    load_< mega::Bitmask32Allocator< 4 > >( p, pArchive );
}
void copy_mega00Bitmask32Allocator344( void* pFrom, void* pTo )
{
    copy_< mega::Bitmask32Allocator< 4 > >( pFrom, pTo );
}

// mega::reference
void new_mega00reference( void* p, void* pMemory ) { new_< mega::reference >( p ); }
void delete_mega00reference( void* p ) { delete_< mega::reference >( p ); }
void store_mega00reference( void* p, void* pArchive ) { store_< mega::reference >( p, pArchive ); }
void load_mega00reference( void* p, void* pArchive ) { load_< mega::reference >( p, pArchive ); }
void copy_mega00reference( void* pFrom, void* pTo ) { copy_< mega::reference >( pFrom, pTo ); }

// mega::reference
void new_classstd00vector3int4( void* p, void* pMemory ) { new_< std::vector< int > >( p ); }
void delete_classstd00vector3int4( void* p ) { delete_< std::vector< int > >( p ); }
void store_classstd00vector3int4( void* p, void* pArchive ) { store_< std::vector< int > >( p, pArchive ); }
void load_classstd00vector3int4( void* p, void* pArchive ) { load_< std::vector< int > >( p, pArchive ); }
void copy_classstd00vector3int4( void* pFrom, void* pTo ) { copy_< std::vector< int > >( pFrom, pTo ); }

// mega::reference
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
void copy_mega00ReferenceVector( void* pFrom, void* pTo ) { copy_< mega::ReferenceVector >( pFrom, pTo ); }

} // namespace mega
