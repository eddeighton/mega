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

#include "mega/reference.hpp"
#include "mega/reference_io.hpp"

#include "mega/iterator.hpp"

#include "traits.hpp"

#include "mega/maths_types_io.hpp"

#include "mega/xml_archive.hpp"
#include "mega/record_archive.hpp"
#include "mega/bin_archive.hpp"

#include "log/file_log.hpp"

#include "service/protocol/common/context.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>
#include <iostream>

namespace mega::mangle
{
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

void log( const char* pszMsg )
{
    mega::log::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::log::Log::Write( mega::log::Log::eTrace, pszMsg ) );
}

void structure_make( const mega::reference& source, const mega::reference& target, mega::U64 relationID )
{
    VERIFY_RTE_MSG( source.isHeapAddress(), "structure_make passed network address in source " );
    VERIFY_RTE_MSG( target.isHeapAddress(), "structure_make passed network address in target " );
    mega::log::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::log::Structure::Write( source, target, relationID, mega::log::Structure::eMake ) );
}
void structure_make_source( const mega::reference& source, const mega::reference& target, mega::U64 relationID )
{
    VERIFY_RTE_MSG( source.isHeapAddress(), "structure_make_source passed network address in source " );
    VERIFY_RTE_MSG( target.isHeapAddress(), "structure_make_source passed network address in target " );
    mega::log::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::log::Structure::Write( source, target, relationID, mega::log::Structure::eMakeSource ) );
}
void structure_make_target( const mega::reference& source, const mega::reference& target, mega::U64 relationID )
{
    VERIFY_RTE_MSG( source.isHeapAddress(), "structure_make_target passed network address in source " );
    VERIFY_RTE_MSG( target.isHeapAddress(), "structure_make_target passed network address in target " );
    mega::log::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::log::Structure::Write( source, target, relationID, mega::log::Structure::eMakeTarget ) );
}
void structure_break( const mega::reference& source, const mega::reference& target, mega::U64 relationID )
{
    VERIFY_RTE_MSG( source.isHeapAddress(), "structure_break passed network address in source " );
    VERIFY_RTE_MSG( target.isHeapAddress(), "structure_break passed network address in target " );
    mega::log::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::log::Structure::Write( source, target, relationID, mega::log::Structure::eBreak ) );
}
void structure_break_source( const mega::reference& source, const mega::reference& target, mega::U64 relationID )
{
    VERIFY_RTE_MSG( source.isHeapAddress(), "structure_break_source passed network address in source " );
    VERIFY_RTE_MSG( target.isHeapAddress(), "structure_break_source passed network address in target " );
    mega::log::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::log::Structure::Write( source, target, relationID, mega::log::Structure::eBreakSource ) );
}
void structure_break_target( const mega::reference& source, const mega::reference& target, mega::U64 relationID )
{
    VERIFY_RTE_MSG( source.isHeapAddress(), "structure_break_target passed network address in source " );
    VERIFY_RTE_MSG( target.isHeapAddress(), "structure_break_target passed network address in target " );
    mega::log::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::log::Structure::Write( source, target, relationID, mega::log::Structure::eBreakTarget ) );
}

void structure_move( const mega::reference& source, const mega::reference& target, mega::U64 relationID )
{
    VERIFY_RTE_MSG( source.isHeapAddress(), "structure_move passed network address in source" );
    // VERIFY_RTE_MSG( target.isHeapAddress(), "structure_move passed network address in target" );
    mega::log::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::log::Structure::Write( source, target, relationID, mega::log::Structure::eMove ) );
}

void action_start( const mega::reference& source )
{
    VERIFY_RTE_MSG( source.isHeapAddress(), "action_start passed network address " );
    mega::log::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::log::Scheduling::Write( source, mega::log::Scheduling::eStart ) );
}
void event_signal( const mega::reference& event )
{
    VERIFY_RTE_MSG( event.isHeapAddress(), "event_signal passed network address " );
    mega::log::FileStorage& log = mega::Context::get()->getLog();
    log.record( mega::log::Scheduling::Write( event, mega::log::Scheduling::eSignal ) );
}

// const reference vector
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
mega::U64 ref_vector_find( void* pData, const mega::reference& ref )
{
    const ReferenceVector& vec   = reify< ReferenceVector >( pData );
    auto                   iFind = std::find( vec.cbegin(), vec.cend(), ref.getNetworkAddress() );
    return std::distance( iFind, vec.cend() );
}

mega::reference& ref_vector_back( void* pData )
{
    ReferenceVector& vec = reify< ReferenceVector >( pData );
    VERIFY_RTE_MSG( !vec.empty(), "ref_vector_back called on empty vector" );
    return vec.back();
}

