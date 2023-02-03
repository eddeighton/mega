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

#include "log/index.hpp"

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

CodeGenerator::CodeGenerator( const mega::network::MegastructureInstallation& megastructureInstallation,
                              const mega::network::Project&                   project )
    : m_pInja( std::make_unique< Inja >( megastructureInstallation, project ) )
{
}

std::string CodeGenerator::allocatorTypeName( const DatabaseInstance&                      database,
                                              FinalStage::Concrete::Dimensions::Allocator* pAllocDim )
{
    using namespace FinalStage;

    std::ostringstream     osTypeName;
    Allocators::Allocator* pAllocator = pAllocDim->get_allocator();
    if( auto pAlloc = db_cast< Allocators::Nothing >( pAllocator ) )
    {
        // do nothing
        THROW_RTE( "Unreachable" );
    }
    else if( auto pAlloc = db_cast< Allocators::Singleton >( pAllocator ) )
    {
        osTypeName << "bool";
    }
    else if( auto pAlloc = db_cast< Allocators::Range32 >( pAllocator ) )
    {
        osTypeName << "mega::Bitmask32Allocator< "
                   << database.getLocalDomainSize( pAlloc->get_allocated_context()->get_concrete_id() ) << " >";
    }
    else if( auto pAlloc = db_cast< Allocators::Range64 >( pAllocator ) )
    {
        osTypeName << "mega::Bitmask64Allocator< "
                   << database.getLocalDomainSize( pAlloc->get_allocated_context()->get_concrete_id() ) << " >";
    }
    else if( auto pAlloc = db_cast< Allocators::RangeAny >( pAllocator ) )
    {
        osTypeName << "mega::RingAllocator< mega::Instance, "
                   << database.getLocalDomainSize( pAlloc->get_allocated_context()->get_concrete_id() ) << " >";
    }
    else
    {
        THROW_RTE( "Unknown allocator type" );
    }
    return osTypeName.str();
}

