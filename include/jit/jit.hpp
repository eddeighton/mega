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

#include "api.hpp"
#include "orc.hpp"
#include "component_manager.hpp"
#include "code_generator.hpp"
#include "allocator.hpp"
#include "program.hpp"
#include "relation.hpp"

#include "database/database.hpp"
#include "database/model/OperationsStage.hxx"

#include "utilities/project.hpp"
#include "service/protocol/common/jit_base.hpp"

#include "mega/invocation_id.hpp"
#include "mega/relation_id.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include <array>
#include <unordered_map>

namespace mega::runtime
{

class JIT_EXPORT JIT : public JITBase
{
    friend class ObjectTypeAllocator;

public:
    JIT( const MegastructureInstallation& megastructureInstallation, const Project& project );
    JIT( const MegastructureInstallation& megastructureInstallation, const Project& project, JIT& oldJIT );
    ~JIT();

    std::unordered_map< std::string, mega::TypeID > getIdentities() const;

    Allocator::Ptr getAllocator( const CodeGenerator::LLVMCompiler& compiler, const TypeID& objectTypeID );

    virtual InvocationTypeInfo compileInvocationFunction( void* pLLVMCompiler, const char* pszUnitName,
                                                          const mega::InvocationID& invocationID,
                                                          void**                    ppFunction ) override;
    virtual void               getProgramFunction( void* pLLVMCompiler, int functionType, void** ppFunction ) override;
    virtual void               getInvocationFunction( void* pLLVMCompiler, const char* pszUnitName,
                                                      const mega::InvocationID& invocationID, int functionType,
                                                      void** ppFunction ) override;
    virtual void getObjectFunction( void* pLLVMCompiler, const char* pszUnitName, mega::TypeID typeID, int functionType,
                                    void** ppFunction ) override;
    virtual void getRelationFunction( void* pLLVMCompiler, const char* pszUnitName, const RelationID& relationID,
                                      int functionType, void** ppFunction ) override;
    virtual void getActionFunction( mega::TypeID concreteTypeID, void** ppFunction, ActionInfo& actionInfo ) override;
    virtual void getPythonFunction( mega::TypeID interfaceTypeID, void** ppFunction ) override;
    virtual void getOperatorFunction( void* pLLVMCompiler, const char* pszUnitName, TypeID target, int functionType,
                                      void** ppFunction ) override;

private:
    Relation::Ptr getRelation( const CodeGenerator::LLVMCompiler& compiler, const RelationID& relationID );
    Program::Ptr  getProgram( const CodeGenerator::LLVMCompiler& compiler );

    JITCompiler::Module::Ptr compile( const std::string& strCode );

    const MegastructureInstallation m_megastructureInstallation;
    const Project                   m_project;

    JITCompiler                            m_jitCompiler;
    DatabaseInstance                       m_database;
    mega::io::FileStore::Ptr               m_pythonFileStore;
    OperationsStage::Database              m_pythonDatabase;
    FinalStage::Database                   m_pythonDatabaseFinal;
    OperationsStage::Symbols::SymbolTable* m_pPythonSymbolTable;
    CodeGenerator                          m_codeGenerator;
    ComponentManager                       m_componentManager;

    using FunctionPtrSet = std::set< void** >;
    FunctionPtrSet m_functionPointers;

    using AllocatorMap = std::map< TypeID, Allocator::Ptr >;
    AllocatorMap m_allocators;

    using RelationMap = std::map< RelationID, Relation::Ptr >;
    RelationMap m_relations;

    using InvocationMap = std::map< InvocationID, JITCompiler::Module::Ptr >;
    InvocationMap m_invocations;

    Program::Ptr m_pProgram;

    using OperatorID   = std::pair< TypeID, int >;
    using OperatorsMap = std::map< OperatorID, JITCompiler::Module::Ptr >;
    OperatorsMap m_operators;
};

} // namespace mega::runtime

#endif // MEGA_RUNTIME_SEPT_19_2022
