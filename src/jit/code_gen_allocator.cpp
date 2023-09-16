
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

#include "code_generator.hpp"
#include "symbol_utils.hpp"

#include "mega/types/traits.hpp"

namespace mega::runtime
{

std::string fullInterfaceTypeName( FinalStage::Interface::IContext* pIContext )
{
    std::ostringstream osFullTypeName;
    {
        std::vector< FinalStage::Interface::IContext* > contexts;
        for( ; pIContext; pIContext = db_cast< FinalStage::Interface::IContext >( pIContext->get_parent() ) )
        {
            contexts.push_back( pIContext );
        }
        std::reverse( contexts.begin(), contexts.end() );
        bool bFirst = true;
        for( auto p : contexts )
        {
            if( bFirst )
                bFirst = false;
            else
                osFullTypeName << '_';
            osFullTypeName << p->get_identifier();
        }
    }
    return osFullTypeName.str();
}

std::string fullInterfaceTypeName( FinalStage::Interface::DimensionTrait* pDim )
{
    std::ostringstream osFullTypeName;
    osFullTypeName << fullInterfaceTypeName( pDim->get_parent() ) << '_' << pDim->get_id()->get_str();
    return osFullTypeName.str();
}

mega::U64 concreteLocalDomainSize( const FinalStage::Concrete::Context* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;
    if( auto pEvent = db_cast< const Event >( pContext ) )
    {
        return pEvent->get_local_size();
    }
    else if( auto pAction = db_cast< const Action >( pContext ) )
    {
        return pAction->get_local_size();
    }
    else
    {
        return 1;
    }
}

std::string getContextTypeClass( const FinalStage::Concrete::Context* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;
    if( db_cast< const Object >( pContext ) )
        return "object";
    else if( db_cast< const Action >( pContext ) )
        return "action";
    else if( db_cast< const Event >( pContext ) )
        return "event";
    else if( db_cast< const Function >( pContext ) )
        return "function";
    else if( db_cast< const Namespace >( pContext ) )
        return "namespace";
    else if( db_cast< const Interupt >( pContext ) )
        return "interupt";
    else
    {
        THROW_RTE( "Unknown context type class" );
    }
}
/*
void getLinkOwnership( const FinalStage::Concrete::Link* pConcreteLink, bool& bSource, bool& bOwning, bool& bOwned )
{
    using namespace FinalStage;

    THROW_TODO;

    const Interface::LinkInterface* pLinkInterface = pConcreteLink->get_link_interface();
    const HyperGraph::Relation*     pRelation      = pLinkInterface->get_relation();

    bSource = false;
    if( pRelation->get_source_interface() == pLinkInterface )
    {
        bSource = true;
    }
    else if( pRelation->get_target_interface() == pLinkInterface )
    {
        bSource = false;
    }
    else
    {
        THROW_RTE( "Invalid link" );
    }

    bOwning = false;
    bOwned  = false;
    {
        if( bSource )
        {
            if( pRelation->get_ownership().get() == mega::Ownership::eOwnTarget )
                bOwning = true;
            if( pRelation->get_ownership().get() == mega::Ownership::eOwnSource )
                bOwned = true;
        }
        else
        {
            if( pRelation->get_ownership().get() == mega::Ownership::eOwnTarget )
                bOwned = true;
            if( pRelation->get_ownership().get() == mega::Ownership::eOwnSource )
                bOwning = true;
        }
    }
}*/

std::string makeStartState( const mega::TypeID& typeID )
{
    return printTypeID( typeID );
}

std::string makeEndState( const mega::TypeID& typeID )
{
    std::ostringstream os;
    os << static_cast< mega::TypeID::ValueType >( TypeID::make_end_state( typeID ) );
    return os.str();
}

void recurseTraversalStates( const JITDatabase& database, nlohmann::json& data,
                             const FinalStage::Concrete::Context* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;

    auto makeContextState = [ & ]( const Context* pContext, bool bStart, const std::string& strSuccessorState )
    {
        bool bOwning = false, bOwned = false;
        THROW_TODO;
        /*if( auto pConcreteLink = db_cast< const Link >( pContext ) )
        {
            bool bSource = false;
            getLinkOwnership( pConcreteLink, bSource, bOwning, bOwned );
        }*/

        nlohmann::json state( { { "value", bStart ? makeStartState( pContext->get_concrete_id() )
                                                  : makeEndState( pContext->get_concrete_id() ) },
                                { "start", bStart },
                                { "owning", bOwning },
                                { "owned", bOwned },
                                { "local_domain_size", concreteLocalDomainSize( pContext ) },
                                { "type", getContextTypeClass( pContext ) },
                                { "successor", strSuccessorState },
                                { "name", fullInterfaceTypeName( pContext->get_interface() ) }

        } );
        data[ "states" ].push_back( state );
    };

    auto makeDimensionState = [ & ]( const Dimensions::User* pDim, const std::string& strSuccessorState )
    {
        nlohmann::json state( { { "value", makeStartState( pDim->get_concrete_id() ) },
                                { "start", true },
                                { "owning", false },
                                { "owned", false },
                                { "local_domain_size", 1 },
                                { "type", "dimension" },
                                { "successor", strSuccessorState },
                                { "name", fullInterfaceTypeName( pDim->get_interface_dimension() ) }

        } );
        data[ "states" ].push_back( state );
    };

    // collate elements
    using ContextElementVariant       = std::variant< const Dimensions::User*, const Context* >;
    using ContextElementVariantVector = std::vector< ContextElementVariant >;
    ContextElementVariantVector elements;
    {
        {
            auto children = pContext->get_children();
            std::copy( children.begin(), children.end(), std::back_inserter( elements ) );
        }

        if( auto pUserDimensionContext = db_cast< const UserDimensionContext >( pContext ) )
        {
            auto dimensions = pUserDimensionContext->get_dimensions();
            std::copy( dimensions.begin(), dimensions.end(), std::back_inserter( elements ) );
        }
    }

    mega::TypeID typeIDSuccessor = pContext->get_concrete_id();
    for( ContextElementVariantVector::iterator i = elements.begin(), iPrev, iEnd = elements.end(); i != iEnd; ++i )
    {
        {
            if( auto ppDim = std::get_if< const Dimensions::User* >( &*i ) )
            {
                const Dimensions::User* pDim = *ppDim;
                typeIDSuccessor              = pDim->get_concrete_id();
            }
            else if( auto ppContext = std::get_if< const Context* >( &*i ) )
            {
                const Context* pChildContext = *ppContext;
                typeIDSuccessor              = pChildContext->get_concrete_id();
            }
            else
            {
                THROW_RTE( "Unknown type" );
            }
        }

        if( i == elements.begin() )
        {
            makeContextState( pContext, true, makeStartState( typeIDSuccessor ) );
        }
        else
        {
            if( auto ppDim = std::get_if< const Dimensions::User* >( &*iPrev ) )
            {
                makeDimensionState( *ppDim, makeStartState( typeIDSuccessor ) );
            }
            else if( auto ppContext = std::get_if< const Context* >( &*iPrev ) )
            {
                const Context* pChildContext = *ppContext;
                makeContextState( pChildContext, false, makeStartState( typeIDSuccessor ) );
                recurseTraversalStates( database, data, pChildContext );
            }
            else
            {
                THROW_RTE( "Unknown type" );
            }
        }
        iPrev = i;
    }

    // generate transition to context end state from final element OR context start state
    if( elements.empty() )
    {
        makeContextState( pContext, true, makeEndState( pContext->get_concrete_id() ) );
    }
    else
    {
        if( auto ppDim = std::get_if< const Dimensions::User* >( &elements.back() ) )
        {
            makeDimensionState( *ppDim, makeEndState( pContext->get_concrete_id() ) );
        }
        else if( auto ppContext = std::get_if< const Context* >( &elements.back() ) )
        {
            const Context* pChildContext = *ppContext;
            makeContextState( pChildContext, false, makeEndState( pContext->get_concrete_id() ) );
            recurseTraversalStates( database, data, pChildContext );
        }
        else
        {
            THROW_RTE( "Unknown type" );
        }
    }
}

void CodeGenerator::generate_alllocator( const LLVMCompiler& compiler, const JITDatabase& database,
                                         mega::TypeID objectTypeID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_alllocator: {}", objectTypeID );

    FinalStage::Concrete::Object*            pObject    = database.getObject( objectTypeID );
    const FinalStage::Components::Component* pComponent = pObject->get_component();

    const std::string strFullTypeName = fullInterfaceTypeName( pObject->get_interface_object() );

    std::ostringstream osObjectTypeID;
    osObjectTypeID << printTypeID( objectTypeID );
    nlohmann::json data( { { "objectTypeID", osObjectTypeID.str() },
                           { "objectName", strFullTypeName },
                           { "parts", nlohmann::json::array() },
                           { "relations", nlohmann::json::array() },
                           { "mangled_data_types", nlohmann::json::array() },
                           { "state", nlohmann::json::array() }

    } );

    recurseTraversalStates( database, data, pObject );
    // special case for Object END where will pop stack
    {
        nlohmann::json state( { { "value", makeEndState( pObject->get_concrete_id() ) },
                                { "start", false },
                                { "type", getContextTypeClass( pObject ) },
                                { "successor", makeEndState( pObject->get_concrete_id() ) },
                                { "name", fullInterfaceTypeName( pObject->get_interface() ) }

        } );
        data[ "states" ].push_back( state );
    }
    // std::vector< FinalStage::HyperGraph::Relation* > owningRelations;

    std::set< std::string > mangledDataTypes;
    {
        using namespace FinalStage;
        for( auto pBuffer : pObject->get_buffers() )
        {
            bool bBufferIsSimple = false;
            if( db_cast< MemoryLayout::SimpleBuffer >( pBuffer ) )
                bBufferIsSimple = true;

            for( auto pPart : pBuffer->get_parts() )
            {
                std::ostringstream osPartType;
                std::ostringstream osPartName;

                osPartType << pPart->get_context()->get_interface()->get_identifier();
                osPartName << pPart->get_context()->get_interface()->get_identifier();

                nlohmann::json part( { { "type", osPartType.str() },
                                       { "name", osPartName.str() },
                                       { "size", pPart->get_size() },
                                       { "offset", pPart->get_offset() },
                                       { "total_domain", pPart->get_total_domain_size() },
                                       { "members", nlohmann::json::array() },
                                       { "links", nlohmann::json::array() }

                } );

                for( auto pUserDim : pPart->get_user_dimensions() )
                {
                    const std::string strMangle = megaMangle( pUserDim->get_interface_dimension()->get_erased_type() );
                    nlohmann::json    member( { { "type", pUserDim->get_interface_dimension()->get_canonical_type() },
                                                { "type_id", printTypeID( pUserDim->get_concrete_id() ) },
                                                { "mangle", strMangle },
                                                { "name", pUserDim->get_interface_dimension()->get_id()->get_str() },
                                                { "offset", pUserDim->get_offset() } } );
                    part[ "members" ].push_back( member );
                    mangledDataTypes.insert( strMangle );
                }

                THROW_TODO;
                /*for( auto pLinkDim : pPart->get_link_dimensions() )
                {
                    Concrete::Link*           pConcreteLink  = pLinkDim->get_link();
                    Interface::LinkInterface* pLinkInterface = pConcreteLink->get_link_interface();
                    HyperGraph::Relation*     pRelation      = pLinkInterface->get_relation();

                    bool bSource = false, bOwning = false, bOwned = false;
                    getLinkOwnership( pConcreteLink, bSource, bOwning, bOwned );

                    std::string    strMangle;
                    RelationID     relationID = pRelation->get_id();
                    nlohmann::json link( { { "type", mega::psz_mega_reference_vector },
                                           { "type_id", printTypeID( pLinkDim->get_concrete_id() ) },
                                           { "link_type_id", printTypeID( pLinkDim->get_link()->get_concrete_id() ) },
                                           { "mangle", "" },
                                           { "name", pLinkDim->get_link()->get_link()->get_identifier() },
                                           { "offset", pLinkDim->get_offset() },
                                           { "singular", false },
                                           { "types", nlohmann::json::array() },
                                           { "owning", bOwning },
                                           { "owned", bOwned },
                                           { "relation_id_lower", printTypeID( relationID.getLower() ) },
                                           { "relation_id_upper", printTypeID( relationID.getUpper() ) }

                    } );

                    if( auto pLinkMany = db_cast< Concrete::Dimensions::LinkMany >( pLinkDim ) )
                    {
                        strMangle          = megaMangle( mega::psz_mega_reference_vector );
                        link[ "singular" ] = false;
                    }
                    else if( auto pLinkSingle = db_cast< Concrete::Dimensions::LinkSingle >( pLinkDim ) )
                    {
                        strMangle          = megaMangle( mega::psz_mega_reference );
                        link[ "singular" ] = true;
                    }
                    else
                    {
                        THROW_RTE( "Unknown link type" );
                    }

                    if( bSource )
                    {
                        for( auto pTarget : pRelation->get_targets() )
                        {
                            for( auto pConcreteLink : pTarget->get_concrete() )
                            {
                                auto pObjectOpt = pConcreteLink->get_concrete_object();
                                VERIFY_RTE( pObjectOpt.has_value() );
                                nlohmann::json type(
                                    { { "link_type_id", printTypeID( pConcreteLink->get_concrete_id() ) },
                                      { "object_type_id", printTypeID( pObjectOpt.value()->get_concrete_id() ) }

                                    } );
                                link[ "types" ].push_back( type );
                            }
                        }
                    }
                    else
                    {
                        for( auto pTarget : pRelation->get_sources() )
                        {
                            for( auto pConcreteLink : pTarget->get_concrete() )
                            {
                                auto pObjectOpt = pConcreteLink->get_concrete_object();
                                VERIFY_RTE( pObjectOpt.has_value() );
                                nlohmann::json type(
                                    { { "link_type_id", printTypeID( pConcreteLink->get_concrete_id() ) },
                                      { "object_type_id", printTypeID( pObjectOpt.value()->get_concrete_id() ) }

                                    } );
                                link[ "types" ].push_back( type );
                            }
                        }
                    }

                    link[ "mangle" ] = strMangle;
                    part[ "links" ].push_back( link );
                    mangledDataTypes.insert( strMangle );
                }*/
                for( auto pAllocDim : pPart->get_allocation_dimensions() )
                {
                    if( auto pAllocator = db_cast< Concrete::Dimensions::Allocator >( pAllocDim ) )
                    {
                        const std::string strAllocatorTypeName = allocatorTypeName( database, pAllocator );
                        const std::string strMangle            = megaMangle( strAllocatorTypeName );

                        std::ostringstream osAllocName;
                        osAllocName << "alloc_"
                                    << printTypeID(
                                           pAllocator->get_allocator()->get_allocated_context()->get_concrete_id() );

                        mangledDataTypes.insert( strMangle );

                        nlohmann::json member( { { "type", strAllocatorTypeName },
                                                 { "type_id", printTypeID( pAllocDim->get_concrete_id() ) },
                                                 { "mangle", strMangle },
                                                 { "name", osAllocName.str() },
                                                 { "offset", pAllocator->get_offset() } } );
                        part[ "members" ].push_back( member );
                    }
                    else
                    {
                        THROW_RTE( "Unknown allocation dimension type" );
                    }
                }

                data[ "parts" ].push_back( part );
            }
        }
    }

    for( const auto& str : mangledDataTypes )
    {
        data[ "mangled_data_types" ].push_back( str );
    }

    std::ostringstream osCPPCode;
    m_pInja->render_allocator( data, osCPPCode );
    // compiler.compileToLLVMIR( osObjectTypeID.str(), osCPPCode.str(), os, pComponent );
    compiler.compileToLLVMIR( osObjectTypeID.str(), osCPPCode.str(), os, std::nullopt );
}

} // namespace mega::runtime
