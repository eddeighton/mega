
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
#include "database/common/serialisation.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fstream>
#include <optional>

class BasicDBTest : public ::testing::Test
{
public:
    boost::filesystem::path    tempDir;
    mega::io::BuildEnvironment environment;

    BasicDBTest()
        : tempDir( boost::filesystem::temp_directory_path() / "testing" )
        , environment( tempDir, tempDir )
    {
    }
    void SetUp()
    {
        std::vector< boost::filesystem::path > componentInfoPaths;
        const mega::io::Manifest         manifest( environment, componentInfoPaths );
        const mega::io::manifestFilePath projectManifestPath = environment.project_manifest();
        manifest.save_temp( environment, projectManifestPath );
        environment.temp_to_real( projectManifestPath );
    }
};

TEST_F( BasicDBTest, BasicTypes )
{
    {
        using namespace BasicStage;
        using namespace BasicStage::TestNamespace;

        Database database( environment, environment.project_manifest() );

        TestObject* pTestObject = database.construct< TestObject >( TestObject::Args( "test", {}, std::nullopt, {}, std::nullopt  ) );

        ASSERT_TRUE( pTestObject );
        ASSERT_EQ( pTestObject->get_string(), "test" );
        ASSERT_TRUE( !pTestObject->get_optional_string().has_value() );

        pTestObject->set_optional_reference( pTestObject );
        pTestObject->set_late_reference( pTestObject );
        pTestObject->push_back_late_array_of_references( pTestObject );
        pTestObject->push_back_late_array_of_references( pTestObject );
        pTestObject->push_back_late_array_of_references( pTestObject );

        database.save_Testing_to_temp();
        environment.temp_to_real( environment.BasicStage_Testing( environment.project_manifest() ) );
    }
    {
        using namespace SecondStage;
        Database database( environment, environment.project_manifest() );

        TestNamespace::TestObject* pTestObject = database.one< TestNamespace::TestObject >( environment.project_manifest() );
        ASSERT_TRUE( pTestObject );
        ASSERT_EQ( pTestObject->get_string(), "test" );
        ASSERT_TRUE( !pTestObject->get_optional_string().has_value() );
        ASSERT_EQ( pTestObject->get_optional_reference().value(), pTestObject );
        ASSERT_EQ( pTestObject->get_late_reference(), pTestObject );
        ASSERT_EQ( pTestObject->get_late_array_of_references().size(), 3 );
        ASSERT_EQ( pTestObject->get_late_array_of_references()[ 0 ], pTestObject );
        ASSERT_EQ( pTestObject->get_late_array_of_references()[ 1 ], pTestObject );
        ASSERT_EQ( pTestObject->get_late_array_of_references()[ 2 ], pTestObject );
    }
}
