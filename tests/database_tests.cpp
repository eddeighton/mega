
#include <gtest/gtest.h>

#include "database/model/InputParse.hxx"
#include "database/model/InterfaceStage.hxx"
#include "database/model/Analysis.hxx"

#include "database/io/manifest.hpp"
#include "database/io/environment.hpp"
#include "database/io/component_info.hpp"
#include "database/io/archive.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fstream>

class DatabaseTest : public ::testing::Test
{
public:
    boost::filesystem::path tempDir;
    mega::io::Environment   environment;
    boost::filesystem::path srcFile;

    DatabaseTest()
        : tempDir( boost::filesystem::temp_directory_path() )
        , environment( tempDir, tempDir )
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
            {
                mega::io::ComponentInfo componentInfo(
                    "test", tempDir, mega::io::ComponentInfo::PathArray{ srcFile }, mega::io::ComponentInfo::PathArray{} );
                const boost::filesystem::path componentInfoPath = tempDir / "test.txt";
                std::ofstream                 of( componentInfoPath.native() );
                mega::OutputArchiveType       oa( of );
                oa << boost::serialization::make_nvp( "componentInfo", componentInfo );
                componentInfoPaths.push_back( componentInfoPath );
            }

            const mega::io::Manifest          manifest( environment, componentInfoPaths );
            const mega::io::Environment::Path projectManifestPath = environment.project_manifest();
            manifest.save( projectManifestPath );
        }
    }
};

TEST_F( DatabaseTest, Basic )
{
    {
        using namespace InputParse;
        Database database( environment, srcFile );

        Parser::Context* pContext = database.construct< Parser::Context >( Parser::Context::Args( "test1", "test2" ) );

        ASSERT_TRUE( pContext );

        ASSERT_EQ( pContext->get_identifier(), "test1" );
        ASSERT_EQ( pContext->get_body(), "test2" );

        Parser::Root* pRoot = database.construct< Parser::Root >( Parser::Root::Args( pContext ) );

        ASSERT_EQ( pRoot->get_identifier(), "test1" );
        ASSERT_EQ( pRoot->get_body(), "test2" );

        ASSERT_EQ( pContext, database.one< Parser::Context >() );
        database.store();
    }
    {
        using namespace InterfaceStage;
        Database database( environment, srcFile );

        Parser::Context* pContext = database.one< Parser::Context >();
        ASSERT_TRUE( pContext );
        ASSERT_EQ( pContext->get_identifier(), "test1" );
        ASSERT_EQ( pContext->get_body(), "test2" );

        Parser::Root* pRoot = database.one< Parser::Root >();

        ASSERT_EQ( pRoot->get_identifier(), "test1" );
        ASSERT_EQ( pRoot->get_body(), "test2" );
    }
}

TEST_F( DatabaseTest, SpecializeAcrossStages )
{
    {
        using namespace InputParse;
        Database database( environment, srcFile );

        Parser::Opaque*    pOpaque = database.construct< Parser::Opaque >( Parser::Opaque::Args( "testopaque" ) );
        Parser::Dimension* pDim
            = database.construct< Parser::Dimension >( Parser::Dimension::Args( true, "test", "int", "otherstring", pOpaque ) );

        database.store();
    }

    {
        using namespace InterfaceStage;
        Database database( environment, srcFile );

        Parser::Dimension* pDim = database.one< Parser::Dimension >();
        ASSERT_TRUE( pDim );
        ASSERT_EQ( pDim->get_identifier(), "test" );
        ASSERT_EQ( pDim->get_type(), "int" );

        Interface::Dimension* pDimSpecialized = database.construct< Interface::Dimension >( Interface::Dimension::Args( pDim, true ) );
        ASSERT_TRUE( pDimSpecialized );
        ASSERT_EQ( pDimSpecialized->get_more(), true );
        ASSERT_EQ( pDimSpecialized->get_identifier(), "test" );
        ASSERT_EQ( pDimSpecialized->get_type(), "int" );
        database.store();
    }

    {
        using namespace Analysis;
        Database database( environment, srcFile );

        Parser::Dimension* pDim1 = database.one< Parser::Dimension >();
        ASSERT_TRUE( pDim1 );
        Interface::Dimension* pDim2 = database.one< Interface::Dimension >();
        ASSERT_TRUE( pDim2 );

        ASSERT_EQ( pDim2->get_more(), true );
        ASSERT_EQ( pDim2->get_identifier(), "test" );
        ASSERT_EQ( pDim2->get_type(), "int" );

        database.store();
    }
}

TEST_F( DatabaseTest, SecondaryParts )
{
    {
        using namespace InputParse;
        Database database( environment, srcFile );

        using namespace InputParse::Parser;

        Context* pContext = database.construct< Context >( Context::Args( "identifier", "body string" ) );

        ASSERT_EQ( pContext->get_identifier(), "identifier" );
        ASSERT_EQ( pContext->get_body(), "body string" );

        database.store();
    }

    {
        using namespace InterfaceStage;
        Database database( environment, srcFile );

        using namespace Parser;

        Context* pContext = database.one< Context >();

        ASSERT_EQ( pContext->get_identifier(), "identifier" );
        ASSERT_EQ( pContext->get_body(), "body string" );

        database.store();
    }
}

TEST_F( DatabaseTest, Pointer )
{
    {
        using namespace InputParse;
        Database database( environment, srcFile );

        using namespace InputParse::Parser;

        Opaque* pOpaque = database.construct< Opaque >( Opaque::Args( "testopaque" ) );

        Dimension* pDim = database.construct< Dimension >( Dimension::Args( true, "test", "int", "otherstring", pOpaque ) );

        database.store();
    }

    {
        using namespace InterfaceStage;
        Database database( environment, srcFile );

        using namespace Parser;

        Dimension* pDim = database.one< Dimension >();

        Opaque* pOpaque = pDim->get_cppTypeName();

        ASSERT_EQ( pOpaque->get_str(), "testopaque" );
        
        database.store();
    }
}