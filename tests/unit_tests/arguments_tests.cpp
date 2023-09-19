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

#include "database/types/arguments.hpp"

#include <gtest/gtest.h>

TEST( Arguments, Basic )
{
    std::string            str;
    mega::TypeName::Vector result;
    mega::parse( str, result );
    ASSERT_TRUE( result.empty() );
}

TEST( Arguments, Single )
{
    std::string            str = "A";
    mega::TypeName::Vector result;
    mega::parse( str, result );
    ASSERT_TRUE( result.size() == 1 );
    ASSERT_EQ( result[ 0 ].getType(), "A" );
    ASSERT_FALSE( result[ 0 ].getName().has_value() );
}

TEST( Arguments, SingleWithName )
{
    std::string            str = "A x";
    mega::TypeName::Vector result;
    mega::parse( str, result );
    ASSERT_TRUE( result.size() == 1 );
    ASSERT_EQ( result[ 0 ].getType(), "A" );
    ASSERT_TRUE( result[ 0 ].getName().has_value() );
    ASSERT_EQ( result[ 0 ].getName().value(), "x" );
}


TEST( Arguments, List )
{
    std::string            str = "A x, B, C y, D";
    mega::TypeName::Vector result;
    mega::parse( str, result );
    ASSERT_TRUE( result.size() == 4 );
    ASSERT_EQ( result[ 0 ].getType(), "A" );
    ASSERT_EQ( result[ 1 ].getType(), "B" );
    ASSERT_EQ( result[ 2 ].getType(), "C" );
    ASSERT_EQ( result[ 3 ].getType(), "D" );

    ASSERT_TRUE( result[ 0 ].getName().has_value() );
    ASSERT_FALSE( result[ 1 ].getName().has_value() );
    ASSERT_TRUE( result[ 2 ].getName().has_value() );
    ASSERT_FALSE( result[ 3 ].getName().has_value() );

    ASSERT_EQ( result[ 0 ].getName().value(), "x" );
    ASSERT_EQ( result[ 2 ].getName().value(), "y" );
}


TEST( Arguments, SingleTemplate )
{
    std::string            str = "A<foo<1,g>,123>";
    mega::TypeName::Vector result;
    mega::parse( str, result );
    ASSERT_TRUE( result.size() == 1 );
    ASSERT_EQ( result[ 0 ].getType(), "A< foo< 1, g >, 123 >" );
    ASSERT_FALSE( result[ 0 ].getName().has_value() );
}

TEST( Arguments, MultiTemplate )
{
    std::string            str = "var< C1.Up, C1.Down, var< C, D > > ev1, var< C2.Up, C2.Down > ev2";
    mega::TypeName::Vector result;
    mega::parse( str, result );
    ASSERT_TRUE( result.size() == 2 );
    ASSERT_EQ( result[ 0 ].getType(), "var< C1.Up, C1.Down, var< C, D > >" );
    ASSERT_EQ( result[ 1 ].getType(), "var< C2.Up, C2.Down >" );
    ASSERT_EQ( result[ 0 ].getName().value(), "ev1" );
    ASSERT_EQ( result[ 1 ].getName().value(), "ev2" );
}
