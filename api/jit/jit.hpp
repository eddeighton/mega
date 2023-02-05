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

#include "service/protocol/common/project.hpp"
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
    JIT( const network::MegastructureInstallation& megastructureInstallation, const network::Project& project );

    Allocator::Ptr getAllocator( const CodeGenerator::LLVMCompiler& compiler, const TypeID& objectTypeID );
    Relation::Ptr getRelation( const CodeGenerator::LLVMCompiler& compiler, const RelationID& relationID );
    Program::Ptr getProgram( const CodeGenerator::LLVMCompiler& compiler ) ;

    virtual void getProgramFunction( void* pLLVMCompiler, int functionType, void** ppFunction ) override;

    virtual void getInvocationFunction( void* pLLVMCompiler, const char* pszUnitName,
                                        const mega::InvocationID& invocationID, int functionType,
                                        void** ppFunction ) override;
    virtual void getObjectFunction( void* pLLVMCompiler, const char* pszUnitName, const mega::TypeID& typeID,
                                    int functionType, void** ppFunction ) override;
    virtual void getRelationFunction( void* pLLVMCompiler, const char* pszUnitName,
                                      const RelationID& relationID, int functionType,
                                      void** ppFunction ) override;

private:
    JITCompiler::Module::Ptr compile( const std::string& strCode );

    const network::MegastructureInstallation m_megastructureInstallation;
    const network::Project                   m_project;

    JITCompiler      m_jitCompiler;
    DatabaseInstance m_database;
    CodeGenerator    m_codeGenerator;
    ComponentManager m_componentManager;

    using AllocatorMap = std::map< TypeID, Allocator::Ptr >;
    AllocatorMap m_allocators;

    using RelationMap = std::map< RelationID, Relation::Ptr >;
    RelationMap m_relations;

    using InvocationMap = std::map< InvocationID, JITCompiler::Module::Ptr >;
    InvocationMap m_invocations;

    using FunctionPtrMap = std::multimap< const char*, void* >;
    FunctionPtrMap m_functionPointers;

    Program::Ptr m_pProgram;
    using FunctionPtrSet = std::set< void* >;
    FunctionPtrSet m_programFunctionPointers;
};

} // namespace mega::runtime

#endif // MEGA_RUNTIME_SEPT_19_2022
