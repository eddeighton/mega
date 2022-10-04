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

#include "database/common/environment_build.hpp"

#include "database/types/sources.hpp"

#include "database/model/FirstStage.hxx"
#include "database/model/SecondStage.hxx"

#include "database/model/manifest.hxx"
#include "database/model/environment.hxx"

#include "database/common/component_info.hpp"
#include "database/common/serialisation.hpp"

#include <common/file.hpp>
#include <common/string.hpp>

#include <gtest/gtest.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fstream>
#include <memory>
#include <optional>

class MultiStageBaseDBTest : public ::testing::Test
{
public:
    boost::filesystem::path                        m_tempDir;
    std::unique_ptr< mega::compiler::Directories > m_pDirectories;
    std::unique_ptr< mega::io::BuildEnvironment >  m_pEnvironment;

    virtual void SetUp() override
    {
        m_tempDir = boost::filesystem::temp_directory_path() / "MultiStageBaseDBTest" / common::uuid();
        boost::filesystem::create_directories( m_tempDir );

        m_pDirectories = std::make_unique< mega::compiler::Directories >(
            mega::compiler::Directories{ m_tempDir, m_tempDir, "", "" } );
        m_pEnvironment = std::make_unique< mega::io::BuildEnvironment >( *m_pDirectories );

        std::vector< boost::filesystem::path > componentInfoPaths;
        const mega::io::Manifest               manifest( *m_pEnvironment, componentInfoPaths );
        const mega::io::manifestFilePath       projectManifestPath = m_pEnvironment->project_manifest();
        manifest.save_temp( *m_pEnvironment, projectManifestPath );
        m_pEnvironment->temp_to_real( projectManifestPath );
    }
    virtual void TearDown() override
    {
        m_pEnvironment.reset();
        namespace bfs = boost::filesystem;
        bfs::remove_all( boost::filesystem::temp_directory_path() / "MultiStageBaseDBTest" );
    }
};

TEST_F( MultiStageBaseDBTest, CreateObjectInTwoStages )
{
    {
        using namespace FirstStage;
        using namespace FirstStage::MultiStageBaseTest;

        Database database( *m_pEnvironment, m_pEnvironment->project_manifest() );

        TestObject* pTestObject = database.construct< TestObject >( TestObject::Args{ 1 } );

        ASSERT_EQ( pTestObject->get_anInt(), 1 );
        ASSERT_TRUE( pTestObject );

        database.save_FirstFile_to_temp();
        m_pEnvironment->temp_to_real( m_pEnvironment->FirstStage_FirstFile( m_pEnvironment->project_manifest() ) );
    }
    {
        using namespace SecondStage;
        using namespace SecondStage::MultiStageBaseTest;

        Database database( *m_pEnvironment, m_pEnvironment->project_manifest() );

        auto existing = database.many< TestObject >( m_pEnvironment->project_manifest() );
        ASSERT_EQ( existing.size(), 1 );
        for ( auto p : existing )
        {
            ASSERT_EQ( p->get_anInt(), 1 );
        }

        // create object in later stage
        TestObject* pTestObject = database.construct< TestObject >( TestObject::Args{ 2 } );
        ASSERT_TRUE( pTestObject );
        ASSERT_EQ( pTestObject->get_anInt(), 2 );

        auto existing2 = database.many< TestObject >( m_pEnvironment->project_manifest() );
        ASSERT_EQ( existing2.size(), 2 );

        database.save_SecondFile_to_temp();
        m_pEnvironment->temp_to_real( m_pEnvironment->SecondStage_SecondFile( m_pEnvironment->project_manifest() ) );
    }

}
