
#include <gtest/gtest.h>

#include "database/common/component_info.hpp"
#include "database/common/serialisation.hpp"
#include "database/common/environments.hpp"

#include "database/types/sources.hpp"

#include "database/model/ComponentListing.hxx"
#include "database/model/ParserStage.hxx"
#include "database/model/manifest.hxx"
#include "database/model/environment.hxx"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fstream>

class DatabaseTest : public ::testing::Test
{
public:
    boost::filesystem::path    m_tempDir;
    std::unique_ptr< mega::io::BuildEnvironment > m_pEnvironment;
    boost::filesystem::path    m_srcFile;

    virtual void SetUp() override
    {
        m_tempDir = boost::filesystem::temp_directory_path() / "DatabaseTest" / common::uuid();
        boost::filesystem::create_directories( m_tempDir );

        m_pEnvironment = std::make_unique< mega::io::BuildEnvironment >( m_tempDir, m_tempDir );
        m_srcFile = m_tempDir / "test1.mega";
        std::ofstream of( m_srcFile.native() );
        of << "Hello world";

        // create a manifest
        {
            std::vector< boost::filesystem::path > componentInfoPaths;
            {
                mega::io::ComponentInfo componentInfo(
                    "test", {}, {}, m_tempDir, mega::io::ComponentInfo::PathArray{ m_srcFile }, mega::io::ComponentInfo::PathArray{} );
                const boost::filesystem::path componentInfoPath = m_tempDir / "test.txt";
                std::ofstream                 of( componentInfoPath.native() );
                mega::OutputArchiveType       oa( of );
                oa << boost::serialization::make_nvp( "componentInfo", componentInfo );
                componentInfoPaths.push_back( componentInfoPath );
            }

            const mega::io::Manifest         manifest( *m_pEnvironment, componentInfoPaths );
            const mega::io::manifestFilePath projectManifestPath = m_pEnvironment->project_manifest();
            //manifest.save( *m_pEnvironment, projectManifestPath );
        }
    }

