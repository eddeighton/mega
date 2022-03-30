
#include "database/io/file_info.hpp"
#include "database/io/manifest.hpp"

#include <gtest/gtest.h>

#include <utility>
#include <sstream>

using namespace mega::io;
/*
TEST( MegaManifest, Manifest_Empty )
{
    Manifest manifestFrom, manifestTo;

    std::stringstream ss;
    ss << manifestFrom;
    ss >> manifestTo;

    std::vector< FileInfo > fileInfosFrom, fileInfosTo;
    manifestFrom.collectFileInfos( fileInfosFrom );
    manifestTo.collectFileInfos( fileInfosTo );

    ASSERT_EQ( fileInfosFrom, fileInfosTo );
}

TEST( MegaManifest, Manifest_One )
{
    Environment environment( "data/test_one", "data/test_one_build" );
    Manifest    manifestFrom( environment ), manifestTo;

    std::stringstream ss;
    ss << manifestFrom;
    ss >> manifestTo;

    std::vector< FileInfo > fileInfosFrom, fileInfosTo;
    manifestFrom.collectFileInfos( fileInfosFrom );
    manifestTo.collectFileInfos( fileInfosTo );

    ASSERT_GT( 0U, fileInfosTo.size() );
    ASSERT_EQ( fileInfosFrom, fileInfosTo );
}
*/
