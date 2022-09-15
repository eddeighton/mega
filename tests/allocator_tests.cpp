
#include <gtest/gtest.h>

#include "mega/allocator.hpp"

#include <set>
#include <algorithm>
#include <random>

TEST( Allocator, RingReturnsZeroFirst )
{
    static constexpr std::size_t CAPACITY = 16;
    using AllocatorType                   = mega::RingAllocator< std::size_t, CAPACITY >;

    AllocatorType alloc;
    ASSERT_TRUE( alloc.empty() );
    ASSERT_EQ( alloc.allocate(), 0U );
    ASSERT_TRUE( !alloc.full() );
}

TEST( Allocator, Ring )
{
    static constexpr std::size_t CAPACITY = 16;
    using AllocatorType                   = mega::RingAllocator< std::size_t, CAPACITY >;

    AllocatorType alloc;
    ASSERT_TRUE( alloc.empty() );

    std::random_device randomDevice;
    std::mt19937       randNumGen( randomDevice() );

    std::set< std::size_t > allocated;
    for ( int i = 0; i < CAPACITY; ++i )
    {
        std::size_t s = alloc.allocate();

        ASSERT_TRUE( allocated.insert( s ).second );
    }
    ASSERT_TRUE( alloc.full() );

    std::vector< std::size_t > shuffled( allocated.begin(), allocated.end() );
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
    static constexpr std::size_t CAPACITY = 64;
    using AllocatorType                   = mega::RingAllocator< std::size_t, CAPACITY >;

    AllocatorType alloc;
    ASSERT_TRUE( alloc.empty() );

    std::random_device randomDevice;
    std::mt19937       randNumGen( randomDevice() );

    std::set< std::size_t > allocated;

    for ( int j = 0; j < 5; ++j )
    {
        // allocate half
        {
            for ( int i = 0; ( i < CAPACITY / 2 ) && !alloc.full(); ++i )
            {
                std::size_t s = alloc.allocate();
                ASSERT_TRUE( allocated.insert( s ).second );
            }
        }

        // remove half
        {
            std::vector< std::size_t > shuffled( allocated.begin(), allocated.end() );
            std::shuffle( shuffled.begin(), shuffled.end(), randNumGen );

            for ( int i = 0; i < CAPACITY / 2; ++i )
            {
                ASSERT_TRUE( !alloc.empty() );
                std::size_t v = shuffled[ i ];
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