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

#include "mega/type_id.hpp"
#include "mega/type_id_limits.hpp"
#include "mega/type_id_io.hpp"

#include <limits>
#include <sstream>

TEST( TypeID, RootSymbol )
{
    ASSERT_EQ( mega::ROOT_SYMBOL_ID.getSymbolID(), -1 );
    ASSERT_EQ( mega::ROOT_TYPE_ID.getObjectID(), 1 );
    ASSERT_EQ( mega::ROOT_TYPE_ID.getSubObjectID(), 0 );

    ASSERT_TRUE( mega::ROOT_SYMBOL_ID.isSymbolID() );
    ASSERT_TRUE( mega::ROOT_TYPE_ID.isContextID() );
    ASSERT_TRUE( !mega::ROOT_SYMBOL_ID.isObject() );
    ASSERT_TRUE( mega::ROOT_TYPE_ID.isObject() );

    ASSERT_TRUE( mega::ROOT_TYPE_ID.is_valid() );
    ASSERT_TRUE( mega::ROOT_SYMBOL_ID.is_valid() );
    ASSERT_TRUE( !mega::TypeID{}.is_valid() );
}

TEST( TypeID, DefaultValue )
{
    const mega::TypeID def{};
    ASSERT_EQ( def.getSymbolID(), 0U );
    ASSERT_EQ( def.getObjectID(), 0U );
    ASSERT_EQ( def.getSubObjectID(), 0U );
}

TEST( TypeID, MaxSymbolIDValues )
{
    const mega::TypeID def{ mega::min_symbol_id };
    ASSERT_EQ( def.getSymbolID(), mega::min_symbol_id );
    ASSERT_TRUE( def.isSymbolID() );
}

TEST( TypeID, FlagIsSignedBit )
{
    ASSERT_EQ( mega::max_typeID_context.getObjectID(), mega::max_object_id );
    ASSERT_EQ( mega::max_typeID_context.getSubObjectID(), mega::max_sub_object_id );

    ASSERT_TRUE( mega::max_typeID_context.isContextID() );
    ASSERT_TRUE( mega::min_typeID_context.isContextID() );
    ASSERT_TRUE( !mega::max_typeID_symbol.isContextID() );
    ASSERT_TRUE( !mega::min_typeID_symbol.isContextID() );

    ASSERT_TRUE( !mega::max_typeID_context.isSymbolID() );
    ASSERT_TRUE( !mega::min_typeID_context.isSymbolID() );
    ASSERT_TRUE( mega::max_typeID_symbol.isSymbolID() );
    ASSERT_TRUE( mega::min_typeID_symbol.isSymbolID() );

    ASSERT_TRUE( mega::max_typeID_context.getSymbolID() > 0 );
}

TEST( TypeID, MakeObjectZeroesSubObject )
{
    const mega::TypeID def    = mega::TypeID::make_context( mega::max_object_id, mega::max_sub_object_id );
    const mega::TypeID zeroed = mega::TypeID::make_object_from_typeID( def );
    ASSERT_EQ( zeroed, mega::TypeID::make_context( mega::max_object_id ) );
    ASSERT_EQ( zeroed.getSubObjectID(), 0U );
    ASSERT_EQ( zeroed.getObjectID(), mega::max_object_id );
}

TEST( TypeID, Equality )
{
    ASSERT_TRUE( mega::max_typeID_symbol == mega::max_typeID_symbol );
    ASSERT_TRUE( mega::min_typeID_symbol == mega::min_typeID_symbol );
    ASSERT_TRUE( mega::max_typeID_context == mega::max_typeID_context );
    ASSERT_TRUE( mega::min_typeID_context == mega::min_typeID_context );
}

TEST( TypeID, InEquality )
{
    ASSERT_TRUE( mega::max_typeID_symbol != mega::min_typeID_symbol );
    ASSERT_TRUE( mega::max_typeID_symbol != mega::max_typeID_context );
    ASSERT_TRUE( mega::max_typeID_symbol != mega::min_typeID_context );
    ASSERT_TRUE( mega::min_typeID_symbol != mega::max_typeID_context );
    ASSERT_TRUE( mega::min_typeID_symbol != mega::min_typeID_context );
    ASSERT_TRUE( mega::max_typeID_context != mega::min_typeID_context );
}

TEST( TypeID, Comparison )
{
    ASSERT_TRUE( mega::min_typeID_symbol < mega::max_typeID_symbol );
    ASSERT_TRUE( mega::min_typeID_context < mega::max_typeID_context );
    ASSERT_TRUE( mega::TypeID::make_context( mega::max_object_id ) < mega::max_typeID_context );
    ASSERT_TRUE( mega::min_typeID_context < mega::TypeID::make_context( mega::max_object_id ) );

    ASSERT_TRUE( mega::TypeID( -2 ) < mega::TypeID( -1 ) );
    ASSERT_TRUE( mega::TypeID::make_context( 3 ) < mega::TypeID::make_context( 4 ) );
    ASSERT_TRUE( mega::TypeID::make_context( 3 ) < mega::TypeID::make_context( 3, 1 ) );
    ASSERT_TRUE( mega::TypeID::make_context( 3, 1 ) < mega::TypeID::make_context( 3, 2 ) );
    ASSERT_TRUE( mega::TypeID( -1 ) < mega::TypeID::make_context( 1 ) );

    ASSERT_FALSE( mega::TypeID( -1 ) < mega::TypeID( -2 ) );
    ASSERT_FALSE( mega::TypeID::make_context( 4 ) < mega::TypeID::make_context( 3 ) );
    ASSERT_FALSE( mega::TypeID::make_context( 3, 1 ) < mega::TypeID::make_context( 3 ) );
    ASSERT_FALSE( mega::TypeID::make_context( 3, 2 ) < mega::TypeID::make_context( 3, 1 ) );
    ASSERT_FALSE( mega::TypeID::make_context( 1 ) < mega::TypeID( -1 ) );
}

struct TypeIDTestData
{
    mega::TypeID expected;
};

class TypeIDIOTest : public ::testing::TestWithParam< TypeIDTestData >
{
protected:
};

TEST_P( TypeIDIOTest, TypeIDIO )
{
    const TypeIDTestData data = GetParam();

    std::string str;
    {
        std::ostringstream os;
        using ::           operator<<;
        os << data.expected;
        str = os.str();
    }

    mega::TypeID result;
    {
        std::istringstream is( str );
        using ::           operator>>;
        is >> result;
    }

    std::ostringstream osError;
    osError << " expected: " << data.expected << " actual: " << result;
    ASSERT_TRUE( result == data.expected ) << osError.str();
}

// clang-format off
INSTANTIATE_TEST_SUITE_P( TypeIDIO, TypeIDIOTest,
        ::testing::Values
        (
            // default
            TypeIDTestData{ mega::TypeID{} },

            TypeIDTestData{ mega::ROOT_TYPE_ID },
            TypeIDTestData{ mega::ROOT_SYMBOL_ID },

            TypeIDTestData{ mega::max_typeID_symbol },
            TypeIDTestData{ mega::min_typeID_symbol },
            TypeIDTestData{ mega::max_typeID_context },
            TypeIDTestData{ mega::min_typeID_context },

            TypeIDTestData{ mega::TypeID::make_object_from_typeID( mega::ROOT_TYPE_ID ) },
            TypeIDTestData{ mega::TypeID::make_object_from_typeID( mega::TypeID::make_context( mega::max_object_id, mega::max_sub_object_id )) }
        ));
// clang-format on
