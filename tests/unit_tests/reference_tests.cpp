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
