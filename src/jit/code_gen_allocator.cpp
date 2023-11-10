
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

#include "mega/mangle/traits.hpp"

#include "mega/common_strings.hpp"

namespace FinalStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
} // namespace FinalStage

namespace mega::runtime
{
namespace
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
    else if( db_cast< const Decider >( pContext ) )
        return "decider";
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

void collectEnums( FinalStage::Automata::Enum* pEnum, std::map< U32, FinalStage::Automata::Enum* >& enums )
{
    enums.insert( { pEnum->get_switch_index(), pEnum } );
    for( auto pChildEnum : pEnum->get_children() )
    {
        collectEnums( pChildEnum, enums );
    }
}

std::string generate_enum( Inja& inja, FinalStage::Concrete::Object* pObject )
{
    static const char* szTemplate =
        R"TEMPLATE(

    switch( iterator )
    {
{% for enum in enums %}
        case {{ enum.switch_index }}:
        {
{% if enum.is_test %}
            if( mega::mangle::test_bitset( pBitset, {{ enum.bit_index }} ) )
            {
                iterator = {{ enum.iterator_next }};
{% if enum.is_action %}
                return mega::SubTypeInstance( {{ enum.result_value }} );
{% endif %}
            }
            else
            {
                iterator = {{ enum.iterator_failure }};
            }
{% else %}
            iterator = {{ enum.iterator_next }};
            return mega::SubTypeInstance( {{ enum.result_value }} );
{% endif %}
        }
        break;
{% endfor %}
        default:
        {
            iterator = 0;
        }
        break;
    }

)TEMPLATE";

    std::map< U32, FinalStage::Automata::Enum* > enums;
    {
        for( auto pEnum : pObject->get_automata_enums() )
        {
            collectEnums( pEnum, enums );
        }
    }

    nlohmann::json data( { { "enums", nlohmann::json::array() } } );

    for( auto [ _, pEnum ] : enums )
    {
        nlohmann::json enumData( {

            { "switch_index", pEnum->get_switch_index() },
            { "iterator_next", pEnum->get_next_switch_index() },
            { "is_action", pEnum->get_action().has_value() },
            { "result_value", printSubTypeInstance( pEnum->get_sub_type_instance() ) }

        } );

        if( auto pTest = FinalStage::db_cast< FinalStage::Automata::Test >( pEnum ) )
        {
            enumData[ "is_test" ]          = true;
            enumData[ "bit_index" ]        = pTest->get_bitset_index();
            enumData[ "iterator_failure" ] = pTest->get_failure_switch_index();
        }
        else
        {
            enumData[ "is_test" ] = false;
        }

        data[ "enums" ].push_back( enumData );
    }

    return inja.render( szTemplate, data );
}

void collectStartStates( FinalStage::Automata::Enum* pEnum, std::vector< mega::U32 >& startBits )
{
    if( auto pTest = FinalStage::db_cast< FinalStage::Automata::Test >( pEnum ) )
    {
        if( pTest->get_start_state() )
        {
            startBits.push_back( pTest->get_bitset_index() );
        }
    }

    for( auto pChildEnum : pEnum->get_children() )
    {
        collectStartStates( pChildEnum, startBits );
    }
}

void collectStartStates( FinalStage::Concrete::Object* pObject, FinalStage::Concrete::Dimensions::Bitset* pBitSet,
                         nlohmann::json& member )
{
    // determine the correct initialisation for the bitset size
    std::vector< mega::U32 > startBits;
    {
        for( auto pEnum : pObject->get_automata_enums() )
        {
            collectStartStates( pEnum, startBits );
        }
        std::sort( startBits.begin(), startBits.end() );
    }

    const auto numberOfBits   = pBitSet->get_number_of_bits();
    mega::U32  numberOfBlocks = numberOfBits / 64;
    if( numberOfBits % 64 )
    {
        ++numberOfBlocks;
    }

    auto iBit = startBits.begin(), iBitEnd = startBits.end();
    for( auto iBlock = 0; iBlock != numberOfBlocks; ++iBlock )
    {
        static const std::string blockBinaryLiteral
            = "0000000000000000000000000000000000000000000000000000000000000000";
        std::string block = blockBinaryLiteral;

        for( int i = 0; i != 64; ++i )
        {
            if( ( iBit != iBitEnd ) && ( *iBit == ( i + ( iBlock * 64 ) ) ) )
            {
                block[ 63 - i ] = '1';
                ++iBit;
            }
        }
        member[ "blocks" ].push_back( block );
    }

    member[ "number_of_blocks" ] = numberOfBlocks;
}

} // namespace

