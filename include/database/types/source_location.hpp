
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

#ifndef GUARD_2023_July_28_source_location
#define GUARD_2023_July_28_source_location

#include "database/common/api.hpp"
#include "database/common/serialisation.hpp"

#include "mega/native_types.hpp"

namespace mega
{
class EGDB_EXPORT SourceLocation
{
public:
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_begin;
        archive& m_end;
    }

    SourceLocation() = default;
    SourceLocation( mega::U64 begin, mega::U64 end )
        : m_begin( begin )
        , m_end( end )
    {
    }

    inline mega::U64 getBegin() const { return m_begin; }
    inline mega::U64 getEnd() const { return m_end; }

    inline bool isValid() const { return m_begin < m_end; }

    inline bool contains( const SourceLocation& other ) const
    {
        return isValid() && other.isValid() && ( m_begin <= other.m_begin ) && ( other.m_end <= m_end );
    }

    inline bool operator==( const SourceLocation& other ) const
    {
        return ( m_begin == other.m_begin ) && ( m_end == other.m_end );
    }
    inline bool operator!=( const SourceLocation& other ) const { return !this->operator==( other ); }
    inline bool operator<( const SourceLocation& other ) const
    {
        return ( m_begin != other.m_begin ) ? ( m_begin < other.m_begin ) : ( m_end < other.m_end );
    }

private:
    mega::U64 m_begin = 0U;
    mega::U64 m_end   = 0U;
};
} // namespace mega

namespace mega
{
inline void to_json( nlohmann::json& j, const mega::SourceLocation& sourceLocation )
{
    j = nlohmann::json{ { "source_begin", sourceLocation.getBegin() }, { "source_end", sourceLocation.getEnd() } };
}
} // namespace mega

#endif // GUARD_2023_July_28_source_location
