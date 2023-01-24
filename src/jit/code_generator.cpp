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

#include "log/record.hpp"

#include "service/network/log.hpp"
#include "service/mpo_context.hpp"

#include "mega/invocation_io.hpp"
#include "mega/relation_io.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <boost/process.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <istream>
#include <sstream>

namespace mega::runtime
{
namespace
{

struct Indent
{
    int        amt = 1;
    inline int operator++()
    {
        ++amt;
        return amt;
    };
    inline int operator--()
    {
        --amt;
        return amt;
    };
    inline operator int() const { return amt; }
};

inline std::ostream& operator<<( std::ostream& os, const Indent& indent )
{
    static const std::string indentation( "    " );
    for( int i = 0; i != indent; ++i )
        os << indentation;
    return os;
}

using VariableMap = std::map< const FinalStage::Invocations::Variables::Variable*, std::string >;

inline std::string get( const VariableMap& varMap, const FinalStage::Invocations::Variables::Variable* pVar )
{
    auto iFind = varMap.find( pVar );
    VERIFY_RTE( iFind != varMap.end() );
    return iFind->second;
}

struct FunctionPointers
{
    using CallSet        = std::set< const FinalStage::Concrete::Context* >;
    using CopySet        = std::set< std::string >;
    using EventSet       = std::set< std::string >;
    using AllocatorSet   = std::set< std::string >;
    using FreerSet       = std::set< std::string >;
    using ObjectAllocSet = std::set< const FinalStage::Concrete::Context* >;
    using ObjectFreeSet  = std::set< const FinalStage::Concrete::Context* >;
    using ObjectSaveSet  = std::set< const FinalStage::Concrete::Context* >;
    using ObjectLoadSet  = std::set< const FinalStage::Concrete::Context* >;

    CallSet        callSet;
    CopySet        copySet;
    EventSet       eventSet;
    AllocatorSet   allocatorSet;
    FreerSet       freerSet;
    ObjectAllocSet objectAllocSet;
    ObjectFreeSet  objectFreeSet;
    ObjectSaveSet  objectSave;
    ObjectLoadSet  objectLoad;

    nlohmann::json init( const InvocationID& invocationID, std::string& strName ) const
    {
        {
            using ::           operator<<;
            std::ostringstream osName;
            osName << invocationID;
            strName = osName.str();
        }

        return { { "name", strName },
                 { "module_name", strName },
                 { "calls", nlohmann::json::array() },
                 { "copiers", nlohmann::json::array() },
                 { "events", nlohmann::json::array() },
                 { "allocators", nlohmann::json::array() },
                 { "freers", nlohmann::json::array() },
                 { "object_allocators", nlohmann::json::array() },
                 { "variables", nlohmann::json::array() },
                 { "assignments", nlohmann::json::array() },
                 { "object_savers", nlohmann::json::array() } };
    }

