
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

#ifndef GUARD_2023_December_11_support
#define GUARD_2023_December_11_support

namespace mega::runtime
{
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

#include "traits.hpp"

#include "mega/values/runtime/pointer.hpp"

#include "mega/iterator.hpp"

#include "mega/values/runtime/maths_types_io.hpp"

#include "mega/record_archive.hpp"
#include "mega/bin_archive.hpp"

#include "event/file_log.hpp"

#include "service/protocol/common/context.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>
#include <iostream>

namespace
{
template < typename T >
inline T& reify( void* p )
{
    return *reinterpret_cast< T* >( p );
}
template < typename T >
inline const T& reify( const void* p )
{
    return *reinterpret_cast< const T* >( p );
}
} // namespace

namespace mega::mangle
{
void log( const char* pszMsg )
{
    mega::event::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::event::Log::Write( mega::event::Log::eTrace, pszMsg ) );
}

void new_bitset_( void* pData, void* pBlockStart, void* pBlockEnd )
{
    const auto pStart = reinterpret_cast< const mega::U64* >( pBlockStart );
    const auto pEnd   = reinterpret_cast< const mega::U64* >( pBlockEnd );
    new( pData ) BitSet{ pStart, pEnd };
}

void structure_make( const mega::Pointer& source, const mega::Pointer& target, mega::U64 relationID )
{
    VERIFY_RTE_MSG( source.isHeap(), "structure_make passed network address in source " );
    VERIFY_RTE_MSG( target.isHeap(), "structure_make passed network address in target " );
    mega::event::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::event::Structure::Write( source, target, relationID, mega::event::Structure::eMake ) );
}

void structure_break( const mega::Pointer& source, const mega::Pointer& target, mega::U64 relationID )
{
    VERIFY_RTE_MSG( source.isHeap(), "structure_break passed network address in source " );
    VERIFY_RTE_MSG( target.isHeap(), "structure_break passed network address in target " );
    mega::event::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::event::Structure::Write( source, target, relationID, mega::event::Structure::eBreak ) );
}

void structure_move( const mega::Pointer& source, const mega::Pointer& target, mega::U64 relationID )
{
    VERIFY_RTE_MSG( source.isHeap(), "structure_move passed network address in source" );
    // VERIFY_RTE_MSG( target.isHeap(), "structure_move passed network address in target" );
    mega::event::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::event::Structure::Write( source, target, relationID, mega::event::Structure::eMove ) );
}

void action_start( const mega::Pointer& source )
{
    VERIFY_RTE_MSG( source.isHeap(), "action_start passed network address " );
    mega::event::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::event::Event::Write( source, mega::event::Event::eStart ) );
}

void action_complete( const mega::Pointer& source )
{
    VERIFY_RTE_MSG( source.isHeap(), "action_start passed network address " );
    mega::event::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::event::Event::Write( source, mega::event::Event::eComplete ) );
}

void event_signal( const mega::Pointer& event )
{
    VERIFY_RTE_MSG( event.isHeap(), "event_signal passed network address " );
    mega::event::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::event::Event::Write( event, mega::event::Event::eSignal ) );
}

void transition( const mega::Pointer& t )
{
    mega::event::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::event::Transition::Write( t ) );
}

/////////////////////////////////////////////////////////////
// const Pointer vector
bool ref_vector_empty( void* pData )
{
    const ReferenceVector& vec = reify< ReferenceVector >( pData );
    return vec.empty();
}

mega::U64 ref_vector_get_size( void* pData )
{
    auto& vec = reify< ReferenceVector >( pData );
    return vec.size();
}
mega::U64 ref_vector_find( void* pData, const mega::Pointer& ref )
{
    THROW_TODO;
    // const ReferenceVector& vec   = reify< ReferenceVector >( pData );
    // auto                   iFind = std::find( vec.cbegin(), vec.cend(), ref.getNetworkAddress() );
    // return std::distance( vec.cbegin(), iFind );
}

