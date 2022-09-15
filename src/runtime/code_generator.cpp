
#include "code_generator.hpp"
#include "database/common/environment_archive.hpp"

#include "database/model/FinalStage.hxx"
#include "mega/common.hpp"
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
    if( iCompilationResult != 0 )
    {
        SPDLOG_ERROR( "Error compilation invocation: {}", osError.str() );
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
    SPDLOG_TRACE( "RUNTIME: generate_allocation: {}", objectTypeID );

    const FinalStage::Concrete::Object*      pObject    = database.getObject( objectTypeID );
    const FinalStage::Components::Component* pComponent = pObject->get_component();

    std::ostringstream osObjectTypeID;
    osObjectTypeID << objectTypeID;
    nlohmann::json data( { { "objectTypeID", osObjectTypeID.str() } } );
    
    /*{
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
                osPartType << pPart->get_context()->get_interface()->get_identifier();
                osPartName << pPart->get_context()->get_interface()->get_identifier();

                const std::size_t szTotalDomainSize
                    = database.getTotalDomainSize( pPart->get_context()->get_concrete_id() );

                nlohmann::json part( { { "type", osPartType.str() },
                                       { "name", osPartName.str() },
                                       { "size", pPart->get_size() },
                                       { "offset", pPart->get_offset() },
                                       { "total_domain", szTotalDomainSize },
                                       { "inits", std::vector< int >( szTotalDomainSize, 0 ) },
                                       { "members", nlohmann::json::array() } } );

                for ( auto pUserDim : pPart->get_user_dimensions() )
                {
                    nlohmann::json member( { { "type", pUserDim->get_interface_dimension()->get_canonical_type() },
                                             { "name", pUserDim->get_interface_dimension()->get_id()->get_str() },
                                             { "offset", pUserDim->get_offset() } } );
                    part[ "members" ].push_back( member );
                }
                for ( auto pLinkDim : pPart->get_link_dimensions() )
                {
                    if ( Concrete::Dimensions::LinkMany* pLinkMany
                         = dynamic_database_cast< Concrete::Dimensions::LinkMany >( pLinkDim ) )
                    {
                        std::ostringstream osLinkName;
                        osLinkName << "link_" << pLinkMany->get_link()->get_concrete_id();
                        nlohmann::json member( { { "type", "mega::ReferenceVector" },
                                                 { "name", osLinkName.str() },
                                                 { "offset", pLinkMany->get_offset() } } );
                        part[ "members" ].push_back( member );
                    }
                    else if ( Concrete::Dimensions::LinkSingle* pLinkSingle
                              = dynamic_database_cast< Concrete::Dimensions::LinkSingle >( pLinkDim ) )
                    {
                        std::ostringstream osLinkName;
                        osLinkName << "link_" << pLinkSingle->get_link()->get_concrete_id();
                        nlohmann::json member( { { "type", "mega::reference" },
                                                 { "name", osLinkName.str() },
                                                 { "offset", pLinkSingle->get_offset() } } );
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
                            nlohmann::json member( { { "type", "bool" },
                                                     { "name", osAllocName.str() },
                                                     { "offset", pAllocator->get_offset() } } );
                            part[ "members" ].push_back( member );
                        }
                        else if ( Allocators::Range32* pAlloc
                                  = dynamic_database_cast< Allocators::Range32 >( pAllocBase ) )
                        {
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();

                            std::ostringstream osTypeName;
                            osTypeName << "mega::Bitmask32Allocator< "
                                       << database.getLocalDomainSize(
                                              pAlloc->get_allocated_context()->get_concrete_id() )
                                       << " >";
                            nlohmann::json member( { { "type", osTypeName.str() },
                                                     { "name", osAllocName.str() },
                                                     { "offset", pAllocator->get_offset() } } );
                            part[ "members" ].push_back( member );
                        }
                        else if ( Allocators::Range64* pAlloc
                                  = dynamic_database_cast< Allocators::Range64 >( pAllocBase ) )
                        {
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();
                            std::ostringstream osTypeName;
                            osTypeName << "mega::Bitmask64Allocator< "
                                       << database.getLocalDomainSize(
                                              pAlloc->get_allocated_context()->get_concrete_id() )
                                       << " >";
                            nlohmann::json member( { { "type", osTypeName.str() },
                                                     { "name", osAllocName.str() },
                                                     { "offset", pAllocator->get_offset() } } );
                            part[ "members" ].push_back( member );
                        }
                        else if ( Allocators::RangeAny* pAlloc
                                  = dynamic_database_cast< Allocators::RangeAny >( pAllocBase ) )
                        {
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();
                            std::ostringstream osTypeName;
                            osTypeName << "mega::RingAllocator< mega::Instance, "
                                       << database.getLocalDomainSize(
                                              pAlloc->get_allocated_context()->get_concrete_id() )
                                       << " >";
                            nlohmann::json member( { { "type", osTypeName.str() },
                                                     { "name", osAllocName.str() },
                                                     { "offset", pAllocator->get_offset() } } );
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
    }*/

    std::ostringstream osCPPCode;
    m_pPimpl->render_allocation( data, osCPPCode );
    m_pPimpl->compileToLLVMIR( osObjectTypeID.str(), osCPPCode.str(), os, pComponent );
}

