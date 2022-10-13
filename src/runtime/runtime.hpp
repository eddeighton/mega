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

#ifndef MEGA_RUNTIME_SEPT_19_2022
#define MEGA_RUNTIME_SEPT_19_2022

#include "object_allocator.hpp"

#include "runtime/api.hpp"
#include "runtime/context.hpp"

#include "address_space.hpp"
#include "jit.hpp"
#include "component_manager.hpp"
#include "database.hpp"
#include "code_generator.hpp"

#include "service/protocol/common/project.hpp"
#include "service/network/log.hpp"

#include "mega/reference.hpp"
#include "mega/common.hpp"
#include "mega/default_traits.hpp"

#include "common/assert_verify.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include <array>
#include <unordered_map>

namespace mega::runtime
{

class Runtime
{
    friend class ObjectTypeAllocator;

    // NetworkAddress -> MachineAddress
    using ObjectTypeAllocatorMap = std::unordered_map< TypeID, ObjectTypeAllocator::Ptr >;

    using MPOSharedMemory    = runtime::ManagedSharedMemory;
    using MPOSharedMemoryPtr = std::unique_ptr< MPOSharedMemory >;
    ManagedSharedMemory& getSharedMemoryManager( MPO mpo );

    using MPOContextMemoryMap = std::unordered_map< MPO, MPOSharedMemoryPtr, MPO::Hash >;
    using MPOContextRoot      = std::unordered_map< MPO, reference, MPO::Hash >;

    ObjectTypeAllocator::Ptr getOrCreateObjectTypeAllocator( TypeID objectTypeID );

public:
    Runtime( const network::MegastructureInstallation& megastructureInstallation,
             const network::Project&                   project,
             const AddressSpace::Names&                addressSpaceNames );

    NetworkAddress allocateNetworkAddress( MPO mpo, TypeID objectTypeID );
    reference      allocateMachineAddress( MPO mpo, TypeID objectTypeID, NetworkAddress networkAddress );
    reference      networkToMachine( TypeID objectTypeID, NetworkAddress networkAddress );

    reference getRoot( const MPO& mpo );
    void      deAllocateRoot( const MPO& mpo );

    void onWrite( reference dimension );

    void get_getter_shared( const char* pszUnitName, TypeID objectTypeID, GetSharedFunction* ppFunction );
    void get_getter_heap( const char* pszUnitName, TypeID objectTypeID, GetSharedFunction* ppFunction );
    void get_getter_call( const char* pszUnitName, TypeID objectTypeID, TypeErasedFunction* ppFunction );

    JITCompiler::Module::Ptr compile( const std::string& strCode );

    JITCompiler::Module::Ptr get_allocation( TypeID objectTypeID );
    void get_allocate( const char* pszUnitName, const InvocationID& invocationID, AllocateFunction* ppFunction );
    void get_read( const char* pszUnitName, const InvocationID& invocationID, ReadFunction* ppFunction );
    void get_write( const char* pszUnitName, const InvocationID& invocationID, WriteFunction* ppFunction );
    void get_call( const char* pszUnitName, const InvocationID& invocationID, CallFunction* ppFunction );
    void get_start( const char* pszUnitName, const mega::InvocationID& invocationID, StartFunction* ppFunction );
    void get_stop( const char* pszUnitName, const mega::InvocationID& invocationID, StopFunction* ppFunction );

private:
    const network::MegastructureInstallation m_megastructureInstallation;
    const network::Project                   m_project;
    std::recursive_mutex                     m_jitMutex;

    JITCompiler      m_jitCompiler;
    CodeGenerator    m_codeGenerator;
    DatabaseInstance m_database;
    ComponentManager m_componentManager;
    AddressSpace     m_addressSpace;

    // these three are order dependent - m_executionContextMemory owns the shared memory
    MPOContextMemoryMap    m_executionContextMemory;
    ObjectTypeAllocatorMap m_objectTypeAllocatorMapping;
    MPOContextRoot         m_executionContextRoot;

    // use unordered_map
    using InvocationMap = std::map< InvocationID, JITCompiler::Module::Ptr >;
    InvocationMap m_invocations;

    using AllocationsMap = std::map< TypeID, JITCompiler::Module::Ptr >;
    AllocationsMap m_allocations;

    using FunctionPtrMap = std::multimap< const char*, void* >;
    FunctionPtrMap m_functionPointers;
};

} // namespace mega::runtime

#endif // MEGA_RUNTIME_SEPT_19_2022
