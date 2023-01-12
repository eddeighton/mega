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

#include "service/network/log.hpp"

#include <utility>

namespace mega::runtime
{
JIT::JIT( const mega::network::MegastructureInstallation& megastructureInstallation,
          const mega::network::Project&                   project )
    : m_megastructureInstallation( megastructureInstallation )
    , m_project( project )
    , m_jitCompiler()
    , m_database( m_project.getProjectDatabase() )
    , m_codeGenerator( m_megastructureInstallation, m_project )
    , m_componentManager( m_project, m_database )
{
    VERIFY_RTE_MSG( !m_project.isEmpty(), "Empty project" );
}

JITCompiler::Module::Ptr JIT::compile( const std::string& strCode )
{
    auto                     startTime = std::chrono::steady_clock::now();
    JITCompiler::Module::Ptr pModule   = m_jitCompiler.compile( strCode );
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
            m_codeGenerator.generate_allocation( compiler, m_database, objectTypeID, osModule );
            JITCompiler::Module::Ptr pModule = compile( osModule.str() );
            pAllocator                       = std::make_shared< Allocator >( objectTypeID, m_database, pModule );
            m_allocators.insert( { objectTypeID, pAllocator } );
        }
    }
    return pAllocator;
}

void JIT::getProgramFunction( void* pLLVMCompiler, int fType, void** ppFunction )
{
    SPDLOG_TRACE( "JIT: getProgramFunction: {}", fType );

    const CodeGenerator::LLVMCompiler& compiler
        = *reinterpret_cast< const CodeGenerator::LLVMCompiler* >( pLLVMCompiler );

    m_programFunctionPointers.insert( ppFunction );

    if( !m_pProgram )
    {
        std::ostringstream osModule;
        m_codeGenerator.generate_program( compiler, m_database, osModule );
        auto pModule = compile( osModule.str() );
        m_pProgram   = std::make_unique< Program >( m_database, pModule );
    }

    const auto functionType = static_cast< mega::runtime::program::FunctionType >( fType );
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
        case program::TOTAL_FUNCTION_TYPES:
        {
            THROW_RTE( "Unsupported program function" );
        }
        break;
    }
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

    const auto functionType = static_cast< mega::runtime::invocation::FunctionType >( fType );
    switch( functionType )
    {
        case invocation::eRead:
        {
            if( !pModule )
            {
                std::ostringstream osModule;
                m_codeGenerator.generate_read( compiler, m_database, invocationID, osModule );
                pModule = compile( osModule.str() );
                m_invocations.insert( std::make_pair( invocationID, pModule ) );
            }
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceE";
            *ppFunction = ( void* )pModule->get< invocation::Read::FunctionPtr >( os.str() );
        }
        break;
        case invocation::eWrite:
        {
            if( !pModule )
            {
                std::ostringstream osModule;
                m_codeGenerator.generate_write( compiler, m_database, invocationID, osModule );
                pModule = compile( osModule.str() );
                m_invocations.insert( std::make_pair( invocationID, pModule ) );
            }
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceEPKv";
            *ppFunction = ( void* )pModule->get< invocation::Write::FunctionPtr >( os.str() );
        }
        break;
        case invocation::eAllocate:
        {
            if( !pModule )
            {
                std::ostringstream osModule;
                m_codeGenerator.generate_allocate( compiler, m_database, invocationID, osModule );
                pModule = compile( osModule.str() );
                m_invocations.insert( std::make_pair( invocationID, pModule ) );
            }
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceE";
            *ppFunction = ( void* )pModule->get< invocation::Allocate::FunctionPtr >( os.str() );
        }
        break;
        case invocation::eCall:
        {
            if( !pModule )
            {
                std::ostringstream osModule;
                m_codeGenerator.generate_call( compiler, m_database, invocationID, osModule );
                pModule = compile( osModule.str() );
                m_invocations.insert( std::make_pair( invocationID, pModule ) );
            }
            std::ostringstream os;
            symbolPrefix( invocationID, os );
            os << "N4mega9referenceE";
            *ppFunction = ( void* )pModule->get< invocation::Call::FunctionPtr >( os.str() );
        }
        break;
        case invocation::TOTAL_FUNCTION_TYPES:
        {
            THROW_RTE( "Unsupported invocation function" );
        }
        break;
    }
}

void JIT::getObjectFunction( void* pLLVMCompiler, const char* pszUnitName, const mega::TypeID& typeID, int fType,
                             void** ppFunction )
{
    SPDLOG_TRACE( "JIT::getObjectFunction : {} {} {}", pszUnitName, typeID, fType );
    const CodeGenerator::LLVMCompiler& compiler
        = *reinterpret_cast< const CodeGenerator::LLVMCompiler* >( pLLVMCompiler );

    const auto functionType = static_cast< mega::runtime::object::FunctionType >( fType );
    switch( functionType )
    {
        case mega::runtime::object::eObjectLoadBin:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getLoadBin();
        }
        break;
        case mega::runtime::object::eObjectSaveBin:
        {
            auto pAllocator = getAllocator( compiler, typeID );
            *ppFunction     = ( void* )pAllocator->getSaveBin();
        }
        break;
        case mega::runtime::object::eCallGetter:
        {
            m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
            *ppFunction = ( void* )m_componentManager.getOperationFunctionPtr( typeID );
        }
        break;
        case mega::runtime::object::TOTAL_FUNCTION_TYPES:
        {
            THROW_RTE( "Unsupported object function" );
        }
        break;
    }
}

} // namespace mega::runtime
