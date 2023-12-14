
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
#include "mega/values/compilation/type_id.h"

#include "common/serialisation.hpp"
#include <limits>
#include <ostream>

namespace mega
{
/*
class TypeID : public c_type_id
{
public:

    // static constexpr inline ContextID make_context_impl( SubType objectID, SubType subObjectID, Flag flag )
    // {
    //     return ContextID{ subObjectID, objectID, flag };
    // }

    // make data members public so can use as compile time parameter
public:
private:
    constexpr TypeID( c_type_id _contextID )
        : c_type_id{ _contextID }
    {
    }

public:

    // struct Hash
    // {
    //     inline U64 operator()( const TypeID& ref ) const noexcept { return ref.getSymbolID(); }
    // };

    constexpr TypeID()
        : c_type_id{ 0U }
    {
    }

    // constexpr explicit TypeID( ValueType _value )
    //     : c_type_id{ _value }
    // {
    // }

    constexpr TypeID( const TypeID& cpy ) = default;

    // constexpr inline bool isSymbolID() const { return contextID.flag == eSymbol; }
    // constexpr inline bool isContextID() const { return contextID.flag == eType; }
    // constexpr inline bool isObject() const { return isContextID() && contextID.subObject == 0; }
    // constexpr inline ValueType getSymbolID() const { return value; }
    // constexpr inline SubType   getObjectID() const { return contextID.object; }
    // constexpr inline SubType   getSubObjectID() const { return contextID.subObject; }
    // constexpr inline operator ValueType() const { return value; }
    // constexpr inline bool     valid() const { return value != 0U; }
    // constexpr inline bool operator==( const TypeID& cmp ) const { return value == cmp.value; }
    // constexpr inline bool operator!=( const TypeID& cmp ) const { return !( *this == cmp ); }
    // constexpr inline bool operator<( const TypeID& cmp ) const { return value < cmp.value; }

    // constexpr static inline TypeID make_context( SubType objectID, SubType subObjectID = 0 )
    // {
    //     return TypeID{ TypeID::make_context_impl( objectID, subObjectID, eType ) };
    // }
    // constexpr static inline TypeID make_object_from_typeID( TypeID typeID )
    // {
    //     return TypeID{ TypeID::make_context_impl( typeID.getObjectID(), 0U, eType ) };
    // }
    // constexpr static inline TypeID make_object_from_objectID( SubType objectID )
    // {
    //     return TypeID{ TypeID::make_context_impl( objectID, 0U, eType ) };
    // }
    // constexpr static inline TypeID make_start_state( TypeID typeID )
    // {
    //     return TypeID{ TypeID::make_context_impl( typeID.getObjectID(), typeID.getSubObjectID(), eType ) };
    // }
    // constexpr static inline TypeID make_end_state( TypeID typeID )
    // {
    //     return TypeID{ TypeID::make_context_impl( typeID.getObjectID(), typeID.getSubObjectID(), eSymbol ) };
    // }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        // if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        // {
        //     archive& boost::serialization::make_nvp( "symbolID", value );
        // }
        // else
        // {
        //     archive& value;
        // }
    }
};

static_assert( sizeof( TypeID ) == 4U, "Invalid TypeID Size" );
std::ostream& operator<<( std::ostream& os, const mega::TypeID& typeID );
std::istream& operator>>( std::istream& is, mega::TypeID& typeID );

// static constexpr TypeID NULL_SYMBOL_ID = TypeID( 0 );
// static constexpr TypeID NULL_TYPE_ID   = TypeID::make_context( 0, 0 );
// static constexpr TypeID ROOT_SYMBOL_ID = TypeID( -1 );
// static constexpr TypeID ROOT_TYPE_ID   = TypeID::make_context( 1, 0 );
*/
} // namespace mega

#endif // GUARD_2023_January_09_type_id
