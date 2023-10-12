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


#include <gtest/gtest.h>

#include "mega/ring_allocator.hpp"

#include <set>
#include <algorithm>
#include <random>

TEST( Allocator, RingReturnsZeroFirst )
{
    static constexpr mega::U64 CAPACITY = 16;
    using AllocatorType                   = mega::RingAllocator< mega::U64, CAPACITY >;

    AllocatorType alloc;
    ASSERT_TRUE( alloc.empty() );
    ASSERT_EQ( alloc.allocate(), 0U );
    ASSERT_TRUE( !alloc.full() );
}

TEST( Allocator, Ring )
{
    static constexpr mega::U64 CAPACITY = 16;
    using AllocatorType                   = mega::RingAllocator< mega::U64, CAPACITY >;

    AllocatorType alloc;
    ASSERT_TRUE( alloc.empty() );

    std::random_device randomDevice;
    std::mt19937       randNumGen( randomDevice() );

    std::set< mega::U64 > allocated;
    for ( int i = 0; i < CAPACITY; ++i )
    {
        mega::U64 s = alloc.allocate();

        ASSERT_TRUE( allocated.insert( s ).second );
    }
    ASSERT_TRUE( alloc.full() );

    std::vector< mega::U64 > shuffled( allocated.begin(), allocated.end() );
    std::shuffle( shuffled.begin(), shuffled.end(), randNumGen );

    for ( int i : shuffled )
    {
        ASSERT_TRUE( !alloc.empty() );
        alloc.free( i );
    }

    ASSERT_TRUE( alloc.empty() );
}

TEST( Allocator, Ring2 )
{
    static constexpr mega::U64 CAPACITY = 64;
    using AllocatorType                   = mega::RingAllocator< mega::U64, CAPACITY >;

    AllocatorType alloc;
    ASSERT_TRUE( alloc.empty() );

    std::random_device randomDevice;
    std::mt19937       randNumGen( randomDevice() );

    std::set< mega::U64 > allocated;

    for ( int j = 0; j < 5; ++j )
    {
        // allocate half
        {
            for ( int i = 0; ( i < CAPACITY / 2 ) && !alloc.full(); ++i )
            {
                mega::U64 s = alloc.allocate();
                ASSERT_TRUE( allocated.insert( s ).second );
            }
        }

        // remove half
        {
            std::vector< mega::U64 > shuffled( allocated.begin(), allocated.end() );
            std::shuffle( shuffled.begin(), shuffled.end(), randNumGen );

            for ( int i = 0; i < CAPACITY / 2; ++i )
            {
                ASSERT_TRUE( !alloc.empty() );
                mega::U64 v = shuffled[ i ];
                alloc.free( v );
                allocated.erase( v );
            }
        }
    }

    for ( int i : allocated )
    {
        ASSERT_TRUE( !alloc.empty() );
        alloc.free( i );
    }
    ASSERT_TRUE( alloc.empty() );
}