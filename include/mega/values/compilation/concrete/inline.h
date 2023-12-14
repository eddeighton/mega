
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

#ifndef GUARD_2023_December_12_concrete_inline
#define GUARD_2023_December_12_concrete_inline

#include "mega/values/runtime.h"
#include "mega/values/native_types.h"

C_RUNTIME_START

using c_concrete_object_id = struct _c_concrete_object_id
{
    c_u16 value;
};

using c_concrete_sub_object_id = struct _c_concrete_sub_object_id
{
    c_u16 value;
};

using c_concrete_type_id = struct _c_concrete_type_id
{
    c_concrete_sub_object_id sub_object_id;
    c_concrete_object_id     object_id;
};

#ifdef __cplusplus
constexpr
#endif
    inline c_concrete_type_id
    c_concrete_type_id_make( c_u16 object_id, c_u16 sub_object_id )
{
    c_concrete_type_id result;
    result.object_id.value     = object_id;
    result.sub_object_id.value = sub_object_id;
    return result;
}

#ifdef __cplusplus
constexpr
#endif
    inline c_u32
    c_concrete_type_id_as_int( c_concrete_type_id id )
{
    return ( ( c_u32 )id.sub_object_id.value ) + ( ( ( c_u32 )id.object_id.value ) << 16 );
}

#ifdef __cplusplus
constexpr
#endif
    inline c_concrete_type_id
    c_concrete_type_id_from_int( c_u32 i )
{
    return c_concrete_type_id{ ( c_u16 )( i ), ( c_u16 )( i >> 16 ) };
}

using c_concrete_instance = struct _c_concrete_instance
{
    c_u16 value;
};

using c_concrete_sub_object_id_instance = struct _c_concrete_sub_object_id_instance
{
    c_concrete_sub_object_id sub_object_id;
    c_concrete_instance      instance;
};

#ifdef __cplusplus
constexpr
#endif
    inline c_concrete_sub_object_id_instance
    c_concrete_sub_object_id_instance_make( c_u16 sub_object_id, c_u16 instance )
{
    c_concrete_sub_object_id_instance result;
    result.sub_object_id.value = sub_object_id;
    result.instance.value      = instance;
    return result;
}

#ifdef __cplusplus
constexpr
#endif
    inline c_u32
    c_concrete_sub_object_id_instance_as_int( c_concrete_sub_object_id_instance id )
{
    return ( ( c_u32 )id.sub_object_id.value ) + ( ( ( c_u32 )id.instance.value ) << 16 );
}

#ifdef __cplusplus
constexpr
#endif
    inline c_concrete_sub_object_id_instance
    c_concrete_sub_object_id_instance_from_int( c_u32 i )
{
    return c_concrete_sub_object_id_instance{ ( c_u16 )( i ), ( c_u16 )( i >> 16 ) };
}

#pragma pack( 1 )
using c_concrete_type_id_instance = struct _c_concrete_type_id_instance
{
    c_concrete_type_id  type_id;
    c_concrete_instance instance;
};
#pragma pack()

#ifdef __cplusplus
constexpr
#endif
    inline c_concrete_type_id_instance
    c_concrete_type_id_instance_make( c_u32 type_id, c_u16 instance )
{
    c_concrete_type_id_instance result;
    result.type_id        = c_concrete_type_id_from_int( type_id );
    result.instance.value = instance;
    return result;
}

C_RUNTIME_END

#endif // GUARD_2023_December_12_concrete_inline
