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

#include "database/database.hpp"

#include "database/model/FinalStage.hxx"

#include "mega/invocation_io.hpp"

namespace mega::runtime
{

DatabaseInstance::DatabaseInstance( const boost::filesystem::path& projectDatabasePath )
    : m_environment( projectDatabasePath )
    , m_manifest( m_environment, m_environment.project_manifest() )
    , m_database( m_environment, m_manifest.getManifestFilePath() )
    , m_components( m_database.many< FinalStage::Components::Component >( m_manifest.getManifestFilePath() ) )
    , m_pSymbolTable( m_database.one< FinalStage::Symbols::SymbolTable >( m_manifest.getManifestFilePath() ) )
    , m_concreteTypeIDs( m_pSymbolTable->get_concrete_type_ids() )
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
        VERIFY_RTE_MSG( m_pConcreteRoot->get_concrete_id() == 1, "Concrete Root Type ID is NOT one!" );
    }
}

mega::SizeAlignment DatabaseInstance::getObjectSize( mega::TypeID objectType ) const
{
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
            else if( db_cast< MemoryLayout::NonSimpleBuffer >( pBuffer ) )
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

const FinalStage::Operations::Invocation* DatabaseInstance::getInvocation( const mega::InvocationID& invocation ) const
{
    using namespace FinalStage;
    using InvocationMap = std::map< mega::InvocationID, Operations::Invocation* >;

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
    THROW_RTE( "Failed to resolve invocation: " << invocation );
}

mega::TypeID DatabaseInstance::getInterfaceTypeID( mega::TypeID concreteTypeID ) const
{
    auto iFind = m_concreteTypeIDs.find( concreteTypeID );
    VERIFY_RTE_MSG( iFind != m_concreteTypeIDs.end(), "Failed to locate concrete type id: " << concreteTypeID );
    auto pConcreteTypeID = iFind->second;

    if( pConcreteTypeID->get_context().has_value() )
    {
        return pConcreteTypeID->get_context().value()->get_interface()->get_interface_id();
    }
    else if( pConcreteTypeID->get_dim_user().has_value() )
    {
        return pConcreteTypeID->get_dim_user().value()->get_interface_dimension()->get_interface_id();
    }
    else if( pConcreteTypeID->get_dim_allocation().has_value() )
    {
        THROW_RTE( "Interface ID asked for allocation dimension" );
    }
    else if( pConcreteTypeID->get_dim_link().has_value() )
    {
        THROW_RTE( "Interface ID asked for link dimension" );
    }
    else
    {
        THROW_RTE( "Unreachable" );
    }
}

FinalStage::Concrete::Object* DatabaseInstance::getObject( mega::TypeID objectType ) const
{
    auto iFind = m_concreteTypeIDs.find( objectType );
    VERIFY_RTE_MSG( iFind != m_concreteTypeIDs.end(), "Failed to locate concrete type id: " << objectType );
    auto pConcreteTypeID = iFind->second;

    FinalStage::Concrete::Object* pObject = nullptr;
    if( pConcreteTypeID->get_context().has_value() )
    {
        pObject = FinalStage::db_cast< FinalStage::Concrete::Object >( pConcreteTypeID->get_context().value() );
    }
    VERIFY_RTE_MSG( pObject, "Failed to locate concrete object id: " << objectType );
    return pObject;
}

const FinalStage::Components::Component* DatabaseInstance::getComponent( mega::TypeID objectType ) const
{
    auto iFind = m_concreteTypeIDs.find( objectType );
    VERIFY_RTE_MSG( iFind != m_concreteTypeIDs.end(), "Failed to locate concrete type id: " << objectType );
    auto pConcreteTypeID = iFind->second;

    if( pConcreteTypeID->get_context().has_value() )
    {
        return pConcreteTypeID->get_context().value()->get_component();
    }
    THROW_RTE( "Unreachable" );
}

const FinalStage::Components::Component* DatabaseInstance::getOperationComponent( mega::TypeID objectType ) const
{
    auto iFind = m_concreteTypeIDs.find( objectType );
    VERIFY_RTE_MSG( iFind != m_concreteTypeIDs.end(), "Failed to locate concrete type id: " << objectType );
    auto pConcreteTypeID = iFind->second;

    if( pConcreteTypeID->get_context().has_value() )
    {
        return pConcreteTypeID->get_context().value()->get_interface()->get_component();
    }
    THROW_RTE( "Unreachable" );
}
/*
mega::U64 DatabaseInstance::getTotalDomainSize( mega::TypeID concreteID ) const
{
    using namespace FinalStage;

    auto iFind = m_concreteIDs.find( concreteID );
    VERIFY_RTE_MSG( iFind != m_concreteIDs.end(), "Failed to locate concrete type id: " << concreteID );
    FinalStage::Concrete::Context* pContext = iFind->second;

    if ( Concrete::Object* pObject = db_cast< Concrete::Object >( pContext ) )
    {
        return 1;
    }
    else if ( Concrete::Event* pEvent = db_cast< Concrete::Event >( pContext ) )
    {
        return pEvent->get_total_size();
    }
    else if ( Concrete::Action* pAction = db_cast< Concrete::Action >( pContext ) )
    {
        return pAction->get_total_size();
    }
    else if ( Concrete::Link* pLink = db_cast< Concrete::Link >( pContext ) )
    {
        return pLink->get_total_size();
    }
    else if ( Concrete::Buffer* pBuffer = db_cast< Concrete::Buffer >( pContext ) )
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

    auto iFind = m_concreteTypeIDs.find( concreteID );
    VERIFY_RTE_MSG( iFind != m_concreteTypeIDs.end(), "Failed to locate concrete type id: " << concreteID );
    auto pConcreteTypeID = iFind->second;

    if( pConcreteTypeID->get_context().has_value() )
    {
        auto pContext = pConcreteTypeID->get_context().value();
        if( auto pObject = db_cast< Concrete::Object >( pContext ) )
        {
            return 1;
        }
        else if( auto pEvent = db_cast< Concrete::Event >( pContext ) )
        {
            return pEvent->get_local_size();
        }
        else if( auto pAction = db_cast< Concrete::Action >( pContext ) )
        {
            return pAction->get_local_size();
        }
        else if( auto pLink = db_cast< Concrete::Link >( pContext ) )
        {
            return 1;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        THROW_RTE( "Unreachable" );
    }
}

template< typename T >
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

std::vector< FinalStage::Concrete::Object* > DatabaseInstance::getObjects() const
{
    return getPerCompilationFileType< FinalStage::Concrete::Object >( m_manifest, m_database );
}

std::vector< FinalStage::Concrete::Dimensions::User* > DatabaseInstance::getUserDimensions() const
{
    return getPerCompilationFileType< FinalStage::Concrete::Dimensions::User >( m_manifest, m_database );
}

std::vector< FinalStage::Concrete::Dimensions::LinkReference* > DatabaseInstance::getLinkDimensions() const
{
    return getPerCompilationFileType< FinalStage::Concrete::Dimensions::LinkReference >( m_manifest, m_database );
}

std::vector< FinalStage::Concrete::Dimensions::Allocation* > DatabaseInstance::getAllocationDimensions() const
{
    return getPerCompilationFileType< FinalStage::Concrete::Dimensions::Allocation >( m_manifest, m_database );
}

} // namespace mega::runtime
