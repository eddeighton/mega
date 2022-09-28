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


#include "runtime.hpp"
#include "symbol_utils.hpp"

namespace mega
{
namespace runtime
{
Runtime::Runtime( const mega::network::MegastructureInstallation& megastructureInstallation,
                  const mega::network::Project&                   project )
    : m_megastructureInstallation( megastructureInstallation )
    , m_project( project )
    , m_jitMutex()
    , m_jitCompiler()
    , m_codeGenerator( m_megastructureInstallation, m_project )
    , m_database( m_project.getProjectDatabase() )
    , m_componentManager( m_project, m_database )
    , m_addressSpace()
    , m_objectTypeAllocatorMapping()
    , m_executionContextRoot()
    , m_executionContextMemory()
{
    VERIFY_RTE_MSG( !m_project.isEmpty(), "Empty project" );
}

ManagedSharedMemory& Runtime::getSharedMemoryManager( mega::MPO mpo )
{
    auto iFind = m_executionContextMemory.find( mpo );
    if ( iFind != m_executionContextMemory.end() )
    {
        return *iFind->second;
    }
    else
    {
        MPOContext* pMPOContext = MPOContext::get();
        VERIFY_RTE( pMPOContext );
        const std::string  strMemoryName = pMPOContext->acquireMemory( mpo );
        MPOSharedMemoryPtr pMemoryPtr
            = std::make_unique< MPOSharedMemory >( boost::interprocess::open_only, strMemoryName.c_str() );
        MPOSharedMemory* pMemory = pMemoryPtr.get();
        m_executionContextMemory.insert( { mpo, std::move( pMemoryPtr ) } );
        return *pMemory;
    }
}

NetworkAddress Runtime::allocateNetworkAddress( MPO mpo, TypeID objectTypeID )
{
    SPDLOG_TRACE( "RUNTIME: allocateNetworkAddress: {} {}", mpo, objectTypeID );

    MPOContext* pMPOContext = MPOContext::get();
    VERIFY_RTE( pMPOContext );
    return pMPOContext->allocateNetworkAddress( mpo, objectTypeID );
}

ObjectTypeAllocator::Ptr Runtime::getOrCreateObjectTypeAllocator( TypeID objectTypeID )
{
    SPDLOG_TRACE( "RUNTIME: getOrCreateObjectTypeAllocator: {}", objectTypeID );

    auto jFind = m_objectTypeAllocatorMapping.find( objectTypeID );
    if ( jFind != m_objectTypeAllocatorMapping.end() )
    {
        return jFind->second;
    }
    else
    {
        ObjectTypeAllocator::Ptr pAllocator = std::make_shared< ObjectTypeAllocator >( *this, objectTypeID );
        m_objectTypeAllocatorMapping.insert( { objectTypeID, pAllocator } );
        return pAllocator;
    }
}

reference Runtime::allocateMachineAddress( MPO mpo, TypeID objectTypeID, NetworkAddress networkAddress )
{
    SPDLOG_TRACE( "RUNTIME: allocateMachineAddress: {} {} {}", mpo, objectTypeID, networkAddress );

    MPOContext* pMPOContext = MPOContext::get();
    VERIFY_RTE( pMPOContext );
    if ( pMPOContext->getThisMPO() != mpo )
    {
        // request write lock
        if ( !pMPOContext->writeLock( mpo ) )
        {
            SPDLOG_ERROR( "RUNTIME: allocateMachineAddress Failed to acquire write lock on: {}", mpo );
            return reference{};
        }
    }

    // AddressSpace::Lock lock( m_addressSpace.mutex() );

    ObjectTypeAllocator::Ptr pAllocator = getOrCreateObjectTypeAllocator( objectTypeID );

    const MPO       thisMPO = pMPOContext->getThisMPO();
    const reference ref     = pAllocator->allocate( mpo );

    if ( thisMPO.getMachineID() != mpo.getMachineID() )
    {
        // allocate the object locally and then acquire over network
    }
    else if ( thisMPO.getProcessID() != mpo.getProcessID() )
    {
    }

    VERIFY_RTE( m_addressSpace.insert( networkAddress, ref ) );
    return ref;
}

reference Runtime::networkToMachine( TypeID objectTypeID, NetworkAddress networkAddress )
{
    // AddressSpace::Lock lock( m_addressSpace.mutex() );

    auto resultOpt = m_addressSpace.find( networkAddress );
    if ( resultOpt.has_value() )
    {
        // ensure the object type allocator is established
        return resultOpt.value();
    }
    else
    {
        MPOContext* pMPOContext = MPOContext::get();
        VERIFY_RTE( pMPOContext );
        const MPO thisMPO = pMPOContext->getThisMPO();
        const MPO mpo     = pMPOContext->getNetworkAddressMPO( networkAddress );

        if ( pMPOContext->getThisMPO() != mpo )
        {
            // request write lock
            if ( !pMPOContext->writeLock( mpo ) )
            {
                SPDLOG_ERROR( "RUNTIME: allocateMachineAddress Failed to acquire write lock on: {}", mpo );
                return reference{};
            }
        }

        ObjectTypeAllocator::Ptr pAllocator = getOrCreateObjectTypeAllocator( objectTypeID );
        const reference ref = pAllocator->allocate( mpo );

        if ( thisMPO.getMachineID() != mpo.getMachineID() )
        {
            // allocate the object locally and then acquire over network
        }
        else if ( thisMPO.getProcessID() != mpo.getProcessID() )
        {
        }

        VERIFY_RTE( m_addressSpace.insert( networkAddress, ref ) );
        return ref;
    }
}

mega::reference Runtime::getRoot( const mega::MPO& mpo )
{
    SPDLOG_TRACE( "RUNTIME: getRoot: {}", mpo );

    auto iFind = m_executionContextRoot.find( mpo );
    if ( iFind != m_executionContextRoot.end() )
    {
        return iFind->second;
    }
    else
    {
        MPOContext* pMPOContext = MPOContext::get();
        VERIFY_RTE( pMPOContext );
        const MPO thisMPO = pMPOContext->getThisMPO();
        SPDLOG_TRACE( "RUNTIME: getRoot: {} from {}", mpo, thisMPO );

        if ( mpo != thisMPO )
        {
            const NetworkAddress rootNetworkAddress = pMPOContext->getRootNetworkAddress( mpo );
            const reference      ref( TypeInstance{ 0U, ROOT_TYPE_ID }, rootNetworkAddress );
            m_executionContextRoot.insert( { mpo, ref } );
            return ref;
        }
        else
        {
            const NetworkAddress rootNetworkAddress = pMPOContext->allocateNetworkAddress( mpo, ROOT_TYPE_ID );
            const reference      ref( TypeInstance{ 0U, ROOT_TYPE_ID }, rootNetworkAddress );
            m_executionContextRoot.insert( { mpo, ref } );
            return ref;
        }
    }
}

void Runtime::deAllocateRoot( const mega::MPO& mpo )
{
    SPDLOG_TRACE( "RUNTIME: deAllocateRoot: {}", mpo );
    auto iFind = m_executionContextRoot.find( mpo );
    if ( iFind != m_executionContextRoot.end() )
    {
        mega::reference& ref = iFind->second;
        if ( ref.isNetwork() )
            ref = networkToMachine( ref.type, ref.network );
        ObjectTypeAllocator::Ptr pAllocator = getOrCreateObjectTypeAllocator( ref.type );
        pAllocator->deAllocate( ref );
        m_executionContextRoot.erase( iFind );
    }
}

void Runtime::get_getter_shared( const char* pszUnitName, mega::TypeID objectTypeID, GetSharedFunction* ppFunction )
{
    std::lock_guard< std::recursive_mutex > lock( m_jitMutex );
    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    ObjectTypeAllocator::Ptr pAllocator = getOrCreateObjectTypeAllocator( objectTypeID );
    *ppFunction                         = pAllocator->getSharedGetter();
}

void Runtime::get_getter_heap( const char* pszUnitName, mega::TypeID objectTypeID, GetSharedFunction* ppFunction )
{
    std::lock_guard< std::recursive_mutex > lock( m_jitMutex );
    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    ObjectTypeAllocator::Ptr pAllocator = getOrCreateObjectTypeAllocator( objectTypeID );
    *ppFunction                         = pAllocator->getHeapGetter();
}

void Runtime::get_getter_call( const char* pszUnitName, mega::TypeID objectTypeID, TypeErasedFunction* ppFunction )
{
    std::lock_guard< std::recursive_mutex > lock( m_jitMutex );
    m_functionPointers.insert( std::make_pair( pszUnitName, ppFunction ) );
    *ppFunction = m_componentManager.getOperationFunctionPtr( objectTypeID );
}

JITCompiler::Module::Ptr Runtime::compile( const std::string& strCode )
{
    auto                     startTime = std::chrono::steady_clock::now();
    JITCompiler::Module::Ptr pModule   = m_jitCompiler.compile( strCode );
    SPDLOG_TRACE( "RUNTIME: JIT Compilation time: {}", std::chrono::steady_clock::now() - startTime );
    return pModule;
}

JITCompiler::Module::Ptr Runtime::get_allocation( mega::TypeID objectTypeID )
{
    std::lock_guard< std::recursive_mutex > lock( m_jitMutex );

    SPDLOG_TRACE( "RUNTIME: get_allocation: {}", objectTypeID );

    JITCompiler::Module::Ptr pModule;
    {
        auto iFind = m_allocations.find( objectTypeID );
        if ( iFind != m_allocations.end() )
        {
            pModule = iFind->second;
        }
        else
        {
            std::ostringstream osModule;
            m_codeGenerator.generate_allocation( m_database, objectTypeID, osModule );
            pModule = compile( osModule.str() );
            m_allocations.insert( std::make_pair( objectTypeID, pModule ) );
        }
    }
    return pModule;
}

void Runtime::get_allocate( const char*               pszUnitName,
                            const mega::InvocationID& invocationID,
                            AllocateFunction*         ppFunction )
{
    std::lock_guard< std::recursive_mutex > lock( m_jitMutex );

    SPDLOG_TRACE( "RUNTIME: get_allocate: {} {}", pszUnitName, invocationID );

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
            m_codeGenerator.generate_allocate( m_database, invocationID, osModule );
            pModule = compile( osModule.str() );
            m_invocations.insert( std::make_pair( invocationID, pModule ) );
        }
    }
    std::ostringstream os;
    symbolPrefix( invocationID, os );
    os << "N4mega9referenceE";
    *ppFunction = pModule->getAllocate( os.str() );
}

