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

#include "service/network/log.hpp"

#include "mega/bin_archive.hpp"

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

const Allocator& JIT::getAllocatorRef( const CodeGenerator::LLVMCompiler& compiler, const mega::TypeID& objectTypeID )
{
    return *getAllocator( compiler, objectTypeID );
}

void JIT::getCallGetter( const char* pszUnitName, mega::TypeID objectTypeID, TypeErasedFunction* ppFunction )
{
    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    *ppFunction = m_componentManager.getOperationFunctionPtr( objectTypeID );
}

void JIT::getObjectCtor( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                         const mega::TypeID& objectTypeID, mega::runtime::CtorFunction* ppFunction )
{
    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    *ppFunction = getAllocatorRef( compiler, objectTypeID ).getCtor();
}

void JIT::getObjectDtor( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                         const mega::TypeID& objectTypeID, mega::runtime::DtorFunction* ppFunction )
{
    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    *ppFunction = getAllocatorRef( compiler, objectTypeID ).getDtor();
}

void JIT::getObjectSaveXML( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                            const mega::TypeID& objectTypeID, mega::runtime::SaveObjectFunction* ppFunction )
{
    SPDLOG_TRACE( "JIT: getObjectSaveXML: {} {}", pszUnitName, objectTypeID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    *ppFunction = getAllocatorRef( compiler, objectTypeID ).getSaveXML();
}

void JIT::getObjectLoadXML( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                            const mega::TypeID& objectTypeID, mega::runtime::LoadObjectFunction* ppFunction )
{
    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    *ppFunction = getAllocatorRef( compiler, objectTypeID ).getLoadXML();
}

void JIT::getObjectSaveBin( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                            const mega::TypeID& objectTypeID, mega::runtime::SaveObjectFunction* ppFunction )
{
    SPDLOG_TRACE( "JIT: getObjectSaveBin: {} {}", pszUnitName, objectTypeID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    *ppFunction = getAllocatorRef( compiler, objectTypeID ).getSaveBin();
}

void JIT::getObjectLoadBin( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                            const mega::TypeID& objectTypeID, mega::runtime::LoadObjectFunction* ppFunction )
{
    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    *ppFunction = getAllocatorRef( compiler, objectTypeID ).getLoadBin();
}

void JIT::getAllocate( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                       const mega::InvocationID& invocationID, AllocateFunction* ppFunction )
{
    // SPDLOG_TRACE( "JIT: getAllocate: {} {}", pszUnitName, invocationID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if( iFind != m_invocations.end() )
        {
            pModule = iFind->second;
        }
        else
        {
            std::ostringstream osModule;
            m_codeGenerator.generate_allocate( compiler, m_database, invocationID, osModule );
            pModule = compile( osModule.str() );
            m_invocations.insert( std::make_pair( invocationID, pModule ) );
        }
    }
    std::ostringstream os;
    symbolPrefix( invocationID, os );
    os << "N4mega9referenceE";
    *ppFunction = pModule->get< AllocateFunction >( os.str() );
}

void JIT::getRead( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                   const mega::InvocationID& invocationID, ReadFunction* ppFunction )
{
    // SPDLOG_TRACE( "JIT: getRead: {} {}", pszUnitName, invocationID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if( iFind != m_invocations.end() )
        {
            pModule = iFind->second;
        }
        else
        {
            std::ostringstream osModule;
            m_codeGenerator.generate_read( compiler, m_database, invocationID, osModule );
            pModule = compile( osModule.str() );
            m_invocations.insert( std::make_pair( invocationID, pModule ) );
        }
    }
    std::ostringstream os;
    symbolPrefix( invocationID, os );
    os << "N4mega9referenceE";
    *ppFunction = pModule->get< ReadFunction >( os.str() );
}

void JIT::getWrite( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                    const mega::InvocationID& invocationID, WriteFunction* ppFunction )
{
    // SPDLOG_TRACE( "JIT: getWrite: {} {}", pszUnitName, invocationID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if( iFind != m_invocations.end() )
        {
            pModule = iFind->second;
        }
        else
        {
            std::ostringstream osModule;
            m_codeGenerator.generate_write( compiler, m_database, invocationID, osModule );
            pModule = compile( osModule.str() );
            m_invocations.insert( std::make_pair( invocationID, pModule ) );
        }
    }

    std::ostringstream os;
    symbolPrefix( invocationID, os );
    os << "N4mega9referenceEPKv";
    *ppFunction = pModule->get< WriteFunction >( os.str() );
}

