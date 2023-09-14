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
            symbols.insert( { pUserDim->get_interface_dimension()->get_id()->get_str(), pUserDim->get_concrete_id() } );
        }
    }

    if( Link* pLink = db_cast< Link >( pContext ) )
    {
        links.insert( { pContext->get_interface()->get_identifier(), pContext->get_concrete_id() } );

        /*Interface::LinkInterface* pLinkInterface = pLink->get_link_interface();
        HyperGraph::Relation*     pRelation      = pLinkInterface->get_relation();

        const mega::Ownership ownership = pRelation->get_ownership();

        // const mega::DerivationDirection derivation =
        //     pLinkInterface->get_link_trait()->get_derivation();

        if( pRelation->get_source_interface() == pLinkInterface )
        {
            if( ownership.get() == mega::Ownership::eOwnTarget )
            {
                for( auto pTarget : pRelation->get_targets() )
                {
                    for( auto pConcreteLink : pTarget->get_concrete() )
                    {
                    }
                }
            }
        }
        else if( pRelation->get_target_interface() == pLinkInterface )
        {
            if( ownership.get() == mega::Ownership::eOwnSource )
            {
                for( auto pTarget : pRelation->get_sources() )
                {
                    for( auto pConcreteLink : pTarget->get_concrete() )
                    {
                    }
                }
            }
        }
        else
        {
            THROW_RTE( "Invalid relation" );
        }*/
    }
    else
    {
        symbols.insert( { pContext->get_interface()->get_identifier(), pContext->get_concrete_id() } );
    }

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
    , m_concreteTypeIDs( m_pSymbolTable->get_concrete_type_ids() )
{
}

void PythonDatabase::getObjectSymbols( TypeID::SubValueType objectID, SymbolTable& symbols, SymbolTable& links )
{
    auto iFind = m_concreteTypeIDs.find( TypeID::make_object_from_objectID( objectID ) );
    VERIFY_RTE_MSG( iFind != m_concreteTypeIDs.end(), "Failed to locate concrete type id for object: " << objectID );

    auto pSymbol     = iFind->second;
    auto pContextOpt = pSymbol->get_context();
    VERIFY_RTE( pContextOpt.has_value() );
    auto pObjectContext = pContextOpt.value();
    auto pObject        = FinalStage::db_cast< FinalStage::Concrete::Object >( pObjectContext );
    VERIFY_RTE( pObject );

    recurseTree( symbols, links, pObject );
}

void PythonDatabase::getLinkObjectTypes( TypeID linkTypeID, std::vector< TypeID::SubValueType >& objectTypes )
{
}

/*
std::unordered_map< std::string, mega::TypeID > JITDatabase::getIdentities() const
{
    std::unordered_map< std::string, mega::TypeID > identities;
    for( const auto& [ name, pSymbol ] : m_pSymbolTable->get_symbol_names() )
    {
        identities.insert( { name, pSymbol->get_id() } );
    }

    // add the operations
    for( mega::TypeID::ValueType id = mega::id_Imp_NoParams; id != mega::HIGHEST_OPERATION_TYPE; ++id )
    {
        identities.insert( { mega::getOperationString( mega::OperationID{ id } ), mega::TypeID{ id } } );
    }

    return identities;
}*/
} // namespace mega::runtime