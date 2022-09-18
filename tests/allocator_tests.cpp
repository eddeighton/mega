
#include <gtest/gtest.h>

#include "mega/allocator.hpp"

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