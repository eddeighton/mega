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

#include "database/component_info.hpp"
#include "database/component_type.hpp"

#include "common/serialisation.hpp"

#include <gtest/gtest.h>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <utility>
#include <sstream>

using namespace mega::io;

TEST( ComponentInfo, SourceListing_Empty )
{
    ComponentInfo::PathArray paths;
    ComponentInfo            componentInfoFrom;
    ComponentInfo            componentInfoTo;

    std::stringstream ss;
    {
        boost::archive::xml_oarchive archive( ss );
        archive&                     boost::serialization::make_nvp( "test", componentInfoFrom );
    }
    {
        boost::archive::xml_iarchive archive( ss );
        archive&                     boost::serialization::make_nvp( "test", componentInfoTo );
    }

    ASSERT_EQ( componentInfoFrom.getName(), componentInfoTo.getName() );
    ASSERT_EQ( componentInfoFrom.getSrcDir(), componentInfoTo.getSrcDir() );
    ASSERT_EQ( componentInfoFrom.getBuildDir(), componentInfoTo.getBuildDir() );
    ASSERT_EQ( componentInfoFrom.getSourceFiles(), componentInfoTo.getSourceFiles() );
    ASSERT_EQ( componentInfoFrom.getIncludeDirectories(), componentInfoTo.getIncludeDirectories() );
}

TEST( ComponentInfo, SourceListing_Single )
{
    ComponentInfo::PathArray paths = { "/a/b/c/test.txt" };
    ComponentInfo            componentInfoFrom(
        mega::ComponentType{}, "test", "test", {}, {}, {}, "some/where", paths, {}, ComponentInfo::PathArray{} );
    ComponentInfo componentInfoTo;

    std::stringstream ss;
    {
        boost::archive::xml_oarchive archive( ss );
        archive&                     boost::serialization::make_nvp( "test", componentInfoFrom );
    }
    {
        boost::archive::xml_iarchive archive( ss );
        archive&                     boost::serialization::make_nvp( "test", componentInfoTo );
    }

    ASSERT_EQ( componentInfoFrom.getName(), componentInfoTo.getName() );
    ASSERT_EQ( componentInfoFrom.getSrcDir(), componentInfoTo.getSrcDir() );
    ASSERT_EQ( componentInfoFrom.getBuildDir(), componentInfoTo.getBuildDir() );
    ASSERT_EQ( componentInfoFrom.getSourceFiles(), componentInfoTo.getSourceFiles() );
    ASSERT_EQ( componentInfoFrom.getIncludeDirectories(), componentInfoTo.getIncludeDirectories() );
}

TEST( ComponentInfo, SourceListing_Multi )
{
    ComponentInfo::PathArray paths    = { "/a/b/c/test.txt", "/d/f/g/test.txt", "/c/foobar.txt" };
    ComponentInfo::PathArray includes = { "somewhere/place", "/other/place" };
    ComponentInfo            componentInfoFrom(
        mega::ComponentType{}, "test", "test", {}, {}, {}, "some/where", paths, {}, ComponentInfo::PathArray{} );
    ComponentInfo componentInfoTo;

    std::stringstream ss;
    {
        boost::archive::xml_oarchive archive( ss );
        archive&                     boost::serialization::make_nvp( "test", componentInfoFrom );
    }
    {
        boost::archive::xml_iarchive archive( ss );
        archive&                     boost::serialization::make_nvp( "test", componentInfoTo );
    }

    ASSERT_EQ( componentInfoFrom.getName(), componentInfoTo.getName() );
    ASSERT_EQ( componentInfoFrom.getSrcDir(), componentInfoTo.getSrcDir() );
    ASSERT_EQ( componentInfoFrom.getBuildDir(), componentInfoTo.getBuildDir() );
    ASSERT_EQ( componentInfoFrom.getSourceFiles(), componentInfoTo.getSourceFiles() );
    ASSERT_EQ( componentInfoFrom.getIncludeDirectories(), componentInfoTo.getIncludeDirectories() );
}
