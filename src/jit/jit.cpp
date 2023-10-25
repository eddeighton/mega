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

#include "jit.hpp"

#include "symbol.hpp"
#include "symbol_utils.hpp"

#include "object_functions.hxx"
#include "invocation_functions.hxx"
#include "program_functions.hxx"
#include "relation_functions.hxx"
#include "operator_functions.hxx"

#include "mega/mangle/traits.hpp"

#include "database/FinalStage.hxx"

#include "invocation/invocation.hpp"

#include "service/network/log.hpp"

#include <utility>

namespace mega::runtime
{
JIT::JIT( const mega::MegastructureInstallation& megastructureInstallation, const mega::Project& project )
    : m_megastructureInstallation( megastructureInstallation )
    , m_project( project )
    , m_jitCompiler( megastructureInstallation )
    , m_database( m_project.getProjectDatabase() )
    , m_codeGenerator( m_megastructureInstallation, m_project )
    , m_componentManager( m_project, m_database )
    , m_pythonFileStore( new mega::io::FileStore(
          m_database.getEnvironment(), m_database.getManifest(), mega::io::FileInfo::FinalStage ) )
    , m_pythonDatabase( m_database.getEnvironment(), m_database.getEnvironment().project_manifest(), m_pythonFileStore )
    , m_pythonDatabaseFinal(
          m_database.getEnvironment(), m_database.getEnvironment().project_manifest(), m_pythonFileStore )
    , m_pPythonSymbolTable( m_pythonDatabase.one< OperationsStage::Symbols::SymbolTable >(
          m_database.getEnvironment().project_manifest() ) )
    , m_symbolTables( m_pPythonSymbolTable )
{
    VERIFY_RTE_MSG( !m_project.isEmpty(), "Empty project" );
}

JIT::JIT( const MegastructureInstallation& megastructureInstallation, const Project& project, JIT& oldJIT )
    : m_megastructureInstallation( megastructureInstallation )
    , m_project( project )
    , m_jitCompiler( std::move( oldJIT.m_jitCompiler ) ) // steal the pre-existing JIT
    , m_database( m_project.getProjectDatabase() )
    , m_codeGenerator( m_megastructureInstallation, m_project )
    , m_componentManager( m_project, m_database )
    , m_pythonFileStore( new mega::io::FileStore(
          m_database.getEnvironment(), m_database.getManifest(), mega::io::FileInfo::FinalStage ) )
    , m_pythonDatabase( m_database.getEnvironment(), m_database.getEnvironment().project_manifest(), m_pythonFileStore )
    , m_pythonDatabaseFinal(
          m_database.getEnvironment(), m_database.getEnvironment().project_manifest(), m_pythonFileStore )
    , m_pPythonSymbolTable( m_pythonDatabase.one< OperationsStage::Symbols::SymbolTable >(
          m_database.getEnvironment().project_manifest() ) )
    , m_symbolTables( m_pPythonSymbolTable )
{
    VERIFY_RTE_MSG( !m_project.isEmpty(), "Empty project" );
}

JIT::~JIT()
{
    SPDLOG_TRACE( "JIT::~JIT {}", m_functionPointers.size() );
    // reset ALL function pointers
    for( auto& pFunctionPtr : m_functionPointers )
    {
        *pFunctionPtr = nullptr;
    }
}

network::JITStatus JIT::getStatus() const
{
    network::JITStatus status;

    status.m_functionPointers       = m_functionPointers.size();
    status.m_allocators             = m_allocators.size();
    status.m_relations              = m_relations.size();
    status.m_invocations            = m_invocations.size();
    status.m_operators              = m_operators.size();
    status.m_componentManagerStatus = m_componentManager.getStatus();

    return status;
}

mega::TypeID JIT::getInterfaceTypeID( TypeID concreteTypeID ) const
{
    return m_database.getInterfaceTypeID( concreteTypeID );
}

JITCompiler::Module::Ptr JIT::compile( const std::string& strCode )
{
    // auto startTime = std::chrono::steady_clock::now();
    JITCompiler::Module::Ptr pModule = m_jitCompiler.compile( strCode );
    // SPDLOG_TRACE( "JIT: JIT Compilation time: {}", std::chrono::steady_clock::now() - startTime );
    return pModule;
}

Allocator::Ptr JIT::getAllocator( const CodeGenerator::LLVMCompiler& compiler, const TypeID& typeID )
{
    Allocator::Ptr pAllocator;
    {
        const TypeID objectTypeID = TypeID::make_object_from_typeID( typeID );
        auto         iFind        = m_allocators.find( objectTypeID );
        if( iFind != m_allocators.end() )
        {
            pAllocator = iFind->second;
        }
        else
        {
            std::ostringstream osModule;
            m_codeGenerator.generate_alllocator( compiler, m_database, objectTypeID, osModule );
            JITCompiler::Module::Ptr pModule = compile( osModule.str() );
            pAllocator                       = std::make_shared< Allocator >( objectTypeID, m_database, pModule );
            m_allocators.insert( { objectTypeID, pAllocator } );
        }
    }
    return pAllocator;
}

Relation::Ptr JIT::getRelation( const CodeGenerator::LLVMCompiler& compiler, const RelationID& relationID )
{
    Relation::Ptr pRelation;
    {
        auto iFind = m_relations.find( relationID );
        if( iFind != m_relations.end() )
        {
            pRelation = iFind->second;
        }
        else
        {
            std::ostringstream osModule;
            m_codeGenerator.generate_relation( compiler, m_database, relationID, osModule );
            JITCompiler::Module::Ptr pModule = compile( osModule.str() );
            pRelation                        = std::make_shared< Relation >( relationID, m_database, pModule );
            m_relations.insert( { relationID, pRelation } );
        }
    }
    return pRelation;
}

Program::Ptr JIT::getProgram( const CodeGenerator::LLVMCompiler& compiler )
{
    if( !m_pProgram )
    {
        std::ostringstream osModule;
        m_codeGenerator.generate_program( compiler, m_database, osModule );
        auto pModule = compile( osModule.str() );
        m_pProgram   = std::make_unique< Program >( m_database, pModule );
    }
    return m_pProgram;
}

void JIT::getProgramFunction( void* pLLVMCompiler, int fType, void** ppFunction )
{
    SPDLOG_TRACE( "JIT: getProgramFunction: {}", fType );

    m_functionPointers.insert( ppFunction );

    const CodeGenerator::LLVMCompiler& compiler
        = *reinterpret_cast< const CodeGenerator::LLVMCompiler* >( pLLVMCompiler );

    getProgram( compiler );

    const auto functionType = static_cast< program::FunctionType >( fType );
    switch( functionType )
    {
        case program::eObjectSaveBin:
        {
            *ppFunction = ( void* )m_pProgram->getObjectSaveBin();
        }
        break;
        case program::eObjectLoadBin:
        {
            *ppFunction = ( void* )m_pProgram->getObjectLoadBin();
        }
        break;
        case program::eRecordLoadBin:
        {
            *ppFunction = ( void* )m_pProgram->getRecordLoadBin();
        }
        break;
        case program::eRecordMake:
        {
            *ppFunction = ( void* )m_pProgram->getRecordMake();
        }
        break;
        case program::eRecordBreak:
        {
            *ppFunction = ( void* )m_pProgram->getRecordBreak();
        }
        break;
        case program::eTraverse:
        {
            *ppFunction = ( void* )m_pProgram->getTraverse();
        }
        break;
        case program::eLinkSize:
        {
            *ppFunction = ( void* )m_pProgram->getLinkSize();
        }
        break;
        case program::eLinkGet:
        {
            *ppFunction = ( void* )m_pProgram->getLinkGet();
        }
        break;
        case program::eReadAny:
        {
            *ppFunction = ( void* )m_pProgram->getReadAny();
        }
        break;
        case program::eWriteAny:
        {
            *ppFunction = ( void* )m_pProgram->getWriteAny();
        }
        break;
        case program::eEnumerate:
        {
            *ppFunction = ( void* )m_pProgram->getEnumerate();
        }
        break;
        default:
        case program::TOTAL_FUNCTION_TYPES:
        {
            THROW_RTE( "Unsupported program function" );
        }
        break;
    }
}

std::string JIT::getReturnTypeMangleType( FinalStage::Operations::ReturnTypes::ReturnType* pReturnType,
                                          const mega::InvocationID&                        invocationID )
{
    using namespace FinalStage;
    using ::operator<<;

    std::optional< std::string > strTypeOpt;

    if( auto pVoid = db_cast< Operations::ReturnTypes::Void >( pReturnType ) )
    {
        THROW_RTE( "void return type in invocation: " << invocationID );
    }
    else if( auto pDimension = db_cast< Operations::ReturnTypes::Dimension >( pReturnType ) )
    {
        if( pDimension->get_homogeneous() )
        {
            for( auto pDim : pDimension->get_dimensions() )
            {
                if( !strTypeOpt.has_value() )
                {
                    strTypeOpt = megaMangle( pDim->get_erased_type() );
                }
                else
                {
                    VERIFY_RTE_MSG( megaMangle( pDim->get_erased_type() ) == strTypeOpt.value(),
                                    "Inconsistent dimension return types in invocation: " << invocationID );
                }
            }
        }
        else
        {
            THROW_RTE( "Non-homogeneous dimensions in invocation return type: " << invocationID );
        }
    }
    else if( auto pFunction = db_cast< Operations::ReturnTypes::Function >( pReturnType ) )
    {
        THROW_RTE( "Read or Write returning function in invocation: " << invocationID );
    }
    else if( auto pRange = db_cast< Operations::ReturnTypes::Range >( pReturnType ) )
    {
        strTypeOpt = megaMangle( mega::psz_mega_reference_vector );
    }
    else if( auto pContextReturnType = db_cast< Operations::ReturnTypes::Context >( pReturnType ) )
    {
        strTypeOpt = megaMangle( mega::psz_mega_reference );
    }
    else
    {
        THROW_RTE( "Unknown return type in invocation: " << invocationID );
    }
    VERIFY_RTE_MSG( strTypeOpt.has_value(), "Failed to resolve type for invocation: " << invocationID );
    return strTypeOpt.value();
}

JITBase::InvocationTypeInfo JIT::compileInvocationFunction( void* pLLVMCompiler, const char* pszUnitName,
                                                            const mega::InvocationID& invocationID, void** ppFunction )
{
    using namespace FinalStage;
    using ::operator<<;
    SPDLOG_TRACE( "JIT: compileInvocationFunction: {} {}", pszUnitName, invocationID );

    const Operations::Invocation* pInvocationFinal = m_database.tryGetInvocation( invocationID );
    if( pInvocationFinal == nullptr )
    {
        // compile the invocation using the OperationsStage
        const OperationsStage::Operations::Invocation* pInvocation
            = mega::invocation::compileInvocation( m_pythonDatabase, m_symbolTables, invocationID );
        // now convert to the FinalStage database
        pInvocationFinal = m_pythonDatabaseFinal.convert< Operations::Invocation >( pInvocation );
        // and store in the database wrapper so will be found as dynamic invocation
        m_database.addDynamicInvocation( invocationID, pInvocationFinal );
    }

    JITBase::InvocationTypeInfo result{ pInvocationFinal->get_explicit_operation() };

    mega::runtime::invocation::FunctionType functionType = mega::runtime::invocation::TOTAL_FUNCTION_TYPES;
    switch( pInvocationFinal->get_explicit_operation() )
    {
        case mega::id_exp_Read:
        {
            functionType       = mega::runtime::invocation::eRead;
            result.mangledType = getReturnTypeMangleType( pInvocationFinal->get_return_type(), invocationID );
        }
        break;
        case mega::id_exp_Write:
        {
            functionType          = mega::runtime::invocation::eWrite;
            auto pWriteInvocation = db_cast< Operations::Write >( pInvocationFinal );
            VERIFY_RTE( pWriteInvocation );
            result.mangledType = getReturnTypeMangleType( pWriteInvocation->get_parameter_type(), invocationID );
        }
        break;
        case mega::id_exp_Link_Read:
        {
            functionType       = mega::runtime::invocation::eLinkRead;
            result.mangledType = getReturnTypeMangleType( pInvocationFinal->get_return_type(), invocationID );
        }
        break;
        case mega::id_exp_Link_Add:
        {
            functionType       = mega::runtime::invocation::eLinkAdd;
            result.mangledType = megaMangle( mega::psz_mega_reference );
        }
        break;

        case mega::id_exp_Link_Remove:
            functionType = mega::runtime::invocation::eLinkRemove;
            break;

        case mega::id_exp_Link_Clear:
            functionType = mega::runtime::invocation::eLinkClear;
            break;

        case mega::id_exp_Call:
            functionType = mega::runtime::invocation::eCall;
            break;

        case mega::id_exp_Signal:
            functionType = mega::runtime::invocation::eStart;
            break;
        case mega::id_exp_Start:
            functionType = mega::runtime::invocation::eStart;
            break;

        case mega::id_exp_GetContext:
            functionType = mega::runtime::invocation::eGet;
            break;
        case mega::id_exp_Move:
            functionType = mega::runtime::invocation::eMove;
            break;
        case mega::id_exp_Range:
        default:
        case mega::HIGHEST_EXPLICIT_OPERATION_TYPE:
            THROW_RTE( "Unknown explicit operation type" );
            break;
    }

    getInvocationFunction( pLLVMCompiler, pszUnitName, invocationID, functionType, ppFunction );

    return result;
}

void JIT::getInvocationFunction( void* pLLVMCompiler, const char* pszUnitName, const mega::InvocationID& invocationID,
                                 int fType, void** ppFunction )
{
    SPDLOG_TRACE( "JIT: getInvocationFunction: {} {} {}", pszUnitName, invocationID, fType );

    m_functionPointers.insert( ppFunction );

    const CodeGenerator::LLVMCompiler& compiler
        = *reinterpret_cast< const CodeGenerator::LLVMCompiler* >( pLLVMCompiler );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if( iFind != m_invocations.end() )
        {
            pModule = iFind->second;
        }
    }

