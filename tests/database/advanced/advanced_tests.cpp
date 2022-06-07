
#include "database/common/environments.hpp"

#include "database/types/sources.hpp"

#include "database/model/FirstStage.hxx"
#include "database/model/SecondStage.hxx"
#include "database/model/ThirdStage.hxx"
#include "database/model/FourthStage.hxx"

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

class FirstDBTest : public ::testing::Test
{
public:
    boost::filesystem::path                       m_tempDir;
    std::unique_ptr< mega::io::BuildEnvironment > m_pEnvironment;

    void SetUp()
    {
        m_tempDir = boost::filesystem::temp_directory_path() / common::uuid();
        boost::filesystem::create_directories( m_tempDir );

        m_pEnvironment = std::make_unique< mega::io::BuildEnvironment >( m_tempDir, m_tempDir );

        std::vector< boost::filesystem::path > componentInfoPaths;
        const mega::io::Manifest               manifest( *m_pEnvironment, componentInfoPaths );
        const mega::io::manifestFilePath       projectManifestPath = m_pEnvironment->project_manifest();
        manifest.save_temp( *m_pEnvironment, projectManifestPath );
        m_pEnvironment->temp_to_real( projectManifestPath );
    }
    void TearDown()
    {
        m_pEnvironment.reset();
        namespace bfs = boost::filesystem;
        bfs::remove_all( m_tempDir );
    }
};

TEST_F( FirstDBTest, BasicTypes )
{
    using namespace FirstStage;
    using namespace FirstStage::TestNamespace;

    
}
