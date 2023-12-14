
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

#ifndef GUARD_2023_sub_type_instance
#define GUARD_2023_sub_type_instance

#include "mega/values/native_types.hpp"

#include "mega/values/compilation/sub_type_instance.h"

#include "mega/values/compilation/instance.hpp"
#include "mega/values/compilation/type_id_concrete.hpp"

#include "common/serialisation.hpp"

#include <limits>
#include <ostream>
#include <istream>

namespace mega
{

#pragma pack( 1 )
class SubTypeInstance : public c_sub_type_instance
{
public:
    using ValueType = U32;

private:

public:
    struct Hash
    {
        //inline U64 operator()( const SubTypeInstance& st ) const noexcept { return st.getValue(); }
    };

    SubTypeInstance()
        :   c_sub_type_instance{ 0, 0 }
    {
    }

    explicit SubTypeInstance( concrete::SubObjectID subType, Instance instance )
    {
    }
    explicit SubTypeInstance( ValueType value )
    {
    }

    // inline operator ValueType() const { return m_value; }

    // inline bool operator==( const SubTypeInstance& cmp ) const { return m_value == cmp.m_value; }
    // inline bool operator!=( const SubTypeInstance& cmp ) const { return !this->operator==( cmp ); }
    // inline bool operator<( const SubTypeInstance& cmp ) const { return m_value < cmp.m_value; }

    // inline ValueType getValue() const { return m_value; }
    // inline SubType   getSubType() const { return m_data.subType; }
    // inline Instance  getInstance() const { return m_data.instance; }

    // void setSubType( SubType subType ) { m_data.subType = subType; }
    // void setInstance( Instance instance ) { m_data.instance = instance; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            //archive& boost::serialization::make_nvp( "subTypeInstance", m_value );
        }
        else
        {
            //archive& m_value;
        }
    }
};
#pragma pack()

static_assert( sizeof( SubTypeInstance ) == 4U, "Invalid SubTypeInstance Size" );
// std::ostream& operator<<( std::ostream& os, const mega::SubTypeInstance& subTypeInstance );
// std::istream& operator>>( std::istream& is, mega::SubTypeInstance& subTypeInstance );

} // namespace mega

#endif // GUARD_2023_sub_type_instance