    const auto functionType = static_cast< invocation::FunctionType >( fType );

    if( !pModule )
    {
        std::ostringstream osModule;
        m_codeGenerator.generate_invocation( compiler, m_database, invocationID, functionType, osModule );
        pModule = compile( osModule.str() );
        m_invocations.insert( std::make_pair( invocationID, pModule ) );
    }

    switch( functionType )
    {
        case invocation::eRead:
        {
            *ppFunction
                = ( void* )pModule->get< invocation::Read::FunctionPtr >( Symbol( invocationID, Symbol::RefR ) );
        }
        break;
        case invocation::eWrite:
        {
            *ppFunction = ( void* )pModule->get< invocation::Write::FunctionPtr >(
                Symbol( invocationID, Symbol::RefR_CVStar ) );
        }
        break;
        case invocation::eLinkRead:
        {
            *ppFunction
                = ( void* )pModule->get< invocation::LinkRead::FunctionPtr >( Symbol( invocationID, Symbol::RefR ) );
        }
        break;
        case invocation::eLinkAdd:
        {
            *ppFunction = ( void* )pModule->get< invocation::LinkAdd::FunctionPtr >(
                Symbol( invocationID, Symbol::RefR_RefR ) );
        }
        break;
        case invocation::eLinkRemove:
        {
            *ppFunction = ( void* )pModule->get< invocation::LinkRemove::FunctionPtr >(
                Symbol( invocationID, Symbol::RefR_RefR ) );
        }
        break;
        case invocation::eLinkClear:
        {
            *ppFunction
                = ( void* )pModule->get< invocation::LinkClear::FunctionPtr >( Symbol( invocationID, Symbol::RefR ) );
        }
        break;
        case invocation::eCall:
        {
            *ppFunction
                = ( void* )pModule->get< invocation::Call::FunctionPtr >( Symbol( invocationID, Symbol::RefR ) );
        }
        break;
        case invocation::eGet:
        {
            *ppFunction = ( void* )pModule->get< invocation::Get::FunctionPtr >( Symbol( invocationID, Symbol::RefR ) );
        }
        break;
        case invocation::eStart:
        {
            *ppFunction
                = ( void* )pModule->get< invocation::Start::FunctionPtr >( Symbol( invocationID, Symbol::RefR ) );
        }
        break;
        case invocation::eMove:
        {
            *ppFunction
                = ( void* )pModule->get< invocation::Move::FunctionPtr >( Symbol( invocationID, Symbol::RefR_RefR ) );
        }
        break;
        default:
        case invocation::TOTAL_FUNCTION_TYPES:
        {
            THROW_RTE( "Unsupported invocation function" );
        }
        break;
    }
}

