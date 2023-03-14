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
#include "symbol_utils.hpp"
#include "object_functions.hxx"
#include "invocation_functions.hxx"
#include "program_functions.hxx"
#include "relation_functions.hxx"
#include "operator_functions.hxx"

#include "database/model/FinalStage.hxx"

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
{
    VERIFY_RTE_MSG( !m_project.isEmpty(), "Empty project" );
}

std::unordered_map< std::string, mega::TypeID > JIT::getIdentities() const
{
    return m_database.getIdentities();
}

JITCompiler::Module::Ptr JIT::compile( const std::string& strCode )
{
    // auto startTime = std::chrono::steady_clock::now();
    JITCompiler::Module::Ptr pModule = m_jitCompiler.compile( strCode );
    // SPDLOG_TRACE( "JIT: JIT Compilation time: {}", std::chrono::steady_clock::now() - startTime );
    return pModule;
}

Allocator::Ptr JIT::getAllocator( const CodeGenerator::LLVMCompiler& compiler, const TypeID& objectTypeID )
{
    Allocator::Ptr pAllocator;
    {
        auto iFind = m_allocators.find( objectTypeID );
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

    const CodeGenerator::LLVMCompiler& compiler
        = *reinterpret_cast< const CodeGenerator::LLVMCompiler* >( pLLVMCompiler );

    m_programFunctionPointers.insert( ppFunction );

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
        case program::TOTAL_FUNCTION_TYPES:
        {
            THROW_RTE( "Unsupported program function" );
        }
        break;
    }
}

JITBase::InvocationTypeInfo JIT::compileInvocationFunction( void* pLLVMCompiler, const char* pszUnitName,
                                                            const mega::InvocationID& invocationID, void** ppFunction )
{
    SPDLOG_TRACE( "JIT: compileInvocationFunction: {} {}", pszUnitName, invocationID );

    const FinalStage::Operations::Invocation* pInvocationFinal = m_database.tryGetInvocation( invocationID );
    if( pInvocationFinal == nullptr )
    {
        // compile the invocation using the OperationsStage
        const OperationsStage::Operations::Invocation* pInvocation
            = mega::invocation::construct( m_pythonDatabase, m_pPythonSymbolTable, invocationID );
        // now convert to the FinalStage database
        pInvocationFinal = m_pythonDatabaseFinal.convert< FinalStage::Operations::Invocation >( pInvocation );
        // and store in the database wrapper so will be found as dynamic invocation
        m_database.addDynamicInvocation( invocationID, pInvocationFinal );
    }

    JITBase::InvocationTypeInfo result{ pInvocationFinal->get_explicit_operation() };

    mega::runtime::invocation::FunctionType functionType = mega::runtime::invocation::TOTAL_FUNCTION_TYPES;
    switch( pInvocationFinal->get_explicit_operation() )
    {
        // clang-format off
        case mega::id_exp_Read:   
        case mega::id_exp_Write:          
        {
            if( pInvocationFinal->get_explicit_operation()  == mega::id_exp_Read )
            {
                functionType = mega::runtime::invocation::eRead; 
            }
            else
            {
                functionType = mega::runtime::invocation::eWrite; 
            }

            const auto returnDim = pInvocationFinal->get_return_type_dimensions();
            const auto returnRef = pInvocationFinal->get_return_type_contexts();

            if( returnDim.size() )
            {
                for( const auto& pDim : returnDim )
                {
                    if( result.mangledType.empty() )
                    {
                        result.mangledType = megaMangle( pDim->get_erased_type() );
                    }
                    else
                    {
                        const std::string strMangle = megaMangle( pDim->get_erased_type() );
                        VERIFY_RTE_MSG( strMangle == result.mangledType, "Inconsistent dimension return types" );
                    }
                }
            }
            else if( returnRef.size() )
            {
                result.mangledType = megaMangle( "mega::reference" );
            }
            else
            {
                THROW_RTE( "Unknown invocation return type" );
            }

            break; 
        }
        case mega::id_exp_Read_Link:       
        {
            functionType = mega::runtime::invocation::eReadLink; 
            break; 
        }        
        case mega::id_exp_Write_Link:      
        {
            functionType = mega::runtime::invocation::eWriteLink;
            //functionType = mega::runtime::invocation::eWriteLinkRange;
        }
        break;  

        case mega::id_exp_Allocate:        functionType = mega::runtime::invocation::eAllocate; break;     
        case mega::id_exp_Call:            functionType = mega::runtime::invocation::eCall; break; 
        case mega::id_exp_Start:           functionType = mega::runtime::invocation::eStart; break;     
        case mega::id_exp_Stop:            functionType = mega::runtime::invocation::eStop; break; 
        case mega::id_exp_Save:            functionType = mega::runtime::invocation::eSave; break; 
        case mega::id_exp_Load:            functionType = mega::runtime::invocation::eLoad; break; 
        case mega::id_exp_GetAction:       functionType = mega::runtime::invocation::eGet; break;         
        case mega::id_exp_GetDimension:    functionType = mega::runtime::invocation::eGet; break;  
        case mega::id_exp_Files:                  
        case mega::id_exp_Done:            
        case mega::id_exp_Range:           
        case mega::id_exp_Raw:             
        case mega::HIGHEST_EXPLICIT_OPERATION_TYPE:
            THROW_RTE( "Unknown explicit operation type" );
            break;
            // clang-format on
    }

    getInvocationFunction( pLLVMCompiler, pszUnitName, invocationID, functionType, ppFunction );

    return result;
}