void JIT::getCall( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                   const mega::InvocationID& invocationID, CallFunction* ppFunction )
{
    // SPDLOG_TRACE( "JIT: getCall: {} {}", pszUnitName, invocationID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if( iFind != m_invocations.end() )
        {
            pModule = iFind->second;
        }
        else
        {
            std::ostringstream osModule;
            m_codeGenerator.generate_call( compiler, m_database, invocationID, osModule );
            pModule = compile( osModule.str() );
            m_invocations.insert( std::make_pair( invocationID, pModule ) );
        }
    }
    std::ostringstream os;
    symbolPrefix( invocationID, os );
    os << "N4mega9referenceE";
    *ppFunction = pModule->get< CallFunction >( os.str() );
}

void JIT::getStart( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                    const mega::InvocationID& invocationID, StartFunction* ppFunction )
{
    // SPDLOG_TRACE( "JIT: getStart: {} {}", pszUnitName, invocationID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if( iFind != m_invocations.end() )
        {
            pModule = iFind->second;
        }
        else
        {
            std::ostringstream osModule;
            m_codeGenerator.generate_start( compiler, m_database, invocationID, osModule );
            pModule = compile( osModule.str() );
            m_invocations.insert( std::make_pair( invocationID, pModule ) );
        }
    }
    std::ostringstream os;
    symbolPrefix( invocationID, os );
    os << "N4mega9referenceE";
    *ppFunction = pModule->get< StartFunction >( os.str() );
}

void JIT::getStop( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                   const mega::InvocationID& invocationID, StopFunction* ppFunction )
{
    // SPDLOG_TRACE( "JIT: getStop: {} {}", pszUnitName, invocationID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if( iFind != m_invocations.end() )
        {
            pModule = iFind->second;
        }
        else
        {
            std::ostringstream osModule;
            m_codeGenerator.generate_stop( compiler, m_database, invocationID, osModule );
            pModule = compile( osModule.str() );
            m_invocations.insert( std::make_pair( invocationID, pModule ) );
        }
    }
    std::ostringstream os;
    symbolPrefix( invocationID, os );
    os << "N4mega9referenceE";
    *ppFunction = pModule->get< StopFunction >( os.str() );
}

void JIT::getSave( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                   const mega::InvocationID& invocationID, SaveFunction* ppFunction )
{
    SPDLOG_TRACE( "JIT: getSave: {} {}", pszUnitName, invocationID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if( iFind != m_invocations.end() )
        {
            pModule = iFind->second;
        }
        else
        {
            std::ostringstream osModule;
            m_codeGenerator.generate_save( compiler, m_database, invocationID, osModule );
            pModule = compile( osModule.str() );
            m_invocations.insert( std::make_pair( invocationID, pModule ) );
        }
    }
    std::ostringstream os;
    symbolPrefix( invocationID, os );
    os << "N4mega9referenceEPv";
    *ppFunction = pModule->get< SaveFunction >( os.str() );
}

void JIT::getLoad( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                   const mega::InvocationID& invocationID, LoadFunction* ppFunction )
{
    // SPDLOG_TRACE( "JIT: getLoad: {} {}", pszUnitName, invocationID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if( iFind != m_invocations.end() )
        {
            pModule = iFind->second;
        }
        else
        {
            std::ostringstream osModule;
            m_codeGenerator.generate_load( compiler, m_database, invocationID, osModule );
            pModule = compile( osModule.str() );
            m_invocations.insert( std::make_pair( invocationID, pModule ) );
        }
    }
    std::ostringstream os;
    symbolPrefix( invocationID, os );
    os << "N4mega9referenceEPv";
    *ppFunction = pModule->get< LoadFunction >( os.str() );
}
/*
void JIT::load( const CodeGenerator::LLVMCompiler& compiler, const reference& root, const Snapshot& snapshot,
            bool bLoadShared, void* pSharedBase )
{
    SPDLOG_TRACE( "JIT: load: {} {}", root, bLoadShared );

    auto pAlloc = getAllocatorRef( compiler, root.type );

    BinLoadArchive archive( snapshot );
    pAlloc.getLoadBin()( root, &archive, bLoadShared );
}
*/

} // namespace mega::runtime