void JIT::getObjectFunction( void* pLLVMCompiler, const char* pszUnitName, mega::TypeID typeID, int fType,
                             void** ppFunction )
{
    SPDLOG_TRACE( "JIT::getObjectFunction : {} {} {}", pszUnitName, typeID, fType );

    m_functionPointers.insert( ppFunction );

    const CodeGenerator::LLVMCompiler& compiler
        = *reinterpret_cast< const CodeGenerator::LLVMCompiler* >( pLLVMCompiler );

    const auto functionType = static_cast< object::FunctionType >( fType );
    switch( functionType )
    {
        case object::eObjectCtor:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getCtor();
        }
        break;
        case object::eObjectDtor:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getDtor();
        }
        break;
        case object::eObjectSaveBin:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getSaveBin();
        }
        break;
        case object::eObjectLoadBin:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getLoadBin();
        }
        break;
        case object::eCallGetter:
        {
            // NOTE: typeID here is an interface type id
            *ppFunction = ( void* )m_componentManager.getOperationFunctionPtr( typeID );
        }
        break;
        case object::eObjectUnparent:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getUnparent();
        }
        break;
        case object::eObjectTraverse:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getTraverse();
        }
        break;
        case object::eLinkSize:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getLinkSize();
        }
        break;
        case object::eLinkGet:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getLinkGet();
        }
        break;
        case object::eReadAny:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getReadAny();
        }
        break;
        case object::eWriteAny:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getWriteAny();
        }
        break;
        case object::eEnumerate:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getEnumerate();
        }
        break;
        default:
        case object::TOTAL_FUNCTION_TYPES:
        {
            THROW_RTE( "Unsupported object function" );
        }
        break;
    }
}

