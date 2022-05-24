

#include "database/model/file_info.hxx"
#include "database/model/manifest.hxx"

#include "database/common/environments.hpp"
#include "database/common/serialisation.hpp"

#include <gtest/gtest.h>

#include <utility>
#include <sstream>

using namespace mega::io;

TEST( MegaManifest, Manifest_Empty )
{
    Manifest manifestFrom;

    std::stringstream ss;
    {
        mega::OutputArchiveType oa( ss );
        oa & boost::serialization::make_nvp( "manifest", manifestFrom );
    }

    Manifest manifestTo;
    {
        mega::InputArchiveType ia( ss );
        ia & boost::serialization::make_nvp( "manifest", manifestTo );
    }

    std::vector< FileInfo > fileInfosFrom, fileInfosTo;

    ASSERT_TRUE( manifestFrom.getCompilationFileInfos().empty() );
    ASSERT_TRUE( manifestFrom.getMegaSourceFiles().empty() );
}
/*
TEST( MegaManifest, Manifest_One )
{
    BuildEnvironment environment( "data/test_one", "data/test_one_build" );
    Manifest    manifestFrom( environment );

    std::stringstream ss;
    {
        ss << manifestFrom;
    }

    Manifest manifestTo;
    {
        mega::InputArchiveType ia( ss );
        ia & manifestTo;
    }

}
*/
