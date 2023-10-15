
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

#include "mega/native_types.hpp"
#include "mega/type_id.hpp"

#ifndef MEGAJIT
#include <limits>
namespace boost::serialization
{
};
#endif

namespace mega
{

#pragma pack( 1 )
class SubTypeInstance
{
public:
    using ValueType = U32;

private:
    struct Data
    {
        U16 instance;
        U16 subType;
    };

    union
    {
        Data      m_data;
        ValueType m_value;
    };

public:
    struct Hash
    {
        inline U64 operator()( const SubTypeInstance& st ) const noexcept { return st.getValue(); }
    };

    SubTypeInstance()
        : m_value{ 0U }
    {
    }

    constexpr SubTypeInstance( SubType subType, Instance instance )
        : m_data{ instance, subType }
    {
    }
    constexpr SubTypeInstance( ValueType value )
        : m_value{ value }
    {
    }

    constexpr inline operator ValueType() const { return m_value; }

    constexpr inline bool operator==( const SubTypeInstance& cmp ) const { return m_value == cmp.m_value; }
    constexpr inline bool operator!=( const SubTypeInstance& cmp ) const { return !this->operator==( cmp ); }
    constexpr inline bool operator<( const SubTypeInstance& cmp ) const { return m_value < cmp.m_value; }

    constexpr inline ValueType getValue() const { return m_value; }
    constexpr inline SubType   getSubType() const { return m_data.subType; }
    constexpr inline Instance  getInstance() const { return m_data.instance; }

    void setSubType( SubType subType ) { m_data.subType = subType; }
    void setInstance( Instance instance ) { m_data.instance = instance; }

#ifndef MEGAJIT
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        using namespace boost::serialization;
        serialize( archive, *this, version );
    }
#endif
};
#pragma pack()

#ifndef MEGAJIT
static_assert( sizeof( SubTypeInstance ) == 4U, "Invalid SubTypeInstance Size" );
#endif

} // namespace mega

#endif // GUARD_2023_sub_type_instance
