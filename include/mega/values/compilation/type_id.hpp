
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

#include "mega/values/native_types.hpp"

#ifndef MEGAJIT

#include "common/serialisation.hpp"
#include <limits>
#include <ostream>
#endif

namespace mega
{

class TypeID
{
public:
    using ValueType = I32;

private:
    struct ContextID
    {
        U32 subObject : 16, object : 15, flag : 1;
    };

    enum Flag
    {
        eType   = 0,
        eSymbol = 1 // must match sign bit - ALL symbolIDs are negative
    };

    static constexpr inline ContextID make_context_impl( SubType objectID, SubType subObjectID, Flag flag )
    {
        return ContextID{ subObjectID, objectID, flag };
    }

    union
    {
        ContextID contextID;
        ValueType value;
    };

    constexpr TypeID( ContextID _contextID )
        : contextID( _contextID )
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

    constexpr explicit TypeID( ValueType _value )
        : value( _value )
    {
    }

    constexpr TypeID( const TypeID& cpy ) = default;

    constexpr inline bool isSymbolID() const { return contextID.flag == eSymbol; }
    constexpr inline bool isContextID() const { return contextID.flag == eType; }
    constexpr inline bool isObject() const { return isContextID() && contextID.subObject == 0; }

    constexpr inline ValueType getSymbolID() const { return value; }
    constexpr inline SubType   getObjectID() const { return contextID.object; }
    constexpr inline SubType   getSubObjectID() const { return contextID.subObject; }

    constexpr inline operator ValueType() const { return value; }
    constexpr inline bool     valid() const { return value != 0U; }

    constexpr inline bool operator==( const TypeID& cmp ) const { return value == cmp.value; }
    constexpr inline bool operator!=( const TypeID& cmp ) const { return !( *this == cmp ); }
    constexpr inline bool operator<( const TypeID& cmp ) const { return value < cmp.value; }

    constexpr static inline TypeID make_context( SubType objectID, SubType subObjectID = 0 )
    {
        return TypeID{ TypeID::make_context_impl( objectID, subObjectID, eType ) };
    }
    constexpr static inline TypeID make_object_from_typeID( TypeID typeID )
    {
        return TypeID{ TypeID::make_context_impl( typeID.getObjectID(), 0U, eType ) };
    }
    constexpr static inline TypeID make_object_from_objectID( SubType objectID )
    {
        return TypeID{ TypeID::make_context_impl( objectID, 0U, eType ) };
    }
    constexpr static inline TypeID make_start_state( TypeID typeID )
    {
        return TypeID{ TypeID::make_context_impl( typeID.getObjectID(), typeID.getSubObjectID(), eType ) };
    }
    constexpr static inline TypeID make_end_state( TypeID typeID )
    {
        return TypeID{ TypeID::make_context_impl( typeID.getObjectID(), typeID.getSubObjectID(), eSymbol ) };
    }

#ifndef MEGAJIT
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "symbolID", value );
        }
        else
        {
            archive& value;
        }
    }
#endif
};

#ifndef MEGAJIT
static_assert( sizeof( TypeID ) == 4U, "Invalid TypeID Size" );
#endif

static constexpr TypeID ROOT_SYMBOL_ID = TypeID( -1 );
static constexpr TypeID ROOT_TYPE_ID   = TypeID::make_context( 1, 0 );

} // namespace mega

#ifndef MEGAJIT
std::ostream& operator<<( std::ostream& os, const mega::TypeID& typeID );
std::istream& operator>>( std::istream& is, mega::TypeID& typeID );
#endif

#endif // GUARD_2023_January_09_type_id