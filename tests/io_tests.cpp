
#include "database/io/component_info.hpp"

#include <gtest/gtest.h>

#include <utility>
#include <sstream>

using namespace mega::io;
/*
TEST( MegaIO, SourceListing_Empty )
{
    ComponentInfo::PathArray paths;
    ComponentInfo componentInfoFrom( paths, ComponentInfo::PathArray{} );
    ComponentInfo componentInfoTo( ComponentInfo::PathArray{}, ComponentInfo::PathArray{} );

    std::stringstream ss;
    ss << componentInfoFrom;
    ss >> componentInfoTo;

    ASSERT_EQ( componentInfoFrom.getSourceFiles(), componentInfoTo.getSourceFiles() );
}

TEST( MegaIO, SourceListing_Single )
{
    ComponentInfo::PathArray paths = { "/a/b/c/test.txt" };
    ComponentInfo componentInfoFrom( paths, ComponentInfo::PathArray{} );
    ComponentInfo componentInfoTo( ComponentInfo::PathArray{}, ComponentInfo::PathArray{} );

    std::stringstream ss;
    ss << componentInfoFrom;
    ss >> componentInfoTo;

    ASSERT_EQ( componentInfoFrom.getSourceFiles(), componentInfoTo.getSourceFiles() );
}

TEST( MegaIO, SourceListing_Multi )
{
    ComponentInfo::PathArray paths = { "/a/b/c/test.txt", "/d/f/g/test.txt", "/c/foobar.txt" };
    ComponentInfo componentInfoFrom( paths, ComponentInfo::PathArray{} );
    ComponentInfo componentInfoTo( ComponentInfo::PathArray{}, ComponentInfo::PathArray{} );

    std::stringstream ss;
    ss << componentInfoFrom;
    ss >> componentInfoTo;

    ASSERT_EQ( componentInfoFrom.getSourceFiles(), componentInfoTo.getSourceFiles() );
}
*/
