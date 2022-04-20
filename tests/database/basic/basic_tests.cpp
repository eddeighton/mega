
#include <gtest/gtest.h>

#include "database/common/environments.hpp"
#include "database/common/sources.hpp"

#include "database/model/BasicStage.hxx"
#include "database/model/SecondStage.hxx"

#include "database/model/ComponentListing.hxx"
#include "database/model/ParserStage.hxx"
#include "database/model/manifest.hxx"
#include "database/model/environment.hxx"

#include "database/common/component_info.hpp"
#include "database/common/archive.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fstream>

class BasicDBTest : public ::testing::Test
{
public:
    boost::filesystem::path tempDir;
    mega::io::BuildEnvironment   environment;
    boost::filesystem::path srcFile;

    BasicDBTest()
        : tempDir( boost::filesystem::temp_directory_path() )
        , environment( tempDir, tempDir, tempDir )
        , srcFile( tempDir / "test1.mega" )
    {
        std::ofstream of( srcFile.native() );
        of << "Hello world";
    }

    void SetUp()
    {
        // create a manifest
        {
            std::vector< boost::filesystem::path > componentInfoPaths;
            /*{
                mega::io::ComponentInfo componentInfo(
                    "test", tempDir, mega::io::ComponentInfo::PathArray{ srcFile }, mega::io::ComponentInfo::PathArray{} );
                const boost::filesystem::path componentInfoPath = tempDir / "test.txt";
                std::ofstream                 of( componentInfoPath.native() );
                mega::OutputArchiveType       oa( of );
                oa << boost::serialization::make_nvp( "componentInfo", componentInfo );
                componentInfoPaths.push_back( componentInfoPath );
            }*/

            const mega::io::Manifest          manifest( environment, componentInfoPaths );
            const mega::io::manifestFilePath projectManifestPath = environment.project_manifest();
            //manifest.save( environment, projectManifestPath );
        }
    }
};

TEST_F( BasicDBTest, LoadPointerToSelf )
{
   /* {
        using namespace BasicStage;
        Database database( environment, environment.project_manifest() );

        TestNamespace::TestObject* pTestObject
            = database.construct< TestNamespace::TestObject >( TestNamespace::TestObject::Args( "test" ) );

        ASSERT_TRUE( pTestObject );
        ASSERT_EQ( pTestObject->get_name(), "test" );

        pTestObject->set_self( pTestObject );

        database.store();
    }
    {
        using namespace SecondStage;
        Database database( environment, environment.project_manifest() );

        TestNamespace::TestObject* pTestObject = database.one< TestNamespace::TestObject >( environment.project_manifest() );
        ASSERT_TRUE( pTestObject );
        ASSERT_EQ( pTestObject->get_self(), pTestObject );
    }*/
}
