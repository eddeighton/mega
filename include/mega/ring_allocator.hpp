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

#ifndef EG_ALLOCATORS_GUARD_20_JULY_2020
#define EG_ALLOCATORS_GUARD_20_JULY_2020

#include <array>
#include <bitset>
#include <cstddef>

#include "mega/values/native_types.hpp"
#include "mega/values/runtime/reference.hpp"
#include "mega/clock.hpp"

#include <boost/circular_buffer.hpp>

#include <vector>

#ifdef __gnu_linux__
#include <string.h>
#endif

#ifdef _WIN32
#include <intrin.h>
#endif

namespace mega
{
/*
template < mega::U64 _Size >
class Bitmask32Allocator
{
public:
    static const mega::U64 Size = _Size;

    const std::bitset< Size >& data() const { return m_bitset; }

    inline Bitmask32Allocator()
    {
        static_assert( Size <= ( sizeof( mega::U32 ) * 8 ) );
        m_bitset.set();
    }

    inline Instance nextFree() const
    {
        const mega::U64 mask = m_bitset.to_ulong();

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
            return Size;
        }
#endif
    }

    inline Instance allocate()
    {
        const Instance next = nextFree();
        allocate( next );
        return next;
    }
    inline void allocate( Instance instance ) { m_bitset.reset( instance ); }
    inline void free( Instance instance ) { m_bitset.set( instance ); }
    inline bool empty() const { return m_bitset.all(); }
    inline bool full() const { return m_bitset.none(); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        // archive& m_bitset;
    }

private:
    std::bitset< Size > m_bitset;
};

template < mega::U64 _Size >
class Bitmask64Allocator
{
public:
    static const mega::U64 Size = _Size;

    const std::bitset< Size >& data() const { return m_bitset; }

    inline Bitmask64Allocator()
    {
        static_assert( Size >= 32 );
        static_assert( Size < 64 );
        m_bitset.set();
    }

    inline Instance nextFree() const
    {
        const mega::U64 mask = m_bitset.to_ullong();

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

    inline Instance allocate()
    {
        Instance next = nextFree();
        allocate( next );
        return next;
    }
    inline void allocate( Instance instance ) { m_bitset.reset( instance ); }
    inline void free( Instance instance ) { m_bitset.set( instance ); }
    inline bool empty() const { return m_bitset.all(); }
    inline bool full() const { return m_bitset.none(); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        // const mega::U64 mask = m_bitset.to_ullong();
        // archive& m_bitset;
    }

private:
    std::bitset< Size > m_bitset;
};*/

template < typename TInstanceType, mega::U64 _Size >
class RingAllocator
{
public:
    using FreeList                  = boost::circular_buffer< TInstanceType >;
    using InstanceType              = TInstanceType;
    static const mega::U64 Size     = _Size;

    inline RingAllocator()
        : m_free( Size )
    {
        reset();
    }

    const FreeList& data() const { return m_free; }

    inline void reset()
    {
        m_free.clear();
        for ( mega::U64 sz = 0; sz != Size; ++sz )
        {
            m_free.push_back( sz );
        }
    }

    inline bool     empty() const { return m_free.full(); }
    inline bool     full() const { return m_free.empty(); }
    const FreeList& getFree() const { return m_free; }

    inline InstanceType allocate()
    {
        InstanceType result = m_free.front();
        m_free.pop_front();
        return result;
    }

    inline void free( InstanceType instance ) { m_free.push_front( instance ); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        // archive& m_free;
    }

    inline std::vector< InstanceType > getAllocated() const
    {
        std::vector< InstanceType > allocated;
        std::vector< InstanceType > freeOwners( m_free.begin(), m_free.end() );
        std::sort( freeOwners.begin(), freeOwners.end() );
        auto iter = freeOwners.begin();
        // calculate the inverse of the free
        for ( mega::U64 id = 0U; id != Size; ++id )
        {
            if ( ( iter != freeOwners.end() ) && ( *iter == id ) )
            {
                ++iter;
            }
            else
            {
                allocated.push_back( id );
            }
        }
        return allocated;
    }

private:
    FreeList m_free;
};
/*
template < Instance _Size >
using InstanceAllocator = RingAllocator< Instance, _Size >;
*/
} // namespace mega

#endif // EG_ALLOCATORS_GUARD_20_JULY_2020
