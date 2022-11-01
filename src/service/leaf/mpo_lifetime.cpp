
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

#include "mega/shared_memory_header.hpp"

#include "service/network/log.hpp"

#include "service/protocol/model/memory.hxx"

#include <new>

namespace mega::service
{

MPOLifetime::MPOLifetime( Leaf& leaf, LeafRequestConversation& conversation, const reference& root,
                          const std::string& strMemory, boost::asio::yield_context& yield_ctx )
    : m_leaf( leaf )
    , m_conversation( conversation )
    , m_yield_ctx( yield_ctx )
    , m_root( root )
    , m_sharedMemory( m_leaf.getSharedMemory().get( m_root, [ this ]( MPO mpo ) { return memoryAccess( mpo ); } ) )
{
    m_leaf.getMPOs().insert( m_root );

    auto& jit      = m_leaf.getJIT();
    auto  compiler = conversation.getLLVMCompiler( yield_ctx );

    void* pSharedMemoryBuffer
        = m_leaf.getSharedMemory().construct( compiler, jit, root, [ this ]( MPO mpo ) { return memoryAccess( mpo ); } );

    m_leaf.getHeapMemory().allocate( compiler, jit, pSharedMemoryBuffer, m_root );

    SPDLOG_TRACE( "MPOLifetime constructed root: {} in memory: {}", m_root, strMemory );
}

MPOLifetime::~MPOLifetime()
{
    auto& jit      = m_leaf.getJIT();
    auto  compiler = m_conversation.getLLVMCompiler( m_yield_ctx );
    m_leaf.getSharedMemory().free( compiler, jit, m_root, [ this ]( MPO mpo ) { return memoryAccess( mpo ); } );
    m_leaf.getMPOs().erase( m_root );
}

std::string MPOLifetime::memoryAccess( MPO mpo )
{
    return network::memory::Request_Sender( m_conversation, m_leaf.getDaemonSender(), m_yield_ctx ).Acquire( mpo );
}

} // namespace mega::service
