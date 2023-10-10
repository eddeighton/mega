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

#include "database/python_database.hpp"

namespace mega::runtime
{

namespace
{

void recurseTree( PythonDatabase::SymbolTable&   symbols,
                  PythonDatabase::SymbolTable&   links,
                  FinalStage::Concrete::Context* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;

    if( auto pDimensionContext = db_cast< UserDimensionContext >( pContext ) )
    {
        for( auto pUserDim : pDimensionContext->get_dimensions() )
        {
            symbols.insert( { pUserDim->get_interface_dimension()->get_id()->get_str(),
                              pUserDim->get_interface_dimension()->get_symbol_id() } );
        }
        for( auto pLinkDim : pDimensionContext->get_links() )
        {
            if( auto pUserLink = db_cast< Dimensions::UserLink >( pLinkDim ) )
            {
                links.insert( { pUserLink->get_interface_link()->get_id()->get_str(),
                                pUserLink->get_interface_link()->get_symbol_id() } );
            }
            else if( auto pOwnershipLink = db_cast< Dimensions::OwnershipLink >( pLinkDim ) )
            {
                // Ownership symbol
            }
            else
            {
                THROW_RTE( "Unknown link type" );
            }
        }
    }

    symbols.insert( { pContext->get_interface()->get_identifier(), pContext->get_interface()->get_symbol_id() } );

    for( Concrete::Context* pChildContext : pContext->get_children() )
    {
        recurseTree( symbols, links, pChildContext );
    }
}

} // namespace

PythonDatabase::PythonDatabase( const boost::filesystem::path& projectDatabasePath )
    : m_environment( projectDatabasePath )
    , m_manifest( m_environment, m_environment.project_manifest() )
    , m_database( m_environment, m_manifest.getManifestFilePath() )
    , m_pSymbolTable( m_database.one< FinalStage::Symbols::SymbolTable >( m_manifest.getManifestFilePath() ) )
    , m_symbolTypeIDs( m_pSymbolTable->get_symbol_type_ids() )
    , m_interfaceTypeIDs( m_pSymbolTable->get_interface_type_ids() )
    , m_concreteTypeIDs( m_pSymbolTable->get_concrete_type_ids() )
{
    
}

PythonDatabase::ObjectTypesMap PythonDatabase::getObjectTypes()
{
    ObjectTypesMap result;

    for( const auto& [ typeID, pInterfaceTypeID ] : m_interfaceTypeIDs )
    {
        if( auto contextOpt = pInterfaceTypeID->get_context(); contextOpt.has_value() )
        {
            auto pContext = contextOpt.value();
            if( db_cast< FinalStage::Interface::Object >( pContext ) )
            {
                result.insert( { pContext->get_identifier(), typeID.getObjectID() } );
            }
        }
    }
    return result;
}

void PythonDatabase::getConcreteObjectSymbols( TypeID::SubValueType objectConcreteID, SymbolTable& symbols,
                                               SymbolTable& links )
{
    auto iFind = m_concreteTypeIDs.find( TypeID::make_object_from_objectID( objectConcreteID ) );
    VERIFY_RTE_MSG(
        iFind != m_concreteTypeIDs.end(), "Failed to locate concrete type id for object: " << objectConcreteID );

    auto pSymbol     = iFind->second;
    auto pContextOpt = pSymbol->get_context();
    VERIFY_RTE( pContextOpt.has_value() );
    auto pObjectContext = pContextOpt.value();
    auto pObject        = FinalStage::db_cast< FinalStage::Concrete::Object >( pObjectContext );
    VERIFY_RTE( pObject );

    recurseTree( symbols, links, pObject );
}

void PythonDatabase::getLinkObjectTypes( TypeID::SubValueType concreteObjectID, TypeID linkTypeID,
                                         std::set< TypeID::SubValueType >& objectTypes )
{
    using namespace FinalStage;

    auto iFind = m_symbolTypeIDs.find( linkTypeID );
    VERIFY_RTE_MSG( iFind != m_symbolTypeIDs.end(), "Failed to locate interface type id for link: " << linkTypeID );
    auto pSymbol = iFind->second;

    std::vector< Interface::LinkTrait* > interfaceLinkTraits;
    for( auto pLinkTrait : pSymbol->get_links() )
    {
        for( auto pConcrete : pLinkTrait->get_concrete() )
        {
            auto object = pConcrete->get_parent_context()->get_concrete_object();
            VERIFY_RTE( object.has_value() );
            auto pObject = object.value();
            if( pObject->get_concrete_id().getObjectID() == concreteObjectID )
            {
                interfaceLinkTraits.push_back( pLinkTrait );
                break;
            }
        }
    }

    VERIFY_RTE_MSG(
        !interfaceLinkTraits.empty(),
        "Failed to determine interface link for object: " << concreteObjectID << " and symbol: " << linkTypeID );

    for( auto pDimLink : interfaceLinkTraits )
    {
        auto pRelation = pDimLink->get_relation();
        if( auto pOwning = db_cast< HyperGraph::OwningObjectRelation >( pRelation ) )
        {
            if( pDimLink->get_owning() )
            {
                auto owners = pOwning->get_owners();
                for( auto i = owners.lower_bound( pDimLink ), iEnd = owners.upper_bound( pDimLink ); i != iEnd; ++i )
                {
                    auto pOwnershipLink = i->second;
                    auto pObject        = pOwnershipLink->get_parent_context()->get_concrete_object().value();
                    objectTypes.insert( pObject->get_concrete_id().getObjectID() );
                }
            }
            else
            {
                auto owned = pOwning->get_owned();
                for( auto pConcrete : pDimLink->get_concrete() )
                {
                    auto pOwnershipLink = db_cast< Concrete::Dimensions::OwnershipLink >( pConcrete );
                    for( auto i = owned.lower_bound( pOwnershipLink ), iEnd = owned.upper_bound( pOwnershipLink );
                         i != iEnd; ++i )
                    {
                        auto pLinkTrait = i->second;
                        for( auto pOwnerLinkTrait : pLinkTrait->get_concrete() )
                        {
                            auto objectOpt = pOwnerLinkTrait->get_parent_context()->get_concrete_object();
                            VERIFY_RTE( objectOpt.has_value() );
                            objectTypes.insert( objectOpt.value()->get_concrete_id().getObjectID() );
                        }
                    }
                }
            }
        }
        else if( auto pNonOwning = db_cast< HyperGraph::NonOwningObjectRelation >( pRelation ) )
        {
            if( pDimLink == pNonOwning->get_source() )
            {
                for( auto pOwnerLinkTrait : pNonOwning->get_target()->get_concrete() )
                {
                    auto objectOpt = pOwnerLinkTrait->get_parent_context()->get_concrete_object();
                    VERIFY_RTE( objectOpt.has_value() );
                    objectTypes.insert( objectOpt.value()->get_concrete_id().getObjectID() );
                }
            }
            else if( pDimLink == pNonOwning->get_target() )
            {
                for( auto pOwnerLinkTrait : pNonOwning->get_source()->get_concrete() )
                {
                    auto objectOpt = pOwnerLinkTrait->get_parent_context()->get_concrete_object();
                    VERIFY_RTE( objectOpt.has_value() );
                    objectTypes.insert( objectOpt.value()->get_concrete_id().getObjectID() );
                }
            }
            else
            {
                THROW_RTE( "Could not match link trait to non owning relation" );
            }
        }
        else
        {
            THROW_RTE( "Unknown relation type" );
        }
    }
}

} // namespace mega::runtime