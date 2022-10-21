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

#include "common/file.hpp"
#include "common/stash.hpp"

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

    CallSet        callSet;
    CopySet        copySet;
    EventSet       eventSet;
    AllocatorSet   allocatorSet;
    FreerSet       freerSet;
    ObjectAllocSet objectAllocSet;
    ObjectFreeSet  objectFreeSet;
};

std::string allocatorTypeName( const DatabaseInstance&                      database,
                               FinalStage::Concrete::Dimensions::Allocator* pAllocDim )
{
    using namespace FinalStage;

    std::ostringstream     osTypeName;
    Allocators::Allocator* pAllocator = pAllocDim->get_allocator();
    if ( auto pAlloc = db_cast< Allocators::Nothing >( pAllocator ) )
    {
        // do nothing
        THROW_RTE( "Unreachable" );
    }
    else if ( auto pAlloc = db_cast< Allocators::Singleton >( pAllocator ) )
    {
        osTypeName << "bool";
    }
    else if ( auto pAlloc = db_cast< Allocators::Range32 >( pAllocator ) )
    {
        osTypeName << "mega::Bitmask32Allocator< "
                   << database.getLocalDomainSize( pAlloc->get_allocated_context()->get_concrete_id() ) << " >";
    }
    else if ( auto pAlloc = db_cast< Allocators::Range64 >( pAllocator ) )
    {
        osTypeName << "mega::Bitmask64Allocator< "
                   << database.getLocalDomainSize( pAlloc->get_allocated_context()->get_concrete_id() ) << " >";
    }
    else if ( auto pAlloc = db_cast< Allocators::RangeAny >( pAllocator ) )
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

void runCompilation( const std::string& strCmd )
{
    SPDLOG_DEBUG( "Compiling: {}", strCmd );

    std::ostringstream osOutput, osError;
    int                iCompilationResult = 0;
    {
        namespace bp = boost::process;

        bp::ipstream errStream, outStream; // reading pipe-stream
        bp::child    c( strCmd, bp::std_out > outStream, bp::std_err > errStream );

        std::string strOutputLine;
        while ( c.running() && std::getline( outStream, strOutputLine ) )
        {
            if ( !strOutputLine.empty() )
            {
                osOutput << "\nOUT    : " << strOutputLine;
            }
        }

        c.wait();
        iCompilationResult = c.exit_code();
        if ( iCompilationResult )
        {
            osError << common::COLOUR_RED_BEGIN << "FAILED : "; // << m_sourcePath.string();
            while ( errStream && std::getline( errStream, strOutputLine ) )
            {
                if ( !strOutputLine.empty() )
                {
                    osError << "\nERROR  : " << strOutputLine;
                }
            }
            osError << common::COLOUR_END;
        }
    }
    if ( iCompilationResult != 0 )
    {
        SPDLOG_ERROR( "Error compilation invocation: {}", osError.str() );
    }
    VERIFY_RTE_MSG( iCompilationResult == 0, "Error compilation invocation: " << osError.str() );
}

void compile( const boost::filesystem::path& clangPath, const boost::filesystem::path& inputCPPFilePath,
              const boost::filesystem::path&                            outputIRFilePath,
              std::optional< const FinalStage::Components::Component* > pComponent,
              const mega::network::MegastructureInstallation&           megastructureInstallation )
{
    auto startTime = std::chrono::steady_clock::now();
    {
        std::ostringstream osCmd;

        osCmd << clangPath << " -S -emit-llvm ";

        if ( pComponent.has_value() )
        {
            // flags
            for ( const std::string& flag : pComponent.value()->get_cpp_flags() )
            {
                VERIFY_RTE( !flag.empty() );
                osCmd << "-" << flag << " ";
            }

            // defines
            for ( const std::string& strDefine : pComponent.value()->get_cpp_defines() )
            {
                VERIFY_RTE( !strDefine.empty() );
                osCmd << "-D" << strDefine << " ";
            }

            // include directories
            for ( const boost::filesystem::path& includeDir : pComponent.value()->get_include_directories() )
            {
                osCmd << "-I " << includeDir.native() << " ";
            }
        }
        else
        {
            // add megastructure include directory
            osCmd << "-I " << megastructureInstallation.getMegaIncludePath() << " ";
        }

        osCmd << "-o " << outputIRFilePath.native() << " -c " << inputCPPFilePath.native();
        runCompilation( osCmd.str() );
    }
    const auto timeDelta = std::chrono::steady_clock::now() - startTime;
    SPDLOG_TRACE( "RUNTIME: Clang Compilation time: {}", timeDelta );
}

} // namespace

