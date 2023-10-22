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

#include "reports/value.hpp"
#include "reports/report.hpp"

#include <limits>
#include <sstream>

TEST( Value, Basic )
{
    using namespace mega;
    using namespace mega::reports;
    using namespace std::string_literals;

    Value str{ "This is a test"s };

    ASSERT_EQ( boost::get< std::string >( str ), "This is a test"s );
}

TEST( Value, TypeID )
{
    using namespace mega;
    using namespace mega::reports;
    using namespace std::string_literals;

    {
        TypeID typeID = TypeID::make_context( 1, 2 );
        Value  var{ typeID };
        ASSERT_EQ( boost::get< TypeID >( var ), typeID );
    }

    {
        const TypeID typeID{ 0x12345678 };
        Value var{ TypeID{ 0x12345678 } };
        ASSERT_EQ( boost::get< TypeID >( var ), typeID );
    }
}

TEST( Branch, Basic )
{
    using namespace mega;
    using namespace mega::reports;
    using namespace std::string_literals;

    {
        Branch b{ { "Test"s } };
        ASSERT_EQ( boost::get< std::string >( b.m_label.front() ), "Test"s );
        ASSERT_EQ( b.m_label.size(), 1 );
    }

    {
        Branch b{ { "Test1"s, "Test2"s } };
        ASSERT_EQ( boost::get< std::string >( b.m_label.front() ), "Test1"s );
        ASSERT_EQ( boost::get< std::string >( b.m_label.back() ), "Test2"s );
        ASSERT_EQ( b.m_label.size(), 2 );
    }

    {
        Project project{ "/somewhere" };
        Branch  b{ { "Test1"s, project } };
        ASSERT_EQ( boost::get< std::string >( b.m_label.front() ), "Test1"s );
        ASSERT_EQ( boost::get< Project >( b.m_label.back() ), project );
        ASSERT_EQ( b.m_label.size(), 2 );
    }
}