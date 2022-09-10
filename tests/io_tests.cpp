
#include "database/common/component_info.hpp"
#include "database/common/serialisation.hpp"
#include "database/types/component_type.hpp"

#include <gtest/gtest.h>

#include <utility>
#include <sstream>

using namespace mega::io;

TEST( MegaIO, SourceListing_Empty )
{
    ComponentInfo::PathArray paths;
    ComponentInfo            componentInfoFrom;
    ComponentInfo            componentInfoTo;

    std::stringstream ss;
    {
        mega::OutputArchiveType archive( ss );
        archive&                boost::serialization::make_nvp( "test", componentInfoFrom );
    }
    {
        mega::InputArchiveType archive( ss );
        archive&               boost::serialization::make_nvp( "test", componentInfoTo );
    }

    ASSERT_EQ( componentInfoFrom.getName(), componentInfoTo.getName() );
    ASSERT_EQ( componentInfoFrom.getSrcDir(), componentInfoTo.getSrcDir() );
    ASSERT_EQ( componentInfoFrom.getBuildDir(), componentInfoTo.getBuildDir() );
    ASSERT_EQ( componentInfoFrom.getSourceFiles(), componentInfoTo.getSourceFiles() );
    ASSERT_EQ( componentInfoFrom.getIncludeDirectories(), componentInfoTo.getIncludeDirectories() );
}

TEST( MegaIO, SourceListing_Single )
{
    ComponentInfo::PathArray paths = { "/a/b/c/test.txt" };
    ComponentInfo            componentInfoFrom(
                   mega::ComponentType{}, "test", "test", {}, {}, {}, "some/where", paths, {}, ComponentInfo::PathArray{} );
    ComponentInfo componentInfoTo;

    std::stringstream ss;
    {
        mega::OutputArchiveType archive( ss );
        archive&                boost::serialization::make_nvp( "test", componentInfoFrom );
    }
    {
        mega::InputArchiveType archive( ss );
        archive&               boost::serialization::make_nvp( "test", componentInfoTo );
    }

    ASSERT_EQ( componentInfoFrom.getName(), componentInfoTo.getName() );
    ASSERT_EQ( componentInfoFrom.getSrcDir(), componentInfoTo.getSrcDir() );
    ASSERT_EQ( componentInfoFrom.getBuildDir(), componentInfoTo.getBuildDir() );
    ASSERT_EQ( componentInfoFrom.getSourceFiles(), componentInfoTo.getSourceFiles() );
    ASSERT_EQ( componentInfoFrom.getIncludeDirectories(), componentInfoTo.getIncludeDirectories() );
}

TEST( MegaIO, SourceListing_Multi )
{
    ComponentInfo::PathArray paths    = { "/a/b/c/test.txt", "/d/f/g/test.txt", "/c/foobar.txt" };
    ComponentInfo::PathArray includes = { "somewhere/place", "/other/place" };
    ComponentInfo            componentInfoFrom(
                   mega::ComponentType{}, "test", "test", {}, {}, {}, "some/where", paths, {}, ComponentInfo::PathArray{} );
    ComponentInfo componentInfoTo;

    std::stringstream ss;
    {
        mega::OutputArchiveType archive( ss );
        archive&                boost::serialization::make_nvp( "test", componentInfoFrom );
    }
    {
        mega::InputArchiveType archive( ss );
        archive&               boost::serialization::make_nvp( "test", componentInfoTo );
    }

    ASSERT_EQ( componentInfoFrom.getName(), componentInfoTo.getName() );
    ASSERT_EQ( componentInfoFrom.getSrcDir(), componentInfoTo.getSrcDir() );
    ASSERT_EQ( componentInfoFrom.getBuildDir(), componentInfoTo.getBuildDir() );
    ASSERT_EQ( componentInfoFrom.getSourceFiles(), componentInfoTo.getSourceFiles() );
    ASSERT_EQ( componentInfoFrom.getIncludeDirectories(), componentInfoTo.getIncludeDirectories() );
}
