
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

#ifndef GUARD_2023_December_21_icontext_flags
#define GUARD_2023_December_21_icontext_flags

#include "common/serialisation.hpp"

#include <bitset>

namespace mega
{
class IContextFlags
{
public:
    enum Value
    {
        eMeta,
        eOR,
        eHistorical,
        TOTAL_FLAGS
    };
    using Bits = std::bitset< TOTAL_FLAGS >;

    static const char* str( Value bit );
    static bool        isDirect( Value bit );

    static const std::array< std::string, TOTAL_FLAGS >& strings();

    std::string str() const;
    inline bool get( Value bit ) const { return m_value.test( bit ); }
    inline void set( Value bit ) { m_value.set( bit ); }
    inline void reset( Value bit ) { m_value.reset( bit ); }

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

private:
    Bits m_value;
};
std::ostream& operator<<( std::ostream& os, IContextFlags iContextFlags );
} // namespace mega

#endif // GUARD_2023_December_21_icontext_flags
