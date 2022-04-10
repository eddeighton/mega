
#include <gtest/gtest.h>

#include "database/io/manifest.hpp"
#include "database/io/environment.hpp"
#include "database/io/component_info.hpp"
#include "database/io/archive.hpp"

#include "database/model/InputParse.hxx"
#include "database/model/Interface.hxx"

#include <boost/filesystem/operations.hpp>

#include <fstream>

TEST( DBTests, Basic )
{
    const boost::filesystem::path tempDir = boost::filesystem::temp_directory_path();
    const boost::filesystem::path srcFile = tempDir / "test1.mega";
    {
        std::ofstream of( srcFile.native() );
        of << "Hello world";
    }

    mega::io::Environment environment( tempDir, tempDir );

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
        using namespace Interface;
        Database database( environment, srcFile );

        ::Interface::Parser::Context* pContext = database.one< ::Interface::Parser::Context >();
        ASSERT_TRUE( pContext );
        ASSERT_EQ( pContext->get_identifier(), "test1" );
        ASSERT_EQ( pContext->get_body(), "test2" );

        ::Interface::Parser::Root* pRoot = database.one< ::Interface::Parser::Root >();

        ASSERT_EQ( pRoot->get_identifier(), "test1" );
        ASSERT_EQ( pRoot->get_body(), "test2" );

    }
}