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

#include "mega/values/compilation/relation_id.hpp"

#include "mega/values/compilation/type_id_limits.hpp"

#include <limits>
#include <sstream>

TEST( RelationID, Basic )
{
    using namespace mega;
    RelationID rel1( max_typeID_context, min_typeID_context );
    RelationID rel2( min_typeID_context, max_typeID_context );

    ASSERT_EQ( rel1, rel2 );
    ASSERT_EQ( rel1.getID(), rel2.getID() );

    ASSERT_EQ( rel1.getLower(), min_typeID_context );
    ASSERT_EQ( rel1.getUpper(), max_typeID_context );
    ASSERT_EQ( rel2.getLower(), min_typeID_context );
    ASSERT_EQ( rel2.getUpper(), max_typeID_context );
}

struct RelationIDTestData
{
    mega::TypeID lower, upper;
};

class RelationIDIOTest : public ::testing::TestWithParam< RelationIDTestData >
{
protected:
};

TEST_P( RelationIDIOTest, RelationIDIO )
{
    using namespace mega;
    const RelationIDTestData data = GetParam();
    RelationID r( data.lower, data.upper );
    ASSERT_EQ( RelationID( r.getID() ), r );
}

// clang-format off
INSTANTIATE_TEST_SUITE_P( RelationIDIO, RelationIDIOTest,
        ::testing::Values
        (
            // default
            RelationIDTestData{ {}, {} },
            RelationIDTestData{ mega::max_typeID_context, {} },
            RelationIDTestData{ {}, mega::max_typeID_context },
            RelationIDTestData{ mega::min_typeID_context, {} },
            RelationIDTestData{ {}, mega::min_typeID_context },
            RelationIDTestData{ mega::min_typeID_context, mega::min_typeID_context },
            RelationIDTestData{ mega::min_typeID_context, mega::max_typeID_context },
            RelationIDTestData{ mega::max_typeID_context, mega::max_typeID_context },
            RelationIDTestData{ mega::min_typeID_context, mega::min_typeID_context }
        ));
// clang-format on
