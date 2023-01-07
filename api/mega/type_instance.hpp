
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

#ifndef GUARD_2023_January_07_type_instance
#define GUARD_2023_January_07_type_instance

#include "mega/native_types.hpp"
namespace mega
{

using Instance = U16;
using SymbolID = I16;
using TypeID   = I16;

static constexpr const char* ROOT_TYPE_NAME = "Root";
static constexpr TypeID      ROOT_TYPE_ID   = 1;

struct TypeInstance
{
    Instance instance = 0U;
    TypeID   type     = 0;

    TypeInstance() = default;
    constexpr TypeInstance( Instance instance, TypeID type )
        : instance( instance )
        , type( type )
    {
    }

    static constexpr TypeInstance Object( TypeID type ) { return { 0, type }; }
    static constexpr TypeInstance Root() { return Object( ROOT_TYPE_ID ); }

    constexpr inline bool operator==( const TypeInstance& cmp ) const
    {
        return ( instance == cmp.instance ) && ( type == cmp.type );
    }
    constexpr inline bool operator!=( const TypeInstance& cmp ) const { return !( *this == cmp ); }
    constexpr inline bool operator<( const TypeInstance& cmp ) const
    {
        return ( instance != cmp.instance ) ? ( instance < cmp.instance )
               : ( type != cmp.type )       ? ( type < cmp.type )
                                            : false;
    }

    constexpr inline bool is_valid() const { return type != 0; }
};
static_assert( sizeof( TypeInstance ) == 4U, "Invalid TypeInstance Size" );

}

#endif //GUARD_2023_January_07_type_instance
