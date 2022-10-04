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
        MPOContext* pMPOContext = MPOContext::get();
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
                           { "allocators", nlohmann::json::array() },
                           { "deallocators", nlohmann::json::array() } } );

    std::set< std::string > allocators, deallocators;
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
                    allocators.insert( strMangle );
                    deallocators.insert( strMangle );
                }
                for ( auto pLinkDim : pPart->get_link_dimensions() )
                {
                    if ( auto pLinkMany = dynamic_database_cast< Concrete::Dimensions::LinkMany >( pLinkDim ) )
                    {
                        const std::string  strMangle = megaMangle( "mega::ReferenceVector" );
                        std::ostringstream osLinkName;
                        osLinkName << "link_" << pLinkMany->get_link()->get_concrete_id();
                        nlohmann::json member( { { "type", "mega::ReferenceVector" },
                                                 { "mangle", strMangle },
                                                 { "name", osLinkName.str() },
                                                 { "offset", pLinkMany->get_offset() } } );
                        part[ "members" ].push_back( member );
                        allocators.insert( strMangle );
                        deallocators.insert( strMangle );
                    }
                    else if ( auto pLinkSingle = dynamic_database_cast< Concrete::Dimensions::LinkSingle >( pLinkDim ) )
                    {
                        const std::string  strMangle = megaMangle( "mega::reference" );
                        std::ostringstream osLinkName;
                        osLinkName << "link_" << pLinkSingle->get_link()->get_concrete_id();
                        nlohmann::json member( { { "type", "mega::reference" },
                                                 { "mangle", strMangle },
                                                 { "name", osLinkName.str() },
                                                 { "offset", pLinkSingle->get_offset() } } );
                        part[ "members" ].push_back( member );
                        allocators.insert( strMangle );
                        deallocators.insert( strMangle );
                    }
                    else
                    {
                        THROW_RTE( "Unknown link type" );
                    }
                }
                for ( auto pAllocDim : pPart->get_allocation_dimensions() )
                {
                    if ( auto pAllocator = dynamic_database_cast< Concrete::Dimensions::Allocator >( pAllocDim ) )
                    {
                        Allocators::Allocator* pAllocBase = pAllocator->get_allocator();

                        if ( auto pAlloc = dynamic_database_cast< Allocators::Nothing >( pAllocBase ) )
                        {
                            // do nothing
                        }
                        else if ( auto pAlloc = dynamic_database_cast< Allocators::Singleton >( pAllocBase ) )
                        {
                            const std::string  strMangle = megaMangle( "bool" );
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();
                            nlohmann::json member( { { "type", "bool" },
                                                     { "mangle", strMangle },
                                                     { "name", osAllocName.str() },
                                                     { "offset", pAllocator->get_offset() } } );
                            part[ "members" ].push_back( member );
                            allocators.insert( strMangle );
                            deallocators.insert( strMangle );
                        }
                        else if ( auto pAlloc = dynamic_database_cast< Allocators::Range32 >( pAllocBase ) )
                        {
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();

                            std::ostringstream osTypeName;
                            osTypeName << "mega::Bitmask32Allocator< "
                                       << database.getLocalDomainSize(
                                              pAlloc->get_allocated_context()->get_concrete_id() )
                                       << " >";
                            const std::string strMangle = megaMangle( osTypeName.str() );
                            nlohmann::json    member( { { "type", osTypeName.str() },
                                                        { "mangle", strMangle },
                                                        { "name", osAllocName.str() },
                                                        { "offset", pAllocator->get_offset() } } );
                            part[ "members" ].push_back( member );
                            allocators.insert( strMangle );
                            deallocators.insert( strMangle );
                        }
                        else if ( auto pAlloc = dynamic_database_cast< Allocators::Range64 >( pAllocBase ) )
                        {
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();
                            std::ostringstream osTypeName;
                            osTypeName << "mega::Bitmask64Allocator< "
                                       << database.getLocalDomainSize(
                                              pAlloc->get_allocated_context()->get_concrete_id() )
                                       << " >";
                            const std::string strMangle = megaMangle( osTypeName.str() );
                            nlohmann::json    member( { { "type", osTypeName.str() },
                                                        { "mangle", strMangle },
                                                        { "name", osAllocName.str() },
                                                        { "offset", pAllocator->get_offset() } } );
                            part[ "members" ].push_back( member );
                            allocators.insert( strMangle );
                            deallocators.insert( strMangle );
                        }
                        else if ( auto pAlloc = dynamic_database_cast< Allocators::RangeAny >( pAllocBase ) )
                        {
                            std::ostringstream osAllocName;
                            osAllocName << "alloc_" << pAlloc->get_allocated_context()->get_concrete_id();
                            std::ostringstream osTypeName;
                            osTypeName << "mega::RingAllocator< mega::Instance, "
                                       << database.getLocalDomainSize(
                                              pAlloc->get_allocated_context()->get_concrete_id() )
                                       << " >";
                            const std::string strMangle = megaMangle( osTypeName.str() );
                            nlohmann::json    member( { { "type", osTypeName.str() },
                                                        { "mangle", strMangle },
                                                        { "name", osAllocName.str() },
                                                        { "offset", pAllocator->get_offset() } } );
                            part[ "members" ].push_back( member );
                            allocators.insert( strMangle );
                            deallocators.insert( strMangle );
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

    for ( const auto& str : allocators )
    {
        data[ "allocators" ].push_back( str );
    }
    for ( const auto& str : deallocators )
    {
        data[ "deallocators" ].push_back( str );
    }

    std::ostringstream osCPPCode;
    m_pPimpl->render_allocation( data, osCPPCode );
    m_pPimpl->compileToLLVMIR( osObjectTypeID.str(), osCPPCode.str(), os, pComponent );
}

using VariableMap = std::map< const FinalStage::Invocations::Variables::Variable*, std::string >;

inline std::string get( const VariableMap& varMap, const FinalStage::Invocations::Variables::Variable* pVar )
{
    auto iFind = varMap.find( pVar );
    VERIFY_RTE( iFind != varMap.end() );
    return iFind->second;
}

static const std::string indent( "    " );

using PartSet = std::set< const FinalStage::MemoryLayout::Part* >;
using CallSet = std::set< const FinalStage::Concrete::Context* >;
using CopySet = std::set< const FinalStage::Interface::DimensionTrait* >;

void generateBufferFPtrCheck( bool bShared, mega::TypeID contextID, std::ostream& os )
{
    const std::string strType = bShared ? "shared" : "heap";
    os << indent << "if( _fptr_get_" << strType << "_" << contextID << " == nullptr ) "
       << "mega::runtime::get_getter_" << strType << "( g_pszModuleName, " << contextID << ", &_fptr_get_" << strType
       << "_" << contextID << " );\n";
}
void generateReferenceCheck( bool bShared, mega::TypeID contextID, const std::string& strInstanceVar, std::ostream& os )
{
    os << indent << "if( " << strInstanceVar << ".isNetwork() ) " << strInstanceVar
       << " = mega::runtime::networkToMachine( " << contextID << ", " << strInstanceVar << ".network );\n";
}
void generateBufferRead( bool bShared, mega::TypeID contextID, FinalStage::MemoryLayout::Part* pPart,
                         const std::string& strInstanceVar, FinalStage::Concrete::Dimensions::User* pDimension,
                         std::ostream& os )
{
    const std::string strType = bShared ? "shared" : "heap";
    os << indent << "return reinterpret_cast< char* >(_fptr_get_" << strType << "_" << contextID << "( "
       << strInstanceVar << " ) )"
       << " + " << pPart->get_offset() << " + ( " << pPart->get_size() << " * " << strInstanceVar << ".instance ) + "
       << pDimension->get_offset() << ";\n";
}

void generateBufferWrite( bool bShared, mega::TypeID contextID, FinalStage::MemoryLayout::Part* pPart,
                          const std::string& strInstanceVar, FinalStage::Concrete::Dimensions::User* pDimension,
                          std::ostream& os )
{
    const std::string strMegaMangledTypeName
        = megaMangle( pDimension->get_interface_dimension()->get_canonical_type() );

    const std::string strType    = bShared ? "shared" : "heap";
    const std::string strBShared = bShared ? "true" : "false";

    os << indent << "{\n";
    os << indent << indent << "char* p = reinterpret_cast< char* >( _fptr_get_" << strType << "_" << contextID << "( "
       << strInstanceVar << " ) ) "
       << " + " << pPart->get_offset() << " + ( " << pPart->get_size() << " * " << strInstanceVar << ".instance ) + "
       << pDimension->get_offset() << ";\n";
    os << indent << indent << "::mega::copy_" << strMegaMangledTypeName << "( pData, p );\n";
    os << indent << indent << "::mega::event_" << strMegaMangledTypeName << "( mega::reference( mega::TypeInstance( "
       << strInstanceVar << ".instance, " << pDimension->get_concrete_id() << "), " << strInstanceVar << " ), "
       << strBShared << ", p );\n";
    os << indent << "}\n";
}

void generateInstructions( const DatabaseInstance&                             database,
                           FinalStage::Invocations::Instructions::Instruction* pInstruction,
                           const VariableMap& variables, PartSet& parts, CallSet& calls, CopySet& copiers,
                           nlohmann::json& data )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    if ( auto pInstructionGroup = dynamic_database_cast< Instructions::InstructionGroup >( pInstruction ) )
    {
        if ( auto pParentDerivation = dynamic_database_cast< Instructions::ParentDerivation >( pInstructionGroup ) )
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
                   << ".instance / " << szLocalSize << ", " << targetType << " }, " << s << ", " << s << ".pointer };";
            }
            else
            {
                os << indent << get( variables, pTo ) << " = mega::reference{ mega::TypeInstance{ " << s
                   << ".instance, " << targetType << " }, " << s << ", " << s << ".pointer };";
            }

            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pChildDerivation = dynamic_database_cast< Instructions::ChildDerivation >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << indent << "// ChildDerivation\n";

            const Variables::Instance* pFrom = pParentDerivation->get_from();
            const Variables::Instance* pTo   = pParentDerivation->get_to();

            const std::string  s           = get( variables, pFrom );
            const mega::TypeID targetType  = pFrom->get_concrete()->get_concrete_id();
            const mega::U64    szLocalSize = database.getLocalDomainSize( targetType );

            os << indent << get( variables, pTo ) << " = mega::reference{ mega::TypeInstance{ " << s << ".instance, "
               << targetType << " }, " << s << ", " << s << ".process };";

            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pEnumDerivation = dynamic_database_cast< Instructions::EnumDerivation >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << indent << "// EnumDerivation\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pEnumeration = dynamic_database_cast< Instructions::Enumeration >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << indent << "// Enumeration\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pDimensionReferenceRead
                  = dynamic_database_cast< Instructions::DimensionReferenceRead >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << indent << "// DimensionReferenceRead\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pMonoReference = dynamic_database_cast< Instructions::MonoReference >( pInstructionGroup ) )
        {
            const Variables::Instance*  pInstance  = pMonoReference->get_instance();
            const Variables::Reference* pReference = pMonoReference->get_reference();

            std::ostringstream os;
            os << indent << "// MonoReference\n";
            os << indent << get( variables, pInstance ) << " = " << get( variables, pReference ) << ";";

            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pPolyReference = dynamic_database_cast< Instructions::PolyReference >( pInstructionGroup ) )
        {
            std::ostringstream os;
            os << indent << "// PolyReference\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pPolyCase = dynamic_database_cast< Instructions::PolyCase >( pInstructionGroup ) )
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
            generateInstructions( database, pChildInstruction, variables, parts, calls, copiers, data );
        }
    }
    else if ( auto pOperation
              = dynamic_database_cast< FinalStage::Invocations::Operations::Operation >( pInstruction ) )
    {
        using namespace FinalStage::Invocations::Operations;

        if ( auto pAllocate = dynamic_database_cast< Allocate >( pOperation ) )
        {
            // Note how no events are needed when allocate a new object
            Variables::Instance* pInstance       = pAllocate->get_instance();
            Concrete::Context*   pConcreteTarget = pAllocate->get_concrete_target();

            std::ostringstream os;
            os << indent << "// Allocate\n";
            generateReferenceCheck(
                false, pInstance->get_concrete()->get_concrete_id(), get( variables, pInstance ), os );

            os << indent
               << "const mega::NetworkAddress networkAddress = mega::runtime::allocateNetworkAddress( "
                  "context, "
               << pConcreteTarget->get_concrete_id() << " );\n";
            os << indent << "mega::reference result = mega::runtime::allocateMachineAddress( context, "
               << pConcreteTarget->get_concrete_id() << ", networkAddress );\n";
            os << indent << "return result;\n";

            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pCall = dynamic_database_cast< Call >( pOperation ) )
        {
            Concrete::Context*   pConcreteTarget = pCall->get_concrete_target();
            Variables::Instance* pInstance       = pCall->get_instance();

            std::ostringstream os;
            {
                os << indent << "// Call Operation\n";
                generateReferenceCheck(
                    false, pInstance->get_concrete()->get_concrete_id(), get( variables, pInstance ), os );

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
        else if ( auto pStart = dynamic_database_cast< Start >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// Start\n";

            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pStop = dynamic_database_cast< Stop >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// Stop\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pPause = dynamic_database_cast< Pause >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// Pause\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pResume = dynamic_database_cast< Resume >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// Resume\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pDone = dynamic_database_cast< Done >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// Done\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pWaitAction = dynamic_database_cast< WaitAction >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// WaitAction\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pWaitDimension = dynamic_database_cast< WaitDimension >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// WaitDimension\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pGetAction = dynamic_database_cast< GetAction >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// GetAction\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pGetDimension = dynamic_database_cast< GetDimension >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// GetDimension\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pRead = dynamic_database_cast< Read >( pOperation ) )
        {
            Concrete::Dimensions::User* pDimension = pRead->get_concrete_dimension();
            Variables::Instance*        pInstance  = pRead->get_instance();
            MemoryLayout::Part*         pPart      = pDimension->get_part();
            const mega::TypeID          contextID  = pPart->get_context()->get_concrete_id();
            const bool                  bSimple    = pDimension->get_interface_dimension()->get_simple();

            {
                std::ostringstream os;
                os << indent << "// Read Operation\n";
                generateBufferFPtrCheck( bSimple, contextID, os );
                generateReferenceCheck( bSimple, contextID, get( variables, pInstance ), os );
                generateBufferRead( bSimple, contextID, pPart, get( variables, pInstance ), pDimension, os );
                data[ "assignments" ].push_back( os.str() );
            }

            parts.insert( pDimension->get_part() );
        }
        else if ( auto pWrite = dynamic_database_cast< Write >( pOperation ) )
        {
            Concrete::Dimensions::User* pDimension = pWrite->get_concrete_dimension();
            Variables::Instance*        pInstance  = pWrite->get_instance();
            MemoryLayout::Part*         pPart      = pDimension->get_part();
            const mega::TypeID          contextID  = pPart->get_context()->get_concrete_id();
            const bool                  bSimple    = pDimension->get_interface_dimension()->get_simple();

            copiers.insert( pDimension->get_interface_dimension() );

            {
                std::ostringstream os;
                os << indent << "// Write Operation\n";
                generateBufferFPtrCheck( bSimple, contextID, os );
                generateReferenceCheck( bSimple, contextID, get( variables, pInstance ), os );
                generateBufferWrite( bSimple, contextID, pPart, get( variables, pInstance ), pDimension, os );
                os << indent << "return " << get( variables, pInstance ) << ";\n";
                data[ "assignments" ].push_back( os.str() );
            }

            parts.insert( pDimension->get_part() );
        }
        else if ( auto pWriteLink = dynamic_database_cast< WriteLink >( pOperation ) )
        {
            std::ostringstream os;
            os << indent << "// WriteLink\n";
            data[ "assignments" ].push_back( os.str() );
        }
        else if ( auto pRange = dynamic_database_cast< Range >( pOperation ) )
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
        if ( auto pInstanceVar = dynamic_database_cast< Variables::Instance >( pVariable ) )
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
        else if ( auto pDimensionVar = dynamic_database_cast< Variables::Dimension >( pVariable ) )
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
        else if ( auto pContextVar = dynamic_database_cast< Variables::Context >( pVariable ) )
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
    CopySet copiers;

    nlohmann::json data( { { "name", strName },
                           { "module_name", strName },
                           { "getters", nlohmann::json::array() },
                           { "calls", nlohmann::json::array() },
                           { "copiers", nlohmann::json::array() },
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
            generateInstructions( database, pInstruction, variables, parts, calls, copiers, data );
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
    for ( auto pCopy : copiers )
    {
        data[ "copiers" ].push_back( megaMangle( pCopy->get_canonical_type() ) );
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

} // namespace mega::runtime
