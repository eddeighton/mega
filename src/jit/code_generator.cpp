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

#include "database/FinalStage.hxx"

#include "log/index_record.hpp"

#include "service/network/log.hpp"
#include "service/mpo_context.hpp"

#include "mega/values/compilation/relation_id.hpp"
#include "mega/common_strings.hpp"

#include "nlohmann/json.hpp"

#include <boost/process.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <istream>
#include <sstream>

namespace FinalStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
} // namespace FinalStage

namespace mega::runtime
{
using namespace FinalStage;

CodeGenerator::CodeGenerator( const mega::MegastructureInstallation& megastructureInstallation,
                              const mega::Project&                   project )
    : m_pInja( std::make_unique< Inja >( megastructureInstallation, project ) )
{
}

nlohmann::json CodeGenerator::generate( const JITDatabase& database, const mega::InvocationID& invocationID,
                                        std::string& strName ) const
{
    FunctionDeclarations functions;

    nlohmann::json data = functions.init( invocationID, strName );

    try
    {
        const Operations::Invocation* pInvocation = database.getInvocation( invocationID );

        Indent indent;

        const VariableMap variables = generateVariables( pInvocation->get_variables(), data, indent );

        for( auto pInstruction : pInvocation->get_root_instruction()->get_children() )
        {
            generateInstructions( database, pInvocation, pInstruction, variables, functions, data, indent );
        }
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate error: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate: " << ex.what() );
    }

    functions.generate( data );
    return data;
}

void CodeGenerator::generate_invocation( const LLVMCompiler& compiler, const JITDatabase& database,
                                         const mega::InvocationID&               invocationID,
                                         mega::runtime::invocation::FunctionType invocationType, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_invocation: {} {}", invocationID, invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    try
    {
        m_pInja->render_invocation( data, osCPPCode );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate_invocation: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate_invocation: " << ex.what() );
    }
    compiler.compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}

void CodeGenerator::generate_relation( const LLVMCompiler& compiler, const JITDatabase& database,
                                       const RelationID& relationID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_relation" );

    auto pRelation = database.getRelation( relationID );

    bool bSourceSingular, bTargetSingular, bTargetOwned = false;

    std::ostringstream osRelationID;
    {
        osRelationID << relationID;
    }
    std::ostringstream osModuleName;
    {
        osModuleName << "relation_" << osRelationID.str();
    }

    nlohmann::json data( { { "relationID", osRelationID.str() },
                           { "module_name", osModuleName.str() },
                           { "sources", nlohmann::json::array() },
                           { "targets", nlohmann::json::array() } } );

    if( auto pOwningRelation = db_cast< HyperGraph::OwningObjectRelation >( pRelation ) )
    {
        data[ "owning" ] = true;

        auto owners = pOwningRelation->get_owners();

        {
            std::vector< Interface::UserLinkTrait* > linkTraits;
            {
                for( auto i = owners.begin(), iEnd = owners.end(); i != iEnd; i = owners.upper_bound( i->first ) )
                {
                    linkTraits.push_back( i->first );
                }
            }
            for( auto pLinkTrait : linkTraits )
            {
                for( auto pConcrete : pLinkTrait->get_concrete() )
                {
                    auto pParent = db_cast< Concrete::UserDimensionContext >( pConcrete->get_parent_context() );
                    VERIFY_RTE( pParent );

                    auto pPart = pConcrete->get_part();
                    VERIFY_RTE( pConcrete->get_link_type()->get_part() == pPart );
                    nlohmann::json source(
                        { { "type", printTypeID( pConcrete->get_concrete_id() ) },
                          { "parent_type", printTypeID( pConcrete->get_parent_context()->get_concrete_id() ) },
                          { "parameter_types", nlohmann::json::array() },
                          { "part_offset", pPart->get_offset() },
                          { "part_size", pPart->get_size() },
                          { "dimension_offset", pConcrete->get_offset() },
                          { "link_type_offset", pConcrete->get_link_type()->get_offset() },
                          { "singular", pLinkTrait->get_parser_link()->get_cardinality().isSingular() }

                        } );

                    // determine if parent context is unique for the parameter
                    std::map< Concrete::Context*, int > contextCount;
                    for( auto i = owners.lower_bound( pLinkTrait ), iEnd = owners.upper_bound( pLinkTrait ); i != iEnd;
                         ++i )
                    {
                        contextCount[ i->second->get_parent_context() ]++;
                    }
                    for( auto i = owners.lower_bound( pLinkTrait ), iEnd = owners.upper_bound( pLinkTrait ); i != iEnd;
                         ++i )
                    {
                        auto           pConcreteOwnershipLink = i->second;
                        nlohmann::json parameter_type(
                            { { "type", printTypeID( pConcreteOwnershipLink->get_concrete_id() ) },
                              { "parent_type",
                                printTypeID( pConcreteOwnershipLink->get_parent_context()->get_concrete_id() ) },
                              { "unique_parent_context",
                                contextCount[ pConcreteOwnershipLink->get_parent_context() ] == 1 } }

                        );
                        source[ "parameter_types" ].push_back( parameter_type );
                    }

                    data[ "sources" ].push_back( source );
                }
            }
        }
        {
            auto                                                owned = pOwningRelation->get_owned();
            std::vector< Concrete::Dimensions::OwnershipLink* > ownershipLinks;
            {
                for( auto i = owned.begin(), iEnd = owned.end(); i != iEnd; i = owned.upper_bound( i->first ) )
                {
                    ownershipLinks.push_back( i->first );
                }
            }
            for( auto pOwnershipLink : ownershipLinks )
            {
                auto pObject = db_cast< Concrete::Object >( pOwnershipLink->get_parent_context() );
                VERIFY_RTE( pObject );

                auto pPart = pOwnershipLink->get_part();
                VERIFY_RTE( pOwnershipLink->get_link_type()->get_part() == pPart );
                nlohmann::json target( { { "type", printTypeID( pOwnershipLink->get_concrete_id() ) },
                                         { "parent_type", printTypeID( pObject->get_concrete_id() ) },
                                         { "parameter_types", nlohmann::json::array() },
                                         { "part_offset", pPart->get_offset() },
                                         { "part_size", pPart->get_size() },
                                         { "dimension_offset", pOwnershipLink->get_offset() },
                                         { "link_type_offset", pOwnershipLink->get_link_type()->get_offset() },
                                         { "singular", true }

                } );

                // determine if parent context is unique for the parameter
                std::map< Concrete::Context*, int > contextCount;
                for( auto i = owned.lower_bound( pOwnershipLink ), iEnd = owned.upper_bound( pOwnershipLink );
                     i != iEnd; ++i )
                {
                    auto pLinkTrait = i->second;
                    for( auto pConcrete : pLinkTrait->get_concrete() )
                    {
                        contextCount[ pConcrete->get_parent_context() ]++;
                    }
                }
                for( auto i = owned.lower_bound( pOwnershipLink ), iEnd = owned.upper_bound( pOwnershipLink );
                     i != iEnd; ++i )
                {
                    auto pLinkTrait = i->second;
                    for( auto pConcrete : pLinkTrait->get_concrete() )
                    {
                        nlohmann::json parameter_type(
                            { { "type", printTypeID( pConcrete->get_concrete_id() ) },
                              { "parent_type", printTypeID( pConcrete->get_parent_context()->get_concrete_id() ) },
                              { "unique_parent_context", contextCount[ pConcrete->get_parent_context() ] == 1 } }

                        );
                        target[ "parameter_types" ].push_back( parameter_type );
                    }
                }

                data[ "targets" ].push_back( target );
            }
        }
    }
    else if( auto pNonOwningRelation = db_cast< HyperGraph::NonOwningObjectRelation >( pRelation ) )
    {
        data[ "owning" ] = false;
        {
            auto pSource = pNonOwningRelation->get_source();
            for( auto pConcrete : pSource->get_concrete() )
            {
                auto pLink = db_cast< Concrete::Dimensions::Link >( pConcrete );
                VERIFY_RTE( pLink );
                auto pPart = pLink->get_part();
                VERIFY_RTE( pLink->get_link_type()->get_part() == pPart );

                nlohmann::json source(
                    { { "type", printTypeID( pLink->get_concrete_id() ) },
                      { "parent_type", printTypeID( pLink->get_parent_context()->get_concrete_id() ) },
                      { "parameter_types", nlohmann::json::array() },
                      { "part_offset", pPart->get_offset() },
                      { "part_size", pPart->get_size() },
                      { "dimension_offset", pLink->get_offset() },
                      { "link_type_offset", pLink->get_link_type()->get_offset() },
                      { "singular", pSource->get_parser_link()->get_cardinality().isSingular() }

                    } );

                // determine if parent context is unique for the parameter
                for( auto pConcrete : pNonOwningRelation->get_target()->get_concrete() )
                {
                    nlohmann::json parameter_type(
                        { { "type", printTypeID( pConcrete->get_concrete_id() ) },
                          { "parent_type", printTypeID( pConcrete->get_parent_context()->get_concrete_id() ) },
                          { "unique_parent_context", true } }

                    );
                    source[ "parameter_types" ].push_back( parameter_type );
                }

                data[ "sources" ].push_back( source );
            }
        }
        {
            auto pTarget = pNonOwningRelation->get_target();
            for( auto pConcrete : pTarget->get_concrete() )
            {
                auto pLink = db_cast< Concrete::Dimensions::Link >( pConcrete );
                VERIFY_RTE( pLink );
                auto pPart = pLink->get_part();

                nlohmann::json target(
                    { { "type", printTypeID( pLink->get_concrete_id() ) },
                      { "parent_type", printTypeID( pLink->get_parent_context()->get_concrete_id() ) },
                      { "parameter_types", nlohmann::json::array() },
                      { "part_offset", pPart->get_offset() },
                      { "part_size", pPart->get_size() },
                      { "dimension_offset", pLink->get_offset() },
                      { "link_type_offset", pLink->get_link_type()->get_offset() },
                      { "singular", pTarget->get_parser_link()->get_cardinality().isSingular() }

                    } );

                // determine if parent context is unique for the parameter
                for( auto pConcrete : pNonOwningRelation->get_source()->get_concrete() )
                {
                    nlohmann::json parameter_type(
                        { { "type", printTypeID( pConcrete->get_concrete_id() ) },
                          { "parent_type", printTypeID( pConcrete->get_parent_context()->get_concrete_id() ) },
                          { "unique_parent_context", true } }

                    );
                    target[ "parameter_types" ].push_back( parameter_type );
                }

                data[ "targets" ].push_back( target );
            }
        }
    }
    else
    {
        THROW_RTE( "Unknown relation type" );
    }

    std::ostringstream osCPPCode;
    try
    {
        m_pInja->render_relation( data, osCPPCode );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate_relation: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate_relation: " << ex.what() );
    }
    compiler.compileToLLVMIR( osModuleName.str(), osCPPCode.str(), os, std::nullopt );
}

void CodeGenerator::generate_program( const LLVMCompiler& compiler, const JITDatabase& database, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_program" );

    nlohmann::json data( { { "record_types", nlohmann::json::array() },
                           { "object_types", nlohmann::json::array() },
                           { "concrete_types", nlohmann::json::array() },
                           { "dimension_types", nlohmann::json::array() },
                           { "relation_types", nlohmann::json::array() },
                           { "event_types", nlohmann::json::array() }

    } );

    // concrete to interface
    {
        JITDatabase::ConcreteToInterface concreteToInterface;
        database.getConcreteToInterface( concreteToInterface );
        for( const auto& type : concreteToInterface )
        {
            nlohmann::json typeInfo(
                { { "concrete", printTypeID( type.first ) }, { "interface", printTypeID( type.second ) } } );
            data[ "concrete_types" ].push_back( typeInfo );
        }
    }

    // object types
    {
        for( const auto pObject : database.getObjects() )
        {
            data[ "object_types" ].push_back( printTypeID( pObject->get_concrete_id() ) );
        }
    }

    // event_types
    {
        // interupt handlers
        using EventHandlers = std::multimap< TypeID, nlohmann::json >;

        EventHandlers eventHandlers;
        {
            for( const Concrete::Interupt* pInterupt : database.getInterupts() )
            {
                for( auto pDispatch : pInterupt->get_dispatches() )
                {
                    auto pContext = db_cast< Concrete::Context >( pDispatch->get_vertex() );
                    VERIFY_RTE( pContext );

                    std::ostringstream osDispatcher;
                    generate_dispatcher( compiler, database, pInterupt, pDispatch, osDispatcher );

                    nlohmann::json eventHandler( {

                        { "type_name", Concrete::printContextFullType( pContext ) },
                        { "comment", Concrete::printContextFullType( pContext ) },
                        { "has_dispatch", true },
                        { "has_transition", false },
                        { "code", osDispatcher.str() }

                    } );

                    eventHandlers.insert( { pContext->get_concrete_id(), eventHandler } );
                }
            }
        }

        // completion handlers
        for( const Concrete::Action* pAction : database.getActions() )
        {
            auto optHandler = pAction->get_completion_handler();
            if( optHandler.has_value() )
            {
                auto pHandler = optHandler.value();

                mega::U32 transitionDivider = 1U;

                if( auto pInterupt = db_cast< Concrete::Interupt >( pHandler ) )
                {
                    for( const Concrete::Context* pIter = pAction; pIter && ( pIter != pInterupt->get_parent() );
                         pIter                          = db_cast< const Concrete::Context >( pIter->get_parent() ) )
                    {
                        transitionDivider = transitionDivider * pIter->get_local_size();
                    }

                    nlohmann::json eventHandler( {

                        { "comment", Concrete::printContextFullType( pInterupt ) },
                        { "has_dispatch", false },
                        { "has_transition", true },
                        { "transition_sub_type", pInterupt->get_concrete_id().getSubObjectID() },
                        { "transition_divider", transitionDivider }

                    } );
                    eventHandlers.insert( { pAction->get_concrete_id(), eventHandler } );
                }
                else if( auto pState = db_cast< Concrete::State >( pHandler ) )
                {
                    for( const Concrete::Context* pIter = pAction; pIter && ( pIter != pState );
                         pIter                          = db_cast< const Concrete::Context >( pIter->get_parent() ) )
                    {
                        transitionDivider = transitionDivider * pIter->get_local_size();
                    }

                    nlohmann::json eventHandler( {

                        { "comment", Concrete::printContextFullType( pState ) },
                        { "has_dispatch", false },
                        { "has_transition", true },
                        { "transition_sub_type", pState->get_concrete_id().getSubObjectID() },
                        { "transition_divider", transitionDivider }

                    } );
                    eventHandlers.insert( { pAction->get_concrete_id(), eventHandler } );
                }
                else
                {
                    THROW_RTE( "Unknown handler type" );
                }
            }
        }

        for( auto i = eventHandlers.begin(), iEnd = eventHandlers.end(); i != iEnd; )
        {
            nlohmann::json eventType(
                { { "type_id", printTypeID( i->first ) }, { "handlers", nlohmann::json::array() } } );
            for( auto iNext = eventHandlers.upper_bound( i->first ); i != iNext; ++i )
            {
                eventType[ "handlers" ].push_back( i->second );
            }
            data[ "event_types" ].push_back( eventType );
        }
    }

    // record types and dimension types
    {
        std::set< std::string > record_types;
        for( auto pUserDimension : database.getUserDimensions() )
        {
            using namespace FinalStage::Concrete;

            {
                MemoryLayout::Part* pPart = pUserDimension->get_part();
                // const bool          bSimple    = pUserDimension->get_interface_dimension()->get_simple();
                const std::string strMangled
                    = megaMangle( pUserDimension->get_interface_dimension()->get_erased_type() );

                nlohmann::json typeInfo( { { "type_id", printTypeID( pUserDimension->get_concrete_id() ) },
                                           { "part_offset", pPart->get_offset() },
                                           { "part_size", pPart->get_size() },
                                           { "dimension_offset", pUserDimension->get_offset() },
                                           { "mangled_type_name", strMangled } } );
                data[ "dimension_types" ].push_back( typeInfo );
                record_types.insert( strMangled );
            }
        }
        for( auto pLinkDimension : database.getLinkDimensions() )
        {
            using namespace FinalStage::Concrete;

            std::string strMangled;
            if( pLinkDimension->get_singular() )
            {
            }
            else
            {
                // const std::string   strMangled
                //     = megaMangle( pLinkDimension->get_link()-> );
            }

            /*
                MemoryLayout::Part* pPart = pLinkDimension->get_part();
                nlohmann::json typeInfo( { { "type_id", printTypeID( pLinkDimension->get_concrete_id()) },
                                            { "part_offset", pPart->get_offset() },
                                            { "part_size", pPart->get_size() },
                                            { "dimension_offset", pLinkDimension->get_offset() },
                                            { "mangled_type_name", strMangled } } );
                data[ "dimension_types" ].push_back( typeInfo );
                events.insert( strMangled );
            */
        }

        for( const auto& record_type : record_types )
        {
            data[ "record_types" ].push_back( record_type );
        }
    }

    // relation types
    {
        for( const auto& [ id, pRelation ] : database.getRelations() )
        {
            std::ostringstream osID;
            using ::           operator<<;
            osID << id;
            nlohmann::json typeInfo( { { "id", osID.str() },
                                       { "lower", printTypeID( id.getLower() ) },
                                       { "upper", printTypeID( id.getUpper() ) } } );
            data[ "relation_types" ].push_back( typeInfo );
        }
    }

    std::ostringstream osCPPCode;
    try
    {
        m_pInja->render_program( data, osCPPCode );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate_program: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate_program: " << ex.what() );
    }
    compiler.compileToLLVMIR( "program_module", osCPPCode.str(), os, std::nullopt );
}

} // namespace mega::runtime