using VariableMap = std::map< const FinalStage::Invocations::Variables::Variable*, std::string >;

inline std::string get( const VariableMap& varMap, const FinalStage::Invocations::Variables::Variable* pVar )
{
    VariableMap::const_iterator iFind = varMap.find( pVar );
    VERIFY_RTE( iFind != varMap.end() );
    return iFind->second;
}

static const std::string indent( "    " );

using PartSet = std::set< const FinalStage::MemoryLayout::Part* >;
using CallSet = std::set< const FinalStage::Concrete::Context* >;

void generateBufferFPtrCheck( bool bShared, mega::TypeID id, std::ostream& os )
{
    const std::string strType = bShared ? "shared" : "heap";
    os << indent << "if( _fptr_get_" << strType << "_" << id << " == nullptr ) "
       << "mega::runtime::get_getter_" << strType << "( g_pszModuleName, " << id << ", &_fptr_get_" << strType << "_"
       << id << " );\n";
}
void generateBufferRead( bool bShared, mega::TypeID id, FinalStage::MemoryLayout::Part* pPart,
                         const std::string& strInstanceVar, FinalStage::Concrete::Dimensions::User* pDimension,
                         std::ostream& os )
{
    const std::string strType = bShared ? "shared" : "heap";
    os << indent << "return (char*)_fptr_get_" << strType << "_" << id << "( " << strInstanceVar << ".object )"
       << " + " << pPart->get_offset() << " + ( " << pPart->get_size() << " * " << strInstanceVar << ".instance ) + "
       << pDimension->get_offset() << ";";
}

void generateBufferWrite( bool bShared, mega::TypeID id, FinalStage::MemoryLayout::Part* pPart,
                          const std::string& strInstanceVar, FinalStage::Concrete::Dimensions::User* pDimension,
                          std::ostream& os )
{
    const std::string strType = bShared ? "shared" : "heap";
    os << indent << "return mega::runtime::WriteResult{ (char*)_fptr_get_" << strType << "_" << id << "( "
       << strInstanceVar << ".object )"
       << " + " << pPart->get_offset() << " + ( " << pPart->get_size() << " * " << strInstanceVar << ".instance ) + "
       << pDimension->get_offset() << ", " << strInstanceVar << "};";
}