    void generate( nlohmann::json& data )
    {
        for( auto pCall : callSet )
        {
            data[ "calls" ].push_back( pCall->get_concrete_id() );
        }
        for( auto copy : copySet )
        {
            data[ "copiers" ].push_back( copy );
        }
        for( auto event : eventSet )
        {
            data[ "events" ].push_back( event );
        }
        for( auto allocator : allocatorSet )
        {
            data[ "allocators" ].push_back( allocator );
        }
        for( auto freer : freerSet )
        {
            data[ "freers" ].push_back( freer );
        }
        for( auto pObject : objectAllocSet )
        {
            data[ "object_allocators" ].push_back( pObject->get_concrete_id() );
        }
        for( auto pObject : objectSave )
        {
            data[ "object_savers" ].push_back( pObject->get_concrete_id() );
        }
        for( auto pObject : objectLoad )
        {
            data[ "object_loaders" ].push_back( pObject->get_concrete_id() );
        }
    }
};

std::string allocatorTypeName( const DatabaseInstance&                      database,
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

} // namespace

class CodeGenerator::Pimpl
{
    ::inja::Environment m_injaEnvironment;
    ::inja::Template    m_allocateTemplate;
    ::inja::Template    m_allocatorTemplate;
    ::inja::Template    m_callTemplate;
    ::inja::Template    m_getTemplate;
    ::inja::Template    m_loadTemplate;
    ::inja::Template    m_programTemplate;
    ::inja::Template    m_readLinkTemplate;
    ::inja::Template    m_readTemplate;
    ::inja::Template    m_relationTemplate;
    ::inja::Template    m_saveTemplate;
    ::inja::Template    m_startTemplate;
    ::inja::Template    m_stopTemplate;
    ::inja::Template    m_writeLinkTemplate;
    ::inja::Template    m_writeTemplate;

public:
    Pimpl( const mega::network::MegastructureInstallation& megaInstall, const mega::network::Project& project )
    {
        m_injaEnvironment.set_trim_blocks( true );
        m_allocateTemplate  = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateAllocate().string() );
        m_allocatorTemplate = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateAllocation().string() );
        m_callTemplate      = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateCall().string() );
        m_getTemplate       = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateGet().string() );
        m_loadTemplate      = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateLoad().string() );
        m_programTemplate   = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateProgram().string() );
        m_readLinkTemplate  = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateReadLink().string() );
        m_readTemplate      = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateRead().string() );
        m_relationTemplate  = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateRelation().string() );
        m_saveTemplate      = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateSave().string() );
        m_startTemplate     = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateStart().string() );
        m_stopTemplate      = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateStop().string() );
        m_writeLinkTemplate = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateWriteLink().string() );
        m_writeTemplate     = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateWrite().string() );
    }

    void render_allocator( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_allocatorTemplate, data );
    }
    void render_allocate( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_allocateTemplate, data );
    }
    void render_read( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_readTemplate, data );
    }
    void render_write( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_writeTemplate, data );
    }
    void render_readLink( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_readLinkTemplate, data );
    }
    void render_writeLink( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_writeLinkTemplate, data );
    }
    void render_call( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_callTemplate, data );
    }
    void render_start( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_startTemplate, data );
    }
    void render_stop( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_stopTemplate, data );
    }
    void render_save( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_saveTemplate, data );
    }
    void render_load( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_loadTemplate, data );
    }
    void render_get( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_getTemplate, data );
    }
    void render_relation( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_relationTemplate, data );
    }
    void render_program( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_programTemplate, data );
    }

    std::string render( const std::string& strTemplate, const nlohmann::json& data )
    {
        try
        {
            return m_injaEnvironment.render( strTemplate, data );
        }
        catch( inja::InjaError& ex )
        {
            SPDLOG_ERROR( "inja::InjaError in CodeGenerator::render rendering: {}", ex.what() );
            THROW_RTE( "inja::InjaError in CodeGenerator::render rendering: " << ex.what() );
        }
    }

    void generateInstructions( const DatabaseInstance&                             database,
                               FinalStage::Invocations::Instructions::Instruction* pInstruction,
                               const VariableMap& variables, FunctionPointers& functions, nlohmann::json& data,
                               Indent& indent )
    {
        using namespace FinalStage;
        using namespace FinalStage::Invocations;

        if( auto pInstructionGroup = db_cast< Instructions::InstructionGroup >( pInstruction ) )
        {
            bool bTailRecursion = true;
            if( auto pParentDerivation = db_cast< Instructions::ParentDerivation >( pInstructionGroup ) )
            {
                std::ostringstream os;
                os << indent << "// ParentDerivation\n";

                const Variables::Instance* pFrom = pParentDerivation->get_from();
                const Variables::Instance* pTo   = pParentDerivation->get_to();

                const std::string  s           = get( variables, pFrom );
                const mega::TypeID targetType  = pFrom->get_concrete()->get_concrete_id();
                const mega::U64    szLocalSize = database.getLocalDomainSize( targetType );

                os << indent << get( variables, pTo ) << " = mega::reference::make( " << get( variables, pFrom )
                   << ", mega::TypeInstance{ static_cast< mega::Instance >( " << s << ".getInstance() / " << szLocalSize
                   << " ), " << targetType << "} );\n";

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pChildDerivation = db_cast< Instructions::ChildDerivation >( pInstructionGroup ) )
            {
                std::ostringstream os;
                os << indent << "// ChildDerivation\n";

                const Variables::Instance* pFrom = pChildDerivation->get_from();
                const Variables::Instance* pTo   = pChildDerivation->get_to();

                const std::string  s           = get( variables, pFrom );
                const mega::TypeID targetType  = pTo->get_concrete()->get_concrete_id();
                const mega::U64    szLocalSize = database.getLocalDomainSize( targetType );

                os << indent << get( variables, pTo ) << " = mega::reference::make( " << get( variables, pFrom ) << ", "
                   << targetType << " );\n";

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pEnumDerivation = db_cast< Instructions::EnumDerivation >( pInstructionGroup ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// EnumDerivation\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pEnumeration = db_cast< Instructions::Enumeration >( pInstructionGroup ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// Enumeration\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pDimensionReferenceRead
                     = db_cast< Instructions::DimensionReferenceRead >( pInstructionGroup ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// DimensionReferenceRead\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pMonoReference = db_cast< Instructions::MonoReference >( pInstructionGroup ) )
            {
                const Variables::Instance*  pInstance  = pMonoReference->get_instance();
                const Variables::Reference* pReference = pMonoReference->get_reference();

                std::ostringstream os;
                os << indent << "// MonoReference\n";
                os << indent << get( variables, pInstance ) << " = mega::reference::make( "
                   << get( variables, pReference ) << ", " << pInstance->get_concrete()->get_concrete_id() << " );";

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pPolyReference = db_cast< Instructions::PolyReference >( pInstructionGroup ) )
            {
                bTailRecursion = false;

                {
                    std::ostringstream os;
                    os << indent << "// PolyReference\n";
                    const Variables::Reference* pReference = pPolyReference->get_from_reference();

                    os << indent << "switch( " << get( variables, pReference ) << ".getType() )\n";
                    os << indent << "{";
                    data[ "assignments" ].push_back( os.str() );
                }

                {
                    ++indent;
                    for( auto pChildInstruction : pInstructionGroup->get_children() )
                    {
                        generateInstructions( database, pChildInstruction, variables, functions, data, indent );
                    }
                    --indent;
                }

                {
                    std::ostringstream os;
                    os << indent << "}";
                    data[ "assignments" ].push_back( os.str() );
                }
            }
            else if( auto pPolyCase = db_cast< Instructions::PolyCase >( pInstructionGroup ) )
            {
                bTailRecursion = false;

                {
                    std::ostringstream os;
                    os << indent << "// PolyCase\n";

                    const Variables::Reference* pReference = pPolyCase->get_reference();
                    const Variables::Instance*  pInstance  = pPolyCase->get_to();

                    os << indent << "case " << pInstance->get_concrete()->get_concrete_id() << " :\n";
                    os << indent << "{\n";
                    ++indent;
                    os << indent << get( variables, pInstance ) << " = mega::reference::make( "
                       << get( variables, pReference ) << ", " << pInstance->get_concrete()->get_concrete_id()
                       << " );\n";
                    data[ "assignments" ].push_back( os.str() );
                }

                {
                    for( auto pChildInstruction : pInstructionGroup->get_children() )
                    {
                        generateInstructions( database, pChildInstruction, variables, functions, data, indent );
                    }
                }

                {
                    --indent;
                    std::ostringstream os;
                    os << indent << "}";
                    data[ "assignments" ].push_back( os.str() );
                }
            }
            else if( db_cast< Instructions::Elimination >( pInstructionGroup ) )
            {
                // do nothing
            }
            else if( db_cast< Instructions::Failure >( pInstructionGroup ) )
            {
                THROW_RTE( "Invalid Failure instruction" );
            }
            else if( db_cast< Instructions::Prune >( pInstructionGroup ) )
            {
                THROW_RTE( "Invalid Prune instruction" );
            }
            else
            {
                THROW_RTE( "Unknown instruction type" );
            }

            if( bTailRecursion )
            {
                ++indent;
                for( auto pChildInstruction : pInstructionGroup->get_children() )
                {
                    generateInstructions( database, pChildInstruction, variables, functions, data, indent );
                }
                --indent;
            }
        }
        else if( auto pOperation = db_cast< FinalStage::Invocations::Operations::Operation >( pInstruction ) )
        {
            using namespace FinalStage::Invocations::Operations;

            if( auto pAllocate = db_cast< Allocate >( pOperation ) )
            {
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    mega::reference allocatedRef = mega::runtime::allocate( {{ instance }}, {{ concrete_type_id }} );
{% if has_owning_link %}
{{ indent }}    static thread_local mega::runtime::relation::LinkMake function( g_pszModuleName, mega::RelationID{ {{ owning_relation_id }} } );
{{ indent }}    function( {{ link_source }}, {{ link_target }} );
{% endif %}
{{ indent }}    return allocatedRef;
{{ indent }}}
)TEMPLATE";
                // clang-format on
                std::ostringstream os;
                {
                    Variables::Instance* pInstance       = pAllocate->get_instance();
                    Concrete::Context*   pConcreteTarget = pAllocate->get_concrete_target();
                    std::ostringstream   osIndent;
                    osIndent << indent;

                    nlohmann::json templateData( { { "indent", osIndent.str() },
                                                   { "concrete_type_id", pConcreteTarget->get_concrete_id() },
                                                   { "instance", get( variables, pInstance ) },
                                                   { "has_owning_link", false },
                                                   { "link_source", "" },
                                                   { "link_target", "" },
                                                   { "owning_relation_id", "" } } );

                    auto pConstructedObject = db_cast< Concrete::Object >( pConcreteTarget );
                    VERIFY_RTE( pConstructedObject );
                    std::vector< Concrete::Link* > owningLinks = pConstructedObject->get_owning_links();
                    if( !owningLinks.empty() )
                    {
                        Concrete::Context* pParentContext = pInstance->get_concrete();
                        if( Concrete::Link* pParentLink = db_cast< Concrete::Link >( pParentContext ) )
                        {
                            Interface::LinkInterface* pParentLinkInterface = pParentLink->get_link_interface();
                            Interface::LinkInterface* pChildLinkInterface  = nullptr;
                            HyperGraph::Relation*     pRelation            = pParentLinkInterface->get_relation();

                            if( pRelation->get_source_interface() == pParentLinkInterface )
                            {
                                pChildLinkInterface = pRelation->get_target_interface();
                            }
                            else if( pRelation->get_target_interface() == pParentLinkInterface )
                            {
                                pChildLinkInterface = pRelation->get_source_interface();
                            }
                            else
                            {
                                THROW_RTE( "Bad relation" );
                            }

                            Concrete::Link* pOwningLink = nullptr;
                            for( Concrete::Link* pLink : owningLinks )
                            {
                                if( pLink->get_link_interface() == pChildLinkInterface )
                                {
                                    VERIFY_RTE_MSG( !pOwningLink, "Duplicate owning links" );
                                    pOwningLink = pLink;
                                }
                            }
                            VERIFY_RTE_MSG( pOwningLink, "Failed to locate owning link interface for allocation" );
                            std::ostringstream osLinkReference;
                            osLinkReference << "mega::reference::make( allocatedRef, " << pOwningLink->get_concrete_id()
                                            << " )";

                            Interface::LinkTrait* pLinkTrait = pChildLinkInterface->get_link_trait();
                            if( pRelation->get_source_interface() == pChildLinkInterface )
                            {
                                templateData[ "link_source" ] = osLinkReference.str();
                                templateData[ "link_target" ] = get( variables, pInstance );
                            }
                            else if( pRelation->get_target_interface() == pChildLinkInterface )
                            {
                                templateData[ "link_source" ] = get( variables, pInstance );
                                templateData[ "link_target" ] = osLinkReference.str();
                            }
                            else
                            {
                                THROW_RTE( "Invalid relation" );
                            }

                            const mega::RelationID& relationID = pRelation->get_id();
                            std::ostringstream      osRelationID;
                            osRelationID << relationID.getLower() << ',' << relationID.getUpper();

                            templateData[ "owning_relation_id" ] = osRelationID.str();
                            templateData[ "has_owning_link" ]    = true;
                        }
                        else
                        {
                            THROW_RTE( "Invalid allocation parent" );
                        }
                    }

                    os << render( szTemplate, templateData );
                }

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pCall = db_cast< Call >( pOperation ) )
            {
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    static thread_local mega::runtime::object::CallGetter function( g_pszModuleName, {{ concrete_type_id }} );
{{ indent }}    return mega::runtime::CallResult{ function(), {{ instance }} };
{{ indent }}}
)TEMPLATE";
                // clang-format on
                std::ostringstream os;
                {
                    Concrete::Context*   pConcreteTarget = pCall->get_concrete_target();
                    Variables::Instance* pInstance       = pCall->get_instance();
                    std::ostringstream   osIndent;
                    osIndent << indent;

                    nlohmann::json templateData( { { "indent", osIndent.str() },
                                                   { "concrete_type_id", pConcreteTarget->get_concrete_id() },
                                                   { "instance", get( variables, pInstance ) } } );

                    os << render( szTemplate, templateData );

                    functions.callSet.insert( pConcreteTarget );
                }

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pStart = db_cast< Start >( pOperation ) )
            {
                THROW_TODO;
                /*std::ostringstream os;
                os << indent << "// Start\n";

                Concrete::Context*   pConcreteTarget = pStart->get_concrete_target();
                Variables::Instance* pInstance       = pStart->get_instance();

                auto optAllocDim = pConcreteTarget->get_allocator()->get_dimension();
                ASSERT( optAllocDim.has_value() );

                Concrete::Dimensions::Allocator* pAllocationDimension = optAllocDim.value();
                Allocators::Allocator*           pAllocator           = pAllocationDimension->get_allocator();

                const std::string strAllocatorTypeName = allocatorTypeName( database, pAllocationDimension );
                const std::string strMangle            = megaMangle( strAllocatorTypeName );

                allocators.insert( strMangle );

                MemoryLayout::Part* pPart     = pAllocationDimension->get_part();
                const mega::TypeID  contextID = pPart->get_context()->get_concrete_id();

                {
                    generateBufferFPtrCheck( true, contextID, os );
                    generateReferenceCheck( true, contextID, get( variables, pInstance ), os );
                    generateAllocate(
                        strMangle, true, contextID, pPart, get( variables, pInstance ), pAllocationDimension, os );
                }

                data[ "assignments" ].push_back( os.str() );

                events.insert( strMangle );

                functions.parts.insert( pAllocationDimension->get_part() );*/
            }
            else if( auto pStop = db_cast< Stop >( pOperation ) )
            {
                THROW_TODO;
                /*std::ostringstream os;
                os << indent << "// Stop\n";

                Concrete::Context*   pConcreteTarget = pStart->get_concrete_target();
                Variables::Instance* pInstance       = pStart->get_instance();

                auto optAllocDim = pConcreteTarget->get_allocator()->get_dimension();
                ASSERT( optAllocDim.has_value() );

                Concrete::Dimensions::Allocator* pAllocationDimension = optAllocDim.value();
                Allocators::Allocator*           pAllocator           = pAllocationDimension->get_allocator();

                const std::string strAllocatorTypeName = allocatorTypeName( database, pAllocationDimension );
                const std::string strMangle            = megaMangle( strAllocatorTypeName );

                MemoryLayout::Part* pPart     = pAllocationDimension->get_part();
                const mega::TypeID  contextID = pPart->get_context()->get_concrete_id();

                {
                    generateBufferFPtrCheck( true, contextID, os );
                    generateReferenceCheck( true, contextID, get( variables, pInstance ), os );
                    generateFreeDimRead( strMangle, get( variables, pInstance ), true, contextID, pPart,
                                         get( variables, pInstance ), pAllocationDimension, os );
                }

                data[ "assignments" ].push_back( os.str() );

                functions.parts.insert( pAllocationDimension->get_part() );*/
            }
            else if( auto pSave = db_cast< Save >( pOperation ) )
            {
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    if( {{ instance }}.getMPO() != mega::runtime::getThisMPO() )
{{ indent }}    {
{{ indent }}        mega::runtime::readLock( {{ instance }} );
{{ indent }}    }
{{ indent }}    else if( {{ instance }}.isNetworkAddress() )
{{ indent }}    {
{{ indent }}        mega::runtime::networkToHeap( {{ instance }} );
{{ indent }}    }
{{ indent }}    static thread_local mega::runtime::object::ObjectSaveXMLStructure functionStructure( g_pszModuleName, {{ concrete_type_id }} );
{{ indent }}    functionStructure( {{ instance }}, pArchive );
{{ indent }}    static thread_local mega::runtime::object::ObjectSaveXML function( g_pszModuleName, {{ concrete_type_id }} );
{{ indent }}    function( {{ instance }}, pArchive );
{{ indent }}}
)TEMPLATE";
                // clang-format on
                std::ostringstream os;
                {
                    Concrete::Context*   pConcreteTarget = pSave->get_concrete_target();
                    Variables::Instance* pInstance       = pSave->get_instance();

                    std::ostringstream osIndent;
                    osIndent << indent;

                    nlohmann::json templateData( { { "indent", osIndent.str() },
                                                   { "concrete_type_id", pConcreteTarget->get_concrete_id() },
                                                   { "instance", get( variables, pInstance ) } } );

                    os << render( szTemplate, templateData );

                    functions.objectSave.insert( pConcreteTarget );
                }

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pLoad = db_cast< Load >( pOperation ) )
            {
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    if( {{ instance }}.getMPO() != mega::runtime::getThisMPO() )
{{ indent }}    {
{{ indent }}        mega::runtime::readLock( {{ instance }} );
{{ indent }}    }
{{ indent }}    else if( {{ instance }}.isNetworkAddress() )
{{ indent }}    {
{{ indent }}        mega::runtime::networkToHeap( {{ instance }} );
{{ indent }}    }
{{ indent }}    static thread_local mega::runtime::object::ObjectLoadXMLStructure functionStructure( g_pszModuleName, {{ concrete_type_id }} );
{{ indent }}    functionStructure( {{ instance }}, pArchive );
{{ indent }}    static thread_local mega::runtime::object::ObjectLoadXML function( g_pszModuleName, {{ concrete_type_id }} );
{{ indent }}    function( {{ instance }}, pArchive );
{{ indent }}}
)TEMPLATE";
                // clang-format on
                std::ostringstream os;
                {
                    Concrete::Context*   pConcreteTarget = pLoad->get_concrete_target();
                    Variables::Instance* pInstance       = pLoad->get_instance();

                    std::ostringstream osIndent;
                    osIndent << indent;

                    nlohmann::json templateData( { { "indent", osIndent.str() },
                                                   { "concrete_type_id", pConcreteTarget->get_concrete_id() },
                                                   { "instance", get( variables, pInstance ) } } );

                    os << render( szTemplate, templateData );

                    functions.objectSave.insert( pConcreteTarget );
                }

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pFiles = db_cast< Files >( pOperation ) )
            {
                THROW_TODO;
            }
            else if( auto pGet = db_cast< GetAction >( pOperation ) )
            {
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    return mega::reference::make( {{ instance }}, {{ concrete_type_id }} );
{{ indent }}}
)TEMPLATE";
                // clang-format on
                std::ostringstream os;
                {
                    Variables::Instance* pInstance = pGet->get_instance();

                    std::ostringstream osIndent;
                    osIndent << indent;

                    nlohmann::json templateData(
                        { { "indent", osIndent.str() },
                          { "instance", get( variables, pInstance ) },
                          { "concrete_type_id", pGet->get_concrete_target()->get_concrete_id() } } );

                    os << render( szTemplate, templateData );
                }

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pGetDimension = db_cast< GetDimension >( pOperation ) )
            {
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    return mega::reference::make( {{ instance }}, {{ concrete_type_id }} );
{{ indent }}}
)TEMPLATE";
                // clang-format on
                std::ostringstream os;
                {
                    Variables::Instance* pInstance = pGetDimension->get_instance();

                    std::ostringstream osIndent;
                    osIndent << indent;

                    nlohmann::json templateData(
                        { { "indent", osIndent.str() },
                          { "instance", get( variables, pInstance ) },
                          { "concrete_type_id", pGetDimension->get_concrete_dimension()->get_concrete_id() } }

                    );

                    os << render( szTemplate, templateData );
                }

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pRead = db_cast< Read >( pOperation ) )
            {
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    // Read
{{ indent }}    if( {{ instance }}.getMPO() != mega::runtime::getThisMPO() )
{{ indent }}    {
{{ indent }}        mega::runtime::readLock( {{ instance }} );
{{ indent }}    }
{{ indent }}    else if( {{ instance }}.isNetworkAddress() )
{{ indent }}    {
{{ indent }}        mega::runtime::networkToHeap( {{ instance }} );
{{ indent }}    }
{{ indent }}    return reinterpret_cast< char* >( {{ instance }}.getHeap() )
{{ indent }}        + {{ part_offset }} + ( {{ part_size }} * {{ instance }}.getInstance() ) 
{{ indent }}        + {{ dimension_offset }};
{{ indent }}}
)TEMPLATE";
                // clang-format on
                std::ostringstream os;
                {
                    Concrete::Dimensions::User* pDimension = pRead->get_concrete_dimension();
                    Variables::Instance*        pInstance  = pRead->get_instance();
                    MemoryLayout::Part*         pPart      = pDimension->get_part();

                    std::ostringstream osIndent;
                    osIndent << indent;

                    nlohmann::json templateData( { { "indent", osIndent.str() },
                                                   { "part_offset", pPart->get_offset() },
                                                   { "part_size", pPart->get_size() },
                                                   { "dimension_offset", pDimension->get_offset() },
                                                   { "instance", get( variables, pInstance ) } } );

                    os << render( szTemplate, templateData );
                }

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pWrite = db_cast< Write >( pOperation ) )
            {
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    // Write
{{ indent }}    if( {{ instance }}.getMPO() != mega::runtime::getThisMPO() )
{{ indent }}    {
{{ indent }}        mega::runtime::writeLock( {{ instance }} );
{{ indent }}    }
{{ indent }}    else if( {{ instance }}.isNetworkAddress() )
{{ indent }}    {
{{ indent }}        mega::runtime::networkToHeap( {{ instance }} );
{{ indent }}    }
{{ indent }}    void* pTarget = 
{{ indent }}        reinterpret_cast< char* >( {{ instance }}.getHeap() )
{{ indent }}        + {{ part_offset }} + ( {{ part_size }} * {{ instance }}.getInstance() ) 
{{ indent }}        + {{ dimension_offset }};
{{ indent }}
{{ indent }}    mega::mangle::copy_{{ mangled_type_name }}( pData, pTarget );
{{ indent }}    mega::mangle::save_record_{{ mangled_type_name }}
{{ indent }}    (
{{ indent }}        mega::runtime::log(),
{{ indent }}        mega::reference
{{ indent }}        ( 
{{ indent }}            mega::TypeInstance
{{ indent }}            ( 
{{ indent }}                {{ instance }}.getInstance(), 
{{ indent }}                {{ concrete_type_id }} 
{{ indent }}            ),
{{ indent }}            {{ instance }}.getMPO(), 
{{ indent }}            {{ instance }}.getObjectID()
{{ indent }}        ),
{{ indent }}        pTarget,
{{ indent }}        {{ log_track_type }}
{{ indent }}    );
{{ indent }}    return {{ instance }};
{{ indent }}}
)TEMPLATE";
                // clang-format on
                std::ostringstream os;
                {
                    Concrete::Dimensions::User* pDimension = pWrite->get_concrete_dimension();
                    Variables::Instance*        pInstance  = pWrite->get_instance();
                    MemoryLayout::Part*         pPart      = pDimension->get_part();
                    const bool                  bSimple    = pDimension->get_interface_dimension()->get_simple();
                    const std::string           strMangled
                        = megaMangle( pDimension->get_interface_dimension()->get_canonical_type() );

                    std::ostringstream osIndent;
                    osIndent << indent;

                    nlohmann::json templateData(
                        { { "indent", osIndent.str() },
                          { "concrete_type_id", pDimension->get_concrete_id() },
                          { "part_offset", pPart->get_offset() },
                          { "part_size", pPart->get_size() },
                          { "dimension_offset", pDimension->get_offset() },
                          { "mangled_type_name", strMangled },
                          { "instance", get( variables, pInstance ) },
                          { "log_track_type", mega::log::toInt( mega::log::TrackType::Simulation ) }

                        } );

                    os << render( szTemplate, templateData );

                    functions.copySet.insert( strMangled );
                    functions.eventSet.insert( strMangled );
                }

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pReadLink = db_cast< ReadLink >( pOperation ) )
            {
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    // ReadLink
{{ indent }}    if( {{ instance }}.getMPO() != mega::runtime::getThisMPO() )
{{ indent }}    {
{{ indent }}        mega::runtime::readLock( {{ instance }} );
{{ indent }}    }
{{ indent }}    else if( {{ instance }}.isNetworkAddress() )
{{ indent }}    {
{{ indent }}        mega::runtime::networkToHeap( {{ instance }} );
{{ indent }}    }
{{ indent }}    return reinterpret_cast< char* >( {{ instance }}.getHeap() )
{{ indent }}        + {{ part_offset }} + ( {{ part_size }} * {{ instance }}.getInstance() ) 
{{ indent }}        + {{ dimension_offset }};
{{ indent }}}
)TEMPLATE";
                // clang-format on
                std::ostringstream os;
                {
                    Concrete::Link*      pLink          = pReadLink->get_concrete_link();
                    auto                 pLinkReference = pLink->get_link_reference();
                    Variables::Instance* pInstance      = pReadLink->get_instance();
                    MemoryLayout::Part*  pPart          = pLinkReference->get_part();

                    std::ostringstream osIndent;
                    osIndent << indent;

                    nlohmann::json templateData( { { "indent", osIndent.str() },
                                                   { "part_offset", pPart->get_offset() },
                                                   { "part_size", pPart->get_size() },
                                                   { "dimension_offset", pLinkReference->get_offset() },
                                                   { "instance", get( variables, pInstance ) } } );

                    os << render( szTemplate, templateData );
                }

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pWriteLink = db_cast< WriteLink >( pOperation ) )
            {
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    switch( overload )
{{ indent }}    {
{{ indent }}        case WriteOperation::DEFAULT:
{{ indent }}        case WriteOperation::INSERT:
{{ indent }}        {
{{ indent }}            static thread_local mega::runtime::relation::LinkMake function( g_pszModuleName, mega::RelationID{ {{ relation_id_lower }}, {{ relation_id_upper }} } );
{{ indent }}            function( {{ instance }}, target );
{{ indent }}        }
{{ indent }}        break;
{{ indent }}        case WriteOperation::REMOVE:
{{ indent }}        {
{{ indent }}            static thread_local mega::runtime::relation::LinkBreak function( g_pszModuleName, mega::RelationID{ {{ relation_id_lower }}, {{ relation_id_upper }} } );
{{ indent }}            function( {{ instance }}, target );
{{ indent }}        }
{{ indent }}        break;
{{ indent }}        case WriteOperation::OVERWRITE:
{{ indent }}        {
{{ indent }}            static thread_local mega::runtime::relation::LinkOverwrite function( g_pszModuleName, mega::RelationID{ {{ relation_id_lower }}, {{ relation_id_upper }} } );
{{ indent }}            function( {{ instance }}, target );
{{ indent }}        }
{{ indent }}        break;
{{ indent }}        case WriteOperation::RESET:
{{ indent }}        {
{{ indent }}            static thread_local mega::runtime::relation::LinkReset function( g_pszModuleName, mega::RelationID{ {{ relation_id_lower }}, {{ relation_id_upper }} } );
{{ indent }}            function( {{ instance }}, target );
{{ indent }}        }
{{ indent }}        break;
{{ indent }}        case WriteOperation::TOTAL_WRITE_OPERATIONS:
{{ indent }}        default:
{{ indent }}            throw mega::runtime::JITException{ "Unrecognised write link overload" };
{{ indent }}        break;
{{ indent }}    }
{{ indent }}}
{{ indent }}return {{ instance }};
)TEMPLATE";
                // clang-format on
                std::ostringstream os;
                {
                    Concrete::Link*      pLink          = pWriteLink->get_concrete_link();
                    auto                 pLinkReference = pLink->get_link_reference();
                    Variables::Instance* pInstance      = pWriteLink->get_instance();
                    RelationID           relationID     = pLink->get_link_interface()->get_relation()->get_id();

                    VERIFY_RTE_MSG( pInstance->get_concrete() == pLink, "Something is wrong!!" );

                    std::ostringstream osIndent;
                    osIndent << indent;

                    nlohmann::json templateData(
                        { { "indent", osIndent.str() },
                          { "relation_id_lower", relationID.getLower() },
                          { "relation_id_upper", relationID.getUpper() },
                          { "instance", get( variables, pInstance ) },
                          { "log_track_type", mega::log::toInt( mega::log::TrackType::Simulation ) }

                        } );

                    os << render( szTemplate, templateData );
                }

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pRange = db_cast< Range >( pOperation ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// Range\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else
            {
                THROW_RTE( "Unknown operation type" );
            }
        }
        else
        {
            THROW_RTE( "Unknown instruction type" );
        }
    }

    VariableMap
    generateVariables( const std::vector< ::FinalStage::Invocations::Variables::Variable* >& invocationVariables,
                       ::FinalStage::Invocations::Variables::Context*                        pRootContext,
                       nlohmann::json&                                                       data,
                       Indent&                                                               indent )
    {
        using namespace FinalStage;
        using namespace FinalStage::Invocations;

        VariableMap variables;

        int iVariableCounter = 0;
        for( auto pVariable : invocationVariables )
        {
            if( auto pInstanceVar = db_cast< Variables::Instance >( pVariable ) )
            {
                std::ostringstream osName;
                {
                    osName << "var_" << pInstanceVar->get_concrete()->get_interface()->get_identifier() << "_"
                           << iVariableCounter++;
                }
                variables.insert( { pVariable, osName.str() } );

                std::ostringstream osVar;
                {
                    osVar << indent << "mega::reference " << osName.str() << ";";
                }
                data[ "variables" ].push_back( osVar.str() );
            }
            else if( auto pDimensionVar = db_cast< Variables::Dimension >( pVariable ) )
            {
                auto               types    = pDimensionVar->get_types();
                Concrete::Context* pContext = types.front();

                std::ostringstream osName;
                {
                    osName << "var_" << iVariableCounter++;
                }
                variables.insert( { pVariable, osName.str() } );

                std::ostringstream osVar;
                {
                    osVar << indent << "mega::reference " << osName.str() << ";";
                }
                data[ "variables" ].push_back( osVar.str() );
            }
            else if( auto pContextVar = db_cast< Variables::Context >( pVariable ) )
            {
                auto               types    = pContextVar->get_types();
                Concrete::Context* pContext = types.front();

                std::ostringstream osName;
                {
                    osName << "var_" << iVariableCounter++;
                }
                variables.insert( { pVariable, osName.str() } );
                std::ostringstream osVar;
                {
                    osVar << indent << "mega::reference " << osName.str() << ";";
                }
                data[ "variables" ].push_back( osVar.str() );

                if( pRootContext == pContextVar )
                {
                    // generate initial assignment
                    std::ostringstream os;
                    os << indent << osName.str() << " = context;";
                    data[ "assignments" ].push_back( os.str() );
                }
            }
            else
            {
                THROW_RTE( "Unknown variable type" );
            }
        }
        return variables;
    }
};

