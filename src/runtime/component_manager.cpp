
#include "runtime/component_manager.hpp"

#include "database/model/FinalStage.hxx"
#include "database/model/manifest.hxx"

namespace mega
{
namespace runtime
{

ComponentManager::ComponentManager( const boost::filesystem::path& archiveDatabase )
{
    reinitialise( archiveDatabase );
}

ComponentManager::~ComponentManager() {}

void ComponentManager::reinitialise( const boost::filesystem::path& archiveDatabase )
{
    m_pArchive.reset();
    m_pArchive.reset( new mega::io::ArchiveEnvironment( archiveDatabase ) );

    mega::io::Manifest manifest( *m_pArchive, m_pArchive->project_manifest() );

    using namespace FinalStage;
    Database database( *m_pArchive, manifest.getManifestFilePath() );

    std::vector< Components::Component* > components
        = database.many< Components::Component >( manifest.getManifestFilePath() );

    Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( manifest.getManifestFilePath() );

    VERIFY_RTE( pSymbolTable );

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

} // namespace runtime
} // namespace mega