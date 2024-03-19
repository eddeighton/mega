
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

#ifndef GUARD_2023_December_11_timestamp
#define GUARD_2023_December_11_timestamp

#include "mega/values/runtime/inline.h"

#include "mega/values/native_types.hpp"

#include "common/serialisation.hpp"

#include <limits>
#include <ostream>

namespace mega::runtime
{
class TimeStamp : public c_time_stamp
{
public:
    using ValueType = U32;

    struct Hash
    {
        inline U64 operator()( const TimeStamp& t ) const noexcept { return t.getValue(); }
    };

    constexpr TimeStamp()
        : c_time_stamp{ 0 }
    {
    }

    explicit TimeStamp( ValueType _value )
        : c_time_stamp{ _value }
    {
    }

    constexpr TimeStamp( const TimeStamp& cpy ) = default;

    inline ValueType getValue() const { return m_value; }

    inline      operator ValueType() const { return getValue(); }
    inline bool valid() const { return getValue() != 0; }

    inline bool operator==( const TimeStamp& cmp ) const { return getValue() == cmp.getValue(); }
    inline bool operator!=( const TimeStamp& cmp ) const { return !( *this == cmp ); }
    inline bool operator<( const TimeStamp& cmp ) const { return getValue() < cmp.getValue(); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "value", m_value );
        }
        else
        {
            archive& m_value;
        }
    }
};
} // namespace mega::runtime

#endif // GUARD_2023_December_11_timestamp