CodeGenerator::CodeGenerator( const mega::network::MegastructureInstallation& megastructureInstallation,
                              const mega::network::Project&                   project )
    : m_pPimpl( std::make_shared< Pimpl >( megastructureInstallation, project ) )
{
}

std::string makeIterName( FinalStage::Concrete::Context* pContext )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;
    std::ostringstream os;
    os << "i_" << pContext->get_interface()->get_identifier() << "_" << pContext->get_concrete_id();
    return os.str();
}

template < typename TDimensionType >
std::string calculateElementOffset( const DatabaseInstance& database, TDimensionType* pUserDim )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;

    MemoryLayout::Part* pPart   = pUserDim->get_part();
    auto                pBuffer = pPart->get_buffer();

    std::vector< Concrete::Context* > parents;
    std::vector< U64 >                sizes;
    {
        Concrete::Context* pContext = pUserDim->get_parent();
        U64                size     = 1;
        while( pContext )
        {
            parents.push_back( pContext );
            sizes.push_back( size );
            size *= database.getLocalDomainSize( pContext->get_concrete_id() );
            if( !!db_cast< Concrete::Object >( pContext ) )
                break;
            pContext = db_cast< Concrete::Context >( pContext->get_parent() );
        }
        std::reverse( parents.begin(), parents.end() );
        std::reverse( sizes.begin(), sizes.end() );
    }

    std::ostringstream offset;
    offset << pPart->get_offset();
    offset << " + " << pPart->get_size() << " * (";
    bool bFirst = true;
    for( U64 i = 0; i != parents.size(); ++i )
    {
        if( bFirst )
            bFirst = false;
        else
            offset << " + ";
        offset << sizes[ i ] << " * " << makeIterName( parents[ i ] ) << " ";
    }

    offset << ") + " << pUserDim->get_offset();

    return offset.str();
}

