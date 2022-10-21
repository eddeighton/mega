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

#include "orc.hpp"
#include "component_manager.hpp"
#include "database.hpp"
#include "code_generator.hpp"
#include "allocator.hpp"

#include "service/protocol/common/project.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include <array>
#include <unordered_map>

namespace mega::runtime
{

class JIT
{
    friend class ObjectTypeAllocator;

public:
    JIT( const network::MegastructureInstallation& megastructureInstallation, const network::Project& project );

    void getObjectSharedAlloc( const char*                        pszUnitName,
                               const mega::TypeID&                objectTypeID,
                               mega::runtime::SharedCtorFunction* ppFunction );
    void getObjectSharedDel( const char*                        pszUnitName,
                             const mega::TypeID&                objectTypeID,
                             mega::runtime::SharedDtorFunction* ppFunction );
    void getObjectHeapAlloc( const char*                      pszUnitName,
                             const mega::TypeID&              objectTypeID,
                             mega::runtime::HeapCtorFunction* ppFunction );
    void getObjectHeapDel( const char*                      pszUnitName,
                           const mega::TypeID&              objectTypeID,
                           mega::runtime::HeapDtorFunction* ppFunction );

    void get_call_getter( const char* pszUnitName, TypeID objectTypeID, TypeErasedFunction* ppFunction );
    void get_allocate( const char* pszUnitName, const InvocationID& invocationID, AllocateFunction* ppFunction );
    void get_read( const char* pszUnitName, const InvocationID& invocationID, ReadFunction* ppFunction );
    void get_write( const char* pszUnitName, const InvocationID& invocationID, WriteFunction* ppFunction );
    void get_call( const char* pszUnitName, const InvocationID& invocationID, CallFunction* ppFunction );
    void get_start( const char* pszUnitName, const mega::InvocationID& invocationID, StartFunction* ppFunction );
    void get_stop( const char* pszUnitName, const mega::InvocationID& invocationID, StopFunction* ppFunction );

private:
    const Allocator&         getAllocator( const mega::TypeID& objectTypeID );
    JITCompiler::Module::Ptr compile( const std::string& strCode );

    const network::MegastructureInstallation m_megastructureInstallation;
    const network::Project                   m_project;

    JITCompiler      m_jitCompiler;
    CodeGenerator    m_codeGenerator;
    DatabaseInstance m_database;
    ComponentManager m_componentManager;

    using AllocatorMap = std::map< TypeID, Allocator::Ptr >;
    AllocatorMap m_allocators;

    using InvocationMap = std::map< InvocationID, JITCompiler::Module::Ptr >;
    InvocationMap m_invocations;

    /*using AllocationsMap = std::map< TypeID, JITCompiler::Module::Ptr >;
    AllocationsMap m_allocations;*/

    using FunctionPtrMap = std::multimap< const char*, void* >;
    FunctionPtrMap m_functionPointers;
};

} // namespace mega::runtime

#endif // MEGA_RUNTIME_SEPT_19_2022
