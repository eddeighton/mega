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

#include "mega/values/compilation/concrete/type_id.hpp"
#include "mega/values/compilation/interface/type_id.hpp"

//#include "mega/bin_archive.hpp"
//#include "mega/record_archive.hpp"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/interprocess/streams/vectorstream.hpp>

#include <limits>
#include <sstream>
/*
TEST( TypeID, RootSymbol )
{
    ASSERT_EQ( mega::ROOT_SYMBOL_ID.getSymbolID(), -1 );
    ASSERT_EQ( mega::ROOT_TYPE_ID.getObjectID(), 1 );
    ASSERT_EQ( mega::ROOT_TYPE_ID.getSubObjectID(), 0 );

    ASSERT_TRUE( mega::ROOT_SYMBOL_ID.isSymbolID() );
    ASSERT_TRUE( mega::ROOT_TYPE_ID.isContextID() );
    ASSERT_TRUE( !mega::ROOT_SYMBOL_ID.isObject() );
    ASSERT_TRUE( mega::ROOT_TYPE_ID.isObject() );

    ASSERT_TRUE( mega::ROOT_TYPE_ID.valid() );
    ASSERT_TRUE( mega::ROOT_SYMBOL_ID.valid() );
    ASSERT_TRUE( !mega::TypeID{}.valid() );
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

TEST( TypeID, Sanity )
{
    const I32 i1 = 0x10000;
    const I32 i2 = -0x10000;
    ASSERT_EQ( i1, -i2 );
}

TEST( TypeID, States )
{
    using namespace mega;

    const TypeID startStateLiteral = TypeID{ 0x10000 };
    const TypeID endStateLiteral   = TypeID{ -2147418112 };
    const TypeID endStateLiteral2  = TypeID{ -0x7FFF0000 };

    ASSERT_EQ( endStateLiteral, TypeID::make_end_state( startStateLiteral ) );
    ASSERT_EQ( startStateLiteral, TypeID::make_start_state( endStateLiteral ) );
    ASSERT_EQ( endStateLiteral, endStateLiteral2 );

    ASSERT_EQ( startStateLiteral.getSubObjectID(), endStateLiteral.getSubObjectID() );
    ASSERT_EQ( startStateLiteral.getObjectID(), endStateLiteral.getObjectID() );

    {
        const TypeID startState = TypeID::make_start_state( startStateLiteral );
        ASSERT_EQ( startState.getSubObjectID(), startStateLiteral.getSubObjectID() );
        ASSERT_EQ( startState.getObjectID(), startStateLiteral.getObjectID() );
        ASSERT_TRUE( startState.isContextID() );
        ASSERT_EQ( startState, startStateLiteral );
    }
    {
        const TypeID startState = TypeID::make_start_state( endStateLiteral );
        ASSERT_EQ( startState.getSubObjectID(), endStateLiteral.getSubObjectID() );
        ASSERT_EQ( startState.getObjectID(), endStateLiteral.getObjectID() );
        ASSERT_TRUE( startState.isContextID() );
        ASSERT_EQ( startState, startStateLiteral );
    }

    {
        const TypeID endState = TypeID::make_end_state( startStateLiteral );
        ASSERT_EQ( endState.getSubObjectID(), startStateLiteral.getSubObjectID() );
        ASSERT_EQ( endState.getObjectID(), startStateLiteral.getObjectID() );
        ASSERT_TRUE( endState.isSymbolID() );
        ASSERT_EQ( endState, endStateLiteral );
    }

    {
        const TypeID endState = TypeID::make_end_state( endStateLiteral );
        ASSERT_EQ( endState.getSubObjectID(), endStateLiteral.getSubObjectID() );
        ASSERT_EQ( endState.getObjectID(), endStateLiteral.getObjectID() );
        ASSERT_TRUE( endState.isSymbolID() );
        ASSERT_EQ( endState, endStateLiteral );
    }
}

TEST( TypeID, FlagIsSignedBit )
{
    using namespace mega;

    const TypeID startStateLiteral = TypeID{ 0x10000 };
    const TypeID endStateLiteral   = TypeID::make_end_state( startStateLiteral );
    ASSERT_TRUE( startStateLiteral.isContextID() );
    ASSERT_TRUE( endStateLiteral.isSymbolID() );
    ASSERT_EQ( startStateLiteral.getObjectID(), 1 );
    ASSERT_EQ( startStateLiteral.getSubObjectID(), 0 );
    ASSERT_EQ( endStateLiteral.getObjectID(), 1 );
    ASSERT_EQ( endStateLiteral.getSubObjectID(), 0 );

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
        os << data.expected;
        str = os.str();
    }

    mega::TypeID result;
    {
        std::istringstream is( str );
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

class TypeIDBinArchiveTest : public ::testing::TestWithParam< TypeIDTestData >
{
protected:
};

TEST_P( TypeIDBinArchiveTest, TypeIDBinArchive )
{
    const TypeIDTestData data = GetParam();

    {
        mega::BinSaveArchive saveArchive;
        saveArchive.save( data.expected );

        mega::BinLoadArchive loadArchive( saveArchive.makeSnapshot( 0 ) );
        mega::TypeID         result;
        loadArchive.load( result );

        std::ostringstream osError;
        osError << " expected: " << data.expected << " actual: " << result;
        ASSERT_TRUE( result == data.expected ) << osError.str();
    }

    {
        boost::interprocess::basic_vectorbuf< std::vector< char > > buffer;
        boost::archive::binary_oarchive                             saveArchive( buffer );
        saveArchive&                                                data.expected;

        boost::archive::binary_iarchive loadArchive( buffer );
        mega::TypeID                    result;
        loadArchive&                    result;

        std::ostringstream osError;
        osError << " expected: " << data.expected << " actual: " << result;
        ASSERT_TRUE( result == data.expected ) << osError.str();
    }
}

// clang-format off
INSTANTIATE_TEST_SUITE_P( TypeIDBinArchive, TypeIDBinArchiveTest,
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

*/