void generateAllocatorDimensions( const DatabaseInstance& database, FinalStage::Concrete::Dimensions::User* pUserDim,
                                  nlohmann::json& data )
{
    nlohmann::json element(
        { { "concrete_id", pUserDim->get_concrete_id() },
          { "name", pUserDim->get_interface_dimension()->get_id()->get_str() },
          { "type", pUserDim->get_interface_dimension()->get_canonical_type() },
          { "mangle", megaMangle( pUserDim->get_interface_dimension()->get_canonical_type() ) },
          { "offset", calculateElementOffset< FinalStage::Concrete::Dimensions::User >( database, pUserDim ) },
          { "is_part_begin", false },
          { "is_part_end", false },
          { "owning", false }

        } );
    data[ "elements" ].push_back( element );
}

void recurseAllocatorElements( const DatabaseInstance& database, FinalStage::Concrete::Context* pContext,
                               nlohmann::json& data )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;

    if( auto pLink = db_cast< Link >( pContext ) )
    {
        HyperGraph::Relation*      pRelation      = pLink->get_link()->get_relation();
        Interface::LinkInterface*  pLinkInterface = pLink->get_link_interface();
        Dimensions::LinkReference* pLinkRef       = pLink->get_link_reference();

        bool bSource = false;
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

        bool bOwning = false;
        {
            if( bSource )
            {
                if( pRelation->get_ownership().get() == mega::Ownership::eOwnTarget )
                    bOwning = true;
            }
            else
            {
                if( pRelation->get_ownership().get() == mega::Ownership::eOwnSource )
                    bOwning = true;
            }
        }

        std::string strCanonicalType;
        bool        bSingular = true;
        {
            if( auto pRange = db_cast< Dimensions::LinkMany >( pLinkRef ) )
            {
                // range
                strCanonicalType = "mega::ReferenceVector";
                bSingular        = false;
            }
            else if( auto pSingle = db_cast< Dimensions::LinkSingle >( pLinkRef ) )
            {
                // singular
                strCanonicalType = "mega::reference";
                bSingular        = true;
            }
            else
            {
                THROW_RTE( "Unknown link type" );
            }
        }

        nlohmann::json element( { { "concrete_id", pLink->get_concrete_id() },
                                  { "name", pLink->get_link()->get_identifier() },
                                  { "type", strCanonicalType },
                                  { "mangle", megaMangle( strCanonicalType ) },
                                  { "offset", calculateElementOffset< FinalStage::Concrete::Dimensions::LinkReference >(
                                                  database, pLink->get_link_reference() ) },
                                  { "is_part_begin", false },
                                  { "is_part_end", false },
                                  { "singular", bSingular },
                                  { "owning", bOwning },
                                  { "types", nlohmann::json::array() }

        } );

        if( bOwning )
        {
            if( bSource )
            {
                for( auto pTarget : pRelation->get_targets() )
                {
                    for( auto pConcreteLink : pTarget->get_concrete() )
                    {
                        auto pObjectOpt = pConcreteLink->get_concrete_object();
                        VERIFY_RTE( pObjectOpt.has_value() );
                        nlohmann::json type( { { "link_type_id", pConcreteLink->get_concrete_id() },
                                               { "object_type_id", pObjectOpt.value()->get_concrete_id() }

                        } );
                        element[ "types" ].push_back( type );
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
                        nlohmann::json type( { { "link_type_id", pConcreteLink->get_concrete_id() },
                                               { "object_type_id", pObjectOpt.value()->get_concrete_id() }

                        } );
                        element[ "types" ].push_back( type );
                    }
                }
            }
        }

        data[ "elements" ].push_back( element );
    }
    else
    {
        {
            nlohmann::json element( { { "concrete_id", pContext->get_concrete_id() },
                                      { "name", pContext->get_interface()->get_identifier() },
                                      { "iter", makeIterName( pContext ) },
                                      { "start", 0 },
                                      { "end", database.getLocalDomainSize( pContext->get_concrete_id() ) },
                                      { "is_part_begin", true },
                                      { "is_part_end", false },
                                      { "owning", false }

            } );
            data[ "elements" ].push_back( element );
        }

        if( auto pNamespace = db_cast< Namespace >( pContext ) )
        {
            for( auto pUserDim : pNamespace->get_dimensions() )
            {
                generateAllocatorDimensions( database, pUserDim, data );
            }
        }
        else if( auto pAction = db_cast< Action >( pContext ) )
        {
            for( auto pUserDim : pAction->get_dimensions() )
            {
                generateAllocatorDimensions( database, pUserDim, data );
            }
        }
        else if( auto pEvent = db_cast< FinalStage::Concrete::Event >( pContext ) )
        {
            for( auto pUserDim : pEvent->get_dimensions() )
            {
                generateAllocatorDimensions( database, pUserDim, data );
            }
        }
        else if( auto pFunction = db_cast< Function >( pContext ) )
        {
        }
        else if( auto pObject = db_cast< Object >( pContext ) )
        {
            for( auto pUserDim : pObject->get_dimensions() )
            {
                generateAllocatorDimensions( database, pUserDim, data );
            }
        }
        else if( auto pBuffer = db_cast< Buffer >( pContext ) )
        {
        }
        else
        {
            THROW_RTE( "Unknown context type" );
        }

        for( auto pChildContext : pContext->get_children() )
        {
            recurseAllocatorElements( database, pChildContext, data );
        }

        {
            nlohmann::json element( { { "concrete_id", pContext->get_concrete_id() },
                                      { "name", pContext->get_interface()->get_identifier() },
                                      { "is_part_begin", false },
                                      { "is_part_end", true },
                                      { "owning", false }

            } );
            data[ "elements" ].push_back( element );
        }
    }
}

