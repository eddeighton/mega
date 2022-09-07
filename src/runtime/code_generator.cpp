
#include "code_generator.hpp"
#include "database/common/environment_archive.hpp"

#include "database/model/FinalStage.hxx"
#include "service/network/log.hpp"

#include "common/file.hpp"
#include "common/string.hpp"
#include "common/stash.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include "boost/process.hpp"

#include <string>
#include <istream>
#include <sstream>

namespace mega
{
namespace runtime
{
namespace
{

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
    VERIFY_RTE_MSG( iCompilationResult == 0, "Error compilation invocation: " << osError.str() );
}

void compile( const boost::filesystem::path& clangPath,
              const boost::filesystem::path& inputCPPFilePath,
              const boost::filesystem::path& outputIRFilePath,
              std::optional< const FinalStage::Components::Component* >
                  pComponent )
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

        osCmd << "-o " << outputIRFilePath.native() << " -c " << inputCPPFilePath.native();
        runCompilation( osCmd.str() );
    }
    const auto timeDelta = std::chrono::steady_clock::now() - startTime;
    SPDLOG_TRACE( "Clang Compilation time: {}", timeDelta );
}

} // namespace

class CodeGenerator::Pimpl
{
    const mega::network::MegastructureInstallation m_megastructureInstallation;
    ::inja::Environment                            m_injaEnvironment;
    ::inja::Template                               m_allocationTemplate;
    ::inja::Template                               m_allocateTemplate;
    ::inja::Template                               m_readTemplate;
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

    void compileToLLVMIR( const std::string& strName, const std::string& strCPPCode, std::ostream& osIR,
                          std::optional< const FinalStage::Components::Component* > pComponent )
    {
        ExecutionContext* pExecutionContext = ExecutionContext::get();
        VERIFY_RTE( pExecutionContext );

        const boost::filesystem::path irFilePath = m_tempDir / ( strName + ".ir" );

        const task::DeterminantHash determinant{ strCPPCode };
        if ( pExecutionContext->restore( irFilePath.native(), determinant.get() ) )
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
            compile( m_clangPath, inputCPPFilePath, irFilePath, pComponent );
            pExecutionContext->stash( irFilePath.native(), determinant.get() );
            boost::filesystem::loadAsciiFile( irFilePath, osIR );
        }
    }
};

CodeGenerator::CodeGenerator( const mega::network::MegastructureInstallation& megastructureInstallation,
                              const mega::network::Project&                   project )
    : m_pPimpl( std::make_shared< Pimpl >( megastructureInstallation, project ) )
{
}

