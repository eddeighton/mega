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
#include "mega/xml_archive.hpp"

#include "traits.hpp"

#include "log/log.hpp"

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
    mega::log::Storage& log = mega::Context::get()->getLog();
    log.record( mega::log::Log::Write( mega::log::Log::eTrace, pszMsg ) );
}

void structure_make( const mega::reference& source, const mega::reference& target, mega::U32 relationID )
{
    mega::log::Storage& log = mega::Context::get()->getLog();
    log.record( mega::log::Structure::Write(
        source.getNetworkAddress(), target.getNetworkAddress(), relationID, mega::log::Structure::eMake ) );
}
void structure_break( const mega::reference& source, const mega::reference& target, mega::U32 relationID )
{
    mega::log::Storage& log = mega::Context::get()->getLog();
    log.record( mega::log::Structure::Write(
        source.getNetworkAddress(), target.getNetworkAddress(), relationID, mega::log::Structure::eBreak ) );
}

void action_start( const mega::reference& source )
{
    mega::log::Storage& log = mega::Context::get()->getLog();
    log.record( mega::log::Scheduling::Write( source.getNetworkAddress(), mega::log::Scheduling::eStart ) );
}
void action_stop( const mega::reference& source )
{
    mega::log::Storage& log = mega::Context::get()->getLog();
    log.record( mega::log::Scheduling::Write( source.getNetworkAddress(), mega::log::Scheduling::eStop ) );
}

bool ref_vector_contains( void* pData, const mega::reference& ref )
{
    const ReferenceVector& vec = reify< ReferenceVector >( pData );
    return std::find( vec.cbegin(), vec.cend(), ref ) != vec.cend();
}
void ref_vector_remove( void* pData, const mega::reference& ref )
{
    auto& vec   = reify< ReferenceVector >( pData );
    auto  iFind = std::find( vec.begin(), vec.end(), ref );
    if( iFind != vec.end() )
    {
        vec.erase( iFind );
    }
}
void ref_vector_add( void* pData, const mega::reference& ref )
{
    auto& vec = reify< ReferenceVector >( pData );
    vec.push_back( ref );
}
void ref_vector_clear( void* pData )
{
    auto& vec = reify< ReferenceVector >( pData );
    vec.clear();
}

mega::U64 ref_vector_get_size( void* pData )
{
    auto& vec = reify< ReferenceVector >( pData );
    return vec.size();
}

mega::reference& ref_vector_get_at( void* pData, mega::U64 index )
{
    auto& vec = reify< ReferenceVector >( pData );
    VERIFY_RTE( index < vec.size() );
    return vec[ index ];
}

void xml_save_begin_structure( const mega::reference& ref, void* pSerialiser )
{
    auto& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.beginStructure( ref );
}
void xml_save_end_structure( const mega::reference& ref, void* pSerialiser )
{
    auto& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.endStructure( ref );
}
void xml_save_begin_data( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.beginData( pszName, bIsObject, ref );
}
void xml_save_end_data( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.endData( pszName, bIsObject, ref );
}

void xml_load_begin_structure( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.beginStructure( pszName, bIsObject, ref );
}
void xml_load_end_structure( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.endStructure( pszName, bIsObject, ref );
}
void xml_load_begin_data( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.beginData( pszName, bIsObject, ref );
}
void xml_load_end_data( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.endData( pszName, bIsObject, ref );
}

bool xml_is_tag( const char* pszTag, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    return archive.is_tag( pszTag );
}

void xml_load_allocation( const mega::reference& ref, void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    archive.allocation( ref );
}

mega::U64 xml_load_tag_count( void* pSerialiser )
{
    auto& archive = reify< XMLLoadArchive >( pSerialiser );
    return archive.tag_count();
}

} // namespace mega::mangle
