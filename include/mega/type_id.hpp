
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

#ifndef MEGAJIT
#include <limits>
namespace boost::serialization{};
#endif

namespace mega
{

class TypeID
{
public:
    using SubValueType = U16;
    using ValueType    = I32;

private:
    struct ContextID
    {
        U32 subObject : 16, object : 15, flag : 1;
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
#ifndef MEGAJIT
    static constexpr ValueType LOWEST_SYMBOL_ID = std::numeric_limits< ValueType >::min(); // -2147483648
    static_assert( LOWEST_SYMBOL_ID == 0x80000000, "Incorrect LOWEST_SYMBOL_ID value" );
#else
    static constexpr ValueType LOWEST_SYMBOL_ID = 0x80000000; // -2147483648
#endif

    struct Hash
    {
        inline U64 operator()( const TypeID& ref ) const noexcept { return ref.getSymbolID(); }
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

    constexpr inline ValueType    getSymbolID() const { return value; }
    constexpr inline SubValueType getObjectID() const { return contextID.object; }
    constexpr inline SubValueType getSubObjectID() const { return contextID.subObject; }

    constexpr inline operator ValueType() const { return value; }
    constexpr inline bool is_valid() const { return value != 0U; }

    constexpr inline bool operator==( const TypeID& cmp ) const { return value == cmp.value; }
    constexpr inline bool operator!=( const TypeID& cmp ) const { return !( *this == cmp ); }
    constexpr inline bool operator<( const TypeID& cmp ) const { return value < cmp.value; }

    constexpr static inline TypeID make_context( SubValueType objectID, SubValueType subObjectID = 0 )
    {
        return TypeID{ ContextID{ subObjectID, objectID, eType } };
    }
    constexpr static inline TypeID make_object_type( TypeID typeID )
    {
        return TypeID{ ContextID{ 0U, typeID.getObjectID(), eType } };
    }

#ifndef MEGAJIT
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        using namespace boost::serialization;
        serialize( archive, *this, version );
    }
#endif

};
static_assert( sizeof( TypeID ) == 4U, "Invalid TypeID Size" );

static constexpr TypeID ROOT_SYMBOL_ID = TypeID( -1 );
static constexpr TypeID ROOT_TYPE_ID   = TypeID::make_context( 1, 0 );

} // namespace mega

#endif // GUARD_2023_January_09_type_id
