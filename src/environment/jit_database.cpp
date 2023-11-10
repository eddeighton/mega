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

#include "environment/jit_database.hpp"

#include "common/unreachable.hpp"

namespace mega::runtime
{

JITDatabase::JITDatabase( const boost::filesystem::path& projectDatabasePath )
    : m_environment( projectDatabasePath )
    , m_manifest( m_environment, m_environment.project_manifest() )
    , m_database( m_environment, m_manifest.getManifestFilePath() )
    , m_components( m_database.many< FinalStage::Components::Component >( m_manifest.getManifestFilePath() ) )
    , m_pSymbolTable( m_database.one< FinalStage::Symbols::SymbolTable >( m_manifest.getManifestFilePath() ) )
    , m_concreteTypeIDs( m_pSymbolTable->get_concrete_type_ids() )
    , m_interfaceTypeIDs( m_pSymbolTable->get_interface_type_ids() )
{
    // determine the root concrete type ID
    {
        const auto symbols = m_pSymbolTable->get_symbol_names();
        auto       iFind   = symbols.find( ROOT_TYPE_NAME );
        VERIFY_RTE_MSG( iFind != symbols.end(), "Failed to locate Root symbol" );
        auto pSymbol  = iFind->second;
        auto contexts = pSymbol->get_contexts();
        VERIFY_RTE_MSG( contexts.size() > 0U, "Failed to locate Root symbol" );

        FinalStage::Interface::IContext* pRoot = nullptr;
        for( auto p : contexts )
        {
            if( FinalStage::db_cast< FinalStage::Interface::Root >( p->get_parent() ) )
            {
                VERIFY_RTE_MSG( !pRoot, "Multiple roots found" );
                pRoot = p;
            }
        }
        VERIFY_RTE_MSG( pRoot, "Failed to locate Root symbol" );
        auto concrete = pRoot->get_concrete();
        VERIFY_RTE_MSG( concrete.size() > 0U, "Failed to locate Root symbol" );
        VERIFY_RTE_MSG( concrete.size() == 1U, "Multiple Root symbols defined" );
        m_pConcreteRoot = FinalStage::db_cast< FinalStage::Concrete::Object >( concrete.front() );
        VERIFY_RTE_MSG( m_pConcreteRoot->get_concrete_id() == ROOT_TYPE_ID, "Concrete Root Type ID is NOT one!" );
    }

    {
        for( FinalStage::HyperGraph::Relation* pRelation :
             m_database.many< FinalStage::HyperGraph::Relation >( m_manifest.getManifestFilePath() ) )
        {
            m_relations.insert( { pRelation->get_id(), pRelation } );
        }
    }
}

const mega::TypeID& getInterfaceTypeID( const FinalStage::Concrete::Graph::Vertex* pVertex )
{
    using namespace FinalStage;
    if( auto pContext = db_cast< const Concrete::Context >( pVertex ) )
    {
        return pContext->get_interface()->get_interface_id();
    }
    else if( auto pDimension = db_cast< const Concrete::Dimensions::User >( pVertex ) )
    {
        return pDimension->get_interface_dimension()->get_interface_id();
    }
    else if( auto pLink = db_cast< const Concrete::Dimensions::Link >( pVertex ) )
    {
        return pLink->get_interface_link()->get_interface_id();
    }
    else if( auto pBitset = db_cast< const Concrete::Dimensions::Bitset >( pVertex ) )
    {
        return pBitset->get_interface_compiler_dimension()->get_interface_id();
    }
    else if( auto pContext = db_cast< const Concrete::Root >( pVertex ) )
    {
        // ignore
        THROW_RTE( "Root has no concrete type" );
    }
    else
    {
        THROW_RTE( "Unknown concrete type" );
    }
    UNREACHABLE;
}

mega::TypeID JITDatabase::getInterfaceTypeID( mega::TypeID concreteTypeID ) const
{
    using ::operator<<;
    VERIFY_RTE_MSG( concreteTypeID != mega::TypeID{}, "Null TypeID in getInterfaceTypeID" );
    auto iFind = m_concreteTypeIDs.find( concreteTypeID );
    VERIFY_RTE_MSG( iFind != m_concreteTypeIDs.end(), "Failed to locate concrete type id: " << concreteTypeID );
    auto pConcreteTypeID = iFind->second;
    return mega::runtime::getInterfaceTypeID( pConcreteTypeID->get_vertex() );
}

void JITDatabase::getConcreteToInterface( ConcreteToInterface& objectTypes ) const
{
    using namespace FinalStage;
    for( const auto& [ id, pConcreteTypeID ] : m_concreteTypeIDs )
    {
        objectTypes.push_back( { id, mega::runtime::getInterfaceTypeID( pConcreteTypeID->get_vertex() ) } );
    }
}

FinalStage::HyperGraph::Relation* JITDatabase::getRelation( const RelationID& relationID ) const
{
    auto    iFind = m_relations.find( relationID );
    using ::operator<<;
    VERIFY_RTE_MSG( iFind != m_relations.end(), "Failed to locate relation: " << relationID );
    return iFind->second;
}

mega::SizeAlignment JITDatabase::getObjectSize( mega::TypeID objectType ) const
{
    VERIFY_RTE_MSG( objectType != mega::TypeID{}, "Null TypeID in getObjectSize" );
    using namespace FinalStage;

    mega::SizeAlignment sizeAlignment;
    {
        for( auto pBuffer : getObject( objectType )->get_buffers() )
        {
            if( db_cast< MemoryLayout::SimpleBuffer >( pBuffer ) )
            {
                VERIFY_RTE( sizeAlignment.size == 0U );
                sizeAlignment.size      = pBuffer->get_size();
                sizeAlignment.alignment = pBuffer->get_alignment();
            }
            else
            {
                THROW_RTE( "Unsupported buffer type" );
            }
        }
    }
    return sizeAlignment;
}

const FinalStage::Operations::Invocation*
JITDatabase::getExistingInvocation( const mega::InvocationID& invocation ) const
{
    using namespace FinalStage;
    using InvocationMap = std::map< mega::InvocationID, Operations::Invocation* >;

    {
        auto iFind = m_dynamicInvocations.find( invocation );
        if( iFind != m_dynamicInvocations.end() )
        {
            return iFind->second;
        }
    }
    {
        for( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            const Operations::Invocations* pInvocations = m_database.one< Operations::Invocations >( sourceFilePath );
            const InvocationMap            invocations  = pInvocations->get_invocations();
            auto                           iFind        = invocations.find( invocation );
            if( iFind != invocations.end() )
            {
                const Operations::Invocation* pInvocation = iFind->second;
                return pInvocation;
            }
        }
    }
    {
        for( const mega::io::cppFilePath& sourceFilePath : m_manifest.getCppSourceFiles() )
        {
            const Operations::Invocations* pInvocations = m_database.one< Operations::Invocations >( sourceFilePath );
            const InvocationMap            invocations  = pInvocations->get_invocations();
            auto                           iFind        = invocations.find( invocation );
            if( iFind != invocations.end() )
            {
                const Operations::Invocation* pInvocation = iFind->second;
                return pInvocation;
            }
        }
    }
    return nullptr;
}

const FinalStage::Operations::Invocation* JITDatabase::getInvocation( const mega::InvocationID& invocation ) const
{
    const FinalStage::Operations::Invocation* pInvocation = getExistingInvocation( invocation );
    using ::                                  operator<<;
    VERIFY_RTE_MSG( pInvocation, "Failed to locate invocation: " << invocation );
    return pInvocation;
}

const FinalStage::Operations::Invocation* JITDatabase::tryGetInvocation( const InvocationID& invocation ) const
{
    return getExistingInvocation( invocation );
}

void JITDatabase::addDynamicInvocation( const InvocationID&                       invocationID,
                                        const FinalStage::Operations::Invocation* pInvocation )
{
    m_dynamicInvocations.insert( { invocationID, pInvocation } );
}

TypeID JITDatabase::getSingularConcreteTypeID( TypeID interfaceTypeID ) const
{
    auto iFind = m_interfaceTypeIDs.find( interfaceTypeID );
    VERIFY_RTE_MSG( iFind != m_interfaceTypeIDs.end(), "Failed to locate interface type id: " << interfaceTypeID );
    auto pInterfaceTypeID = iFind->second;

    // assume only for context
    auto pContextOPT = pInterfaceTypeID->get_context();
    VERIFY_RTE_MSG( pContextOPT.has_value(), "Interface type not a context for type id: " << interfaceTypeID );
    auto pInterfaceContext = pContextOPT.value();

    std::optional< TypeID > matchingConcrete;
    for( auto pConcrete : pInterfaceContext->get_concrete() )
    {
        if( pConcrete->get_interface() == pInterfaceContext )
        {
            VERIFY_RTE( !matchingConcrete.has_value() );
            matchingConcrete = pConcrete->get_concrete_id();
        }
    }
    VERIFY_RTE_MSG( matchingConcrete.has_value(),
                    "Failed to determine unique concrete type for interface type: " << interfaceTypeID );
    return matchingConcrete.value();
}

std::vector< TypeID > JITDatabase::getCompatibleConcreteTypes( TypeID interfaceTypeID ) const
{
    using ::operator<<;
    VERIFY_RTE_MSG( interfaceTypeID != mega::TypeID{}, "Null TypeID in getCompatibleConcreteTypes" );
    std::vector< TypeID > result;

    auto iFind = m_interfaceTypeIDs.find( interfaceTypeID );
    VERIFY_RTE_MSG( iFind != m_interfaceTypeIDs.end(), "Failed to locate interface type id: " << interfaceTypeID );
    auto pInterfaceTypeID = iFind->second;

    auto pContextOPT = pInterfaceTypeID->get_context();
    VERIFY_RTE_MSG( pContextOPT.has_value(), "Interface type not a context for type id: " << interfaceTypeID );
    if( pContextOPT.has_value() )
    {
        FinalStage::Interface::IContext* pIContext = pContextOPT.value();
        for( auto pConcrete : pIContext->get_concrete() )
        {
            result.push_back( pConcrete->get_concrete_id() );
        }
    }
    return result;
}

FinalStage::Concrete::Object* JITDatabase::getObject( mega::TypeID objectType ) const
{
    using ::operator<<;
    VERIFY_RTE_MSG( objectType != mega::TypeID{}, "Null TypeID in getObject" );
    auto iFind = m_concreteTypeIDs.find( objectType );
    VERIFY_RTE_MSG( iFind != m_concreteTypeIDs.end(), "Failed to locate concrete type id: " << objectType );
    auto pConcreteTypeID = iFind->second;

    FinalStage::Concrete::Object* pObject = db_cast< FinalStage::Concrete::Object >( pConcreteTypeID->get_vertex() );
    VERIFY_RTE_MSG( pObject, "Failed to locate concrete object id: " << objectType );
    return pObject;
}

FinalStage::Interface::Action* JITDatabase::getAction( mega::TypeID interfaceTypeID ) const
{
    using ::operator<<;
    VERIFY_RTE_MSG( interfaceTypeID != mega::TypeID{}, "Null TypeID in getAction" );
    auto iFind = m_interfaceTypeIDs.find( interfaceTypeID );
    VERIFY_RTE_MSG( iFind != m_interfaceTypeIDs.end(), "Failed to locate interface type id: " << interfaceTypeID );
    auto                           pInterfaceTypeID = iFind->second;
    FinalStage::Interface::Action* pAction          = nullptr;
    if( pInterfaceTypeID->get_context().has_value() )
    {
        pAction = FinalStage::db_cast< FinalStage::Interface::Action >( pInterfaceTypeID->get_context().value() );
    }
    VERIFY_RTE_MSG( pAction, "Failed to locate concrete action id: " << interfaceTypeID );
    return pAction;
}

const FinalStage::Components::Component* JITDatabase::getComponent( mega::TypeID objectType ) const
{
    using ::operator<<;
    VERIFY_RTE_MSG( objectType != mega::TypeID{}, "Null TypeID in getComponent" );
    auto iFind = m_concreteTypeIDs.find( objectType );
    VERIFY_RTE_MSG( iFind != m_concreteTypeIDs.end(), "Failed to locate concrete type id: " << objectType );
    auto pConcreteTypeID = iFind->second;
    return pConcreteTypeID->get_vertex()->get_component();
}

const FinalStage::Components::Component* JITDatabase::getOperationComponent( mega::TypeID interfaceTypeID ) const
{
    using ::operator<<;
    VERIFY_RTE_MSG( interfaceTypeID != mega::TypeID{}, "Null TypeID in getOperationComponent" );
    auto iFind = m_interfaceTypeIDs.find( interfaceTypeID );
    VERIFY_RTE_MSG( iFind != m_interfaceTypeIDs.end(), "Failed to locate concrete type id: " << interfaceTypeID );
    auto pInterfaceTypeID = iFind->second;

    if( pInterfaceTypeID->get_context().has_value() )
    {
        return pInterfaceTypeID->get_context().value()->get_component();
    }
    THROW_RTE( "JITDatabase::getOperationComponent Unreachable" );
}

template < typename T >
std::vector< T* > getPerCompilationFileType( const mega::io::Manifest& manifest, const FinalStage::Database& database )
{
    std::vector< T* > result;
    for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
    {
        for( auto pObject : database.many< T >( sourceFilePath ) )
        {
            result.push_back( pObject );
        }
    }
    return result;
}

std::vector< FinalStage::Concrete::Object* > JITDatabase::getObjects() const
{
    return getPerCompilationFileType< FinalStage::Concrete::Object >( m_manifest, m_database );
}
std::vector< FinalStage::Concrete::Action* > JITDatabase::getActions() const
{
    return getPerCompilationFileType< FinalStage::Concrete::Action >( m_manifest, m_database );
}

std::vector< FinalStage::Concrete::Dimensions::User* > JITDatabase::getUserDimensions() const
{
    return getPerCompilationFileType< FinalStage::Concrete::Dimensions::User >( m_manifest, m_database );
}

std::vector< FinalStage::Concrete::Dimensions::Link* > JITDatabase::getLinkDimensions() const
{
    return getPerCompilationFileType< FinalStage::Concrete::Dimensions::Link >( m_manifest, m_database );
}

JITDatabase::PrefabBindings JITDatabase::getPrefabBindings() const
{
    return m_database.many< FinalStage::UnityAnalysis::Binding >( m_environment.project_manifest() );
}

} // namespace mega::runtime
