
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

#include "mega/common_strings.hpp"

namespace FinalStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
} // namespace FinalStage

namespace mega::runtime
{

mega::U64 concreteLocalDomainSize( const FinalStage::Concrete::Context* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;
    if( auto pEvent = db_cast< const Event >( pContext ) )
    {
        return pEvent->get_local_size();
    }
    else if( auto pState = db_cast< const State >( pContext ) )
    {
        return pState->get_local_size();
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
    else if( db_cast< const Component >( pContext ) )
        return "component";
    else if( db_cast< const State >( pContext ) )
        return "state";
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
        nlohmann::json state( { { "value", bStart ? makeStartState( pContext->get_concrete_id() )
                                                  : makeEndState( pContext->get_concrete_id() ) },
                                { "start", bStart },
                                { "owning", false },
                                { "owned", false },
                                { "local_domain_size", concreteLocalDomainSize( pContext ) },
                                { "type", getContextTypeClass( pContext ) },
                                { "successor", strSuccessorState },
                                { "name", printContextFullType( pContext ) }

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
                                { "name", printContextFullType( pDim ) }

        } );
        data[ "states" ].push_back( state );
    };

    auto makeLinkState = [ & ]( const Dimensions::Link* pLink, bool bStart, const std::string& strSuccessorState )
    {
        nlohmann::json state( { { "value", bStart ? makeStartState( pLink->get_concrete_id() )
                                                  : makeEndState( pLink->get_concrete_id() ) },
                                { "start", bStart },
                                { "owning", pLink->get_owning() },
                                { "owned", pLink->get_owned() },
                                { "local_domain_size", 1 },
                                { "type", "link" },
                                { "successor", strSuccessorState },
                                { "name", printContextFullType( pLink ) }

        } );
        data[ "states" ].push_back( state );
    };

    // collate elements
    using ContextElementVariant = std::variant< const Dimensions::User*, const Dimensions::Link*, const Context* >;
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

            auto links = pUserDimensionContext->get_links();
            std::copy( links.begin(), links.end(), std::back_inserter( elements ) );
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
            else if( auto ppLink = std::get_if< const Dimensions::Link* >( &*i ) )
            {
                const Dimensions::Link* pLink = *ppLink;
                typeIDSuccessor               = pLink->get_concrete_id();
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
            else if( auto ppLink = std::get_if< const Dimensions::Link* >( &*iPrev ) )
            {
                makeLinkState( *ppLink, true, makeEndState( ( *ppLink )->get_concrete_id() ) );
                makeLinkState( *ppLink, false, makeStartState( typeIDSuccessor ) );
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
        else if( auto ppLink = std::get_if< const Dimensions::Link* >( &elements.back() ) )
        {
            makeLinkState( *ppLink, true, makeEndState( ( *ppLink )->get_concrete_id() ) );
            makeLinkState( *ppLink, false, makeEndState( pContext->get_concrete_id() ) );
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

    const std::string strFullTypeName = printContextFullType( pObject );

    std::ostringstream osObjectTypeID;
    osObjectTypeID << printTypeID( objectTypeID );
    nlohmann::json data( { { "objectTypeID", osObjectTypeID.str() },
                           { "objectName", strFullTypeName },
                           { "parts", nlohmann::json::array() },
                           { "relations", nlohmann::json::array() },
                           { "mangled_data_types", nlohmann::json::array() },
                           { "states", nlohmann::json::array() }

    } );

    recurseTraversalStates( database, data, pObject );
    // special case for Object END where will pop stack
    {
        nlohmann::json state( { { "value", makeEndState( pObject->get_concrete_id() ) },
                                { "start", false },
                                { "type", getContextTypeClass( pObject ) },
                                { "successor", makeEndState( pObject->get_concrete_id() ) },
                                { "name", printContextFullType( pObject ) }

        } );
        data[ "states" ].push_back( state );
    }

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

                for( auto pLinkDim : pPart->get_link_dimensions() )
                {
                    HyperGraph::Relation* pRelation = pLinkDim->get_relation();

                    std::string strMangle, strLinkTypeMangle;
                    if( pLinkDim->get_singular() )
                    {
                        strMangle         = megaMangle( mega::psz_mega_reference );
                        strLinkTypeMangle = megaMangle( mega::psz_link_type );
                    }
                    else
                    {
                        strMangle         = megaMangle( mega::psz_mega_reference_vector );
                        strLinkTypeMangle = megaMangle( mega::psz_link_type_vector );
                    }
                    mangledDataTypes.insert( strMangle );
                    mangledDataTypes.insert( strLinkTypeMangle );

                    RelationID     relationID = pRelation->get_id();
                    nlohmann::json link( { { "link_type_id", printTypeID( pLinkDim->get_concrete_id() ) },
                                           { "mangle", strMangle },
                                           { "offset", pLinkDim->get_offset() },
                                           { "link_type_offset", pLinkDim->get_link_type()->get_offset() },
                                           { "link_type_mangle", strLinkTypeMangle },
                                           { "singular", pLinkDim->get_singular() },
                                           { "owning", pLinkDim->get_owning() },
                                           { "owned", pLinkDim->get_owned() },
                                           { "relation_id_lower", printTypeID( relationID.getLower() ) },
                                           { "relation_id_upper", printTypeID( relationID.getUpper() ) }

                    } );

                    part[ "links" ].push_back( link );
                }

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
    compiler.compileToLLVMIR( osObjectTypeID.str(), osCPPCode.str(), os, std::nullopt );
}

} // namespace mega::runtime