mega::Pointer& ref_vector_back( void* pData )
{
    ReferenceVector& vec = reify< ReferenceVector >( pData );
    VERIFY_RTE_MSG( !vec.empty(), "ref_vector_back called on empty vector" );
    return vec.back();
}

mega::Pointer& ref_vector_get_at( void* pData, mega::U64 index )
{
    auto& vec = reify< ReferenceVector >( pData );
    VERIFY_RTE( index < vec.size() );
    return vec[ index ];
}

mega::concrete::TypeID& type_vector_get_at( void* pData, mega::U64 index )
{
    auto& vec = reify< LinkTypeVector >( pData );
    VERIFY_RTE( index < vec.size() );
    return vec[ index ];
}

/////////////////////////////////////////////////////////////
// non-const Pointer vector
void ref_vector_remove( void* pData, const mega::Pointer& ref )
{
    THROW_TODO;
    // auto& vec   = reify< ReferenceVector >( pData );
    // auto  iFind = std::find( vec.begin(), vec.end(), ref.getNetworkAddress() );
    // if( iFind != vec.end() )
    // {
    //     vec.erase( iFind );
    // }
}

void ref_vector_remove_at( void* pData, mega::U64 index )
{
    auto& vec = reify< ReferenceVector >( pData );
    if( vec.size() > index )
    {
        vec.erase( vec.begin() + index );
    }
}

void ref_vector_pop( void* pData )
{
    ReferenceVector& vec = reify< ReferenceVector >( pData );
    vec.pop_back();
}

void ref_vector_add( void* pData, const mega::Pointer& ref )
{
    THROW_TODO;
    // auto& vec = reify< ReferenceVector >( pData );
    // vec.push_back( ref.getNetworkAddress() );
}

/////////////////////////////////////////////////////////////
void ref_vectors_remove( void* pData1, void* pData2, const mega::Pointer& ref )
{
    THROW_TODO;
    // auto& refs  = reify< ReferenceVector >( pData1 );
    // auto& types = reify< LinkTypeVector >( pData2 );
    // auto  iFind = std::find( refs.begin(), refs.end(), ref.getNetworkAddress() );
    // if( iFind != refs.end() )
    // {
    //     types.erase( types.begin() + std::distance( refs.begin(), iFind ) );
    //     refs.erase( iFind );
    // }
}
void ref_vectors_add( void* pData1, void* pData2, const mega::Pointer& ref, const mega::concrete::TypeID& type )
{
    THROW_TODO;
    // auto& refs  = reify< ReferenceVector >( pData1 );
    // auto& types = reify< LinkTypeVector >( pData2 );
    // refs.push_back( ref.getNetworkAddress() );
    // types.push_back( type );
}
void ref_vectors_remove_at( void* pData1, void* pData2, mega::U64 index )
{
    auto& refs  = reify< ReferenceVector >( pData1 );
    auto& types = reify< LinkTypeVector >( pData2 );
    refs.erase( refs.begin() + index );
    types.erase( types.begin() + index );
}

/////////////////////////////////////////////////////////////
// bitset routines
bool bitset_test( void* pBitset, mega::U32 index )
{
    auto& bitset = reify< mega::BitSet >( pBitset );
    return bitset.test( index );
}

void bitset_set( void* pBitset, mega::U32 position, mega::U32 length )
{
    auto& bitset = reify< mega::BitSet >( pBitset );
    bitset.set( position, length, true );
}

void bitset_unset( void* pBitset, mega::U32 position, mega::U32 length )
{
    auto& bitset = reify< mega::BitSet >( pBitset );
    bitset.set( position, length, false );
}

