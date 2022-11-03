
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

#ifndef GUARD_2022_November_02_snapshot
#define GUARD_2022_November_02_snapshot

#include "mega/reference.hpp"
#include "mega/reference_io.hpp"
#include "mega/native_types.hpp"

#include "common/assert_verify.hpp"

#include <boost/serialization/split_member.hpp>
#include <boost/serialization/array_wrapper.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>

#include <unordered_map>

namespace mega
{

class Snapshot
{
public:
    using Index      = U64;
    using IndexTable = std::unordered_map< reference, Index, reference::Hash >;
    using IndexArray = std::vector< reference >;

    Snapshot()
        : m_timeStamp( 0U )
    {
    }
    Snapshot( mega::TimeStamp timeStamp )
        : m_timeStamp( timeStamp )
    {
    }

    TimeStamp getTimeStamp() const { return m_timeStamp; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_timeStamp;
        archive& m_table;
        archive& m_array;
    }

    const Index& refToIndex( const reference& ref )
    {
        auto iFind = m_table.find( ref );
        if ( iFind == m_table.end() )
        {
            const Index index = m_table.size();
            m_array.push_back( ref );
            iFind = m_table.insert( { ref, index } ).first;
        }
        return iFind->second;
    }

    const reference& indexToRef( Index index ) const
    {
        ASSERT( index < m_array.size() );
        return m_array[ index ];
    }

    void remap( const Index& index, const reference& ref )
    {
        const auto existing = indexToRef( index );
        auto       iFind    = m_table.find( existing );
        ASSERT( iFind != m_table.end() );
        m_table.erase( iFind );
        m_table[ ref ]   = index;
        m_array[ index ] = ref;
    }

private:
    TimeStamp  m_timeStamp;
    IndexTable m_table;
    IndexArray m_array;
};

} // namespace mega

#endif // GUARD_2022_November_02_snapshot