void generateInstructions( const DatabaseInstance&                             database,
                           FinalStage::Invocations::Instructions::Instruction* pInstruction,
                           const VariableMap& variables, PartSet& parts, CallSet& calls, nlohmann::json& data )
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
            os << indent << "// ParentDerivation\n";

            const Variables::Instance* pFrom = pParentDerivation->get_from();
            const Variables::Instance* pTo   = pParentDerivation->get_to();

            const std::string  s           = get( variables, pFrom );
            const mega::TypeID targetType  = pFrom->get_concrete()->get_concrete_id();
            const std::size_t  szLocalSize = database.getLocalDomainSize( targetType );

            if ( szLocalSize > 1 )
            {
                os << indent << get( variables, pTo ) << " = mega::reference{ mega::TypeInstance{ " << s
                   << ".instance / " << szLocalSize << ", " << targetType << " }, " << s << ", " << s << ".pointer };";
            }
            else
            {
                os << indent << get( variables, pTo ) << " = mega::reference{ mega::TypeInstance{ " << s
                   << ".instance, " << targetType << " }, " << s << ", " << s << ".pointer };";
            }

            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Instructions::ChildDerivation* pChildDerivation
                  = dynamic_database_cast< Instructions::ChildDerivation >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << indent << "// ChildDerivation\n";

            const Variables::Instance* pFrom = pParentDerivation->get_from();
            const Variables::Instance* pTo   = pParentDerivation->get_to();

            const std::string  s           = get( variables, pFrom );
            const mega::TypeID targetType  = pFrom->get_concrete()->get_concrete_id();
            const std::size_t  szLocalSize = database.getLocalDomainSize( targetType );

            os << indent << get( variables, pTo ) << " = mega::reference{ mega::TypeInstance{ " << s << ".instance, "
               << targetType << " }, " << s << ", " << s << ".process };";

            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Instructions::EnumDerivation* pEnumDerivation
                  = dynamic_database_cast< Instructions::EnumDerivation >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << indent << "// EnumDerivation\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Instructions::Enumeration* pEnumeration
                  = dynamic_database_cast< Instructions::Enumeration >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << indent << "// Enumeration\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Instructions::DimensionReferenceRead* pDimensionReferenceRead
                  = dynamic_database_cast< Instructions::DimensionReferenceRead >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << indent << "// DimensionReferenceRead\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Instructions::MonoReference* pMonoReference
                  = dynamic_database_cast< Instructions::MonoReference >( pInstructionGroup ) )
        {
            const Variables::Instance*  pInstance  = pMonoReference->get_instance();
            const Variables::Reference* pReference = pMonoReference->get_reference();

            std::ostringstream os;
            os << indent << "// MonoReference\n";
            os << indent << get( variables, pInstance ) << " = " << get( variables, pReference ) << ";";

            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Instructions::PolyReference* pPolyReference
                  = dynamic_database_cast< Instructions::PolyReference >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << indent << "// PolyReference\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Instructions::PolyCase* pPolyCase
                  = dynamic_database_cast< Instructions::PolyCase >( pInstructionGroup ) )
        {
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
            generateInstructions( database, pChildInstruction, variables, parts, calls, data );
        }
    }
    else if ( FinalStage::Invocations::Operations::Operation* pOperation
              = dynamic_database_cast< FinalStage::Invocations::Operations::Operation >( pInstruction ) )
    {
        using namespace FinalStage::Invocations::Operations;

        if ( Allocate* pAllocate = dynamic_database_cast< Allocate >( pOperation ) )
        {
            Concrete::Context* pConcreteTarget = pAllocate->get_concrete_target();

            std::ostringstream os;
            os << indent << "// Allocate\n";
            os << indent
               << "const mega::NetworkAddress networkAddress = mega::runtime::allocateNetworkAddress( "
                  "context, "
               << pConcreteTarget->get_concrete_id() << " );\n";

            os << indent << "mega::reference result;\n";
            os << indent << "{\n";
            os << indent << "    result = mega::runtime::networkToMachine( context, "
               << pConcreteTarget->get_concrete_id()
               << ", "
                  "networkAddress );\n";
            os << indent << "    result.instance = 0;\n";
            os << indent << "    result.typeID = " << pConcreteTarget->get_concrete_id() << ";\n";
            os << indent << "}\n";
            os << indent << "return result;\n";
            
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Call* pCall = dynamic_database_cast< Call >( pOperation ) )
        {
            Concrete::Context*   pConcreteTarget = pCall->get_concrete_target();
            Variables::Instance* pInstance       = pCall->get_instance();

            std::ostringstream os;
            {
                os << indent << "// Call Operation\n";

                std::ostringstream osCall;
                osCall << "_fptr_call_" << pConcreteTarget->get_concrete_id();

                os << indent << "if( " << osCall.str() << " == nullptr )\n";
                os << indent << "{\n";
                os << indent << "    mega::runtime::get_getter_call( g_pszModuleName, "
                   << pConcreteTarget->get_concrete_id() << ", &" << osCall.str() << " );\n";
                os << indent << "}\n";
                os << indent << "return mega::runtime::CallResult{" << osCall.str() << ", "
                   << get( variables, pInstance ) << " };\n";
            }

            calls.insert( pConcreteTarget );

            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Start* pStart = dynamic_database_cast< Start >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// Start\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Stop* pStop = dynamic_database_cast< Stop >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// Stop\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Pause* pPause = dynamic_database_cast< Pause >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// Pause\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Resume* pResume = dynamic_database_cast< Resume >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// Resume\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Done* pDone = dynamic_database_cast< Done >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// Done\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( WaitAction* pWaitAction = dynamic_database_cast< WaitAction >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// WaitAction\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( WaitDimension* pWaitDimension = dynamic_database_cast< WaitDimension >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// WaitDimension\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( GetAction* pGetAction = dynamic_database_cast< GetAction >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// GetAction\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( GetDimension* pGetDimension = dynamic_database_cast< GetDimension >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// GetDimension\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Read* pRead = dynamic_database_cast< Read >( pOperation ) )
        {
            Concrete::Dimensions::User* pDimension = pRead->get_concrete_dimension();
            Variables::Instance*        pInstance  = pRead->get_instance();
            MemoryLayout::Part*         pPart      = pDimension->get_part();
            const mega::TypeID          id         = pPart->get_context()->get_concrete_id();
            const bool                  bSimple    = pDimension->get_interface_dimension()->get_simple();

            {
                std::ostringstream os;
                os << indent << "// Read Operation\n";
                generateBufferFPtrCheck( bSimple, id, os );
                generateBufferRead( bSimple, id, pPart, get( variables, pInstance ), pDimension, os );
                data[ "assignments" ].push_back( os.str() );
            }

            parts.insert( pDimension->get_part() );
        }
        else if ( Write* pWrite = dynamic_database_cast< Write >( pOperation ) )
        {
            Concrete::Dimensions::User* pDimension = pWrite->get_concrete_dimension();
            Variables::Instance*        pInstance  = pWrite->get_instance();
            MemoryLayout::Part*         pPart      = pDimension->get_part();
            const mega::TypeID          id         = pPart->get_context()->get_concrete_id();
            const bool                  bSimple    = pDimension->get_interface_dimension()->get_simple();

            {
                std::ostringstream os;
                os << indent << "// Write Operation\n";
                generateBufferFPtrCheck( bSimple, id, os );
                generateBufferWrite( bSimple, id, pPart, get( variables, pInstance ), pDimension, os );
                data[ "assignments" ].push_back( os.str() );
            }

            parts.insert( pDimension->get_part() );
        }
        else if ( WriteLink* pWriteLink = dynamic_database_cast< WriteLink >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// WriteLink\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( Range* pRange = dynamic_database_cast< Range >( pOperation ) )
        {
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
        if ( Variables::Instance* pInstanceVar = dynamic_database_cast< Variables::Instance >( pVariable ) )
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
        else if ( Variables::Dimension* pDimensionVar = dynamic_database_cast< Variables::Dimension >( pVariable ) )
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
        else if ( Variables::Context* pContextVar = dynamic_database_cast< Variables::Context >( pVariable ) )
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

nlohmann::json CodeGenerator::generate( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                        std::string& strName ) const
{
    {
        std::ostringstream osName;
        osName << invocationID;
        strName = osName.str();
    }

    PartSet parts;
    CallSet calls;

    nlohmann::json data( { { "name", strName },
                           { "module_name", strName },
                           { "getters", nlohmann::json::array() },
                           { "calls", nlohmann::json::array() },
                           { "variables", nlohmann::json::array() },
                           { "assignments", nlohmann::json::array() } } );

    {
        using namespace FinalStage;
        using namespace FinalStage::Invocations;

        const FinalStage::Operations::Invocation* pInvocation = database.getInvocation( invocationID );

        const VariableMap variables = generateVariables(
            pInvocation->get_variables(), pInvocation->get_root_instruction()->get_context(), data );

        for ( auto pInstruction : pInvocation->get_root_instruction()->get_children() )
        {
            generateInstructions( database, pInstruction, variables, parts, calls, data );
        }
    }

    for ( auto pPart : parts )
    {
        data[ "getters" ].push_back( pPart->get_context()->get_concrete_id() );
    }
    for ( auto pCall : calls )
    {
        data[ "calls" ].push_back( pCall->get_concrete_id() );
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

} // namespace runtime
} // namespace mega
