
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

#ifndef GUARD_2023_December_12_concrete_type_id_instance
#define GUARD_2023_December_12_concrete_type_id_instance

#include "mega/values/compilation/concrete/inline.h"

#include "mega/values/compilation/concrete/type_id.hpp"
#include "mega/values/compilation/concrete/instance.hpp"

#include "mega/values/native_types.hpp"

#include "common/serialisation.hpp"

#include <limits>
#include <ostream>
#include <iomanip>

namespace mega::concrete
{

class TypeIDInstance : public c_concrete_type_id_instance
{
public:
    struct Hash
    {
        inline U64 operator()( const TypeIDInstance& ref ) const noexcept
        {
            return ref.getTypeID().getValue() + ref.getInstance().getValue();
        }
    };

    constexpr inline TypeIDInstance()
        : c_concrete_type_id_instance(
            c_concrete_type_id_instance_make( NULL_TYPE_ID.getValue(), INSTANCE_ZERO.getValue() ) )
    {
    }
    constexpr inline explicit TypeIDInstance( c_concrete_type_id_instance value )
        : c_concrete_type_id_instance( value )
    {
    }
    constexpr inline explicit TypeIDInstance( TypeID typeID, Instance instance )
        : c_concrete_type_id_instance( c_concrete_type_id_instance_make( typeID.getValue(), instance.getValue() ) )
    {
    }

    constexpr inline TypeIDInstance( const TypeIDInstance& cpy ) = default;

    constexpr inline TypeID   getTypeID() const { return TypeID{ type_id }; }
    constexpr inline Instance getInstance() const { return Instance{ instance }; }

    constexpr inline bool operator==( const TypeIDInstance& cmp ) const
    {
        return getTypeID() == cmp.getTypeID() && getInstance() == cmp.getInstance();
    }
    constexpr inline bool operator!=( const TypeIDInstance& cmp ) const { return !this->operator==( cmp ); }
    constexpr inline bool operator<( const TypeIDInstance& cmp ) const
    {
        return ( getTypeID() != cmp.getTypeID() ) ? ( getTypeID() < cmp.getTypeID() )
                                                  : ( getInstance() < cmp.getInstance() );
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "concrete_type_id_object_id", type_id.object_id.value );
            archive& boost::serialization::make_nvp( "concrete_type_id_sub_object_id", type_id.sub_object_id.value );
            archive& boost::serialization::make_nvp( "concrete_instance", instance.value );
        }
        else
        {
            archive& type_id.object_id.value;
            archive& type_id.sub_object_id.value;
            archive& instance.value;
        }
    }
};

static_assert( sizeof( TypeIDInstance ) == 6, "Invalid Concrete ObjectID Size" );

inline std::ostream& operator<<( std::ostream& os, const TypeIDInstance& value )
{
    return os << value.getTypeID() << value.getInstance();
}

} // namespace mega::concrete

#endif // GUARD_2023_December_12_concrete_type_id_instance
