

#include "database/model/file_info.hxx"
#include "database/model/manifest.hxx"

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