    virtual void TearDown() override
    {
        namespace bfs = boost::filesystem;
        bfs::remove_all( boost::filesystem::temp_directory_path() / "DatabaseTest" );
    }
};
/*
TEST_F( DatabaseTest, Basic )
{
    {
        using namespace InputParse;
        Database database( *m_pEnvironment, m_srcFile );

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
        Database database( *m_pEnvironment, m_srcFile );

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
        Database database( *m_pEnvironment, m_srcFile );

        Parser::Opaque*    pOpaque = database.construct< Parser::Opaque >( Parser::Opaque::Args( "testopaque" ) );
        Parser::Dimension* pDim
            = database.construct< Parser::Dimension >( Parser::Dimension::Args( true, "test", "int", "otherstring", pOpaque, {}, {}, {} ) );

        database.store();
    }

    {
        using namespace InterfaceStage;
        Database database( *m_pEnvironment, m_srcFile );

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
        Database database( *m_pEnvironment, m_srcFile );

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
        Database database( *m_pEnvironment, m_srcFile );

        using namespace InputParse::Parser;

        Context* pContext = database.construct< Context >( Context::Args( "identifier", "body string" ) );

        ASSERT_EQ( pContext->get_identifier(), "identifier" );
        ASSERT_EQ( pContext->get_body(), "body string" );

        database.store();
    }

    {
        using namespace InterfaceStage;
        Database database( *m_pEnvironment, m_srcFile );

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
        Database database( *m_pEnvironment, m_srcFile );

        using namespace InputParse::Parser;

        Opaque* pOpaque = database.construct< Opaque >( Opaque::Args( "testopaque" ) );

        Dimension* pDim = database.construct< Dimension >( Dimension::Args( true, "test", "int", "otherstring", pOpaque, {}, {}, {} ) );

        database.store();
    }

    {
        using namespace InterfaceStage;
        Database database( *m_pEnvironment, m_srcFile );

        using namespace Parser;

        Dimension* pDim = database.one< Dimension >();

        Opaque* pOpaque = pDim->get_cppTypeName();

        ASSERT_EQ( pOpaque->get_str(), "testopaque" );

        database.store();
    }
}

TEST_F( DatabaseTest, ArrayOfPointers )
{
    {
        using namespace InputParse;
        Database database( *m_pEnvironment, m_srcFile );

        using namespace InputParse::Parser;

        Opaque* pOpaque1 = database.construct< Opaque >( Opaque::Args( "testopaque1" ) );
        Opaque* pOpaque2 = database.construct< Opaque >( Opaque::Args( "testopaque2" ) );
        Opaque* pOpaque3 = database.construct< Opaque >( Opaque::Args( "testopaque3" ) );
        Opaque* pOpaque4 = database.construct< Opaque >( Opaque::Args( "testopaque4" ) );

        Dimension* pDim = database.construct< Dimension >(
            Dimension::Args( true, "test", "int", "otherstring", pOpaque1, std::vector< Opaque* >{ pOpaque3, pOpaque2, pOpaque4 }, {}, {})
);

        database.store();
    }

    {
        using namespace InterfaceStage;
        Database database( *m_pEnvironment, m_srcFile );

        using namespace Parser;

        Dimension* pDim = database.one< Dimension >();

        std::vector< Opaque* > list = pDim->get_children1();

        ASSERT_EQ( list.size(), 3U );
        ASSERT_EQ( list[ 0 ]->get_str(), "testopaque3" );
        ASSERT_EQ( list[ 1 ]->get_str(), "testopaque2" );
        ASSERT_EQ( list[ 2 ]->get_str(), "testopaque4" );

        database.store();
    }
}

TEST_F( DatabaseTest, MapOfPointers )
{
    {
        using namespace InputParse;
        Database database( *m_pEnvironment, m_srcFile );

        using namespace InputParse::Parser;

        Opaque* pOpaque1 = database.construct< Opaque >( Opaque::Args( "testopaque1" ) );
        Opaque* pOpaque2 = database.construct< Opaque >( Opaque::Args( "testopaque2" ) );
        Opaque* pOpaque3 = database.construct< Opaque >( Opaque::Args( "testopaque3" ) );
        Opaque* pOpaque4 = database.construct< Opaque >( Opaque::Args( "testopaque4" ) );

        Dimension::Args args( true, "test", "int", "otherstring", pOpaque1, {}, {}, {});

        args.map1.value().insert( std::make_pair( 123   , pOpaque1 ) );
        args.map1.value().insert( std::make_pair( 2     , pOpaque2 ) );
        args.map1.value().insert( std::make_pair( 43    , pOpaque3 ) );
        args.map1.value().insert( std::make_pair( 345   , pOpaque4 ) );

        args.map2.value().insert( std::make_pair( pOpaque1, 123 ) );
        args.map2.value().insert( std::make_pair( pOpaque2, 2   ) );
        args.map2.value().insert( std::make_pair( pOpaque3, 43  ) );
        args.map2.value().insert( std::make_pair( pOpaque4, 345 ) );

        Dimension* pDim = database.construct< Dimension >(args );

        database.store();
    }

    {
        using namespace InterfaceStage;
        Database database( *m_pEnvironment, m_srcFile );

        using namespace Parser;

        Dimension* pDim = database.one< Dimension >();

        std::map< int, Opaque* > map1 = pDim->get_map1();
        std::map< Opaque*, int > map2 = pDim->get_map2();

        ASSERT_EQ( map1.size(), 4U );
        ASSERT_EQ( map1[ 123  ]->get_str(), "testopaque1" );
        ASSERT_EQ( map1[ 2    ]->get_str(), "testopaque2" );
        ASSERT_EQ( map1[ 43   ]->get_str(), "testopaque3" );
        ASSERT_EQ( map1[ 345  ]->get_str(), "testopaque4" );

        ASSERT_EQ( map2.size(), 4U );
        ASSERT_EQ( map2[ map1[ 123  ] ], 123  );
        ASSERT_EQ( map2[ map1[ 2    ] ], 2    );
        ASSERT_EQ( map2[ map1[ 43   ] ], 43   );
        ASSERT_EQ( map2[ map1[ 345  ] ], 345  );

        database.store();
    }
}
*/
TEST_F( DatabaseTest, Inserter )
{
    {
        // using namespace ComponentListing;
        // Database database( *m_pEnvironment );
        //
        // using namespace ComponentListing::Components;
        //
        // Component::Args args( "test", "somepath", {}, {});
        // Component* pComponent = database.construct< Component >( args );
        //
        // ASSERT_TRUE( pComponent->get_includeDirectories().empty() );
        //
        // pComponent->push_back_includeDirectories("testpath");
        //
        // ASSERT_EQ( pComponent->get_includeDirectories().front(), "testpath" );
    }
}