void JIT::getInvocationFunction( void* pLLVMCompiler, const char* pszUnitName, const mega::InvocationID& invocationID,
                                 int fType, void** ppFunction )
{
    SPDLOG_TRACE( "JIT: getInvocationFunction: {} {} {}", pszUnitName, invocationID, fType );

    const CodeGenerator::LLVMCompiler& compiler
        = *reinterpret_cast< const CodeGenerator::LLVMCompiler* >( pLLVMCompiler );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

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
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceE";
            *ppFunction = ( void* )pModule->get< invocation::Read::FunctionPtr >( os.str() );
        }
        break;
        case invocation::eWrite:
        {
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceEPKv";
            *ppFunction = ( void* )pModule->get< invocation::Write::FunctionPtr >( os.str() );
        }
        break;
        case invocation::eReadLink:
        {
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceE";
            *ppFunction = ( void* )pModule->get< invocation::ReadLink::FunctionPtr >( os.str() );
        }
        break;
        case invocation::eWriteLink:
        {
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceE14WriteOperationRKS0_";
            *ppFunction = ( void* )pModule->get< invocation::WriteLink::FunctionPtr >( os.str() );
        }
        break;
        case invocation::eWriteLinkRange:
        {
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceE14WriteOperationRKS0_";
            *ppFunction = ( void* )pModule->get< invocation::WriteLinkRange::FunctionPtr >( os.str() );
        }
        break;
        case invocation::eAllocate:
        {
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceE";
            *ppFunction = ( void* )pModule->get< invocation::Allocate::FunctionPtr >( os.str() );
        }
        break;
        case invocation::eCall:
        {
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceE";
            *ppFunction = ( void* )pModule->get< invocation::Call::FunctionPtr >( os.str() );
        }
        break;
        case invocation::eGet:
        {
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceE";
            *ppFunction = ( void* )pModule->get< invocation::Get::FunctionPtr >( os.str() );
        }
        break;
        case invocation::eSave:
        {
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceEPv";
            *ppFunction = ( void* )pModule->get< invocation::Save::FunctionPtr >( os.str() );
        }
        break;
        case invocation::eLoad:
        {
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceEPv";
            *ppFunction = ( void* )pModule->get< invocation::Load::FunctionPtr >( os.str() );
        }
        break;
        case invocation::eStart:
        {
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceE";
            *ppFunction = ( void* )pModule->get< invocation::Start::FunctionPtr >( os.str() );
        }
        break;
        case invocation::eStop:
        {
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceE";
            *ppFunction = ( void* )pModule->get< invocation::Stop::FunctionPtr >( os.str() );
        }
        break;
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
            m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
            *ppFunction = ( void* )m_componentManager.getOperationFunctionPtr( typeID );
        }
        break;
        case object::eObjectSaveXMLStructure:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getSaveXMLStructure();
        }
        break;
        case object::eObjectLoadXMLStructure:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getLoadXMLStructure();
        }
        break;
        case object::eObjectSaveXML:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getSaveXML();
        }
        break;
        case object::eObjectLoadXML:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getLoadXML();
        }
        break;
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
        case relation::eLinkOverwrite:
        {
            auto pRelation = getRelation( compiler, relationID );
            *ppFunction    = ( void* )pRelation->getOverwrite();
        }
        break;
        case relation::eLinkReset:
        {
            auto pRelation = getRelation( compiler, relationID );
            *ppFunction    = ( void* )pRelation->getReset();
        }
        break;
        case relation::TOTAL_FUNCTION_TYPES:
        {
            THROW_RTE( "Unsupported object function" );
        }
        break;
    }
}

void JIT::getActionFunction( mega::TypeID typeID, void** ppFunction, ActionInfo& actionInfo )
{
    SPDLOG_TRACE( "JIT::getActionFunction : {}", typeID );
    *ppFunction = ( void* )m_componentManager.getOperationFunctionPtr( typeID );

    const FinalStage::Concrete::Action* pAction = m_database.getAction( typeID );
    actionInfo.type                             = ActionInfo::eAction;
}

void JIT::getOperatorFunction( void* pLLVMCompiler, const char* pszUnitName, TypeID target, int fType,
                               void** ppFunction )
{
    SPDLOG_TRACE( "JIT::getOperatorFunction : {} {} {}", pszUnitName, target, fType );
    const CodeGenerator::LLVMCompiler& compiler
        = *reinterpret_cast< const CodeGenerator::LLVMCompiler* >( pLLVMCompiler );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

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
        case operators::eCast:
        {
            std::ostringstream os;
            symbolPrefix( "mega_cast_", target, os );
            os << "RKN4mega9referenceE";
            *ppFunction = ( void* )pModule->get< operators::Cast::FunctionPtr >( os.str() );
        }
        break;
        case operators::eActive:
        {
            std::ostringstream os;
            symbolPrefix( "mega_cast_", target, os );
            os << "RKN4mega9referenceE";
            *ppFunction = ( void* )pModule->get< operators::Active::FunctionPtr >( os.str() );
        }
        break;
        case operators::eStopped:
        {
            std::ostringstream os;
            symbolPrefix( "mega_cast_", target, os );
            os << "RKN4mega9referenceE";
            *ppFunction = ( void* )pModule->get< operators::Stopped::FunctionPtr >( os.str() );
        }
        break;
        case operators::TOTAL_FUNCTION_TYPES:
        {
            THROW_RTE( "Unsupported object function" );
        }
        break;
    }
}
} // namespace mega::runtime