/////////////////////////////////////////////////////////////
/*
void xml_save_begin_structure( const mega::Pointer& ref, void* pSerialiser )
{
    auto& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.beginStructure( ref.getNetworkAddress() );
}
void xml_save_end_structure( const mega::Pointer& ref, void* pSerialiser )
{
    auto& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.endStructure( ref.getNetworkAddress() );
}
void xml_save_begin_data( const mega::Pointer& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.beginData( pszName, bIsObject, ref.getNetworkAddress() );
}
void xml_save_end_data( const mega::Pointer& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.endData( pszName, bIsObject, ref.getNetworkAddress() );
}*/
/*
void xml_load_begin_structure( const mega::Pointer& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.beginStructure( pszName, bIsObject, ref.getNetworkAddress() );
}
void xml_load_end_structure( const mega::Pointer& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.endStructure( pszName, bIsObject, ref.getNetworkAddress() );
}
void xml_load_begin_data( const mega::Pointer& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.beginData( pszName, bIsObject, ref.getNetworkAddress() );
}
void xml_load_end_data( const mega::Pointer& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.endData( pszName, bIsObject, ref.getNetworkAddress() );
}

bool xml_is_tag( const char* pszTag, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    return archive.is_tag( pszTag );
}

void xml_load_allocation( const mega::Pointer& ref, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.allocation( ref.getNetworkAddress() );
}

mega::U64 xml_load_tag_count( void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    return archive.tag_count();
}
*/
// iterator routines
mega::concrete::TypeID iterator_state( void* pIterator )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    return iterator.getState();
}
/*
void iterator_object_start( void* pIterator, const char* pszType, mega::concrete::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.object_start( pszType, successor );
}
void iterator_object_end( void* pIterator, const char* pszType )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.object_end( pszType );
}
void iterator_component_start( void* pIterator, const char* pszType, mega::concrete::TypeID successor,
                               mega::Instance localDomainSize )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.component_start( pszType, successor, localDomainSize );
}
void iterator_component_end( void* pIterator, const char* pszType, mega::concrete::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.component_end( pszType, successor );
}
void iterator_action_start( void* pIterator, const char* pszType, mega::concrete::TypeID successor,
                            mega::Instance localDomainSize )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.action_start( pszType, successor, localDomainSize );
}
void iterator_action_end( void* pIterator, const char* pszType, mega::concrete::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.action_end( pszType, successor );
}
void iterator_state_start( void* pIterator, const char* pszType, mega::concrete::TypeID successor,
                           mega::Instance localDomainSize )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.state_start( pszType, successor, localDomainSize );
}
void iterator_state_end( void* pIterator, const char* pszType, mega::concrete::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.state_end( pszType, successor );
}
void iterator_event_start( void* pIterator, const char* pszType, mega::concrete::TypeID successor,
                           mega::Instance localDomainSize )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.event_start( pszType, successor, localDomainSize );
}
void iterator_event_end( void* pIterator, const char* pszType, mega::concrete::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.event_end( pszType, successor );
}
void iterator_link_start( void* pIterator, const char* pszType, mega::concrete::TypeID successor, bool bOwning, bool bOwned )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.link_start( pszType, successor, bOwning, bOwned );
}
void iterator_link_end( void* pIterator, const char* pszType, mega::concrete::TypeID successor, bool bOwning, bool bOwned )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.link_end( pszType, successor, bOwning, bOwned );
}
void iterator_interupt_start( void* pIterator, const char* pszType, mega::concrete::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.interupt_start( pszType, successor );
}
void iterator_interupt_end( void* pIterator, const char* pszType, mega::concrete::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.interupt_end( pszType, successor );
}
void iterator_function_start( void* pIterator, const char* pszType, mega::concrete::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.function_start( pszType, successor );
}
void iterator_function_end( void* pIterator, const char* pszType, mega::concrete::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.function_end( pszType, successor );
}
void iterator_decider_start( void* pIterator, const char* pszType, mega::concrete::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.decider_start( pszType, successor );
}
void iterator_decider_end( void* pIterator, const char* pszType, mega::concrete::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.decider_end( pszType, successor );
}
void iterator_namespace_start( void* pIterator, const char* pszType, mega::concrete::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.namespace_start( pszType, successor );
}
void iterator_namespace_end( void* pIterator, const char* pszType, mega::concrete::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.namespace_end( pszType, successor );
}
void iterator_dimension( void* pIterator, const char* pszType, mega::concrete::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.dimension( pszType, successor );
}
*/
} // namespace mega::mangle

}

#endif //GUARD_2023_December_11_support
