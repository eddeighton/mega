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

#include "mega/tag_parser.hpp"

#include "common/file.hpp"

#include <gtest/gtest.h>

#include <sstream>

TEST( XMLTagParser, Basic1 )
{
    std::string        strInput = "<Testing></Testing>";
    std::istringstream is( strInput );

    mega::XMLTag tag = mega::parse( is );
    ASSERT_EQ( tag.key, "Testing" );
    ASSERT_FALSE( tag.indexOpt.has_value() );
}

TEST( XMLTagParser, Basic2 )
{
    std::string        strInput = "<Testing index=\"123\"></Testing>";
    std::istringstream is( strInput );

    mega::XMLTag tag = mega::parse( is );
    ASSERT_EQ( tag.key, "Testing" );
    ASSERT_TRUE( tag.indexOpt.has_value() );
    ASSERT_EQ( tag.indexOpt.value(), 123 );
}

TEST( XMLTagParser, Basic3 )
{
    std::string        strInput = "< Testing  index  =  \"123\"  ></Testing>";
    std::istringstream is( strInput );

    mega::XMLTag tag = mega::parse( is );

    ASSERT_EQ( tag.key, "Testing" );
    ASSERT_TRUE( tag.indexOpt.has_value() );
    ASSERT_EQ( tag.indexOpt.value(), 123 );
}

TEST( XMLTagParser, Basic4 )
{
    std::string strInput =
R"TEMPLATE(
    <Testing>
        <A index="2">
        </A>
        <B index="3">
        </B>
    </Testing>
)TEMPLATE";

    std::istringstream is( strInput );

    mega::XMLTag tag = mega::parse( is );

    ASSERT_EQ( tag.key, "Testing" );
    ASSERT_TRUE( !tag.indexOpt.has_value() );

    ASSERT_EQ( tag.children.size(), 2 );
    ASSERT_EQ( tag.children[ 0 ].key, "A" );
    ASSERT_EQ( tag.children[ 1 ].key, "B" );
    ASSERT_EQ( tag.children[ 0 ].indexOpt.value(), 2 );
    ASSERT_EQ( tag.children[ 1 ].indexOpt.value(), 3 );
}

TEST( XMLTagParser, ConsumeStart )
{
    std::string        strInput = "<Testing>";
    std::istringstream is( strInput );
    mega::consumeStart( is, "Testing" );
}
TEST( XMLTagParser, ConsumeEnd )
{
    std::string        strInput = "</Testing>";
    std::istringstream is( strInput );
    mega::consumeEnd( is, "Testing" );
}
/*
TEST( XMLTagParser, Mismatch )
{
    std::string strInput =
R"TEMPLATE(
    <Testing>
        <B index="2">
        </A>
        <A index="3">
        </B>
    </Testing>
)TEMPLATE";

    std::istringstream is( strInput );
    ASSERT_THROW( mega::parse( is ), std::runtime_error );
}
*/
/*
TEST( XMLTagParser, BigFile )
{
    std::string strInput;
    boost::filesystem::loadAsciiFile( "test_data/xml_tags_data.xml", strInput );
    std::istringstream is( strInput );
    auto result = mega::parse( is );
    ASSERT_EQ( result.key, "Root" );
}
*/