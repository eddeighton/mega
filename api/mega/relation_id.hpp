
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

#ifndef GUARD_2023_January_19_relation_id
#define GUARD_2023_January_19_relation_id

#include "mega/type_id.hpp"

namespace mega
{

struct RelationID
{
public:
    constexpr inline RelationID()                    = default;
    constexpr inline RelationID( const RelationID& ) = default;
    constexpr inline RelationID( TypeID lower, TypeID upper )
        : m_lower( ( lower < upper ) ? lower : upper )
        , m_upper( ( lower < upper ) ? upper : lower )
    {
    }

    constexpr inline bool operator==( const RelationID& cmp ) const
    {
        return m_lower == cmp.m_lower && m_upper == cmp.m_upper;
    }
    constexpr inline bool operator!=( const RelationID& cmp ) const { return !this->operator==( cmp ); }
    constexpr inline bool operator<( const RelationID& cmp ) const
    {
        return ( m_lower != cmp.m_lower ) ? ( m_lower < cmp.m_lower ) : ( m_upper < cmp.m_upper );
    }

    TypeID getLower() const { return m_lower; }
    TypeID getUpper() const { return m_upper; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_lower;
        archive& m_upper;
    }
    struct Hash
    {
        inline U64 operator()( const RelationID& id ) const noexcept
        {
            return id.m_lower + ( id.m_upper << sizeof( TypeID ) );
        }
    };

private:
    TypeID m_lower = TypeID{}, m_upper = TypeID{};
};

} // namespace mega

#endif // GUARD_2023_January_19_relation_id