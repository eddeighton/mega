
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
    static InvocationInfo getInvocationInfo( const mega::io::Manifest& manifest, Database& database )
    {
        InvocationInfo invocationInfo;

        for( const auto& megaSrcFile : manifest.getMegaSourceFiles() )
        {
            Operations::Invocations* pInvocations = database.one< Operations::Invocations >( megaSrcFile );

            for( const auto& [ id, pInvocation ] : pInvocations->get_invocations() )
            {
                const auto context = pInvocation->get_context();

                for( const auto vec : context->get_vectors() )
                {
                    for( const auto element : vec->get_elements() )
                    {
                        auto interfaceContext = element->get_interface();
                        if( interfaceContext->get_context().has_value() )
                        {
                            invocationInfo.contextInvocations.insert(
                                { interfaceContext->get_context().value(), pInvocation } );
                        }
                        else
                        {
                            invocationInfo.dimensionInvocations.insert(
                                { interfaceContext->get_dimension().value(), pInvocation } );
                        }
                    }
                }
            }
        }
        return invocationInfo;
    }

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
        const InvocationInfo invocationInfo = getInvocationInfo( manifest, database );

        Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( environment.project_manifest() );

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

        nlohmann::json data( { { "cppIncludes", nlohmann::json::array() },
                               { "systemIncludes", nlohmann::json::array() },
                               { "result_types", nlohmann::json::array() },
                               { "interface", osInterface.str() },
                               { "objects", nlohmann::json::array() },
                               { "trait_structs", traitStructs },
                               { "operations", interfaceOperations },
                               { "operation_bodies", "" }

        } );

        generateDataStructures( manifest, database, data );

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

        {
            std::ostringstream osOperations;
            for( const auto& megaSrcFile : manifest.getMegaSourceFiles() )
            {
                ::inja::Environment injaEnvironment;
                {
                    injaEnvironment.set_trim_blocks( true );
                }
                OperationsGen::TemplateEngine templateEngine(
                    environment, injaEnvironment, operationsTemplateFilePath );
                Interface::Root* pRoot         = database.one< Interface::Root >( megaSrcFile );
                std::string      strOperations = OperationsGen::generate( templateEngine, pRoot );

                // rewrite invocations
                for( Interface::IContext* pContext : database.many< Interface::IContext >( megaSrcFile ) )
                {
                    for( auto i    = invocationInfo.contextInvocations.lower_bound( pContext ),
                              iEnd = invocationInfo.contextInvocations.upper_bound( pContext );
                         i != iEnd;
                         ++i )
                    {
                        const Operations::Invocation*              pInvocation     = i->second;
                        const std::vector< mega::SourceLocation >& sourceLocations = pInvocation->get_file_offsets();

                        nlohmann::json invocation = { { "name", generateInvocationName( pInvocation ) } };
                        // contextInvocations.push_back( invocation );
                    }
                }

                osOperations << strOperations;
            }
            data[ "operation_bodies" ] = osOperations.str();
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