void Runtime::get_read( const char* pszUnitName, const mega::InvocationID& invocationID, ReadFunction* ppFunction )
{
    std::lock_guard< std::recursive_mutex > lock( m_jitMutex );

    SPDLOG_TRACE( "RUNTIME: get_read: {} {}", pszUnitName, invocationID );

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
            m_codeGenerator.generate_read( m_database, invocationID, osModule );
            pModule = compile( osModule.str() );
            m_invocations.insert( std::make_pair( invocationID, pModule ) );
        }
    }
    std::ostringstream os;
    symbolPrefix( invocationID, os );
    os << "N4mega9referenceE";
    *ppFunction = pModule->getRead( os.str() );
}

void Runtime::get_write( const char* pszUnitName, const mega::InvocationID& invocationID, WriteFunction* ppFunction )
{
    std::lock_guard< std::recursive_mutex > lock( m_jitMutex );

    SPDLOG_TRACE( "RUNTIME: get_write: {} {}", pszUnitName, invocationID );

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
            m_codeGenerator.generate_write( m_database, invocationID, osModule );
            pModule = compile( osModule.str() );
            m_invocations.insert( std::make_pair( invocationID, pModule ) );
        }
    }
    std::ostringstream os;
    symbolPrefix( invocationID, os );
    os << "N4mega9referenceE";
    *ppFunction = pModule->getWrite( os.str() );
}

void Runtime::get_call( const char* pszUnitName, const mega::InvocationID& invocationID, CallFunction* ppFunction )
{
    std::lock_guard< std::recursive_mutex > lock( m_jitMutex );

    SPDLOG_TRACE( "RUNTIME: get_call: {} {}", pszUnitName, invocationID );

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
            m_codeGenerator.generate_call( m_database, invocationID, osModule );
            pModule = compile( osModule.str() );
            m_invocations.insert( std::make_pair( invocationID, pModule ) );
        }
    }
    std::ostringstream os;
    symbolPrefix( invocationID, os );
    os << "N4mega9referenceE";
    *ppFunction = pModule->getCall( os.str() );
}
} // namespace runtime
} // namespace mega