void JIT::getRelationFunction( void* pLLVMCompiler, const char* pszUnitName, const RelationID& relationID, int fType,
                               void** ppFunction )
{
    SPDLOG_TRACE(
        "JIT::getRelationFunction : {} {}.{} {}", pszUnitName, relationID.getLower(), relationID.getUpper(), fType );

    m_functionPointers.insert( ppFunction );

    const CodeGenerator::LLVMCompiler& compiler
        = *reinterpret_cast< const CodeGenerator::LLVMCompiler* >( pLLVMCompiler );

    const auto functionType = static_cast< relation::FunctionType >( fType );
    switch( functionType )
    {
        case relation::eLinkMake:
        {
            auto pRelation = getRelation( compiler, relationID );
            *ppFunction    = ( void* )pRelation->getMake();
        }
        break;
        case relation::eLinkBreak:
        {
            auto pRelation = getRelation( compiler, relationID );
            *ppFunction    = ( void* )pRelation->getBreak();
        }
        break;
        case relation::eLinkReset:
        {
            auto pRelation = getRelation( compiler, relationID );
            *ppFunction    = ( void* )pRelation->getReset();
        }
        break;
        case relation::eLinkSize:
        {
            auto pRelation = getRelation( compiler, relationID );
            *ppFunction    = ( void* )pRelation->getSize();
        }
        break;
        case relation::eLinkGet:
        {
            auto pRelation = getRelation( compiler, relationID );
            *ppFunction    = ( void* )pRelation->getGet();
        }
        break;
        default:
        case relation::TOTAL_FUNCTION_TYPES:
        {
            THROW_RTE( "Unsupported object function" );
        }
        break;
    }
}

