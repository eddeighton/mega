//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#ifndef EG_ALLOCATORS_GUARD_20_JULY_2020
#define EG_ALLOCATORS_GUARD_20_JULY_2020

#include <array>
#include <bitset>
#include <cstddef>

#include "common.hpp"
#include "clock.hpp"

#include "boost/circular_buffer.hpp"

#ifdef __gnu_linux__
#include <string.h>
#endif

#ifdef _WIN32
#include <intrin.h>
#endif

namespace mega
{

template < typename T >
struct DimensionTraits;

template < std::size_t _Size >
class Bitmask32Allocator
{
    friend struct mega::DimensionTraits< mega::Bitmask32Allocator< _Size > >;

public:
    static const std::size_t Size = _Size;

    inline Bitmask32Allocator()
    {
        static_assert( Size <= ( sizeof( std::uint32_t ) * 8 ) );
        m_bitset.set();
    }

    inline Instance nextFree() const
    {
        unsigned long index;
        if ( _BitScanForward( &index, m_bitset.to_ulong() ) )
        {
            return index;
        }
        else
        {
            return Size;
        }
    }

    inline void allocate( Instance instance ) { m_bitset.reset( instance ); }

    inline void free( Instance instance ) { m_bitset.set( instance ); }

    inline bool empty() const { return m_bitset.all(); }
    inline bool full() const { return m_bitset.none(); }

private:
    std::bitset< Size > m_bitset;
};

template < std::size_t _Size >
class Bitmask64Allocator
{
    friend struct mega::DimensionTraits< mega::Bitmask64Allocator< _Size > >;

public:
    static const std::size_t Size = _Size;

    inline Bitmask64Allocator()
    {
        static_assert( Size > sizeof( std::uint32_t ) * 8 );
        static_assert( Size <= sizeof( std::uint64_t ) * 8 );
        m_bitset.set();
    }

    inline Instance nextFree() const
    {
        const std::uint64_t mask = m_bitset.to_ullong();

#ifdef __gnu_linux__
        return ffsll( mask );
#endif

#ifdef _WIN32
        unsigned long index;

        if ( _BitScanForward( &index, mask ) )
        {
            return index;
        }
        else
        {
            if ( _BitScanForward( &index, mask >> 32 ) )
            {
                return index + 32U;
            }
            else
            {
                return Size;
            }
        }
#endif
    }

    inline void allocate( Instance instance ) { m_bitset.reset( instance ); }

    inline void free( Instance instance ) { m_bitset.set( instance ); }

    inline bool empty() const { return m_bitset.all(); }
    inline bool full() const { return m_bitset.none(); }

private:
    std::bitset< Size > m_bitset;
};

template < typename TInstanceType, TInstanceType _Size >
class RingAllocator
{
    friend struct mega::DimensionTraits< mega::RingAllocator< TInstanceType, _Size > >;

    using FreeList = boost::circular_buffer< TInstanceType >;

public:
    using InstanceType              = TInstanceType;
    static const TInstanceType Size = _Size;

    inline RingAllocator()
        : m_free( Size )
    {
        reset();
    }

    inline void reset()
    {
        m_free.clear();
        for ( InstanceType sz = 0; sz != Size; ++sz )
        {
            m_free.push_back( sz );
        }
    }

    inline bool empty() const { return m_free.full(); }
    inline bool full() const { return m_free.empty(); }

    inline InstanceType allocate()
    {
        InstanceType result = m_free.front();
        m_free.pop_front();
        return result;
    }

    inline void free( InstanceType instance ) 
    { 
        m_free.push_front( instance ); 
    }

private:
    FreeList m_free;
};

} // namespace mega

#endif // EG_ALLOCATORS_GUARD_20_JULY_2020
