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

#include "mega/values/runtime/pointer.hpp"

#include <limits>
#include <sstream>

TEST( Pointer, Basic )
{
    using namespace mega;

    Pointer p;
}

/*
mega::MP fromStr( const std::string& str )
{
    std::istringstream is( str );
    mega::MP           mp;
    is >> mp;
    return mp;
}

std::string toStr( mega::MP mp )
{
    std::ostringstream os;
    os << mp;
    return os.str();
}

TEST( Pointer, MPIO )
{
    // clang-format off
    ASSERT_EQ( fromStr( "0.0" ), mega::MP( 0, 0 ) );
    ASSERT_EQ( fromStr( "0.1" ), mega::MP( 0, 1 ) );
    ASSERT_EQ( fromStr( "0.2" ), mega::MP( 0, 2 ) );
    ASSERT_EQ( fromStr( "1.0" ), mega::MP( 1, 0 ) );
    ASSERT_EQ( fromStr( "2.0" ), mega::MP( 2, 0 ) );
    // clang-format on
}

TEST( Pointer, InvalidByDefault )
{
    mega::Pointer def{};
    ASSERT_EQ( def.isNetworkAddress(), true );
    ASSERT_FALSE( def.valid() );
}

TEST( Pointer, HeapAccess )
{
    mega::Pointer h{ mega::TypeInstance{ mega::max_typeID_context, 123 }, ( mega::HeapAddress )0xFFFFFFFF };

    ASSERT_TRUE( h.isHeapAddress() );
    ASSERT_EQ( h.getType(), mega::max_typeID_context );
    ASSERT_EQ( h.getInstance(), 123 );
    ASSERT_EQ( h.getHeap(), ( mega::HeapAddress )0xFFFFFFFF );
}

TEST( Pointer, NetAccess )
{
    using namespace mega;
    mega::MPO       testMPO( 1, 2, 3 );
    mega::Pointer h{ mega::TypeInstance{ mega::max_typeID_context, 123 }, testMPO, 4 };

    ASSERT_TRUE( h.isNetworkAddress() );
    ASSERT_EQ( h.getType(), mega::max_typeID_context );
    ASSERT_EQ( h.getInstance(), 123 );
    ASSERT_EQ( h.getMPO(), testMPO );
    ASSERT_EQ( h.getAllocationID(), 4 );
}

TEST( Pointer, HeaderAccess )
{
    mega::TypeInstance     typeInstance{ mega::max_typeID_context, 123 };
    mega::MPO              testMPO( 1, 2, 3 );
    mega::Pointer        networkAddress{ typeInstance, testMPO, 4 };
    mega::ObjectHeaderBase header{ networkAddress, 1U, 9 };

    mega::Pointer h{ typeInstance, &header };

    ASSERT_TRUE( h.isHeapAddress() );
    ASSERT_EQ( h.getMPO(), testMPO );
    ASSERT_EQ( h.getAllocationID(), 4 );
    ASSERT_EQ( h.getRefCount(), 9 );
    ASSERT_EQ( h.getLockCycle(), 1 );
}*/
/*
// struct OtherTypeInstance
// {
//     mega::Instance        instance = 0U;
//     mega::TypeID          type     = mega::min_typeID_context;
//     constexpr inline bool operator==( const OtherTypeInstance& cmp ) const
//     {
//         return ( instance == cmp.instance ) && ( type == cmp.type );
//     }
// };
// inline std::ostream& operator<<( std::ostream& os, const OtherTypeInstance& typeInstance )
// {
//     return os << '[' << typeInstance.type << "." << std::hex << typeInstance.instance << ']';
// }
// inline std::istream& operator>>( std::istream& is, OtherTypeInstance& typeInstance )
// {
//     char c;
//     return is >> c >> typeInstance.type >> c >> std::hex >> typeInstance.instance >> c;
// }
//
// TEST( Pointer, TypeInstance )
// {
//     const OtherTypeInstance expected{
//         mega::min_typeID_context, std::numeric_limits< mega::Instance >::min() };
//
//     std::string str;
//     {
//         std::ostringstream os;
//         os << expected;
//         str = os.str();
//     }
//
//     OtherTypeInstance result;
//     {
//         std::istringstream is( str );
//         is >> result;
//     }
//     ASSERT_TRUE( result == expected ) << " failed io with str: " << str;
// }
*/
/*
TEST( Pointer, TypeInstance2 )
{
    const mega::TypeInstance expected{ mega::max_typeID_context, std::numeric_limits< mega::Instance >::max() };

    std::string str;
    {
        std::ostringstream os;
        os << expected;
        str = os.str();
    }

    mega::TypeInstance result;
    {
        std::istringstream is( str );
        is >> result;
    }
    ASSERT_TRUE( result == expected ) << " failed io with str: " << str;
}

struct PointerTestData
{
    mega::Pointer expected;
};

class PointerIOTest : public ::testing::TestWithParam< PointerTestData >
{
protected:
};

TEST_P( PointerIOTest, PointerIO )
{
    const PointerTestData data = GetParam();

    std::string str;
    {
        std::ostringstream os;
        os << data.expected;
        str = os.str();
    }

    mega::Pointer result;
    {
        std::istringstream is( str );
        is >> result;
    }

    std::ostringstream osError;
    osError << " expected: " << data.expected << " actual: " << result;
    ASSERT_TRUE( result == data.expected ) << osError.str();
}

// clang-format off
INSTANTIATE_TEST_SUITE_P( Pointer, PointerIOTest,
        ::testing::Values
        (
            // default
            PointerTestData{ mega::Pointer{} },

            // network address
            PointerTestData{ mega::Pointer{ mega::TypeInstance{}, mega::MPO{}, mega::AllocationID{} } },
            PointerTestData{ mega::max_net_ref },
            PointerTestData{ mega::min_net_ref },

            // heap address
            PointerTestData{ mega::Pointer{ mega::TypeInstance{}, mega::HeapAddress{} } },
            PointerTestData{ mega::max_heap_ref },
            PointerTestData{ mega::min_heap_ref }
        ));
// clang-format on
*/