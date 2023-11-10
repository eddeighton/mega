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

#include "mega/mangle/traits.hpp"
#include "mega/bitset_io.hpp"

#include <gtest/gtest.h>

TEST( Bitset, Basic )
{
    static constexpr auto size = 4;

    static const mega::U64 data[ size ] = {
        0b0101010101010101010101010101010101010101010101010101010101010101,
        0b0101010101010101010101010101010101010101010101010101010101010101,
        0b0101010101010101010101010101010101010101010101010101010101010101,
        0b0101010101010101010101010101010101010101010101010101010101010101
    };

    const mega::BitSet bitset( &data[ 0 ], &data[ 0 ] + size );

    for( int i = 0; i != size * 64; ++i )
    {
        if( i % 2 )
        {
            ASSERT_FALSE( bitset[ i ] );
        }
        else
        {
            ASSERT_TRUE( bitset[ i ] );
        }
    }
}

TEST( Bitset, Paranoia )
{
    static constexpr auto size = 4;

    static const mega::U64 data[ size ] = {
        0b0001000010000100001000010000100001000010000100001000010000100001,
        0b0010000100001000010000100001000010000100001000010000100001000010,
        0b0100001000010000100001000010000100001000010000100001000010000100,
        0b1000010000100001000010000100001000010000100001000010000100001000
    };

    const mega::BitSet bitset( &data[ 0 ], &data[ 0 ] + size );

    for( int i = 0; i != size * 64; ++i )
    {
        if( i % 5 )
        {
            ASSERT_FALSE( bitset[ i ] ) << i;
        }
        else
        {
            ASSERT_TRUE( bitset[ i ] ) << i;
        }
    }
}

TEST( Bitset, MoreParanoia )
{
    static constexpr auto size = 4;

    static const mega::U64 data[ 1 ] = { 0b0000000000000000000000000000000000000000000000000000000000000001 };

    const mega::BitSet bitset( &data[ 0 ], &data[ 0 ] + 1 );

    ASSERT_TRUE( bitset[ 0 ] );
    ASSERT_FALSE( bitset[ 1 ] );
    ASSERT_FALSE( bitset[ 63 ] );
}