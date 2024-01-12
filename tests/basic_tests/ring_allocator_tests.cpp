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
#include <limits>

static constexpr auto MAX_PROCESS_PER_MACHINE = std::numeric_limits< runtime::ProcessID::ValueType >::max();
static constexpr auto MAX_OWNER_PER_PROCESS   = std::numeric_limits< runtime::OwnerID::ValueType >::max();

using ProcessAllocator = RingAllocator< mega::runtime::ProcessID, MAX_PROCESS_PER_MACHINE >;
using OwnerAllocator   = RingAllocator< mega::runtime::OwnerID, MAX_OWNER_PER_PROCESS >;

TEST( Allocator, RingReturnsZeroFirst )
{
    using AllocatorType = OwnerAllocator;

    AllocatorType alloc;
    ASSERT_TRUE( alloc.empty() );
    ASSERT_EQ( alloc.allocate(), mega::runtime::OwnerID{} );
    ASSERT_TRUE( !alloc.full() );
}

TEST( Allocator, Ring )
{
    using AllocatorType = OwnerAllocator;

    AllocatorType alloc;
    ASSERT_TRUE( alloc.empty() );

    std::random_device randomDevice;
    std::mt19937       randNumGen( randomDevice() );

    std::set< mega::runtime::OwnerID > allocated;
    for( int i = 0; i < MAX_OWNER_PER_PROCESS; ++i )
    {
        mega::runtime::OwnerID s = alloc.allocate();
        ASSERT_TRUE( allocated.insert( s ).second );
    }
    ASSERT_TRUE( alloc.full() );

    std::vector< mega::U64 > shuffled( allocated.begin(), allocated.end() );
    std::shuffle( shuffled.begin(), shuffled.end(), randNumGen );

    for( auto i : shuffled )
    {
        ASSERT_TRUE( !alloc.empty() );
        alloc.free( i );
    }
    ASSERT_TRUE( alloc.empty() );
}

TEST( Allocator, Ring2 )
{
    using AllocatorType = OwnerAllocator;

    AllocatorType alloc;
    ASSERT_TRUE( alloc.empty() );

    std::random_device randomDevice;
    std::mt19937       randNumGen( randomDevice() );

    std::set< mega::U64 > allocated;

    for( int j = 0; j < 5; ++j )
    {
        // allocate half
        {
            for( int i = 0; ( i < MAX_OWNER_PER_PROCESS / 2 ) && !alloc.full(); ++i )
            {
                auto s = alloc.allocate();
                ASSERT_TRUE( allocated.insert( s ).second );
            }
        }

        // remove half
        {
            std::vector< mega::U64 > shuffled( allocated.begin(), allocated.end() );
            std::shuffle( shuffled.begin(), shuffled.end(), randNumGen );

            for( int i = 0; i < MAX_OWNER_PER_PROCESS / 2; ++i )
            {
                ASSERT_TRUE( !alloc.empty() );
                auto v = shuffled[ i ];
                alloc.free( v );
                allocated.erase( v );
            }
        }
    }

    for( auto i : allocated )
    {
        ASSERT_TRUE( !alloc.empty() );
        alloc.free( i );
    }
    ASSERT_TRUE( alloc.empty() );
}