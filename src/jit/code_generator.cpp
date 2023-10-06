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

#include "database/model/FinalStage.hxx"

#include "log/index_record.hpp"

#include "service/network/log.hpp"
#include "service/mpo_context.hpp"

#include "mega/relation_io.hpp"

#include "nlohmann/json.hpp"

#include <boost/process.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <istream>
#include <sstream>

namespace mega::runtime
{

CodeGenerator::CodeGenerator( const mega::MegastructureInstallation& megastructureInstallation,
                              const mega::Project&                   project )
    : m_pInja( std::make_unique< Inja >( megastructureInstallation, project ) )
{
}

std::string CodeGenerator::allocatorTypeName( const JITDatabase&                           database,
                                              FinalStage::Concrete::Dimensions::Allocator* pAllocDim )
{
    using namespace FinalStage;

    std::ostringstream     osTypeName;
    Allocators::Allocator* pAllocator = pAllocDim->get_allocator();

    // paranoia check - ensure the local domain size matches up
    const auto localDomainSize = database.getLocalDomainSize( pAllocator->get_allocated_context()->get_concrete_id() );

    if( db_cast< Allocators::Nothing >( pAllocator ) )
    {
        // do nothing
        THROW_RTE( "Unreachable" );
    }
    else if( db_cast< Allocators::Singleton >( pAllocator ) )
    {
        VERIFY_RTE( localDomainSize == 1 );
        osTypeName << "bool";
    }
    else if( auto pAlloc32 = db_cast< Allocators::Range32 >( pAllocator ) )
    {
        const auto allocatorSize = pAlloc32->get_local_size();
        VERIFY_RTE( localDomainSize == allocatorSize );
        osTypeName << "mega::Bitmask32Allocator< " << allocatorSize << " >";
    }
    else if( auto pAlloc64 = db_cast< Allocators::Range64 >( pAllocator ) )
    {
        const auto allocatorSize = pAlloc32->get_local_size();
        VERIFY_RTE( localDomainSize == allocatorSize );
        osTypeName << "mega::Bitmask64Allocator< " << allocatorSize << " >";
    }
    else if( auto pAllocAny = db_cast< Allocators::RangeAny >( pAllocator ) )
    {
        const auto allocatorSize = pAlloc32->get_local_size();
        VERIFY_RTE( localDomainSize == allocatorSize );
        osTypeName << "mega::RingAllocator< mega::Instance, " << allocatorSize << " >";
    }
    else
    {
        THROW_RTE( "Unknown allocator type" );
    }
    return osTypeName.str();
}

nlohmann::json CodeGenerator::generate( const JITDatabase& database, const mega::InvocationID& invocationID,
                                        std::string& strName ) const
{
    FunctionDeclarations functions;

    nlohmann::json data = functions.init( invocationID, strName );

    try
    {
        using namespace FinalStage;
        using namespace FinalStage::Invocations;

        const FinalStage::Operations::Invocation* pInvocation = database.getInvocation( invocationID );

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
        switch( invocationType )
        {
            case mega::runtime::invocation::eRead:
                m_pInja->render_read( data, osCPPCode );
                break;
            case mega::runtime::invocation::eWrite:
                m_pInja->render_write( data, osCPPCode );
                break;
            case mega::runtime::invocation::eCall:
                m_pInja->render_call( data, osCPPCode );
                break;
            case mega::runtime::invocation::eReadLink:
                m_pInja->render_readLink( data, osCPPCode );
                break;
            case mega::runtime::invocation::eWriteLink:
                m_pInja->render_writeLink( data, osCPPCode );
                break;
            case mega::runtime::invocation::eGet:
                m_pInja->render_get( data, osCPPCode );
                break;
            case mega::runtime::invocation::eStart:
                // also works for event
                m_pInja->render_start( data, osCPPCode );
                break;
            case mega::runtime::invocation::eMove:
                m_pInja->render_move( data, osCPPCode );
                break;
            case mega::runtime::invocation::eWriteLinkRange:
            case mega::runtime::invocation::TOTAL_FUNCTION_TYPES:
            default:
                THROW_RTE( "Unsupported operation type" );
        }
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

    using namespace FinalStage;

    auto pRelation = database.getRelation( relationID );

    bool bSourceSingular, bTargetSingular, bTargetOwned = false;

    std::ostringstream osRelationID;
    {
        using ::operator<<;
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

        {
            std::vector< Interface::LinkTrait* > linkTraits;
            {
                auto owners = pOwningRelation->get_owners();
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
                    const bool bHasUniqueParent = pParent->get_links().size() == 1;

                    auto pPart = pConcrete->get_part();
                    VERIFY_RTE( pConcrete->get_link_type()->get_part() == pPart );
                    nlohmann::json source(
                        { { "type", printTypeID( pConcrete->get_concrete_id() ) },
                          { "parent_type", printTypeID( pConcrete->get_parent_context()->get_concrete_id() ) },
                          { "has_unique_parent", bHasUniqueParent },
                          { "part_offset", pPart->get_offset() },
                          { "part_size", pPart->get_size() },
                          { "dimension_offset", pConcrete->get_offset() },
                          { "link_type_offset", pConcrete->get_link_type()->get_offset() },
                          { "singular", pLinkTrait->get_cardinality().isSingular() }

                        } );
                    data[ "sources" ].push_back( source );
                }
            }
        }
        {
            std::vector< Concrete::Dimensions::OwnershipLink* > ownershipLinks;
            {
                auto owned = pOwningRelation->get_owned();
                for( auto i = owned.begin(), iEnd = owned.end(); i != iEnd; i = owned.upper_bound( i->first ) )
                {
                    ownershipLinks.push_back( i->first );
                }
            }
            for( auto pOwnershipLink : ownershipLinks )
            {
                auto pObject = db_cast< Concrete::Object >( pOwnershipLink->get_parent_context() );
                VERIFY_RTE( pObject );

                auto pParent = db_cast< Concrete::UserDimensionContext >( pObject );
                VERIFY_RTE( pParent );
                const bool bHasUniqueParent = pParent->get_links().size() == 1;

                auto pPart = pOwnershipLink->get_part();
                VERIFY_RTE( pOwnershipLink->get_link_type()->get_part() == pPart );
                nlohmann::json target( { { "type", printTypeID( pOwnershipLink->get_concrete_id() ) },
                                         { "parent_type", printTypeID( pObject->get_concrete_id() ) },
                                         { "has_unique_parent", bHasUniqueParent },
                                         { "part_offset", pPart->get_offset() },
                                         { "part_size", pPart->get_size() },
                                         { "dimension_offset", pOwnershipLink->get_offset() },
                                         { "link_type_offset", pOwnershipLink->get_link_type()->get_offset() },
                                         { "singular", true }

                } );
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

                auto pParent = db_cast< Concrete::UserDimensionContext >( pConcrete->get_parent_context() );
                VERIFY_RTE( pParent );
                const bool bHasUniqueParent = pParent->get_links().size() == 1;

                nlohmann::json source(
                    { { "type", printTypeID( pLink->get_concrete_id() ) },
                      { "parent_type", printTypeID( pLink->get_parent_context()->get_concrete_id() ) },
                      { "has_unique_parent", bHasUniqueParent },
                      { "part_offset", pPart->get_offset() },
                      { "part_size", pPart->get_size() },
                      { "dimension_offset", pLink->get_offset() },
                      { "link_type_offset", pLink->get_link_type()->get_offset() },
                      { "singular", pSource->get_cardinality().isSingular() }

                    } );
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

                auto pParent = db_cast< Concrete::UserDimensionContext >( pConcrete->get_parent_context() );
                VERIFY_RTE( pParent );
                const bool bHasUniqueParent = pParent->get_links().size() == 1;

                nlohmann::json target(
                    { { "type", printTypeID( pLink->get_concrete_id() ) },
                      { "parent_type", printTypeID( pLink->get_parent_context()->get_concrete_id() ) },
                      { "has_unique_parent", bHasUniqueParent },
                      { "part_offset", pPart->get_offset() },
                      { "part_size", pPart->get_size() },
                      { "dimension_offset", pLink->get_offset() },
                      { "link_type_offset", pLink->get_link_type()->get_offset() },
                      { "singular", pTarget->get_cardinality().isSingular() }

                    } );
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

    nlohmann::json data( { { "events", nlohmann::json::array() },
                           { "object_types", nlohmann::json::array() },
                           { "concrete_types", nlohmann::json::array() },
                           { "concrete_link_types", nlohmann::json::array() },
                           { "dimension_types", nlohmann::json::array() },
                           { "relation_types", nlohmann::json::array() } } );

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
    {
        JITDatabase::ConcreteToInterface concreteToLinkInterface;
        database.getConcreteToLinkInterface( concreteToLinkInterface );
        for( const auto& type : concreteToLinkInterface )
        {
            nlohmann::json typeInfo(
                { { "concrete", printTypeID( type.first ) }, { "interface", printTypeID( type.second ) } } );
            data[ "concrete_link_types" ].push_back( typeInfo );
        }
    }

    for( const auto pObject : database.getObjects() )
    {
        data[ "object_types" ].push_back( printTypeID( pObject->get_concrete_id() ) );
    }
    std::set< std::string > events;
    for( auto pUserDimension : database.getUserDimensions() )
    {
        using namespace FinalStage;
        using namespace FinalStage::Concrete;

        {
            MemoryLayout::Part* pPart = pUserDimension->get_part();
            // const bool          bSimple    = pUserDimension->get_interface_dimension()->get_simple();
            const std::string strMangled = megaMangle( pUserDimension->get_interface_dimension()->get_erased_type() );

            nlohmann::json typeInfo( { { "type_id", printTypeID( pUserDimension->get_concrete_id() ) },
                                       { "part_offset", pPart->get_offset() },
                                       { "part_size", pPart->get_size() },
                                       { "dimension_offset", pUserDimension->get_offset() },
                                       { "mangled_type_name", strMangled } } );
            data[ "dimension_types" ].push_back( typeInfo );
            events.insert( strMangled );
        }
    }
    for( auto pLinkDimension : database.getLinkDimensions() )
    {
        using namespace FinalStage;
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

    for( const auto& event : events )
    {
        data[ "events" ].push_back( event );
    }

    // relations
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