class CodeGenerator::Pimpl
{
    const mega::network::MegastructureInstallation m_megastructureInstallation;
    ::inja::Environment                            m_injaEnvironment;
    ::inja::Template                               m_allocationTemplate;
    ::inja::Template                               m_allocateTemplate;
    ::inja::Template                               m_readTemplate;
    ::inja::Template                               m_writeTemplate;
    ::inja::Template                               m_callTemplate;
    ::inja::Template                               m_startTemplate;
    ::inja::Template                               m_stopTemplate;
    boost::filesystem::path                        m_clangPath;
    boost::filesystem::path                        m_tempDir;

public:
    Pimpl( const mega::network::MegastructureInstallation& megastructureInstallation,
           const mega::network::Project&                   project )
        : m_megastructureInstallation( megastructureInstallation )
        , m_tempDir( project.getProjectTempDir() )
        , m_clangPath( megastructureInstallation.getClangPath() )
    {
        if ( !boost::filesystem::exists( m_tempDir ) )
        {
            boost::filesystem::create_directories( m_tempDir );
        }

        m_injaEnvironment.set_trim_blocks( true );

        m_allocationTemplate
            = m_injaEnvironment.parse_template( m_megastructureInstallation.getRuntimeTemplateAllocation().native() );
        m_allocateTemplate
            = m_injaEnvironment.parse_template( m_megastructureInstallation.getRuntimeTemplateAllocate().native() );
        m_readTemplate
            = m_injaEnvironment.parse_template( m_megastructureInstallation.getRuntimeTemplateRead().native() );
        m_writeTemplate
            = m_injaEnvironment.parse_template( m_megastructureInstallation.getRuntimeTemplateWrite().native() );
        m_callTemplate
            = m_injaEnvironment.parse_template( m_megastructureInstallation.getRuntimeTemplateCall().native() );
        m_startTemplate
            = m_injaEnvironment.parse_template( m_megastructureInstallation.getRuntimeTemplateStart().native() );
        m_stopTemplate
            = m_injaEnvironment.parse_template( m_megastructureInstallation.getRuntimeTemplateStop().native() );
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

    void compileToLLVMIR( const std::string& strName, const std::string& strCPPCode, std::ostream& osIR,
                          std::optional< const FinalStage::Components::Component* > pComponent )
    {
        MPOContext* pMPOContext = getMPOContext();
        VERIFY_RTE( pMPOContext );

        const boost::filesystem::path irFilePath = m_tempDir / ( strName + ".ir" );

        const task::DeterminantHash determinant{ strCPPCode };
        if ( pMPOContext->restore( irFilePath.native(), determinant.get() ) )
        {
            boost::filesystem::loadAsciiFile( irFilePath, osIR );
        }
        else
        {
            boost::filesystem::path inputCPPFilePath = m_tempDir / ( strName + ".cpp" );
            {
                auto pFStream = boost::filesystem::createNewFileStream( inputCPPFilePath );
                *pFStream << strCPPCode;
            }
            compile( m_clangPath, inputCPPFilePath, irFilePath, pComponent, m_megastructureInstallation );
            pMPOContext->stash( irFilePath.native(), determinant.get() );
            boost::filesystem::loadAsciiFile( irFilePath, osIR );
        }
    }

    std::string render( const std::string& strTemplate, const nlohmann::json& data )
    {
        return m_injaEnvironment.render( strTemplate, data );
    }
    /*
        void generateBufferFPtrCheck( bool bShared, mega::TypeID contextID, std::ostream& os )
        {
            const std::string strType = bShared ? "shared" : "heap";
            os << indent << "if( _fptr_get_" << strType << "_" << contextID << " == nullptr ) "
               << "mega::runtime::get_getter_" << strType << "( g_pszModuleName, " << contextID << ", &_fptr_get_"
               << strType << "_" << contextID << " );\n";
        }
    */
    /*
        void generateReferenceCheck( bool bShared, mega::TypeID contextID, const std::string& strInstanceVar,
                                     std::ostream& os )
        {
            os << indent << "if( " << strInstanceVar << ".isNetwork() ) " << strInstanceVar
               << " = mega::runtime::networkToMachine( " << contextID << ", " << strInstanceVar << ".network );\n";
        }
    */
    /*
        void generateUserDimRead( bool bShared, mega::TypeID contextID, FinalStage::MemoryLayout::Part* pPart,
                                  const std::string&                            strInstanceVar,
                                  const FinalStage::Concrete::Dimensions::User* pDimension, std::ostream& os )
        {
            const std::string strType = bShared ? "shared" : "heap";
            os << indent << "return reinterpret_cast< char* >(_fptr_get_" << strType << "_" << contextID << "( "
               << strInstanceVar << " ) )"
               << " + " << pPart->get_offset() << " + ( " << pPart->get_size() << " * " << strInstanceVar
               << ".instance ) + " << pDimension->get_offset() << ";\n";
        }
    */
    /*
        void generateAllocate( const std::string& strAllocatorMangledType, bool bShared, mega::TypeID contextID,
                               FinalStage::MemoryLayout::Part* pPart, const std::string& strInstanceVar,
                               const FinalStage::Concrete::Dimensions::Allocator* pAllocator, std::ostream& os )
        {
            // clang-format off
    static const char* szTemplate =
    R"TEMPLATE(
        {
            char* pAllocator
                = reinterpret_cast< char* >(
                    _fptr_get_{{ memory_type }}_{{ context_type_id }}( {{ instance_var }} ) ) +
                        {{ part_offset }} +
                        ( {{ part_size }} * {{ instance_var }}.instance ) +
                        {{ allocator_offset }};

            mega::Instance newInstance = mega::allocate_{{ allocator_type }}( pAllocator );

            mega::reference allocated( mega::TypeInstance( newInstance, 1 ), {{ instance_var }} );

            // generate memory record
            mega::event_{{ allocator_type }}( mega::reference( mega::TypeInstance( {{ instance_var }}.instance,
                {{ allocator_type_id }} ), var_Root_1 ), false, pAllocator );

            // generate scheduling record
            mega::schedule_start( allocated );

            return allocated;
        }
    )TEMPLATE";

            nlohmann::json data(
              { { "memory_type",        bShared ? "shared" : "heap" },
                { "context_type_id",    contextID },
                { "instance_var",       strInstanceVar },
                { "part_offset",        pPart->get_offset() },
                { "part_size",          pPart->get_size() },
                { "allocator_type",     strAllocatorMangledType },
                { "allocator_offset",   pAllocator->get_offset() },
                { "allocator_type_id",  pAllocator->get_concrete_id() } } );

            // clang-format on
            os << render( szTemplate, data );
        }
    */
    /*
        void generateFreeDimRead( const std::string& strFreer, const std::string& strFreeInstanceVar, bool bShared,
                                  mega::TypeID contextID, FinalStage::MemoryLayout::Part* pPart,
                                  const std::string&                                 strInstanceVar,
                                  const FinalStage::Concrete::Dimensions::Allocator* pAllocator, std::ostream& os )
        {
            const std::string strType = bShared ? "shared" : "heap";
            os << indent << "mega::free_" << strFreer << "( reinterpret_cast< char* >(_fptr_get_" << strType << "_"
               << contextID << "( " << strInstanceVar << " ) )"
               << " + " << pPart->get_offset() << " + ( " << pPart->get_size() << " * " << strInstanceVar
               << ".instance ) + " << pAllocator->get_offset() << ", " << strFreeInstanceVar << ") ;\n";
        }
    */
    /*
        void generateBufferWrite( bool bShared, mega::TypeID contextID, FinalStage::MemoryLayout::Part* pPart,
                                  const std::string& strInstanceVar, FinalStage::Concrete::Dimensions::User* pDimension,
                                  std::ostream& os )
        {
            const std::string strMegaMangledTypeName
                = megaMangle( pDimension->get_interface_dimension()->get_canonical_type() );

            const std::string strType    = bShared ? "shared" : "heap";
            const std::string strBShared = bShared ? "true" : "false";

            os << indent << "{\n";
            os << indent << indent << "char* p = reinterpret_cast< char* >( _fptr_get_" << strType << "_" << contextID
               << "( " << strInstanceVar << " ) ) "
               << " + " << pPart->get_offset() << " + ( " << pPart->get_size() << " * " << strInstanceVar
               << ".instance ) + " << pDimension->get_offset() << ";\n";
            os << indent << indent << "::mega::copy_" << strMegaMangledTypeName << "( pData, p );\n";
            os << indent << indent << "::mega::event_" << strMegaMangledTypeName
               << "( mega::reference( mega::TypeInstance( " << strInstanceVar << ".instance, "
               << pDimension->get_concrete_id() << "), " << strInstanceVar << " ), " << strBShared << ", p );\n";
            os << indent << "}\n";
        }
    */
    void generateInstructions( const DatabaseInstance&                             database,
                               FinalStage::Invocations::Instructions::Instruction* pInstruction,
                               const VariableMap& variables, FunctionPointers& functions, nlohmann::json& data )
    {
        using namespace FinalStage;
        using namespace FinalStage::Invocations;

        if ( auto pInstructionGroup = db_cast< Instructions::InstructionGroup >( pInstruction ) )
        {
            if ( auto pParentDerivation = db_cast< Instructions::ParentDerivation >( pInstructionGroup ) )
            {
                std::ostringstream os;
                os << indent << "// ParentDerivation\n";

                const Variables::Instance* pFrom = pParentDerivation->get_from();
                const Variables::Instance* pTo   = pParentDerivation->get_to();

                const std::string  s           = get( variables, pFrom );
                const mega::TypeID targetType  = pFrom->get_concrete()->get_concrete_id();
                const mega::U64    szLocalSize = database.getLocalDomainSize( targetType );

                if ( szLocalSize > 1 )
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
            else if ( auto pChildDerivation = db_cast< Instructions::ChildDerivation >( pInstructionGroup ) )
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
            else if ( auto pEnumDerivation = db_cast< Instructions::EnumDerivation >( pInstructionGroup ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// EnumDerivation\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if ( auto pEnumeration = db_cast< Instructions::Enumeration >( pInstructionGroup ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// Enumeration\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if ( auto pDimensionReferenceRead
                      = db_cast< Instructions::DimensionReferenceRead >( pInstructionGroup ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// DimensionReferenceRead\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if ( auto pMonoReference = db_cast< Instructions::MonoReference >( pInstructionGroup ) )
            {
                const Variables::Instance*  pInstance  = pMonoReference->get_instance();
                const Variables::Reference* pReference = pMonoReference->get_reference();

                std::ostringstream os;
                os << indent << "// MonoReference\n";
                os << indent << get( variables, pInstance ) << " = " << get( variables, pReference ) << ";";

                data[ "assignments" ].push_back( os.str() );
            }
            else if ( auto pPolyReference = db_cast< Instructions::PolyReference >( pInstructionGroup ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// PolyReference\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if ( auto pPolyCase = db_cast< Instructions::PolyCase >( pInstructionGroup ) )
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

            for ( auto pChildInstruction : pInstructionGroup->get_children() )
            {
                generateInstructions( database, pChildInstruction, variables, functions, data );
            }
        }
        else if ( auto pOperation = db_cast< FinalStage::Invocations::Operations::Operation >( pInstruction ) )
        {
            using namespace FinalStage::Invocations::Operations;

            if ( auto pAllocate = db_cast< Allocate >( pOperation ) )
            {
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
    {
        void* pMemoryManager = mega::runtime::get_this_shared_memory();
        U64 sharedOffset     = mega::runtime::allocate_shared( {{ shared_size }}, {{ shared_alignment }} );
        void* pHeap          = mega::runtime::allocate_heap( {{ heap_size }}, {{ heap_alignment }} );
        void* pShared        = mega::runtime::shared_offset_to_abs( sharedOffset, pMemoryManager );

        if( _fptr_object_shared_alloc_{{ concrete_type_id }} == nullptr )
        {
            mega::runtime::get_object_shared_alloc( g_pszModuleName, {{ concrete_type_id }}, &_fptr_object_shared_alloc_{{ concrete_type_id }} );
        }
        if( _fptr_object_heap_alloc_{{ concrete_type_id }} == nullptr )
        {
            mega::runtime::get_object_heap_alloc( g_pszModuleName, {{ concrete_type_id }}, &_fptr_object_heap_alloc_{{ concrete_type_id }} );
        }

        _fptr_object_shared_alloc_{{ concrete_type_id }}( pShared, pMemoryManager );
        _fptr_object_heap_alloc_{{ concrete_type_id }}( pHeap );

        return mega::reference( mega::TypeInstance( 0U, {{ concrete_type_id }} ), {{ instance }}, pShared );
    }
)TEMPLATE";

                // Note how no events are needed when allocate a new object
                Variables::Instance* pInstance       = pAllocate->get_instance();
                Concrete::Context*   pConcreteTarget = pAllocate->get_concrete_target();
                auto pObject = db_cast< Concrete::Object >( pConcreteTarget );

                mega::U64 szSizeShared, szSizeHeap;
                mega::U64 szAlignShared, szAlignHeap;
                {
                    for ( auto pBuffer : pObject->get_buffers() )
                    {
                        if ( db_cast< MemoryLayout::SimpleBuffer >( pBuffer ) )
                        {
                            VERIFY_RTE( szSizeShared == 0U );
                            szSizeShared  = pBuffer->get_size();
                            szAlignShared = pBuffer->get_alignment();
                        }
                        else if ( db_cast< MemoryLayout::NonSimpleBuffer >( pBuffer ) )
                        {
                            VERIFY_RTE( szSizeHeap == 0U );
                            szSizeHeap  = pBuffer->get_size();
                            szAlignHeap = pBuffer->get_alignment();
                        }
                        else
                        {
                            THROW_RTE( "Unsupported buffer type" );
                        }
                    }
                }

                nlohmann::json data( 
                { 
                    { "concrete_type_id",   pConcreteTarget->get_concrete_id() }, 
                    { "instance",           get( variables, pInstance ) },
                    { "shared_size",        szSizeShared },
                    { "shared_alignment",   szAlignShared },
                    { "heap_size",          szSizeHeap },
                    { "heap_alignment",     szAlignHeap }
                } );

                // clang-format on
                std::ostringstream os;
                os << render( szTemplate, data );

                functions.objectAllocSet.insert( pObject );

                data[ "assignments" ].push_back( os.str() );
            }
            else if ( auto pCall = db_cast< Call >( pOperation ) )
            {
                // clang-format off
static const char* szTemplate =
R"TEMPLATE(
    {
        if( _fptr_call_{{ concrete_type_id }} == nullptr )
        {
            mega::runtime::get_call_getter( g_pszModuleName, {{ concrete_type_id }},
                &_fptr_call_{{ concrete_type_id }} );
        }
        return mega::runtime::CallResult{ _fptr_call_{{ concrete_type_id }}, {{ instance }} };
    }
)TEMPLATE";

                Concrete::Context*   pConcreteTarget = pCall->get_concrete_target();
                Variables::Instance* pInstance       = pCall->get_instance();

                nlohmann::json data( 
                { 
                    { "concrete_type_id",    pConcreteTarget->get_concrete_id() }, 
                    { "instance",            get( variables, pInstance ) } 
                } );
                // clang-format on

                std::ostringstream os;
                os << render( szTemplate, data );

                functions.callSet.insert( pConcreteTarget );

                data[ "assignments" ].push_back( os.str() );
            }
            else if ( auto pStart = db_cast< Start >( pOperation ) )
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
            else if ( auto pStop = db_cast< Stop >( pOperation ) )
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
            else if ( auto pPause = db_cast< Pause >( pOperation ) )
            {
                THROW_RTE( "Not implemented" );
            }
            else if ( auto pResume = db_cast< Resume >( pOperation ) )
            {
                THROW_RTE( "Not implemented" );
            }
            else if ( auto pDone = db_cast< Done >( pOperation ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// Done\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if ( auto pWaitAction = db_cast< WaitAction >( pOperation ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// WaitAction\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if ( auto pWaitDimension = db_cast< WaitDimension >( pOperation ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// WaitDimension\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if ( auto pGetAction = db_cast< GetAction >( pOperation ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// GetAction\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if ( auto pGetDimension = db_cast< GetDimension >( pOperation ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// GetDimension\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if ( auto pRead = db_cast< Read >( pOperation ) )
            {
                THROW_TODO;
                /*Concrete::Dimensions::User* pDimension = pRead->get_concrete_dimension();
                Variables::Instance*        pInstance  = pRead->get_instance();
                MemoryLayout::Part*         pPart      = pDimension->get_part();
                const mega::TypeID          contextID  = pPart->get_context()->get_concrete_id();
                const bool                  bSimple    = pDimension->get_interface_dimension()->get_simple();

                {
                    std::ostringstream os;
                    os << indent << "// Read Operation\n";
                    generateBufferFPtrCheck( bSimple, contextID, os );
                    generateReferenceCheck( bSimple, contextID, get( variables, pInstance ), os );
                    generateUserDimRead( bSimple, contextID, pPart, get( variables, pInstance ), pDimension, os );
                    data[ "assignments" ].push_back( os.str() );
                }

                functions.parts.insert( pDimension->get_part() );*/
            }
            else if ( auto pWrite = db_cast< Write >( pOperation ) )
            {
                THROW_TODO;
                /*Concrete::Dimensions::User* pDimension = pWrite->get_concrete_dimension();
                Variables::Instance*        pInstance  = pWrite->get_instance();
                MemoryLayout::Part*         pPart      = pDimension->get_part();
                const mega::TypeID          contextID  = pPart->get_context()->get_concrete_id();
                const bool                  bSimple    = pDimension->get_interface_dimension()->get_simple();

                copiers.insert( pDimension->get_interface_dimension() );
                events.insert( megaMangle( pDimension->get_interface_dimension()->get_canonical_type() ) );

                {
                    std::ostringstream os;
                    os << indent << "// Write Operation\n";
                    generateBufferFPtrCheck( bSimple, contextID, os );
                    generateReferenceCheck( bSimple, contextID, get( variables, pInstance ), os );
                    generateBufferWrite( bSimple, contextID, pPart, get( variables, pInstance ), pDimension, os );
                    os << indent << "return " << get( variables, pInstance ) << ";\n";
                    data[ "assignments" ].push_back( os.str() );
                }

                functions.parts.insert( pDimension->get_part() );*/
            }
            else if ( auto pWriteLink = db_cast< WriteLink >( pOperation ) )
            {
                THROW_TODO;
                std::ostringstream os;
                os << indent << "// WriteLink\n";
                data[ "assignments" ].push_back( os.str() );
            }
            else if ( auto pRange = db_cast< Range >( pOperation ) )
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
        for ( auto pVariable : invocationVariables )
        {
            if ( auto pInstanceVar = db_cast< Variables::Instance >( pVariable ) )
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
            else if ( auto pDimensionVar = db_cast< Variables::Dimension >( pVariable ) )
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
            else if ( auto pContextVar = db_cast< Variables::Context >( pVariable ) )
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

                if ( pRootContext == pContextVar )
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

void CodeGenerator::generate_allocation( const DatabaseInstance& database, mega::TypeID objectTypeID, std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_allocation: {}", objectTypeID );

    const FinalStage::Concrete::Object*      pObject    = database.getObject( objectTypeID );
    const FinalStage::Components::Component* pComponent = pObject->get_component();

    std::ostringstream osObjectTypeID;
    osObjectTypeID << objectTypeID;
    nlohmann::json data( { { "objectTypeID", osObjectTypeID.str() },
                           { "shared_parts", nlohmann::json::array() },
                           { "heap_parts", nlohmann::json::array() },
                           { "news", nlohmann::json::array() },
                           { "deleters", nlohmann::json::array() } } );

    std::set< std::string > newers, deleters;
    {
        using namespace FinalStage;
        for ( auto pBuffer : pObject->get_buffers() )
        {
            bool bBufferIsShared = false;
            if ( db_cast< MemoryLayout::SimpleBuffer >( pBuffer ) )
                bBufferIsShared = true;

            for ( auto pPart : pBuffer->get_parts() )
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

                for ( auto pUserDim : pPart->get_user_dimensions() )
                {
                    const std::string strMangle
                        = megaMangle( pUserDim->get_interface_dimension()->get_canonical_type() );
                    nlohmann::json member( { { "type", pUserDim->get_interface_dimension()->get_canonical_type() },
                                             { "mangle", strMangle },
                                             { "name", pUserDim->get_interface_dimension()->get_id()->get_str() },
                                             { "offset", pUserDim->get_offset() } } );
                    part[ "members" ].push_back( member );
                    newers.insert( strMangle );
                    deleters.insert( strMangle );
                }
                for ( auto pLinkDim : pPart->get_link_dimensions() )
                {
                    if ( auto pLinkMany = db_cast< Concrete::Dimensions::LinkMany >( pLinkDim ) )
                    {
                        const std::string  strMangle = megaMangle( "mega::ReferenceVector" );
                        std::ostringstream osLinkName;
                        osLinkName << "link_" << pLinkMany->get_link()->get_concrete_id();
                        nlohmann::json member( { { "type", "mega::ReferenceVector" },
                                                 { "mangle", strMangle },
                                                 { "name", osLinkName.str() },
                                                 { "offset", pLinkMany->get_offset() } } );
                        part[ "members" ].push_back( member );
                        newers.insert( strMangle );
                        deleters.insert( strMangle );
                    }
                    else if ( auto pLinkSingle = db_cast< Concrete::Dimensions::LinkSingle >( pLinkDim ) )
                    {
                        const std::string  strMangle = megaMangle( "mega::reference" );
                        std::ostringstream osLinkName;
                        osLinkName << "link_" << pLinkSingle->get_link()->get_concrete_id();
                        nlohmann::json member( { { "type", "mega::reference" },
                                                 { "mangle", strMangle },
                                                 { "name", osLinkName.str() },
                                                 { "offset", pLinkSingle->get_offset() } } );
                        part[ "members" ].push_back( member );
                        newers.insert( strMangle );
                        deleters.insert( strMangle );
                    }
                    else
                    {
                        THROW_RTE( "Unknown link type" );
                    }
                }
                for ( auto pAllocDim : pPart->get_allocation_dimensions() )
                {
                    if ( auto pAllocator = db_cast< Concrete::Dimensions::Allocator >( pAllocDim ) )
                    {
                        const std::string strAllocatorTypeName = allocatorTypeName( database, pAllocator );
                        const std::string strMangle            = megaMangle( strAllocatorTypeName );

                        std::ostringstream osAllocName;
                        osAllocName << "alloc_"
                                    << pAllocator->get_allocator()->get_allocated_context()->get_concrete_id();

                        newers.insert( strMangle );
                        deleters.insert( strMangle );

                        nlohmann::json member( { { "type", strAllocatorTypeName },
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

                if ( bBufferIsShared )
                    data[ "shared_parts" ].push_back( part );
                else
                    data[ "heap_parts" ].push_back( part );
            }
        }
    }

    for ( const auto& str : newers )
    {
        data[ "news" ].push_back( str );
    }
    for ( const auto& str : deleters )
    {
        data[ "deleters" ].push_back( str );
    }

    std::ostringstream osCPPCode;
    m_pPimpl->render_allocation( data, osCPPCode );
    m_pPimpl->compileToLLVMIR( osObjectTypeID.str(), osCPPCode.str(), os, pComponent );
}

nlohmann::json CodeGenerator::generate( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                        std::string& strName ) const
{
    {
        std::ostringstream osName;
        osName << invocationID;
        strName = osName.str();
    }

    FunctionPointers functions;

    nlohmann::json data( { { "name", strName },
                           { "module_name", strName },
                           { "calls", nlohmann::json::array() },
                           { "copiers", nlohmann::json::array() },
                           { "events", nlohmann::json::array() },
                           { "allocators", nlohmann::json::array() },
                           { "freers", nlohmann::json::array() },
                           { "object_allocators", nlohmann::json::array() },
                           { "variables", nlohmann::json::array() },
                           { "assignments", nlohmann::json::array() } } );

    {
        using namespace FinalStage;
        using namespace FinalStage::Invocations;

        const FinalStage::Operations::Invocation* pInvocation = database.getInvocation( invocationID );

        const VariableMap variables = m_pPimpl->generateVariables(
            pInvocation->get_variables(), pInvocation->get_root_instruction()->get_context(), data );

        for ( auto pInstruction : pInvocation->get_root_instruction()->get_children() )
        {
            m_pPimpl->generateInstructions( database, pInstruction, variables, functions, data );
        }
    }

    for ( auto pCall : functions.callSet )
    {
        data[ "calls" ].push_back( pCall->get_concrete_id() );
    }
    for ( auto pCopy : functions.copySet )
    {
        data[ "copiers" ].push_back( megaMangle( pCopy->get_canonical_type() ) );
    }
    for ( auto event : functions.eventSet )
    {
        data[ "events" ].push_back( event );
    }
    for ( auto allocator : functions.allocatorSet )
    {
        data[ "allocators" ].push_back( allocator );
    }
    for ( auto freer : functions.freerSet )
    {
        data[ "freers" ].push_back( freer );
    }
    for( auto pObject : functions.objectAllocSet )
    {
        data[ "object_allocators" ].push_back( pObject->get_concrete_id() );
    }
    return data;
}

void CodeGenerator::generate_allocate( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                       std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_allocate: {}", invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    {
        m_pPimpl->render_allocate( data, osCPPCode );
    }
    m_pPimpl->compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}

void CodeGenerator::generate_read( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                   std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_read: {}", invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    {
        m_pPimpl->render_read( data, osCPPCode );
    }
    m_pPimpl->compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}

void CodeGenerator::generate_write( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                    std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_write: {}", invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    {
        m_pPimpl->render_write( data, osCPPCode );
    }
    m_pPimpl->compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}

void CodeGenerator::generate_call( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                   std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_call: {}", invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    {
        m_pPimpl->render_call( data, osCPPCode );
    }
    m_pPimpl->compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}

void CodeGenerator::generate_start( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                    std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_start: {}", invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    {
        m_pPimpl->render_start( data, osCPPCode );
    }
    m_pPimpl->compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}
void CodeGenerator::generate_stop( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                   std::ostream& os )
{
    SPDLOG_TRACE( "RUNTIME: generate_stop: {}", invocationID );

    std::string          strName;
    const nlohmann::json data = generate( database, invocationID, strName );

    std::ostringstream osCPPCode;
    {
        m_pPimpl->render_stop( data, osCPPCode );
    }
    m_pPimpl->compileToLLVMIR( strName, osCPPCode.str(), os, std::nullopt );
}
} // namespace mega::runtime
