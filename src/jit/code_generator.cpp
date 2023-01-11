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
#include "service/network/log.hpp"

#include "service/mpo_context.hpp"

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

static const std::string indent( "    " );

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
    using CopySet        = std::set< const FinalStage::Interface::DimensionTrait* >;
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
        for( auto pCopy : copySet )
        {
            data[ "copiers" ].push_back( megaMangle( pCopy->get_canonical_type() ) );
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
    ::inja::Template    m_allocationTemplate;
    ::inja::Template    m_allocateTemplate;
    ::inja::Template    m_readTemplate;
    ::inja::Template    m_writeTemplate;
    ::inja::Template    m_callTemplate;
    ::inja::Template    m_startTemplate;
    ::inja::Template    m_stopTemplate;
    ::inja::Template    m_saveTemplate;
    ::inja::Template    m_loadTemplate;
    ::inja::Template    m_programTemplate;

public:
    Pimpl( const mega::network::MegastructureInstallation& megaInstall, const mega::network::Project& project )
    {
        m_injaEnvironment.set_trim_blocks( true );
        m_allocationTemplate = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateAllocation().string() );
        m_allocateTemplate   = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateAllocate().string() );
        m_readTemplate       = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateRead().string() );
        m_writeTemplate      = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateWrite().string() );
        m_callTemplate       = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateCall().string() );
        m_startTemplate      = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateStart().string() );
        m_stopTemplate       = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateStop().string() );
        m_saveTemplate       = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateSave().string() );
        m_loadTemplate       = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateLoad().string() );
        m_programTemplate    = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateProgram().string() );
    }

    void render_allocation( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_allocationTemplate, data );
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
                               const VariableMap& variables, FunctionPointers& functions, nlohmann::json& data )
    {
        using namespace FinalStage;
        using namespace FinalStage::Invocations;

        if( auto pInstructionGroup = db_cast< Instructions::InstructionGroup >( pInstruction ) )
        {
            if( auto pParentDerivation = db_cast< Instructions::ParentDerivation >( pInstructionGroup ) )
            {
                std::ostringstream os;
                os << indent << "// ParentDerivation\n";

                const Variables::Instance* pFrom = pParentDerivation->get_from();
                const Variables::Instance* pTo   = pParentDerivation->get_to();

                const std::string  s           = get( variables, pFrom );
                const mega::TypeID targetType  = pFrom->get_concrete()->get_concrete_id();
                const mega::U64    szLocalSize = database.getLocalDomainSize( targetType );

                if( szLocalSize > 1 )
                {
                    os << indent << get( variables, pTo ) << " = mega::reference{ mega::TypeInstance{ " << s
                       << ".instance / " << szLocalSize << ", " << targetType << " }, " << s << ", " << s
                       << ".pointer };";
                }
                else
                {
                    os << indent << get( variables, pTo ) << " = mega::reference{ mega::TypeInstance{ " << s
                       << ".instance, " << targetType << " }, " << s << ", " << s << ".pointer };";
                }

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pChildDerivation = db_cast< Instructions::ChildDerivation >( pInstructionGroup ) )
            {
                std::ostringstream os;
                os << indent << "// ChildDerivation\n";

                const Variables::Instance* pFrom = pParentDerivation->get_from();
                const Variables::Instance* pTo   = pParentDerivation->get_to();

                const std::string  s           = get( variables, pFrom );
                const mega::TypeID targetType  = pFrom->get_concrete()->get_concrete_id();
                const mega::U64    szLocalSize = database.getLocalDomainSize( targetType );

                os << indent << get( variables, pTo ) << " = mega::reference{ mega::TypeInstance{ " << s
                   << ".instance, " << targetType << " }, " << s << ", " << s << ".process };";

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
                os << indent << get( variables, pInstance ) << " = " << get( variables, pReference ) << ";";

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pPolyReference = db_cast< Instructions::PolyReference >( pInstructionGroup ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// PolyReference\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pPolyCase = db_cast< Instructions::PolyCase >( pInstructionGroup ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// PolyCase\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else
            {
                THROW_RTE( "Unknown instruction type" );
            }

            for( auto pChildInstruction : pInstructionGroup->get_children() )
            {
                generateInstructions( database, pChildInstruction, variables, functions, data );
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
    {
        return mega::runtime::allocate( {{ instance }}, {{ concrete_type_id }} );
    }
)TEMPLATE";
                // clang-format on
                std::ostringstream os;
                {
                    Variables::Instance* pInstance       = pAllocate->get_instance();
                    Concrete::Context*   pConcreteTarget = pAllocate->get_concrete_target();

                    nlohmann::json templateData( {
                        { "concrete_type_id", pConcreteTarget->get_concrete_id() },
                        { "instance", get( variables, pInstance ) },
                    } );

                    os << render( szTemplate, templateData );
                }

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pCall = db_cast< Call >( pOperation ) )
            {
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
    {
        static thread_local mega::runtime::object::CallGetter function( g_pszModuleName, {{ concrete_type_id }} );
        return mega::runtime::CallResult{ function(), {{ instance }} };
    }
)TEMPLATE";
                // clang-format on
                std::ostringstream os;
                {
                    Concrete::Context*   pConcreteTarget = pCall->get_concrete_target();
                    Variables::Instance* pInstance       = pCall->get_instance();

                    nlohmann::json templateData( { { "concrete_type_id", pConcreteTarget->get_concrete_id() },
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
                THROW_TODO;
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
    {
        while( _fptr_save_xml_object_{{ concrete_type_id }} == nullptr )
        {
            mega::runtime::get_save_xml_object( g_pszModuleName, {{ concrete_type_id }},
                &_fptr_save_xml_object_{{ concrete_type_id }} );
        }
        _fptr_save_xml_object_{{ concrete_type_id }}( {{ instance }}, pArchive, true );
    }
)TEMPLATE";
                // clang-format on
                std::ostringstream os;
                {
                    Concrete::Context*   pConcreteTarget = pSave->get_concrete_target();
                    Variables::Instance* pInstance       = pSave->get_instance();

                    nlohmann::json templateData( { { "concrete_type_id", pConcreteTarget->get_concrete_id() },
                                                   { "instance", get( variables, pInstance ) } } );

                    os << render( szTemplate, templateData );

                    functions.objectSave.insert( pConcreteTarget );
                }

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pLoad = db_cast< Load >( pOperation ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// Load\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pFiles = db_cast< Files >( pOperation ) )
            {
                THROW_TODO;
            }
            else if( auto pGetAction = db_cast< GetAction >( pOperation ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// GetAction\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pGetDimension = db_cast< GetDimension >( pOperation ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// GetDimension\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pRead = db_cast< Read >( pOperation ) )
            {
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
    {
        if( {{ instance }}.getMPO() != mega::runtime::getThisMPO() )
        {
            mega::runtime::readLock( {{ instance }} );
        }
        else if( {{ instance }}.isNetworkAddress() )
        {
            mega::runtime::networkToHeap( {{ instance }} );
        }
        return reinterpret_cast< char* >( {{ instance }}.getHeap() )
            + {{ part_offset }} + ( {{ part_size }} * {{ instance }}.getInstance() ) 
            + {{ dimension_offset }};
    }
)TEMPLATE";
                // clang-format on
                std::ostringstream os;
                {
                    Concrete::Dimensions::User* pDimension = pRead->get_concrete_dimension();
                    Variables::Instance*        pInstance  = pRead->get_instance();
                    MemoryLayout::Part*         pPart      = pDimension->get_part();

                    nlohmann::json templateData( { { "part_offset", pPart->get_offset() },
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
    {
        if( {{ instance }}.getMPO() != mega::runtime::getThisMPO() )
        {
            mega::runtime::writeLock( {{ instance }} );
        }
        else if( {{ instance }}.isNetworkAddress() )
        {
            mega::runtime::networkToHeap( {{ instance }} );
        }
        void* pTarget = 
            reinterpret_cast< char* >( {{ instance }}.getHeap() )
            + {{ part_offset }} + ( {{ part_size }} * {{ instance }}.getInstance() ) 
            + {{ dimension_offset }};

        mega::copy_{{ mangled_type_name }}( pData, pTarget );
        mega::event_{{ mangled_type_name }}
        (
            mega::reference
            ( 
                mega::TypeInstance
                ( 
                    {{ instance }}.getInstance(), 
                    {{ concrete_type_id }} 
                ),
                {{ instance }}.getMPO(), 
                {{ instance }}.getObjectID()
            ),
            pTarget
        );
        return {{ instance }};
    }
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

                    nlohmann::json templateData( { { "concrete_type_id", pDimension->get_concrete_id() },
                                                   { "part_offset", pPart->get_offset() },
                                                   { "part_size", pPart->get_size() },
                                                   { "dimension_offset", pDimension->get_offset() },
                                                   { "mangled_type_name", strMangled },
                                                   { "instance", get( variables, pInstance ) }

                    } );

                    os << render( szTemplate, templateData );

                    functions.copySet.insert( pDimension->get_interface_dimension() );
                    functions.eventSet.insert( strMangled );
                }

                data[ "assignments" ].push_back( os.str() );
            }
            else if( auto pWriteLink = db_cast< WriteLink >( pOperation ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// WriteLink\n";
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
                       nlohmann::json&                                                       data )
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
                auto types = pDimensionVar->get_types();
                VERIFY_RTE_MSG( types.size() == 1U, "Multiple typed contexts not implemented!" );
                Concrete::Context* pContext = types.front();

                std::ostringstream osName;
                {
                    osName << "var_" << pContext->get_interface()->get_identifier() << "_" << iVariableCounter++;
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
                auto types = pContextVar->get_types();
                VERIFY_RTE_MSG( types.size() == 1U, "Multiple typed contexts not implemented!" );
                Concrete::Context* pContext = types.front();

                std::ostringstream osName;
                {
                    osName << "var_" << pContext->get_interface()->get_identifier() << "_" << iVariableCounter++;
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

void generateAllocatorDimensions( const DatabaseInstance& database, FinalStage::Concrete::Dimensions::User* pUserDim,
                                  nlohmann::json& data )
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

    nlohmann::json element(
        { { "concrete_id", pUserDim->get_concrete_id() },
          { "name", pUserDim->get_interface_dimension()->get_id()->get_str() },
          { "type", pUserDim->get_interface_dimension()->get_canonical_type() },
          { "mangle", megaMangle( pUserDim->get_interface_dimension()->get_canonical_type() ) },
          { "is_shared", !!db_cast< MemoryLayout::SimpleBuffer >( pUserDim->get_part()->get_buffer() ) },
          { "offset", offset.str() },
          { "is_part_begin", false },
          { "is_part_end", false }

        } );
    data[ "elements" ].push_back( element );
}

void recurseAllocatorElements( const DatabaseInstance& database, FinalStage::Concrete::Context* pContext,
                               nlohmann::json& data )
{
    using namespace FinalStage;
    using namespace FinalStage::Concrete;

    {
        nlohmann::json element( { { "concrete_id", pContext->get_concrete_id() },
                                  { "name", pContext->get_interface()->get_identifier() },
                                  { "iter", makeIterName( pContext ) },
                                  { "start", 0 },
                                  { "end", database.getLocalDomainSize( pContext->get_concrete_id() ) },
                                  { "is_part_begin", true },
                                  { "is_part_end", false }

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
    else if( auto pLink = db_cast< Link >( pContext ) )
    {
        HyperGraph::Relation*      pRelation = pLink->get_link()->get_relation();
        Dimensions::LinkReference* pLinkRef  = pLink->get_link_reference();
        if( auto pRange = db_cast< Dimensions::LinkMany >( pLinkRef ) )
        {
            // range
        }
        else if( auto pSingle = db_cast< Dimensions::LinkSingle >( pLinkRef ) )
        {
            // singular
        }
        else
        {
            THROW_RTE( "Unknown link type" );
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
                                  { "is_part_end", true }

        } );
        data[ "elements" ].push_back( element );
    }
}

void CodeGenerator::generate_allocation( const LLVMCompiler& compiler, const DatabaseInstance& database,
                                         mega::TypeID objectTypeID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_allocation: {}", objectTypeID );

    FinalStage::Concrete::Object*            pObject    = database.getObject( objectTypeID );
    const FinalStage::Components::Component* pComponent = pObject->get_component();

    std::ostringstream osObjectTypeID;
    osObjectTypeID << objectTypeID;
    nlohmann::json data( { { "objectTypeID", osObjectTypeID.str() },
                           { "simple_parts", nlohmann::json::array() },
                           { "parts", nlohmann::json::array() },
                           { "has_non_simple_parts", false },
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
                                       { "members", nlohmann::json::array() } } );

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
                    if( auto pLinkMany = db_cast< Concrete::Dimensions::LinkMany >( pLinkDim ) )
                    {
                        const std::string  strMangle = megaMangle( "mega::ReferenceVector" );
                        std::ostringstream osLinkName;
                        osLinkName << "link_" << pLinkMany->get_link()->get_concrete_id();
                        nlohmann::json member( { { "type", "mega::ReferenceVector" },
                                                 { "type_id", pLinkMany->get_concrete_id() },
                                                 { "mangle", strMangle },
                                                 { "name", osLinkName.str() },
                                                 { "offset", pLinkMany->get_offset() } } );
                        part[ "members" ].push_back( member );
                        mangledDataTypes.insert( strMangle );
                    }
                    else if( auto pLinkSingle = db_cast< Concrete::Dimensions::LinkSingle >( pLinkDim ) )
                    {
                        const std::string  strMangle = megaMangle( "mega::reference" );
                        std::ostringstream osLinkName;
                        osLinkName << "link_" << pLinkSingle->get_link()->get_concrete_id();
                        nlohmann::json member( { { "type", "mega::reference" },
                                                 { "type_id", pLinkSingle->get_concrete_id() },
                                                 { "mangle", strMangle },
                                                 { "name", osLinkName.str() },
                                                 { "offset", pLinkSingle->get_offset() } } );
                        part[ "members" ].push_back( member );
                        mangledDataTypes.insert( strMangle );
                    }
                    else
                    {
                        THROW_RTE( "Unknown link type" );
                    }
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
                if( bBufferIsSimple )
                {
                    data[ "simple_parts" ].push_back( part );
                }
                else
                {
                    data[ "has_non_simple_parts" ] = true;
                }
            }
        }
    }

    for( const auto& str : mangledDataTypes )
    {
        data[ "mangled_data_types" ].push_back( str );
    }

    std::ostringstream osCPPCode;
    m_pPimpl->render_allocation( data, osCPPCode );
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

        const VariableMap variables = m_pPimpl->generateVariables(
            pInvocation->get_variables(), pInvocation->get_root_instruction()->get_context(), data );

        for( auto pInstruction : pInvocation->get_root_instruction()->get_children() )
        {
            m_pPimpl->generateInstructions( database, pInstruction, variables, functions, data );
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

void CodeGenerator::generate_allocate( const LLVMCompiler& compiler, const DatabaseInstance& database,
                                       const mega::InvocationID& invocationID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_allocate: {}", invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    try
    {
        m_pPimpl->render_allocate( data, osCPPCode );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate_allocate: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate_allocate: " << ex.what() );
    }
    compiler.compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}

void CodeGenerator::generate_read( const LLVMCompiler& compiler, const DatabaseInstance& database,
                                   const mega::InvocationID& invocationID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_read: {}", invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    try
    {
        m_pPimpl->render_read( data, osCPPCode );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate_read: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate_read: " << ex.what() );
    }
    compiler.compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}

void CodeGenerator::generate_write( const LLVMCompiler& compiler, const DatabaseInstance& database,
                                    const mega::InvocationID& invocationID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_write: {}", invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    try
    {
        m_pPimpl->render_write( data, osCPPCode );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate_write: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate_write: " << ex.what() );
    }
    compiler.compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}

void CodeGenerator::generate_call( const LLVMCompiler& compiler, const DatabaseInstance& database,
                                   const mega::InvocationID& invocationID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_call: {}", invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    try
    {
        m_pPimpl->render_call( data, osCPPCode );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate_call: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate_call: " << ex.what() );
    }
    compiler.compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}
/*
void CodeGenerator::generate_start( const LLVMCompiler& compiler, const DatabaseInstance& database,
                                    const mega::InvocationID& invocationID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_start: {}", invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    try
    {
        m_pPimpl->render_start( data, osCPPCode );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate_start: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate_start: " << ex.what() );
    }
    compiler.compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}
void CodeGenerator::generate_stop( const LLVMCompiler& compiler, const DatabaseInstance& database,
                                   const mega::InvocationID& invocationID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_stop: {}", invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    try
    {
        m_pPimpl->render_stop( data, osCPPCode );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate_stop : {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate_stop: " << ex.what() );
    }
    compiler.compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}
void CodeGenerator::generate_save( const LLVMCompiler& compiler, const DatabaseInstance& database,
                                   const mega::InvocationID& invocationID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_save: {}", invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    try
    {
        m_pPimpl->render_save( data, osCPPCode );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate_save: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate_save: " << ex.what() );
    }
    compiler.compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}
void CodeGenerator::generate_load( const LLVMCompiler& compiler, const DatabaseInstance& database,
                                   const mega::InvocationID& invocationID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_load: {}", invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    try
    {
        m_pPimpl->render_load( data, osCPPCode );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate_load: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate_load: " << ex.what() );
    }
    compiler.compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}*/

void CodeGenerator::generate_program( const LLVMCompiler& compiler, const DatabaseInstance& database, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_program" );

    nlohmann::json data( { { "types", nlohmann::json::array() } } );
    {
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