void CodeGenerator::generate_allocation( const DatabaseInstance& database, mega::TypeID objectTypeID, std::ostream& os )
{
    SPDLOG_TRACE( "generate_allocation: {}", objectTypeID );

    const FinalStage::Concrete::Object*      pObject    = database.getObject( objectTypeID );
    const FinalStage::Components::Component* pComponent = pObject->get_component();

    std::ostringstream osObjectTypeID;
    osObjectTypeID << objectTypeID;
    nlohmann::json data( { { "ObjectTypeID", osObjectTypeID.str() },
                           { "shared_parts", nlohmann::json::array() },
                           { "heap_parts", nlohmann::json::array() } } );

    {
        using namespace FinalStage;
        for ( auto pBuffer : pObject->get_buffers() )
        {
            bool bBufferIsShared = false;
            if ( dynamic_database_cast< MemoryLayout::SimpleBuffer >( pBuffer ) )
                bBufferIsShared = true;

            for ( auto pPart : pBuffer->get_parts() )
            {
                std::ostringstream osPartType;
                std::ostringstream osPartName;
                // osPart << "part_" << pPart->get_context()->get_concrete_id();
                osPartType << "_Part" << pPart->get_context()->get_interface()->get_identifier();
                osPartName << pPart->get_context()->get_interface()->get_identifier();

                const std::size_t szTotalDomainSize
                    = database.getConcreteContextTotalAllocation( pPart->get_context()->get_concrete_id() );

                nlohmann::json part( { { "type", osPartType.str() },
                                       { "name", osPartName.str() },
                                       { "size", szTotalDomainSize },
                                       { "inits", std::vector< int >( szTotalDomainSize, 0 ) },
                                       { "members", nlohmann::json::array() } } );

                for ( auto pUserDim : pPart->get_user_dimensions() )
                {
                    nlohmann::json member( { { "type", pUserDim->get_interface_dimension()->get_canonical_type() },
                                             { "name", pUserDim->get_interface_dimension()->get_id()->get_str() } } );
                    part[ "members" ].push_back( member );
                }
                for ( auto pLinkDim : pPart->get_link_dimensions() )
                {
                    if ( Concrete::Dimensions::LinkMany* pLinkMany
                         = dynamic_database_cast< Concrete::Dimensions::LinkMany >( pLinkDim ) )
                    {
                        std::ostringstream osLinkName;
                        osLinkName << "link_" << pLinkMany->get_link()->get_concrete_id();
                        nlohmann::json member( { { "type", "mega::reference" }, { "name", osLinkName.str() } } );
                        part[ "members" ].push_back( member );
                    }
                    else if ( Concrete::Dimensions::LinkSingle* pLinkSingle
                              = dynamic_database_cast< Concrete::Dimensions::LinkSingle >( pLinkDim ) )
                    {
                        std::ostringstream osLinkName;
                        osLinkName << "link_" << pLinkSingle->get_link()->get_concrete_id();
                        nlohmann::json member( { { "type", "mega::ReferenceVector" }, { "name", osLinkName.str() } } );
                        part[ "members" ].push_back( member );
                    }
                    else
                    {
                        THROW_RTE( "Unknown link type" );
                    }
                }
                for ( auto pAllocDim : pPart->get_allocation_dimensions() )
                {
                    if ( Concrete::Dimensions::Allocator* pAllocator
                         = dynamic_database_cast< Concrete::Dimensions::Allocator >( pAllocDim ) )
                    {
                        Allocators::Allocator* pAllocBase = pAllocator->get_allocator();

                        if ( Allocators::Nothing* pAlloc = dynamic_database_cast< Allocators::Nothing >( pAllocBase ) )
                        {
                            // do nothing
                        }
                        else if ( Allocators::Singleton* pAlloc
                                  = dynamic_database_cast< Allocators::Singleton >( pAllocBase ) )
                        {
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();
                            nlohmann::json member( { { "type", "bool" }, { "name", osAllocName.str() } } );
                            part[ "members" ].push_back( member );
                        }
                        else if ( Allocators::Range32* pAlloc
                                  = dynamic_database_cast< Allocators::Range32 >( pAllocBase ) )
                        {
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();
                            std::ostringstream osTypeName;
                            osTypeName << "Bitmask32Allocator< "
                                       << database.getConcreteContextTotalAllocation(
                                              pAlloc->get_allocated_context()->get_concrete_id() )
                                       << " >";
                            nlohmann::json member( { { "type", osTypeName.str() }, { "name", osAllocName.str() } } );
                            part[ "members" ].push_back( member );
                        }
                        else if ( Allocators::Range64* pAlloc
                                  = dynamic_database_cast< Allocators::Range64 >( pAllocBase ) )
                        {
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();
                            std::ostringstream osTypeName;
                            osTypeName << "Bitmask64Allocator< "
                                       << database.getConcreteContextTotalAllocation(
                                              pAlloc->get_allocated_context()->get_concrete_id() )
                                       << " >";
                            nlohmann::json member( { { "type", osTypeName.str() }, { "name", osAllocName.str() } } );
                            part[ "members" ].push_back( member );
                        }
                        else if ( Allocators::RangeAny* pAlloc
                                  = dynamic_database_cast< Allocators::RangeAny >( pAllocBase ) )
                        {
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();
                            std::ostringstream osTypeName;
                            osTypeName << "RingAllocator< mega::Instance, "
                                       << database.getConcreteContextTotalAllocation(
                                              pAlloc->get_allocated_context()->get_concrete_id() )
                                       << " >";
                            nlohmann::json member( { { "type", osTypeName.str() }, { "name", osAllocName.str() } } );
                            part[ "members" ].push_back( member );
                        }
                        else
                        {
                            THROW_RTE( "Unknown allocator type" );
                        }
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

    std::ostringstream osCPPCode;
    m_pPimpl->render_allocation( data, osCPPCode );
    m_pPimpl->compileToLLVMIR( osObjectTypeID.str(), osCPPCode.str(), os, pComponent );
}

void CodeGenerator::generate_allocate( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                       std::ostream& os )
{
    SPDLOG_TRACE( "generate_allocate: {}", invocationID );
    // const FinalStage::Operations::Invocation* pInvocation = database.getInvocation( invocation );
    // using namespace FinalStage::Invocations;
    // Variables::Context* pRootVariable = pInvocation->get_root_variable();
    // pInvocation->get_root_instruction()

    std::ostringstream osName;
    osName << invocationID;
    nlohmann::json     data( { { "name", osName.str() } } );
    std::ostringstream osCPPCode;
    m_pPimpl->render_allocate( data, osCPPCode );
    m_pPimpl->compileToLLVMIR( osName.str(), osCPPCode.str(), os, std::nullopt );
}

using VariableMap = std::map< FinalStage::Invocations::Variables::Variable*, std::string >;

void recurse( FinalStage::Invocations::Instructions::Instruction* pInstruction, const VariableMap& variables,
              nlohmann::json& data )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    if ( Instructions::InstructionGroup* pInstructionGroup
         = dynamic_database_cast< Instructions::InstructionGroup >( pInstruction ) )
    {
        if ( Instructions::ParentDerivation* pParentDerivation
             = dynamic_database_cast< Instructions::ParentDerivation >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << "//ParentDerivation";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Instructions::ChildDerivation* pChildDerivation
                  = dynamic_database_cast< Instructions::ChildDerivation >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << "//ChildDerivation";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Instructions::EnumDerivation* pEnumDerivation
                  = dynamic_database_cast< Instructions::EnumDerivation >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << "//EnumDerivation";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Instructions::Enumeration* pEnumeration
                  = dynamic_database_cast< Instructions::Enumeration >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << "//Enumeration";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Instructions::DimensionReferenceRead* pDimensionReferenceRead
                  = dynamic_database_cast< Instructions::DimensionReferenceRead >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << "//DimensionReferenceRead";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Instructions::MonoReference* pMonoReference
                  = dynamic_database_cast< Instructions::MonoReference >( pInstructionGroup ) )
        {
            // pMonoReference->get_instance()
            // pMonoReference->get_reference()

            std::ostringstream os;
            os << "//MonoReference";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Instructions::PolyReference* pPolyReference
                  = dynamic_database_cast< Instructions::PolyReference >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << "//PolyReference";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Instructions::PolyCase* pPolyCase
                  = dynamic_database_cast< Instructions::PolyCase >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << "//PolyCase";
            data[ "assignments" ].push_back( os.str() );
        }
        else
        {
            THROW_RTE( "Unknown instruction type" );
        }

        for ( auto pInstruction : pInstructionGroup->get_children() )
        {
            recurse( pInstruction, variables, data );
        }
    }
    else if ( FinalStage::Invocations::Operations::Operation* pOperation
              = dynamic_database_cast< FinalStage::Invocations::Operations::Operation >( pInstruction ) )
    {
        using namespace FinalStage::Invocations::Operations;

        if ( Allocate* pAllocate = dynamic_database_cast< Allocate >( pOperation ) )
        {
            std::ostringstream os;
            os << "//Allocate";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Call* pCall = dynamic_database_cast< Call >( pOperation ) )
        {
            std::ostringstream os;
            os << "//Call";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Start* pStart = dynamic_database_cast< Start >( pOperation ) )
        {
            std::ostringstream os;
            os << "//Start";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Stop* pStop = dynamic_database_cast< Stop >( pOperation ) )
        {
            std::ostringstream os;
            os << "//Stop";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Pause* pPause = dynamic_database_cast< Pause >( pOperation ) )
        {
            std::ostringstream os;
            os << "//Pause";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Resume* pResume = dynamic_database_cast< Resume >( pOperation ) )
        {
            std::ostringstream os;
            os << "//Resume";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Done* pDone = dynamic_database_cast< Done >( pOperation ) )
        {
            std::ostringstream os;
            os << "//Done";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( WaitAction* pWaitAction = dynamic_database_cast< WaitAction >( pOperation ) )
        {
            std::ostringstream os;
            os << "//WaitAction";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( WaitDimension* pWaitDimension = dynamic_database_cast< WaitDimension >( pOperation ) )
        {
            std::ostringstream os;
            os << "//WaitDimension";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( GetAction* pGetAction = dynamic_database_cast< GetAction >( pOperation ) )
        {
            std::ostringstream os;
            os << "//GetAction";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( GetDimension* pGetDimension = dynamic_database_cast< GetDimension >( pOperation ) )
        {
            std::ostringstream os;
            os << "//GetDimension";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Read* pRead = dynamic_database_cast< Read >( pOperation ) )
        {
            std::ostringstream os;
            os << "//Read";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Write* pWrite = dynamic_database_cast< Write >( pOperation ) )
        {
            std::ostringstream os;
            os << "//Write";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( WriteLink* pWriteLink = dynamic_database_cast< WriteLink >( pOperation ) )
        {
            std::ostringstream os;
            os << "//WriteLink";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Range* pRange = dynamic_database_cast< Range >( pOperation ) )
        {
            std::ostringstream os;
            os << "//Range";
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

void CodeGenerator::generate_read( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                   std::ostream& os )
{
    SPDLOG_TRACE( "generate_read: {}", invocationID );

    std::ostringstream osName;
    {
        osName << invocationID;
    }

    nlohmann::json data( { { "name", osName.str() },
                           { "module_name", osName.str() },
                           { "getters", nlohmann::json::array() },
                           { "variables", nlohmann::json::array() },
                           { "assignments", nlohmann::json::array() } } );

    {
        using namespace FinalStage;
        using namespace FinalStage::Invocations;

        const FinalStage::Operations::Invocation* pInvocation = database.getInvocation( invocationID );

        VariableMap variables;
        {
            int iVariableCounter = 0;
            for ( auto pVariable : pInvocation->get_variables() )
            {
                if ( Variables::Instance* pInstanceVar = dynamic_database_cast< Variables::Instance >( pVariable ) )
                {
                    std::ostringstream osName;
                    osName << "mega::Instance var_" << pInstanceVar->get_concrete()->get_interface()->get_identifier();
                    osName << "_" << iVariableCounter++;
                    data[ "variables" ].push_back( osName.str() );
                    variables.insert( { pVariable, osName.str() } );
                }
                else if ( Variables::Dimension* pDimensionVar
                          = dynamic_database_cast< Variables::Dimension >( pVariable ) )
                {
                    std::ostringstream osName;
                    for ( Concrete::Context* pContext : pDimensionVar->get_types() )
                    {
                        osName << "mega::reference var_" << pContext->get_interface()->get_identifier();
                        osName << "_" << iVariableCounter++;
                    }
                    data[ "variables" ].push_back( osName.str() );
                    variables.insert( { pVariable, osName.str() } );
                }
                else if ( Variables::Context* pContextVar = dynamic_database_cast< Variables::Context >( pVariable ) )
                {
                    std::ostringstream osName;
                    for ( Concrete::Context* pContext : pContextVar->get_types() )
                    {
                        osName << "mega::reference var_" << pContext->get_interface()->get_identifier();
                        osName << "_" << iVariableCounter++;
                    }
                    data[ "variables" ].push_back( osName.str() );
                    variables.insert( { pVariable, osName.str() } );
                }
                else
                {
                    THROW_RTE( "Unknown variable type" );
                }
            }
        }

        for ( auto pInstruction : pInvocation->get_root_instruction()->get_children() )
        {
            recurse( pInstruction, variables, data );
        }
    }

    std::ostringstream osCPPCode;
    {
        m_pPimpl->render_read( data, osCPPCode );
    }
    m_pPimpl->compileToLLVMIR( osName.str(), osCPPCode.str(), os, std::nullopt );
}

} // namespace runtime
} // namespace mega
