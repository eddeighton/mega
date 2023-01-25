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

void hyper_create( const mega::reference& source, const mega::reference& target )
{
    // mega::log::Storage& log = mega::getMPOContext()->getLog();
    // log.record( track, mega::log::HyperRecord( source, target, CREATE ) );
}
void hyper_delete( const mega::reference& source, const mega::reference& target )
{
    // mega::log::Storage& log = mega::getMPOContext()->getLog();
    // log.record( track, mega::log::HyperRecord( source, target, DELETE ) );
}

bool ref_vector_contains( void* pData, const mega::reference& ref )
{
    const ReferenceVector& vec = reify< ReferenceVector >( pData );
    return std::find( vec.cbegin(), vec.cend(), ref ) != vec.cend();
}
void ref_vector_remove( void* pData, const mega::reference& ref )
{
    // std::cout << "ref_vector_remove : " << ref << std::endl;
    auto& vec   = reify< ReferenceVector >( pData );
    auto  iFind = std::find( vec.begin(), vec.end(), ref );
    if( iFind != vec.end() )
    {
        vec.erase( iFind );
    }
}
void ref_vector_add( void* pData, const mega::reference& ref )
{
    // std::cout << "ref_vector_add : " << ref << std::endl;
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

void xml_save_begin_object_structure( const mega::reference& ref, void* pSerialiser )
{
    // std::cout << "xml_begin_object_structure : " << ref << std::endl;
    XMLSaveArchive& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.internalReference( ref );
}
void xml_save_end_object_structure( const mega::reference& ref, void* pSerialiser )
{
    // std::cout << "xml_end_object_structure : " << ref << std::endl;
    XMLSaveArchive& archive = reify< XMLSaveArchive >( pSerialiser );
}
void xml_save_begin_part( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    // std::cout << "xml_begin_object : " << ref << std::endl;
    XMLSaveArchive& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.beginPart( pszName, bIsObject, ref );
}
void xml_save_end_part( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    // std::cout << "xml_end_object : " << ref << std::endl;
    XMLSaveArchive& archive = reify< XMLSaveArchive >( pSerialiser );
    archive.endPart( pszName, bIsObject, ref );
}

void xml_load_begin_object_structure( const mega::reference& ref, void* pSerialiser )
{
    // std::cout << "xml_begin_object_structure : " << ref << std::endl;
    XMLLoadArchive& archive = reify< XMLLoadArchive >( pSerialiser );
    
}
void xml_load_end_object_structure( const mega::reference& ref, void* pSerialiser )
{
    // std::cout << "xml_end_object_structure : " << ref << std::endl;
    XMLLoadArchive& archive = reify< XMLLoadArchive >( pSerialiser );
}
void xml_load_begin_part( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    // std::cout << "xml_begin_object : " << ref << std::endl;
    XMLLoadArchive& archive = reify< XMLLoadArchive >( pSerialiser );
}
void xml_load_end_part( const mega::reference& ref, const char* pszName, bool bIsObject, void* pSerialiser )
{
    // std::cout << "xml_end_object : " << ref << std::endl;
    XMLLoadArchive& archive = reify< XMLLoadArchive >( pSerialiser );
}
} // namespace mega::mangle
