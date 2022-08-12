
#include "database.hpp"

namespace mega
{
namespace runtime
{

DatabaseInstance::DatabaseInstance( const boost::filesystem::path& projectDatabasePath )
    : m_environment( projectDatabasePath )
    , m_manifest( m_environment, m_environment.project_manifest() )
    , m_database( m_environment, m_manifest.getManifestFilePath() )
    , m_components( m_database.many< FinalStage::Components::Component >( m_manifest.getManifestFilePath() ) )
    , m_pSymbolTable( m_database.one< FinalStage::Symbols::SymbolTable >( m_manifest.getManifestFilePath() ) )
{
}

const FinalStage::Operations::Invocation* DatabaseInstance::getInvocation( const mega::InvocationID& invocation ) const
{
    using namespace FinalStage;
    using InvocationMap = std::map< mega::InvocationID, Operations::Invocation* >;

    {
        for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            const Operations::Invocations* pInvocations = m_database.one< Operations::Invocations >( sourceFilePath );
            const InvocationMap            invocations  = pInvocations->get_invocations();
            InvocationMap::const_iterator  iFind        = invocations.find( invocation );
            if ( iFind != invocations.end() )
            {
                const Operations::Invocation* pInvocation = iFind->second;
                return pInvocation;
            }
        }
    }
    {
        for ( const mega::io::cppFilePath& sourceFilePath : m_manifest.getCppSourceFiles() )
        {
            const Operations::Invocations* pInvocations = m_database.one< Operations::Invocations >( sourceFilePath );
            const InvocationMap            invocations  = pInvocations->get_invocations();
            InvocationMap::const_iterator  iFind        = invocations.find( invocation );
            if ( iFind != invocations.end() )
            {
                const Operations::Invocation* pInvocation = iFind->second;
                return pInvocation;
            }
        }
    }
    THROW_RTE( "Failed to resolve invocation: " << invocation );
}
} // namespace runtime
} // namespace mega