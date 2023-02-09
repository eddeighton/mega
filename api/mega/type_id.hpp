
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

#ifndef GUARD_2023_January_09_type_id
#define GUARD_2023_January_09_type_id

#include "mega/native_types.hpp"

namespace mega
{

class TypeID
{
public:
    using ValueType = I16;

private:
    struct ContextID
    {
        U16 object : 8, subObject : 7, flag : 1;
    };

    union
    {
        ContextID contextID;
        ValueType value;
    };

    enum Flag
    {
        eType   = 0,
        eSymbol = 1 // must match sign bit - ALL symbolIDs are negative
    };

    constexpr TypeID( ContextID contextID )
        : contextID( contextID )
    {
    }

public:
    static constexpr I16 LOWEST_SYMBOL_ID = -32768; // -std::numeric_limits< TypeID::ValueType >::max()

    struct Hash
    {
        inline U64 operator()( const TypeID& ref ) const noexcept
        {
            return ref.getSymbolID();
        }
    };

    constexpr TypeID()
        : value( 0U )
    {
    }

    constexpr explicit TypeID( ValueType value )
        : value( value )
    {
    }

    constexpr TypeID( const TypeID& cpy ) = default;

    constexpr inline bool isSymbolID() const { return contextID.flag == eSymbol; }
    constexpr inline bool isContextID() const { return contextID.flag == eType; }
    constexpr inline bool isObject() const { return isContextID() && contextID.subObject == 0; }

    constexpr inline I16 getSymbolID() const { return value; }
    constexpr inline U8  getObjectID() const { return contextID.object; }
    constexpr inline U8  getSubObjectID() const { return contextID.subObject; }

    constexpr inline operator ValueType() const { return value; }

    constexpr inline bool operator==( const TypeID& cmp ) const { return value == cmp.value; }
    constexpr inline bool operator!=( const TypeID& cmp ) const { return !( *this == cmp ); }
    constexpr inline bool operator<( const TypeID& cmp ) const { return value < cmp.value; }

    constexpr static inline TypeID make_context( U8 objectID, U8 subObjectID = 0 )
    {
        return TypeID{ ContextID{ objectID, subObjectID, eType } };
    }
    constexpr static inline TypeID make_object_type( TypeID typeID )
    {
        return TypeID{ ContextID{ typeID.getObjectID(), 0U, eType } };
    }
};
static_assert( sizeof( TypeID ) == 2U, "Invalid TypeID Size" );

static constexpr TypeID ROOT_SYMBOL_ID = TypeID( -1 );
static constexpr TypeID ROOT_TYPE_ID = TypeID::make_context( 1, 0 );

} // namespace mega

#endif // GUARD_2023_January_09_type_id
