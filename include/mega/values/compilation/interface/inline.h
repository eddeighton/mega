
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

#ifndef GUARD_2023_December_12_inline
#define GUARD_2023_December_12_inline

#include "mega/values/runtime.h"
#include "mega/values/native_types.h"

C_RUNTIME_START

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
typedef struct _c_symbol_id
{
    c_u32 value;
} c_symbol_id;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
typedef struct _c_interface_object_id
{
    c_u16 value;
} c_interface_object_id;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
typedef struct _c_interface_sub_object_id
{
    c_u16 value;
} c_interface_sub_object_id;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
typedef struct _c_interface_type_id
{
    c_interface_sub_object_id sub_object_id;
    c_interface_object_id     object_id;
} c_interface_type_id;

#ifdef __cplusplus
constexpr
#endif
    inline c_interface_type_id
    c_interface_type_id_make( c_u16 object_id, c_u16 sub_object_id )
{
    c_interface_type_id result;
    result.object_id.value     = object_id;
    result.sub_object_id.value = sub_object_id;
    return result;
}

#ifdef __cplusplus
constexpr
#endif
    inline c_u32
    c_interface_type_id_as_int( c_interface_type_id id )
{
    return ( ( c_u32 )id.sub_object_id.value ) + ( ( ( c_u32 )id.object_id.value ) << 16 );
}

#ifdef __cplusplus
constexpr
#endif
    inline c_interface_type_id
    c_interface_type_id_from_int( c_u32 i )
{
    return c_interface_type_id{ ( c_u16 )( i ), ( c_u16 )( i >> 16 ) };
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
typedef struct _c_interface_relation_id
{
    c_interface_type_id lower;
    c_interface_type_id upper;
} c_interface_relation_id;

#ifdef __cplusplus
constexpr
#endif
    inline c_interface_relation_id
    c_interface_relation_id_make( c_u32 upper, c_u32 lower )
{
    c_interface_relation_id result;
    result.lower = c_interface_type_id_from_int( lower );
    result.upper = c_interface_type_id_from_int( upper );
    return result;
}

#ifdef __cplusplus
constexpr
#endif
    inline c_u64
    c_interface_relation_id_as_int( c_interface_relation_id id )
{
    return ( ( c_u64 )c_interface_type_id_as_int( id.lower ) )
           + ( ( c_u64 )c_interface_type_id_as_int( id.upper ) << 32 );
}

#ifdef __cplusplus
constexpr
#endif
    inline c_interface_relation_id
    c_interface_relation_id_from_int( c_u64 i )
{
    return c_interface_relation_id_make( ( c_u32 )( i >> 32 ), ( c_u32 )( i ) );
}

C_RUNTIME_END
#endif // GUARD_2023_December_12_inline
