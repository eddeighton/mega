
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

#ifndef GUARD_2023_July_28_generator_retail
#define GUARD_2023_July_28_generator_retail

#include "templates.hpp"
#include "invocations.hpp"
#include "interface.hpp"

#include "database/common/environment_archive.hpp"
#include "database/types/source_location.hpp"

#include <string>

namespace FinalStage
{
using namespace FinalStage;
using namespace FinalStage::Interface;
#include "compiler/generator_operations.hpp"
} // namespace FinalStage

using namespace FinalStage;

namespace driver::retail
{
class RetailGen
{
    static void generateDataStructures( const mega::io::Manifest& manifest, Database& database, nlohmann::json& data )
    {
        using ConcreteNames = std::map< Concrete::Context*, std::string >;
        ConcreteNames concreteNames;
        {
            for( const auto& megaSrcFile : manifest.getMegaSourceFiles() )
            {
                for( Concrete::Object* pObject : database.many< Concrete::Object >( megaSrcFile ) )
                {
                    std::ostringstream os;
                    os << "o_" << pObject->get_concrete_id().getObjectID();
                    concreteNames.insert( { ( Concrete::Context* )pObject, os.str() } );

                    nlohmann::json object( { { "type", os.str() }, { "members", nlohmann::json::array() }

                    } );

                    for( auto pBuffer : pObject->get_buffers() )
                    {
                        for( auto pPart : pBuffer->get_parts() )
                        {
                            for( auto pAllocation : pPart->get_allocation_dimensions() )
                            {
                                auto pAllocatorDim = db_cast< Concrete::Dimensions::Allocator >( pAllocation );
                                VERIFY_RTE( pAllocatorDim );

                                auto getLocalDomainSize = []( Concrete::Context* pContext ) -> mega::U64
                                {
                                    if( auto pObject = db_cast< Concrete::Object >( pContext ) )
                                    {
                                        return 1U;
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
                                        return 1U;
                                    }
                                    else
                                    {
                                        return 1U;
                                    }
                                };

                                std::ostringstream     osTypeName;
                                Allocators::Allocator* pAllocator = pAllocatorDim->get_allocator();
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
                                               << getLocalDomainSize( pAlloc->get_allocated_context() ) << " >";
                                }
                                else if( auto pAlloc = db_cast< Allocators::Range64 >( pAllocator ) )
                                {
                                    osTypeName << "mega::Bitmask64Allocator< "
                                               << getLocalDomainSize( pAlloc->get_allocated_context() ) << " >";
                                }
                                else if( auto pAlloc = db_cast< Allocators::RangeAny >( pAllocator ) )
                                {
                                    osTypeName << "mega::RingAllocator< mega::Instance, "
                                               << getLocalDomainSize( pAlloc->get_allocated_context() ) << " >";
                                }
                                else
                                {
                                    THROW_RTE( "Unknown allocator type" );
                                }

                                std::ostringstream osName;
                                osName << "m_" << pAllocation->get_concrete_id().getSubObjectID();
                                nlohmann::json member( { { "type", osTypeName.str() }, { "name", osName.str() } } );
                                object[ "members" ].push_back( member );
                                concreteNames.insert( { ( Concrete::Context* )pAllocation, osName.str() } );
                            }
                            for( auto pLink : pPart->get_link_dimensions() )
                            {
                                std::ostringstream osType;
                                if( auto pLinkSingle = db_cast< Concrete::Dimensions::LinkSingle >( pLink ) )
                                {
                                    osType << "mega::reference";
                                }
                                else if( auto pLinkMany = db_cast< Concrete::Dimensions::LinkMany >( pLink ) )
                                {
                                    osType << "std::vector< mega::reference >";
                                }
                                else
                                {
                                    THROW_RTE( "Unknown link reference type" );
                                }
                                std::ostringstream osName;
                                osName << "m_" << pLink->get_concrete_id().getSubObjectID();
                                nlohmann::json member( { { "type", osType.str() }, { "name", osName.str() } } );
                                object[ "members" ].push_back( member );
                                concreteNames.insert( { ( Concrete::Context* )pLink, osName.str() } );
                            }
                            for( auto pDim : pPart->get_user_dimensions() )
                            {
                                std::ostringstream osName;
                                osName << "m_" << pDim->get_concrete_id().getSubObjectID();
                                nlohmann::json member(
                                    { { "type", pDim->get_interface_dimension()->get_canonical_type() },
                                      { "name", osName.str() } } );
                                object[ "members" ].push_back( member );
                                concreteNames.insert( { ( Concrete::Context* )pDim, osName.str() } );
                            }
                        }
                    }
                    data[ "objects" ].push_back( object );
                }
            }
        }
    }

public:
    static std::string generate( TemplateEngine& templateEngine, mega::io::ArchiveEnvironment& environment,
                                 const mega::io::Manifest& manifest, Database& database,
                                 const boost::filesystem::path& operationsTemplateFilePath )
    {
        nlohmann::json data( { { "cppIncludes", nlohmann::json::array() },
                               { "systemIncludes", nlohmann::json::array() },
                               { "result_types", nlohmann::json::array() },
                               { "interface", "" },
                               { "objects", nlohmann::json::array() },
                               { "trait_structs", nlohmann::json::array() },
                               { "result_types", nlohmann::json::array() },
                               { "invocations", nlohmann::json::array() },
                               { "operation_bodies", nlohmann::json::array() }

        } );

        const InvocationInfo invocationInfo( manifest, database );

        Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( environment.project_manifest() );

        // user includes
        {
            std::set< boost::filesystem::path > cppIncludes;
            for( const mega::io::megaFilePath& megaFile : manifest.getMegaSourceFiles() )
            {
                for( Parser::CPPInclude* pCPPInclude : database.many< Parser::CPPInclude >( megaFile ) )
                {
                    cppIncludes.insert( pCPPInclude->get_cppSourceFilePath() );
                }
            }
            for( const auto& cppInclude : cppIncludes )
            {
                data[ "cppIncludes" ].push_back( cppInclude.string() );
            }
        }

        // system includes
        {
            std::set< std::string > systemIncludes;
            for( const mega::io::megaFilePath& megaFile : manifest.getMegaSourceFiles() )
            {
                for( Parser::SystemInclude* pSystemInclude : database.many< Parser::SystemInclude >( megaFile ) )
                {
                    systemIncludes.insert( pSystemInclude->get_str() );
                }
            }
            for( const auto& systemInclude : systemIncludes )
            {
                data[ "systemIncludes" ].push_back( systemInclude );
            }
        }

        // interface and traits
        {
            std::ostringstream osInterface;
            nlohmann::json     interfaceOperations = nlohmann::json::array();
            nlohmann::json     traitStructs;
            {
                // NOTE: need to observe dependency order of source files
                for( const auto& megaSrcFile : manifest.getMegaSourceFiles() )
                {
                    Interface::Root*      pRoot = database.one< Interface::Root >( megaSrcFile );
                    CleverUtility::IDList namespaces, types;
                    for( Interface::IContext* pContext : pRoot->get_children() )
                    {
                        recurseInterface( invocationInfo, pSymbolTable, templateEngine, namespaces, types, pContext,
                                          osInterface, interfaceOperations, traitStructs );
                    }
                }
            }
            data[ "interface" ] = osInterface.str();
            // data[ "operations" ]    = interfaceOperations;
            data[ "trait_structs" ] = traitStructs;
        }

        // concrete data structures
        {
            generateDataStructures( manifest, database, data );
        }

        // traits

        // result types and invocations
        {
            nlohmann::json resultTypes = nlohmann::json::array();
            nlohmann::json invocations = nlohmann::json::array();
            {
                // NOTE: need to observe dependency order of source files
                for( const auto& megaSrcFile : manifest.getMegaSourceFiles() )
                {
                    Interface::Root*      pRoot = database.one< Interface::Root >( megaSrcFile );
                    CleverUtility::IDList namespaces, types;
                    for( Interface::IContext* pContext : pRoot->get_children() )
                    {
                        recurseInvocations(
                            invocationInfo, templateEngine, namespaces, types, pContext, resultTypes, invocations );
                    }
                }
            }
            data[ "result_types" ] = resultTypes;
            data[ "invocations" ]  = invocations;
        }

        // operation bodies
        {
            for( const auto& megaSrcFile : manifest.getMegaSourceFiles() )
            {
                ::inja::Environment injaEnvironment;
                {
                    injaEnvironment.set_trim_blocks( true );
                }
                OperationsGen::TemplateEngine templateEngine(
                    environment, injaEnvironment, operationsTemplateFilePath );
                Interface::Root*  pRoot         = database.one< Interface::Root >( megaSrcFile );
                const std::string strOperations = OperationsGen::generate( templateEngine, pRoot, false );

                std::map< mega::SourceLocation, Operations::Invocation* > rewrites;

                for( auto pInvocationContext : database.many< Interface::InvocationContext >( megaSrcFile ) )
                {
                    for( auto pInvocationInstance : pInvocationContext->get_invocation_instances() )
                    {
                        auto pInvocation = pInvocationInstance->get_invocation();
                        auto sourceLoc   = pInvocationInstance->get_source_location();

                        VERIFY_RTE_MSG(
                            rewrites.insert( { sourceLoc, pInvocation } )
                                .second,
                            "Found duplicate rewrite location for: " << pInvocation->get_id() );
                    }
                }

                std::ostringstream osReWrite;
                {
                    auto iReWrite = rewrites.begin(), iReWriteEnd = rewrites.end();
                    for( auto i = strOperations.begin(), iEnd = strOperations.end(); i != iEnd; )
                    {
                        if( iReWrite != iReWriteEnd )
                        {
                            VERIFY_RTE_MSG(
                                iReWrite->first.getBegin() < strOperations.size(), "Invalid range in operation" );
                            auto iRewriteStart = strOperations.begin() + iReWrite->first.getBegin();
                            osReWrite << std::string_view( i, iRewriteStart );

                            // increment to just after the openning parenthesis
                            i = std::find( iRewriteStart, iEnd, '(' );

                            /*bool bAddComma = false;
                            {
                                // need to deterime if the invocation has additional arguments or not
                                auto iUntilRParen = i;
                                while( *iUntilRParen != ')' )
                                {
                                    if( !std::iswspace( *iUntilRParen ) )
                                    {
                                        bAddComma = true;
                                        break;
                                    }
                                    ++iUntilRParen;
                                }
                            }*/
                            
                            osReWrite << generateInvocationUse( invocationInfo, iReWrite->second );
                            ++iReWrite;
                        }
                        else
                        {
                            osReWrite << std::string_view( i, iEnd );
                            i = iEnd;
                        }
                    }
                }
                data[ "operation_bodies" ].push_back( osReWrite.str() );
            }
        }

        std::ostringstream os;
        templateEngine.renderRetail( data, os );

        std::string strCode = os.str();
        mega::utilities::clang_format( strCode, std::optional< boost::filesystem::path >() );
        return strCode;
    }
};

} // namespace driver::retail

#endif // GUARD_2023_July_28_generator_retail