void JIT::getActionFunction( mega::TypeID concreteTypeID, void** ppFunction )
{
    SPDLOG_TRACE( "JIT::getActionFunction : {}", concreteTypeID );

    m_functionPointers.insert( ppFunction );

    const mega::TypeID interfaceTypeID = m_database.getInterfaceTypeID( concreteTypeID );

    *ppFunction = ( void* )m_componentManager.getOperationFunctionPtr( interfaceTypeID );
}

void JIT::getPythonFunction( mega::TypeID interfaceTypeID, void** ppFunction, void* pPythonCaster )
{
    SPDLOG_TRACE( "JIT::getPythonFunction : {}", interfaceTypeID );

    m_functionPointers.insert( ppFunction );

    *ppFunction = ( void* )m_componentManager.getPythonFunctionPtr( interfaceTypeID, pPythonCaster );
}

void JIT::getOperatorFunction( void* pLLVMCompiler, const char* pszUnitName, TypeID target, int fType,
                               void** ppFunction )
{
    SPDLOG_TRACE( "JIT::getOperatorFunction : {} {} {}", pszUnitName, target, fType );

    m_functionPointers.insert( ppFunction );

    const CodeGenerator::LLVMCompiler& compiler
        = *reinterpret_cast< const CodeGenerator::LLVMCompiler* >( pLLVMCompiler );

    const OperatorID operatorID = { target, fType };

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_operators.find( operatorID );
        if( iFind != m_operators.end() )
        {
            pModule = iFind->second;
        }
    }

    const auto functionType = static_cast< operators::FunctionType >( fType );
    if( !pModule )
    {
        std::ostringstream osModule;
        m_codeGenerator.generate_operator( compiler, m_database, target, functionType, osModule );
        pModule = compile( osModule.str() );
        m_operators.insert( std::make_pair( operatorID, pModule ) );
    }

    switch( functionType )
    {
        case operators::eNew:
        {
            *ppFunction
                = ( void* )pModule->get< operators::New::FunctionPtr >( Symbol( "mega_new_", target, Symbol::None ) );
        }
        break;
        case operators::eDelete:
        {
            *ppFunction = ( void* )pModule->get< operators::Delete::FunctionPtr >(
                Symbol( "mega_delete_", target, Symbol::Ref ) );
        }
        break;
        case operators::eCast:
        {
            *ppFunction
                = ( void* )pModule->get< operators::Cast::FunctionPtr >( Symbol( "mega_cast_", target, Symbol::Ref ) );
        }
        break;
        case operators::eActive:
        {
            *ppFunction = ( void* )pModule->get< operators::Active::FunctionPtr >(
                Symbol( "mega_active_", target, Symbol::Ref ) );
        }
        break;
        case operators::eEnabled:
        {
            *ppFunction = ( void* )pModule->get< operators::Enabled::FunctionPtr >(
                Symbol( "mega_enabled_", target, Symbol::Ref ) );
        }
        break;
        default:
        case operators::TOTAL_FUNCTION_TYPES:
        {
            THROW_RTE( "Unsupported object function" );
        }
        break;
    }
}
} // namespace mega::runtime