nlohmann::json CodeGenerator::generate( const DatabaseInstance& database, const mega::InvocationID& invocationID,
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

        const VariableMap variables = generateVariables(
            pInvocation->get_variables(), pInvocation->get_root_instruction()->get_context(), data, indent );

        for( auto pInstruction : pInvocation->get_root_instruction()->get_children() )
        {
            generateInstructions( database, pInstruction, variables, functions, data, indent );
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

void CodeGenerator::generate_invocation( const LLVMCompiler& compiler, const DatabaseInstance& database,
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
            case mega::runtime::invocation::eAllocate:
                m_pInja->render_allocate( data, osCPPCode );
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
            case mega::runtime::invocation::eSave:
                m_pInja->render_save( data, osCPPCode );
                break;
            case mega::runtime::invocation::eLoad:
                m_pInja->render_load( data, osCPPCode );
                break;
            case mega::runtime::invocation::eStart:
                m_pInja->render_start( data, osCPPCode );
                break;
            case mega::runtime::invocation::eStop:
                m_pInja->render_stop( data, osCPPCode );
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

void CodeGenerator::generate_relation( const LLVMCompiler& compiler, const DatabaseInstance& database,
                                       const RelationID& relationID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_relation" );

    auto       pRelation = database.getRelation( relationID );
    const bool bSourceSingular
        = !pRelation->get_source_interface()->get_link_trait()->get_cardinality().maximum().isMany();
    const bool bTargetSingular
        = !pRelation->get_target_interface()->get_link_trait()->get_cardinality().maximum().isMany();

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
                           { "source_singular", bSourceSingular },
                           { "target_singular", bTargetSingular },
                           { "sources", nlohmann::json::array() },
                           { "targets", nlohmann::json::array() } } );

    for( FinalStage::Interface::Link* pSource : pRelation->get_sources() )
    {
        using namespace FinalStage;
        for( auto pConcrete : pSource->get_concrete() )
        {
            Concrete::Link*     pLink          = db_cast< Concrete::Link >( pConcrete );
            auto                pLinkReference = pLink->get_link_reference();
            MemoryLayout::Part* pPart          = pLinkReference->get_part();

            nlohmann::json source( { { "type", pLink->get_concrete_id() },
                                     { "part_offset", pPart->get_offset() },
                                     { "part_size", pPart->get_size() },
                                     { "dimension_offset", pLinkReference->get_offset() }

            } );
            data[ "sources" ].push_back( source );
        }
    }
    for( auto pTarget : pRelation->get_targets() )
    {
        using namespace FinalStage;
        for( auto pConcrete : pTarget->get_concrete() )
        {
            Concrete::Link*     pLink          = db_cast< Concrete::Link >( pConcrete );
            auto                pLinkReference = pLink->get_link_reference();
            MemoryLayout::Part* pPart          = pLinkReference->get_part();

            nlohmann::json target( { { "type", pLink->get_concrete_id() },
                                     { "part_offset", pPart->get_offset() },
                                     { "part_size", pPart->get_size() },
                                     { "dimension_offset", pLinkReference->get_offset() }

            } );
            data[ "targets" ].push_back( target );
        }
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

void CodeGenerator::generate_program( const LLVMCompiler& compiler, const DatabaseInstance& database, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_program" );

    nlohmann::json data( { { "events", nlohmann::json::array() },
                           { "object_types", nlohmann::json::array() },
                           { "concrete_types", nlohmann::json::array() },
                           { "dimension_types", nlohmann::json::array() } } );
    for( const auto pObject : database.getObjects() )
    {
        data[ "object_types" ].push_back( pObject->get_concrete_id() );
        nlohmann::json typeInfo( { { "from", pObject->get_concrete_id() }, { "to", pObject->get_concrete_id() } } );
        data[ "concrete_types" ].push_back( typeInfo );
    }
    std::set< std::string > events;
    for( auto pUserDimension : database.getUserDimensions() )
    {
        using namespace FinalStage;
        using namespace FinalStage::Concrete;

        MemoryLayout::Part* pPart      = pUserDimension->get_part();
        const bool          bSimple    = pUserDimension->get_interface_dimension()->get_simple();
        const std::string   strMangled = megaMangle( pUserDimension->get_interface_dimension()->get_erased_type() );

        nlohmann::json typeInfo( { { "type_id", pUserDimension->get_concrete_id() },
                                   { "part_offset", pPart->get_offset() },
                                   { "part_size", pPart->get_size() },
                                   { "dimension_offset", pUserDimension->get_offset() },
                                   { "mangled_type_name", strMangled } } );
        data[ "dimension_types" ].push_back( typeInfo );
        events.insert( strMangled );

        auto pContext = pUserDimension->get_parent();
        while( pContext )
        {
            if( db_cast< FinalStage::Concrete::Object >( pContext ) )
                break;
            pContext = db_cast< FinalStage::Concrete::Context >( pContext->get_parent() );
        }
        VERIFY_RTE_MSG( pContext, "Failed to locate parent object for type: " << pUserDimension->get_concrete_id() );

        nlohmann::json objectTypeInfo(
            { { "from", pUserDimension->get_concrete_id() }, { "to", pContext->get_concrete_id() } } );
        data[ "concrete_types" ].push_back( objectTypeInfo );
    }
    /*for( auto pLinkDimension : database.getLinkDimensions() )
    {
        using namespace FinalStage;
        using namespace FinalStage::Concrete;

        std::string strMangled;
        if ( db_cast< Concrete::Dimensions::LinkSingle >( pLinkDimension ) )
        {
            //const std::string   strMangled
            //    = megaMangle( pLinkDimension->get_link()-> );
        }
        else if ( db_cast< Concrete::Dimensions::LinkSingle >( pLinkDimension ) )
        {

        }
        else
        {
            THROW_RTE( "Unknown link reference type" );
        }

        MemoryLayout::Part* pPart   = pLinkDimension->get_part();

        nlohmann::json typeInfo( { { "type_id", pLinkDimension->get_concrete_id() },
                                    { "part_offset", pPart->get_offset() },
                                    { "part_size", pPart->get_size() },
                                    { "dimension_offset", pLinkDimension->get_offset() },
                                    { "mangled_type_name", strMangled } } );
        data[ "dimension_types" ].push_back( typeInfo );
        events.insert( strMangled );
    }*/

    for( const auto& event : events )
    {
        data[ "events" ].push_back( event );
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
