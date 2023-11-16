
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

#include "mega/values/native_types.hpp"

#include "mega/values/compilation/type_id.hpp"
#include "mega/values/compilation/sub_type_instance.hpp"

#ifndef MEGAJIT
#include <limits>
#include <ostream>
#include <istream>
#endif

namespace mega
{

static constexpr const char* ROOT_TYPE_NAME = "Root";

#pragma pack( 1 )
struct TypeInstance
{
    TypeID   type     = {};
    Instance instance = 0U;

    TypeInstance() = default;

    constexpr explicit TypeInstance( TypeID _type, Instance _instance )
        : type( _type )
        , instance( _instance )
    {
    }
    constexpr explicit TypeInstance( TypeID::ValueType _type, Instance _instance )
        : type( _type )
        , instance( _instance )
    {
    }

    constexpr explicit TypeInstance( SubType objectID, SubTypeInstance subTypeInstance )
        : type( TypeID::make_context( objectID, subTypeInstance.getSubType() ) )
        , instance( subTypeInstance.getInstance() )
    {
    }

    static constexpr TypeInstance make_object( TypeID type )
    {
        return TypeInstance{ TypeID::make_object_from_typeID( type ), 0 };
    }
    static constexpr TypeInstance make_root() { return make_object( ROOT_TYPE_ID ); }

    constexpr inline bool operator==( const TypeInstance& cmp ) const
    {
        return ( type == cmp.type ) && ( instance == cmp.instance );
    }
    constexpr inline bool operator!=( const TypeInstance& cmp ) const { return !( *this == cmp ); }
    constexpr inline bool operator<( const TypeInstance& cmp ) const
    {
        return ( type != cmp.type )           ? ( type < cmp.type )
               : ( instance != cmp.instance ) ? ( instance < cmp.instance )
                                              : false;
    }

    constexpr inline bool valid() const { return type.valid(); }

#ifndef MEGAJIT
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "type", type );
            archive& boost::serialization::make_nvp( "instance", instance );
        }
        else
        {
            archive& type;
            archive& instance;
        }
    }
#endif
};
#pragma pack()

#ifndef MEGAJIT
static_assert( sizeof( TypeInstance ) == 6U, "Invalid TypeInstance Size" );
std::ostream& operator<<( std::ostream& os, const mega::TypeInstance& subTypeInstance );
std::istream& operator>>( std::istream& is, mega::TypeInstance& subTypeInstance );
#endif

} // namespace mega

#endif // GUARD_2023_January_07_type_instance
