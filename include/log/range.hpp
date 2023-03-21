
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

#ifndef GUARD_2023_March_21_range
#define GUARD_2023_March_21_range

#include "mega/native_types.hpp"

namespace mega::log
{

struct TrackRange
{
    using RawPtr   = U64;
    RawPtr m_begin = {}, m_end = {}, m_begin2 = {}, m_end2 = {};

    TrackRange() = default;
    TrackRange( const TrackRange& ) = default;
    TrackRange( TrackRange&& ) = default;
    TrackRange& operator=( const TrackRange& ) = default;

    TrackRange( const void* pBegin, const void* pEnd, const void* pBegin2, const void* pEnd2 )
        : m_begin( reinterpret_cast< RawPtr >( pBegin ) )
        , m_end( reinterpret_cast< RawPtr >( pEnd ) )
        , m_begin2( reinterpret_cast< RawPtr >( pBegin2 ) )
        , m_end2( reinterpret_cast< RawPtr >( pEnd2 ) )

    {
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        archive& m_begin;
        archive& m_end;
        archive& m_begin2;
        archive& m_end2;
    }
};

struct Range
{
    TrackRange m_structure, m_schedule, m_memory;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        archive& m_structure;
        archive& m_schedule;
        archive& m_memory;
    }
};

} // namespace mega::log

#endif // GUARD_2023_March_21_range