mega::reference& ref_vector_get_at( void* pData, mega::U64 index )
{
    auto& vec = reify< ReferenceVector >( pData );
    VERIFY_RTE( index < vec.size() );
    return vec[ index ];
}

// non-const reference vector
void ref_vector_remove( void* pData, const mega::reference& ref )
{
    auto& vec   = reify< ReferenceVector >( pData );
    auto  iFind = std::find( vec.begin(), vec.end(), ref.getNetworkAddress() );
    if( iFind != vec.end() )
    {
        vec.erase( iFind );
    }
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

void ref_vector_add( void* pData, const mega::reference& ref )
{
    auto& vec = reify< ReferenceVector >( pData );
    vec.push_back( ref.getNetworkAddress() );
}


/////////////////////////////////////////////////////////////
void ref_vectors_remove( void* pData1, void* pData2, const mega::reference& ref )
{
    auto& refs  = reify< ReferenceVector >( pData1 );
    auto& types = reify< LinkTypeVector >( pData2 );
    auto  iFind = std::find( refs.begin(), refs.end(), ref.getNetworkAddress() );
    if( iFind != refs.end() )
    {
        types.erase( types.begin() + std::distance( refs.begin(), iFind ) );
        refs.erase( iFind );
    }
}
void ref_vectors_add( void* pData1, void* pData2, const mega::reference& ref, const mega::TypeID& type )
{
    auto& refs  = reify< ReferenceVector >( pData1 );
    auto& types = reify< LinkTypeVector >( pData2 );
    refs.push_back( ref.getNetworkAddress() );
    types.push_back( type );
}
void ref_vectors_remove_at( void* pData1, void* pData2, mega::U64 index )
{
    auto& refs  = reify< ReferenceVector >( pData1 );
    auto& types = reify< LinkTypeVector >( pData2 );
    refs.erase( refs.begin() + index );
    types.erase( types.begin() + index );
}

/////////////////////////////////////////////////////////////
void xml_save_begin_structure( const mega::reference& ref, void* pSerialiser )
{
    auto& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.beginStructure( ref.getNetworkAddress() );
}
void xml_save_end_structure( const mega::reference& ref, void* pSerialiser )
{
    auto& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.endStructure( ref.getNetworkAddress() );
}
void xml_save_begin_data( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.beginData( pszName, bIsObject, ref.getNetworkAddress() );
}
void xml_save_end_data( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.endData( pszName, bIsObject, ref.getNetworkAddress() );
}

void xml_load_begin_structure( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.beginStructure( pszName, bIsObject, ref.getNetworkAddress() );
}
void xml_load_end_structure( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.endStructure( pszName, bIsObject, ref.getNetworkAddress() );
}
void xml_load_begin_data( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.beginData( pszName, bIsObject, ref.getNetworkAddress() );
}
void xml_load_end_data( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.endData( pszName, bIsObject, ref.getNetworkAddress() );
}

bool xml_is_tag( const char* pszTag, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    return archive.is_tag( pszTag );
}

void xml_load_allocation( const mega::reference& ref, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.allocation( ref.getNetworkAddress() );
}

mega::U64 xml_load_tag_count( void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    return archive.tag_count();
}

// iterator routines
mega::TypeID iterator_state( void* pIterator )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    return iterator.getState();
}

void iterator_object_start( void* pIterator, mega::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.object_start( successor );
}
void iterator_object_end( void* pIterator )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.object_end();
}
void iterator_action_start( void* pIterator, mega::TypeID successor, mega::Instance localDomainSize )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.action_start( successor, localDomainSize );
}
void iterator_action_end( void* pIterator, mega::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.action_end( successor );
}
void iterator_event_start( void* pIterator, mega::TypeID successor, mega::Instance localDomainSize )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.event_start( successor, localDomainSize );
}
void iterator_event_end( void* pIterator, mega::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.event_end( successor );
}
void iterator_link_start( void* pIterator, mega::TypeID successor, bool bOwning, bool bOwned )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.link_start( successor, bOwning, bOwned );
}
void iterator_link_end( void* pIterator, mega::TypeID successor, bool bOwning, bool bOwned )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.link_end( successor, bOwning, bOwned );
}
void iterator_interupt_start( void* pIterator, mega::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.interupt_start( successor );
}
void iterator_interupt_end( void* pIterator, mega::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.interupt_end( successor );
}
void iterator_function_start( void* pIterator, mega::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.function_start( successor );
}
void iterator_function_end( void* pIterator, mega::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.function_end( successor );
}
void iterator_namespace_start( void* pIterator, mega::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.namespace_start( successor );
}
void iterator_namespace_end( void* pIterator, mega::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.namespace_end( successor );
}
void iterator_dimension( void* pIterator, mega::TypeID successor )
{
    auto& iterator = reify< mega::Iterator >( pIterator );
    iterator.dimension( successor );
}
} // namespace mega::mangle