void CodeGenerator::generate_alllocator( const LLVMCompiler& compiler, const JITDatabase& database,
                                         mega::TypeID objectTypeID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_alllocator: {}", objectTypeID );

    FinalStage::Concrete::Object*            pObject    = database.getObject( objectTypeID );
    const FinalStage::Components::Component* pComponent = pObject->get_component();

    const std::string strFullTypeName = printContextFullType( pObject );

    std::ostringstream osObjectTypeID;
    osObjectTypeID << printTypeID( objectTypeID );
    nlohmann::json data( {

        { "component", pObject->get_component()->get_name() },
        { "objectTypeID", osObjectTypeID.str() },
        { "objectName", strFullTypeName },
        { "parts", nlohmann::json::array() },
        { "mangled_data_types", nlohmann::json::array() },
        { "states", nlohmann::json::array() },
        { "enumeration", nlohmann::json::array() }

    } );

    // generate traversal states
    {
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
    }

    // generate enumeration ranges
    {
        data[ "enumeration" ] = generate_enum( *m_pInja, pObject );
        data[ "bitset_offset" ]
            = pObject->get_activation()->get_part()->get_offset() + pObject->get_activation()->get_offset();
    }

    // generate binary snapshot supporting buffer layout parts
    {
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
                    nlohmann::json part( { { "size", pPart->get_size() },
                                           { "offset", pPart->get_offset() },
                                           { "total_domain", pPart->get_total_domain_size() },
                                           { "members", nlohmann::json::array() },
                                           { "links", nlohmann::json::array() }

                    } );

                    for( auto pBitsetDim : pPart->get_bitset_dimensions() )
                    {
                        const std::string strMangle
                            = megaMangle( pBitsetDim->get_interface_compiler_dimension()->get_erased_type() );
                        nlohmann::json member(
                            { { "type", pBitsetDim->get_interface_compiler_dimension()->get_canonical_type() },
                              { "type_id", printTypeID( pBitsetDim->get_concrete_id() ) },
                              { "mangle", strMangle },
                              { "name", Concrete::getIdentifier( pBitsetDim ) },
                              { "offset", pBitsetDim->get_offset() },
                              { "is_bitset", true },
                              { "number_of_blocks", 0 },
                              { "blocks", nlohmann::json::array() }

                            } );

                        collectStartStates( pObject, pBitsetDim, member );

                        part[ "members" ].push_back( member );
                        mangledDataTypes.insert( strMangle );
                    }

                    for( auto pUserDim : pPart->get_user_dimensions() )
                    {
                        const std::string strMangle
                            = megaMangle( pUserDim->get_interface_dimension()->get_erased_type() );
                        nlohmann::json member( { { "type", pUserDim->get_interface_dimension()->get_canonical_type() },
                                                 { "type_id", printTypeID( pUserDim->get_concrete_id() ) },
                                                 { "mangle", strMangle },
                                                 { "name", Concrete::getIdentifier( pUserDim ) },
                                                 { "offset", pUserDim->get_offset() },
                                                 { "is_bitset", false }

                        } );
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

                    data[ "parts" ].push_back( part );
                }
            }
        }

        for( const auto& str : mangledDataTypes )
        {
            data[ "mangled_data_types" ].push_back( str );
        }
    }

    std::ostringstream osCPPCode;
    m_pInja->render_allocator( data, osCPPCode );
    compiler.compileToLLVMIR( osObjectTypeID.str(), osCPPCode.str(), os, std::nullopt );
}

} // namespace mega::runtime
