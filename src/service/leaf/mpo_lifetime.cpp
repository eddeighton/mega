
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

#include "mpo_lifetime.hpp"
#include "llvm_compiler.hpp"

#include "request.hpp"
#include "shared_memory.hpp"
#include "heap_memory.hpp"

#include "database/types/shared_memory_header.hpp"

#include "service/network/log.hpp"

#include <new>

namespace mega::service
{

MPOLifetime::MPOLifetime( Leaf& leaf, LeafRequestConversation& conversation, const reference& root,
                          const std::string& strMemory, boost::asio::yield_context& yield_ctx )
    : m_leaf( leaf )
    , m_conversation( conversation )
    , m_root( root )
    , m_sharedMemory( m_leaf.getSharedMemory().get( m_root, strMemory ) )
{
    m_leaf.getMPOs().insert( m_root );

    auto&                        jit      = m_leaf.getJIT();
    const network::SizeAlignment rootSize = jit.getRootSize();

    void* pSharedMemoryBuffer = fromProcessAddress( m_sharedMemory.get_address(), m_root.pointer );

    // ensure the shared memory is constructed
    if ( rootSize.shared_size )
    {
        static mega::runtime::SharedCtorFunction _fptr_object_shared_alloc_1 = nullptr;
        if ( _fptr_object_shared_alloc_1 == nullptr )
        {
            jit.getObjectSharedAlloc(
                conversation.getLLVMCompiler( yield_ctx ), "leaf", 1, &_fptr_object_shared_alloc_1 );
        }
        _fptr_object_shared_alloc_1( pSharedMemoryBuffer, m_sharedMemory.get_address() );
    }

    m_leaf.getHeapMemory().allocate(
        conversation.getLLVMCompiler( yield_ctx ), jit, rootSize, pSharedMemoryBuffer, m_root, m_root.getProcessID() );
    SPDLOG_TRACE( "MPOLifetime constructed root: {} in memory: {}", m_root, strMemory );
}

MPOLifetime::~MPOLifetime()
{
    m_leaf.getSharedMemory().release( m_root );
    m_leaf.getMPOs().erase( m_root );
}
} // namespace mega::service
