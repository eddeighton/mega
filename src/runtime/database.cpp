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

#include "database.hpp"
#include "database/model/FinalStage.hxx"

#include "service/network/log.hpp"

namespace mega::runtime
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
        auto       iFind   = symbols.find( ROOT_TYPE_NAME );
        VERIFY_RTE_MSG( iFind != symbols.end(), "Failed to locate Root symbol" );
        auto pSymbol  = iFind->second;
        auto contexts = pSymbol->get_contexts();
        VERIFY_RTE_MSG( contexts.size() > 0U, "Failed to locate Root symbol" );

        FinalStage::Interface::IContext* pRoot = nullptr;
        for ( auto p : contexts )
        {
            if ( FinalStage::dynamic_database_cast< FinalStage::Interface::Root >( p->get_parent() ) )
            {
                VERIFY_RTE_MSG( !pRoot, "Multiple roots found" );
                pRoot = p;
            }
        }
        VERIFY_RTE_MSG( pRoot, "Failed to locate Root symbol" );
        auto concrete = pRoot->get_concrete();
        VERIFY_RTE_MSG( concrete.size() > 0U, "Failed to locate Root symbol" );
        VERIFY_RTE_MSG( concrete.size() == 1U, "Multiple Root symbols defined" );
        auto pConcrete = concrete.front();
        VERIFY_RTE_MSG( pConcrete->get_concrete_id() == 1, "Concrete Root Type ID is NOT one!" );
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

mega::TypeID DatabaseInstance::getInterfaceTypeID( mega::TypeID concreteTypeID ) const
{
    auto iFind = m_concreteIDs.find( concreteTypeID );
    VERIFY_RTE_MSG( iFind != m_concreteIDs.end(), "Failed to locate concrete type id: " << concreteTypeID );
    FinalStage::Concrete::Context* pContext = iFind->second;
    return pContext->get_interface()->get_type_id();
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

const FinalStage::Components::Component* DatabaseInstance::getOperationComponent( mega::TypeID objectType ) const
{
    SPDLOG_TRACE( "RUNTIME: DatabaseInstance::getOperationComponent : {}", objectType );
    auto iFind = m_concreteIDs.find( objectType );
    VERIFY_RTE_MSG( iFind != m_concreteIDs.end(), "Failed to locate concrete type id: " << objectType );
    FinalStage::Concrete::Context*   pContext  = iFind->second;
    FinalStage::Interface::IContext* pIContext = pContext->get_interface();
    return pIContext->get_component();
}
/*
mega::U64 DatabaseInstance::getTotalDomainSize( mega::TypeID concreteID ) const
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
    else if ( Concrete::Buffer* pBuffer = dynamic_database_cast< Concrete::Buffer >( pContext ) )
    {
        return pBuffer->get_total_size();
    }
    else
    {
        return 1;
    }
}
*/
mega::U64 DatabaseInstance::getLocalDomainSize( mega::TypeID concreteID ) const
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
        return pEvent->get_local_size();
    }
    else if ( Concrete::Action* pAction = dynamic_database_cast< Concrete::Action >( pContext ) )
    {
        return pAction->get_local_size();
    }
    else if ( Concrete::Link* pLink = dynamic_database_cast< Concrete::Link >( pContext ) )
    {
        return 1;
    }
    else
    {
        return 1;
    }
}

} // namespace mega::runtime
