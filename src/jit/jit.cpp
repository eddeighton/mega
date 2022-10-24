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
    // SPDLOG_TRACE( "RUNTIME: JIT Compilation time: {}", std::chrono::steady_clock::now() - startTime );
    return pModule;
}

const Allocator& JIT::getAllocator( const CodeGenerator::LLVMCompiler& compiler, const mega::TypeID& objectTypeID )
{
    Allocator* pAllocator;
    {
        auto iFind = m_allocators.find( objectTypeID );
        if ( iFind != m_allocators.end() )
        {
            pAllocator = iFind->second.get();
        }
        else
        {
            std::ostringstream osModule;
            m_codeGenerator.generate_allocation( compiler, m_database, objectTypeID, osModule );
            JITCompiler::Module::Ptr pModule = compile( osModule.str() );
            Allocator::Ptr           pAlloc  = std::make_unique< Allocator >( objectTypeID, m_database, pModule );
            pAllocator                       = pAlloc.get();
            m_allocators.insert( { objectTypeID, std::move( pAlloc ) } );
        }
    }
    return *pAllocator;
}

void JIT::get_call_getter( const char* pszUnitName, mega::TypeID objectTypeID, TypeErasedFunction* ppFunction )
{
    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    *ppFunction = m_componentManager.getOperationFunctionPtr( objectTypeID );
}

network::SizeAlignment JIT::getRootSize() const { return m_database.getObjectSize( ROOT_TYPE_ID ); }

network::SizeAlignment JIT::getSize( TypeID typeID ) const { return m_database.getObjectSize( typeID ); }

void JIT::getObjectSharedAlloc( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                                const mega::TypeID& objectTypeID, mega::runtime::SharedCtorFunction* ppFunction )
{
    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    *ppFunction = getAllocator( compiler, objectTypeID ).getSharedCtor();
}

void JIT::getObjectSharedDel( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                              const mega::TypeID& objectTypeID, mega::runtime::SharedDtorFunction* ppFunction )
{
    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    *ppFunction = getAllocator( compiler, objectTypeID ).getSharedDtor();
}

void JIT::getObjectHeapAlloc( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                              const mega::TypeID& objectTypeID, mega::runtime::HeapCtorFunction* ppFunction )
{
    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    *ppFunction = getAllocator( compiler, objectTypeID ).getHeapCtor();
}

void JIT::getObjectHeapDel( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                            const mega::TypeID& objectTypeID, mega::runtime::HeapDtorFunction* ppFunction )
{
    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    *ppFunction = getAllocator( compiler, objectTypeID ).getHeapDtor();
}

void JIT::get_allocate( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                        const mega::InvocationID& invocationID, AllocateFunction* ppFunction )
{
    // SPDLOG_TRACE( "RUNTIME: get_allocate: {} {}", pszUnitName, invocationID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if ( iFind != m_invocations.end() )
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

void JIT::get_read( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                    const mega::InvocationID& invocationID, ReadFunction* ppFunction )
{
    // SPDLOG_TRACE( "RUNTIME: get_read: {} {}", pszUnitName, invocationID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if ( iFind != m_invocations.end() )
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

void JIT::get_write( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                     const mega::InvocationID& invocationID, WriteFunction* ppFunction )
{
    // SPDLOG_TRACE( "RUNTIME: get_write: {} {}", pszUnitName, invocationID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if ( iFind != m_invocations.end() )
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

void JIT::get_call( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                    const mega::InvocationID& invocationID, CallFunction* ppFunction )
{
    // SPDLOG_TRACE( "RUNTIME: get_call: {} {}", pszUnitName, invocationID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if ( iFind != m_invocations.end() )
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

void JIT::get_start( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                     const mega::InvocationID& invocationID, StartFunction* ppFunction )
{
    // SPDLOG_TRACE( "RUNTIME: get_start: {} {}", pszUnitName, invocationID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if ( iFind != m_invocations.end() )
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

void JIT::get_stop( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                    const mega::InvocationID& invocationID, StopFunction* ppFunction )
{
    // SPDLOG_TRACE( "RUNTIME: get_stop: {} {}", pszUnitName, invocationID );

    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_invocations.find( invocationID );
        if ( iFind != m_invocations.end() )
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

} // namespace mega::runtime
