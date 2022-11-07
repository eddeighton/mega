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
#include "code_generator.hpp"
#include "allocator.hpp"

#include "database/database.hpp"

#include "service/protocol/common/project.hpp"

#include "mega/snapshot.hpp"

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

    network::SizeAlignment getRootSize() const;
    network::SizeAlignment getSize( TypeID typeID ) const;

    void getObjectSharedAlloc( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                               const TypeID& objectTypeID, runtime::SharedCtorFunction* ppFunction );
    void getObjectSharedDel( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                             const TypeID& objectTypeID, runtime::SharedDtorFunction* ppFunction );
    void getObjectHeapAlloc( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                             const TypeID& objectTypeID, runtime::HeapCtorFunction* ppFunction );
    void getObjectHeapDel( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                           const TypeID& objectTypeID, runtime::HeapDtorFunction* ppFunction );
    void getObjectSaveXML( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                           const TypeID& objectTypeID, runtime::SaveObjectFunction* ppFunction );
    void getObjectLoadXML( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                           const TypeID& objectTypeID, runtime::LoadObjectFunction* ppFunction );
    void getObjectSaveBin( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                           const TypeID& objectTypeID, runtime::SaveObjectFunction* ppFunction );
    void getObjectLoadBin( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                           const TypeID& objectTypeID, runtime::LoadObjectFunction* ppFunction );

    void getCallGetter( const char* pszUnitName, TypeID objectTypeID, TypeErasedFunction* ppFunction );

    void getAllocate( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                      const InvocationID& invocationID, AllocateFunction* ppFunction );
    void getRead( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                  const InvocationID& invocationID, ReadFunction* ppFunction );
    void getWrite( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                   const InvocationID& invocationID, WriteFunction* ppFunction );
    void getCall( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                  const InvocationID& invocationID, CallFunction* ppFunction );
    void getStart( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                   const InvocationID& invocationID, StartFunction* ppFunction );
    void getStop( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                  const InvocationID& invocationID, StopFunction* ppFunction );
    void getSave( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                  const InvocationID& invocationID, SaveFunction* ppFunction );
    void getLoad( const CodeGenerator::LLVMCompiler& compiler, const char* pszUnitName,
                  const InvocationID& invocationID, LoadFunction* ppFunction );

    //void load( const CodeGenerator::LLVMCompiler& compiler, const reference& root, const Snapshot& snapshot,
    //           bool bLoadShared, void* pSharedBase );

private:
    const Allocator&         getAllocator( const CodeGenerator::LLVMCompiler& compiler, const TypeID& objectTypeID );
    JITCompiler::Module::Ptr compile( const std::string& strCode );

    const network::MegastructureInstallation m_megastructureInstallation;
    const network::Project                   m_project;

    JITCompiler      m_jitCompiler;
    DatabaseInstance m_database;
    CodeGenerator    m_codeGenerator;
    ComponentManager m_componentManager;

    using AllocatorMap = std::map< TypeID, Allocator::Ptr >;
    AllocatorMap m_allocators;

    using InvocationMap = std::map< InvocationID, JITCompiler::Module::Ptr >;
    InvocationMap m_invocations;

    using FunctionPtrMap = std::multimap< const char*, void* >;
    FunctionPtrMap m_functionPointers;
};

} // namespace mega::runtime

#endif // MEGA_RUNTIME_SEPT_19_2022
