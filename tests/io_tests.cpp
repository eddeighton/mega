
#include "database/io/source_listing.hpp"

#include <gtest/gtest.h>

#include <utility>
#include <sstream>

using namespace mega::io;

TEST( MegaIO, SourceListing_Empty )
{
    SourceListing::PathArray paths;
    SourceListing sourceListingFrom( paths, false );
    SourceListing sourceListingTo( SourceListing::PathArray{}, false );

    std::stringstream ss;
    ss << sourceListingFrom;
    ss >> sourceListingTo;

    ASSERT_EQ( sourceListingFrom.isComponent(), sourceListingTo.isComponent() );
    ASSERT_EQ( sourceListingFrom.getSourceFiles(), sourceListingTo.getSourceFiles() );
}

TEST( MegaIO, SourceListing_Single )
{
    SourceListing::PathArray paths = { "/a/b/c/test.txt" };
    SourceListing sourceListingFrom( paths, true );
    SourceListing sourceListingTo( SourceListing::PathArray{}, false );

    std::stringstream ss;
    ss << sourceListingFrom;
    ss >> sourceListingTo;

    ASSERT_EQ( sourceListingFrom.isComponent(), sourceListingTo.isComponent() );
    ASSERT_EQ( sourceListingFrom.getSourceFiles(), sourceListingTo.getSourceFiles() );
}

TEST( MegaIO, SourceListing_Multi )
{
    SourceListing::PathArray paths = { "/a/b/c/test.txt", "/d/f/g/test.txt", "/c/foobar.txt" };
    SourceListing sourceListingFrom( paths, true );
    SourceListing sourceListingTo( SourceListing::PathArray{}, false );

    std::stringstream ss;
    ss << sourceListingFrom;
    ss >> sourceListingTo;

    ASSERT_EQ( sourceListingFrom.isComponent(), sourceListingTo.isComponent() );
    ASSERT_EQ( sourceListingFrom.getSourceFiles(), sourceListingTo.getSourceFiles() );
}