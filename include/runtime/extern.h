
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

#ifndef GUARD_2024_January_12_extern
#define GUARD_2024_January_12_extern

#include "mega/values/native_types.h"
#include "mega/values/runtime/inline.h"
#include "mega/values/compilation/interface/inline.h"
#include "mega/values/compilation/concrete/inline.h"

namespace mega::mangle
{

void log( const char* pszMsg );

void new_bitset_( void* pData, void* pBlockStart, void* pBlockEnd );
void structure_make( const c_pointer_heap& source, const c_pointer_heap& target, c_u64 relationID );
void structure_break( const c_pointer_heap& source, const c_pointer_heap& target, c_u64 relationID );
void structure_move( const c_pointer_heap& source, const c_pointer_heap& target, c_u64 relationID );
void action_start( const c_pointer_heap& source );
void action_complete( const c_pointer_heap& source );
void event_signal( const c_pointer_heap& event );
void transition( const c_pointer_heap& t );

/////////////////////////////////////////////////////////////
// const Pointer vector
bool                ref_vector_empty( void* pData );
c_u64               ref_vector_get_size( void* pData );
c_u64               ref_vector_find( void* pData, const c_pointer& ref );
c_pointer&          ref_vector_back( void* pData );
c_pointer&          ref_vector_get_at( void* pData, c_u64 index );
c_concrete_type_id& type_vector_get_at( void* pData, c_u64 index );

/////////////////////////////////////////////////////////////
// non-const Pointer vector
void ref_vector_remove( void* pData, const c_pointer& ref );
void ref_vector_remove_at( void* pData, c_u64 index );
void ref_vector_pop( void* pData );
void ref_vector_add( void* pData, const c_pointer& ref );

/////////////////////////////////////////////////////////////
void ref_vectors_remove( void* pData1, void* pData2, const c_pointer& ref );
void ref_vectors_add( void* pData1, void* pData2, const c_pointer& ref, const c_concrete_type_id& type );
void ref_vectors_remove_at( void* pData1, void* pData2, c_u64 index );

/////////////////////////////////////////////////////////////
// bitset routines

bool bitset_test( void* pBitset, c_u32 index );
void bitset_set( void* pBitset, c_u32 position, c_u32 length );
void bitset_unset( void* pBitset, c_u32 position, c_u32 length );

/////////////////////////////////////////////////////////////

void xml_save_begin_structure( const c_pointer& ref, void* pSerialiser );
void xml_save_end_structure( const c_pointer& ref, void* pSerialiser );
void xml_save_begin_data( const c_pointer& ref, const char* pszName, bool bIsObject, void* pSerialiser );
void xml_save_end_data( const c_pointer& ref, const char* pszName, bool bIsObject, void* pSerialiser );
void xml_load_begin_structure( const c_pointer& ref, const char* pszName, bool bIsObject, void* pSerialiser );
void xml_load_end_structure( const c_pointer& ref, const char* pszName, bool bIsObject, void* pSerialiser );
void xml_load_begin_data( const c_pointer& ref, const char* pszName, bool bIsObject, void* pSerialiser );
void xml_load_end_data( const c_pointer& ref, const char* pszName, bool bIsObject, void* pSerialiser );

bool  xml_is_tag( const char* pszTag, void* pSerialiser );
void  xml_load_allocation( const c_pointer& ref, void* pSerialiser );
c_u64 xml_load_tag_count( void* pSerialiser );

// iterator routines
c_concrete_type_id iterator_state( void* pIterator );

void iterator_object_start( void* pIterator, const char* pszType, c_concrete_type_id successor );
void iterator_object_end( void* pIterator, const char* pszType );
void iterator_component_start( void* pIterator, const char* pszType, c_concrete_type_id successor,
                               c_concrete_instance localDomainSize );
void iterator_component_end( void* pIterator, const char* pszType, c_concrete_type_id successor );
void iterator_action_start( void* pIterator, const char* pszType, c_concrete_type_id successor,
                            c_concrete_instance localDomainSize );
void iterator_action_end( void* pIterator, const char* pszType, c_concrete_type_id successor );
void iterator_state_start( void* pIterator, const char* pszType, c_concrete_type_id successor,
                           c_concrete_instance localDomainSize );
void iterator_state_end( void* pIterator, const char* pszType, c_concrete_type_id successor );
void iterator_event_start( void* pIterator, const char* pszType, c_concrete_type_id successor,
                           c_concrete_instance localDomainSize );
void iterator_event_end( void* pIterator, const char* pszType, c_concrete_type_id successor );
void iterator_link_start( void* pIterator, const char* pszType, c_concrete_type_id successor, bool bOwning,
                          bool bOwned );
void iterator_link_end( void* pIterator, const char* pszType, c_concrete_type_id successor, bool bOwning, bool bOwned );
void iterator_interupt_start( void* pIterator, const char* pszType, c_concrete_type_id successor );
void iterator_interupt_end( void* pIterator, const char* pszType, c_concrete_type_id successor );
void iterator_function_start( void* pIterator, const char* pszType, c_concrete_type_id successor );
void iterator_function_end( void* pIterator, const char* pszType, c_concrete_type_id successor );
void iterator_decider_start( void* pIterator, const char* pszType, c_concrete_type_id successor );
void iterator_decider_end( void* pIterator, const char* pszType, c_concrete_type_id successor );
void iterator_namespace_start( void* pIterator, const char* pszType, c_concrete_type_id successor );
void iterator_namespace_end( void* pIterator, const char* pszType, c_concrete_type_id successor );
void iterator_dimension( void* pIterator, const char* pszType, c_concrete_type_id successor );

} // namespace mega::mangle

#endif // GUARD_2024_January_12_extern
