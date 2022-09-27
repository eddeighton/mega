
#include <gtest/gtest.h>

#include "mega/reference_io.hpp"

#include <sstream>

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

TEST( Reference, MPIO )
{
    // clang-format off
    ASSERT_EQ( fromStr( "0" ),      mega::MP( 0, 0, true ) );
    ASSERT_EQ( fromStr( "1" ),      mega::MP( 1, 0, true ) );
    ASSERT_EQ( fromStr( "0.0" ),    mega::MP( 0, 0, false ) );
    ASSERT_EQ( fromStr( "1.0" ),    mega::MP( 1, 0, false ) );
    ASSERT_EQ( fromStr( "0.1" ),    mega::MP( 0, 1, false ) );
    // clang-format on
}
