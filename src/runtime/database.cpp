
#include "database.hpp"
#include "database/model/FinalStage.hxx"

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
    , m_concreteIDs( m_pSymbolTable->get_concrete_context_map() )
{
    // determine the root concrete type ID
    {
        const auto symbols = m_pSymbolTable->get_symbols();
        auto iFind = symbols.find( "Root" );
        VERIFY_RTE_MSG( iFind != symbols.end(), "Failed to locate Root symbol" );
        auto pSymbol = iFind->second;
        auto contexts = pSymbol->get_contexts();
        VERIFY_RTE_MSG( contexts.size() > 0U, "Failed to locate Root symbol" );
        VERIFY_RTE_MSG( contexts.size() == 1U, "Multiple Root symbols defined" );
        auto pIContext = contexts.front();
        auto concrete = pIContext->get_concrete();
        VERIFY_RTE_MSG( concrete.size() > 0U, "Failed to locate Root symbol" );
        VERIFY_RTE_MSG( concrete.size() == 1U, "Multiple Root symbols defined" );
        auto pConcrete = concrete.front();
        m_rootTypeID = pConcrete->get_concrete_id();
    }
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

const FinalStage::Concrete::Object* DatabaseInstance::getObject( mega::TypeID objectType ) const
{
    auto iFind = m_concreteIDs.find( objectType );
    VERIFY_RTE_MSG( iFind != m_concreteIDs.end(), "Failed to locate concrete type id: " << objectType );
    FinalStage::Concrete::Context* pContext = iFind->second;
    FinalStage::Concrete::Object*  pObject
        = FinalStage::dynamic_database_cast< FinalStage::Concrete::Object >( pContext );
    VERIFY_RTE_MSG( pObject, "Failed to locate concrete object id: " << objectType );
    return pObject;
}

const FinalStage::Components::Component* DatabaseInstance::getComponent( mega::TypeID objectType ) const
{
    auto iFind = m_concreteIDs.find( objectType );
    VERIFY_RTE_MSG( iFind != m_concreteIDs.end(), "Failed to locate concrete type id: " << objectType );
    FinalStage::Concrete::Context* pContext = iFind->second;
    return pContext->get_component();
}

std::size_t DatabaseInstance::getConcreteContextTotalAllocation( mega::TypeID concreteID ) const
{
    using namespace FinalStage;

    auto iFind = m_concreteIDs.find( concreteID );
    VERIFY_RTE_MSG( iFind != m_concreteIDs.end(), "Failed to locate concrete type id: " << concreteID );
    FinalStage::Concrete::Context* pContext = iFind->second;

    if ( Concrete::Object* pObject = dynamic_database_cast< Concrete::Object >( pContext ) )
    {
        return 1;
    }
    else if ( Concrete::Event* pEvent = dynamic_database_cast< Concrete::Event >( pContext ) )
    {
        return pEvent->get_total_size();
    }
    else if ( Concrete::Action* pAction = dynamic_database_cast< Concrete::Action >( pContext ) )
    {
        return pAction->get_total_size();
    }
    else if ( Concrete::Link* pLink = dynamic_database_cast< Concrete::Link >( pContext ) )
    {
        return pLink->get_total_size();
    }
    else
    {
        return 1;
    }
}

mega::TypeID DatabaseInstance::getRootTypeID() const 
{
    return m_rootTypeID;
}

} // namespace runtime
} // namespace mega