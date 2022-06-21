
#include "runtime/component_manager.hpp"

#include "service/network/log.hpp"

#include "database/common/exception.hpp"
#include "database/common/manifest_data.hpp"
#include "database/model/FinalStage.hxx"
#include "database/model/manifest.hxx"

namespace mega
{
namespace runtime
{

ComponentManager::ComponentManager( const mega::network::Project& project ) { reinitialise( project ); }

ComponentManager::~ComponentManager() {}

void ComponentManager::reinitialise( const mega::network::Project& project )
{
    try
    {
        m_pArchive.reset();
        m_pArchive.reset( new mega::io::ArchiveEnvironment( project.getProjectDatabase() ) );

        mega::io::Manifest manifest( *m_pArchive, m_pArchive->project_manifest() );

        using namespace FinalStage;
        Database database( *m_pArchive, manifest.getManifestFilePath() );

        std::vector< Components::Component* > components
            = database.many< Components::Component >( manifest.getManifestFilePath() );

        Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( manifest.getManifestFilePath() );

        VERIFY_RTE( pSymbolTable );

        SPDLOG_INFO( "ComponentManager initialised with project: {}", project.getProjectInstallPath().string() );

        /*
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            for ( Interface::Root* pRoot : database.many< Interface::Root >( sourceFilePath ) )
            {
                for ( Interface::IContext* pChildContext : pRoot->get_children() )
                {
                }
            }
        }*/
    }
    catch ( mega::io::DatabaseVersionException& ex )
    {
        SPDLOG_ERROR( "ComponentManager failed to initialise project: {} database version exception: {}",
                      project.getProjectInstallPath().string(), ex.what() );
    }
    catch ( std::exception& ex )
    {
        SPDLOG_ERROR( "ComponentManager failed to initialise project: {} error: {}",
                      project.getProjectInstallPath().string(), ex.what() );
    }
}

} // namespace runtime
} // namespace mega