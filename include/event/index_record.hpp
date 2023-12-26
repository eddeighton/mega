
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

#ifndef GUARD_2022_October_01_index
#define GUARD_2022_October_01_index

#include "event/offset.hpp"
#include "event/records.hxx"

#include <utility>
#include <vector>
#include <string_view>
#include <string>
#include <array>

namespace mega::event
{

class IndexRecord
{
    static_assert( toInt( TrackID::TOTAL ) == 8, "Unexpected number of tracks" );

public:

    template< TrackID trackType >
    inline const Offset& get() const { return m_offsets[ toInt( trackType ) ]; }

    inline const Offset& get( TrackID track ) const { return m_offsets[ toInt( track ) ]; }
    inline Offset&       get( TrackID track ) { return m_offsets[ toInt( track ) ]; }

    inline bool operator==( const IndexRecord& cmp ) const { return m_offsets == cmp.m_offsets; }
    inline bool operator!=( const IndexRecord& cmp ) const { return m_offsets != cmp.m_offsets; }
    inline bool operator<( const IndexRecord& cmp ) const { return m_offsets < cmp.m_offsets; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        for( auto i = 0; i != toInt( TrackID::TOTAL ); ++i )
        {
            archive& m_offsets[ i ];
        }
    }

private:
    std::array< Offset, toInt( TrackID::TOTAL ) > m_offsets;
};

} // namespace mega::event

#endif // GUARD_2022_October_01_index