void CodeGenerator::generate_alllocator( const LLVMCompiler& compiler, const DatabaseInstance& database,
                                         mega::TypeID objectTypeID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_alllocator: {}", objectTypeID );

    FinalStage::Concrete::Object*            pObject    = database.getObject( objectTypeID );
    const FinalStage::Components::Component* pComponent = pObject->get_component();

    std::ostringstream osFullTypeName;
    {
        bool bFirst = true;
        for( FinalStage::Interface::IContext* pIContext = pObject->get_interface_object(); pIContext;
             pIContext = db_cast< FinalStage::Interface::IContext >( pIContext->get_parent() ) )
        {
            if( bFirst )
                bFirst = false;
            else
                osFullTypeName << '_';
            osFullTypeName << pIContext->get_identifier();
        }
    }

    std::ostringstream osObjectTypeID;
    osObjectTypeID << objectTypeID;
    nlohmann::json data( { { "objectTypeID", osObjectTypeID.str() },
                           { "objectName", osFullTypeName.str() },
                           { "parts", nlohmann::json::array() },
                           { "mangled_data_types", nlohmann::json::array() },
                           { "elements", nlohmann::json::array() } } );

    recurseAllocatorElements( database, pObject, data );

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
                    const std::string strMangle
                        = megaMangle( pUserDim->get_interface_dimension()->get_canonical_type() );
                    nlohmann::json member( { { "type", pUserDim->get_interface_dimension()->get_canonical_type() },
                                             { "type_id", pUserDim->get_concrete_id() },
                                             { "mangle", strMangle },
                                             { "name", pUserDim->get_interface_dimension()->get_id()->get_str() },
                                             { "offset", pUserDim->get_offset() } } );
                    part[ "members" ].push_back( member );
                    mangledDataTypes.insert( strMangle );
                }

                for( auto pLinkDim : pPart->get_link_dimensions() )
                {
                    Concrete::Link*           pConcreteLink  = pLinkDim->get_link();
                    Interface::LinkInterface* pLinkInterface = pConcreteLink->get_link_interface();
                    HyperGraph::Relation*     pRelation      = pLinkInterface->get_relation();

                    bool bSource = false;
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

                    bool bOwning = false;
                    {
                        if( bSource )
                        {
                            if( pRelation->get_ownership().get() == mega::Ownership::eOwnTarget )
                                bOwning = true;
                        }
                        else
                        {
                            if( pRelation->get_ownership().get() == mega::Ownership::eOwnSource )
                                bOwning = true;
                        }
                    }

                    std::string    strMangle;
                    nlohmann::json link( { { "type", "mega::ReferenceVector" },
                                           { "type_id", pLinkDim->get_concrete_id() },
                                           { "mangle", "" },
                                           { "name", pLinkDim->get_link()->get_link()->get_identifier() },
                                           { "offset", pLinkDim->get_offset() },
                                           { "singular", false },
                                           { "types", nlohmann::json::array() },
                                           { "owning", bOwning }

                    } );

                    if( auto pLinkMany = db_cast< Concrete::Dimensions::LinkMany >( pLinkDim ) )
                    {
                        strMangle          = megaMangle( "mega::ReferenceVector" );
                        link[ "singular" ] = false;
                    }
                    else if( auto pLinkSingle = db_cast< Concrete::Dimensions::LinkSingle >( pLinkDim ) )
                    {
                        strMangle          = megaMangle( "mega::reference" );
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
                                nlohmann::json type( { { "link_type_id", pConcreteLink->get_concrete_id() },
                                                       { "object_type_id", pObjectOpt.value()->get_concrete_id() }

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
                                nlohmann::json type( { { "link_type_id", pConcreteLink->get_concrete_id() },
                                                       { "object_type_id", pObjectOpt.value()->get_concrete_id() }

                                } );
                                link[ "types" ].push_back( type );
                            }
                        }
                    }

                    link[ "mangle" ] = strMangle;
                    part[ "links" ].push_back( link );
                    mangledDataTypes.insert( strMangle );
                }
                for( auto pAllocDim : pPart->get_allocation_dimensions() )
                {
                    if( auto pAllocator = db_cast< Concrete::Dimensions::Allocator >( pAllocDim ) )
                    {
                        const std::string strAllocatorTypeName = allocatorTypeName( database, pAllocator );
                        const std::string strMangle            = megaMangle( strAllocatorTypeName );

                        std::ostringstream osAllocName;
                        osAllocName << "alloc_"
                                    << pAllocator->get_allocator()->get_allocated_context()->get_concrete_id();

                        mangledDataTypes.insert( strMangle );

                        nlohmann::json member( { { "type", strAllocatorTypeName },
                                                 { "type_id", pAllocDim->get_concrete_id() },
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
    m_pPimpl->render_allocator( data, osCPPCode );
    compiler.compileToLLVMIR( osObjectTypeID.str(), osCPPCode.str(), os, pComponent );
}

nlohmann::json CodeGenerator::generate( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                        std::string& strName ) const
{
    FunctionPointers functions;

    nlohmann::json data = functions.init( invocationID, strName );

    try
    {
        using namespace FinalStage;
        using namespace FinalStage::Invocations;

        const FinalStage::Operations::Invocation* pInvocation = database.getInvocation( invocationID );

        Indent indent;

        const VariableMap variables = m_pPimpl->generateVariables(
            pInvocation->get_variables(), pInvocation->get_root_instruction()->get_context(), data, indent );

        for( auto pInstruction : pInvocation->get_root_instruction()->get_children() )
        {
            m_pPimpl->generateInstructions( database, pInstruction, variables, functions, data, indent );
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
                m_pPimpl->render_read( data, osCPPCode );
                break;
            case mega::runtime::invocation::eWrite:
                m_pPimpl->render_write( data, osCPPCode );
                break;
            case mega::runtime::invocation::eAllocate:
                m_pPimpl->render_allocate( data, osCPPCode );
                break;
            case mega::runtime::invocation::eCall:
                m_pPimpl->render_call( data, osCPPCode );
                break;
            case mega::runtime::invocation::eReadLink:
                m_pPimpl->render_readLink( data, osCPPCode );
                break;
            case mega::runtime::invocation::eWriteLink:
                m_pPimpl->render_writeLink( data, osCPPCode );
                break;
            case mega::runtime::invocation::eGet:
                m_pPimpl->render_get( data, osCPPCode );
                break;
            case mega::runtime::invocation::eSave:
                m_pPimpl->render_save( data, osCPPCode );
                break;
            case mega::runtime::invocation::eLoad:
                m_pPimpl->render_load( data, osCPPCode );
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
        m_pPimpl->render_relation( data, osCPPCode );
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
        const std::string   strMangled = megaMangle( pUserDimension->get_interface_dimension()->get_canonical_type() );

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
        m_pPimpl->render_program( data, osCPPCode );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate_program: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate_program: " << ex.what() );
    }
    compiler.compileToLLVMIR( "program_module", osCPPCode.str(), os, std::nullopt );
}

} // namespace mega::runtime
