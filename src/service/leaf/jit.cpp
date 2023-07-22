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

#include "request.hpp"

#include "llvm_compiler.hpp"

#include "service/protocol/common/type_erase.hpp"

namespace mega::service
{

// network::jit::Impl

void LeafRequestLogicalThread::GetAllocator( const mega::TypeID&         typeID,
                                            const mega::U64&            jitAllocatorPtr,
                                            boost::asio::yield_context& yield_ctx )
{
    runtime::Allocator::Ptr* pAllocatorPtr = type_reify< runtime::Allocator::Ptr >( jitAllocatorPtr );
    *pAllocatorPtr                         = m_leaf.m_pJIT->getAllocator( getLLVMCompiler( yield_ctx ), typeID );
}

void LeafRequestLogicalThread::ExecuteJIT( const runtime::JITFunctor& func, boost::asio::yield_context& yield_ctx )
{
    auto llvmCompiler = getLLVMCompiler( yield_ctx );
    func( m_leaf.getJIT(), ( void* )&llvmCompiler );
}

TypeID LeafRequestLogicalThread::GetInterfaceTypeID( const mega::TypeID& concreteTypeID, boost::asio::yield_context& )
{
    VERIFY_RTE_MSG( m_leaf.m_pJIT, "JIT not initialised" );
    return m_leaf.m_pJIT->getInterfaceTypeID( concreteTypeID );
}

std::unordered_map< std::string, mega::TypeID >
LeafRequestLogicalThread::GetIdentities( boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE_MSG( m_leaf.m_pJIT, "JIT not initialised" );
    return m_leaf.m_pJIT->getIdentities();
}

} // namespace mega::service
