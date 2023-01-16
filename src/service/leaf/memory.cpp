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

#include "jit/program_functions.hxx"

#include "service/network/log.hpp"

#include "service/protocol/model/memory.hxx"
#include "service/protocol/model/sim.hxx"

#include "mega/bin_archive.hpp"

#include "common/assert_verify.hpp"
#include "common/optimisation.hpp"

// #pragma DISABLE_GCC_OPTIMIZATIONS

namespace mega::service
{

// network::memory::Impl
void LeafRequestConversation::MPODestroyed( const MPO& mpo, const bool& bDeleteShared,
                                            boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE_MSG( m_leaf.m_pJIT.get(), "JIT not initialised" );
    // auto& jit      = m_leaf.getJIT();
    // auto  compiler = getLLVMCompiler( yield_ctx );

    // auto memoryAccess = [ &conversationBase = *this, &leaf = m_leaf, &yield_ctx = yield_ctx ]( MPO mpo ) ->
    // std::string { return network::memory::Request_Sender( conversationBase, leaf.getDaemonSender(), yield_ctx
    //).Acquire( mpo ); };

    // if( bDeleteShared )
    //{
    //     m_leaf.getSharedMemory().freeAll( compiler, jit, mpo, memoryAccess );
    // }

    // m_leaf.getHeapMemory().freeAll( compiler, jit, mpo );
}
/*
void LeafRequestConversation::replicateSnapshot( const Snapshot& snapshot, const reference& machineRef, bool bGetShared,
                                                 boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::replicateSnapshot {} {}", machineRef, bGetShared );

    auto& jit      = m_leaf.getJIT();
    auto  compiler = getLLVMCompiler( yield_ctx );

    // auto daemonMemoryRequest = network::memory::Request_Sender( *this, m_leaf.getDaemonSender(), yield_ctx );
    // auto memoryAccessor
    //     = [ &daemonMemoryRequest ]( MPO mpo ) -> std::string { return daemonMemoryRequest.Acquire( mpo ); };

    ASSERT( machineRef.isHeapAddress() );
    VERIFY_RTE_MSG( snapshot.getTimeStamp() > 0, "Snapshot failed" );

    // construct all heap buffers in snapshot objects
    const reference mpoRoot( TypeInstance::Root(), machineRef.getOwnerID(), machineRef.getHeap() );
    bool            bFoundRoot = false;
    for( const auto& objectIndex : snapshot.getObjects() )
    {
        reference object = snapshot.getTable().indexToRef( objectIndex );
        ASSERT_MSG( object.isHeapAddress(), "Snapshot object not machine ref" );
        THROW_TODO;
        // auto sharedMem = m_leaf.getSharedMemory().getOrConstruct( compiler, jit, object, memoryAccessor );
        //  m_leaf.getHeapMemory().ensureAllocated( compiler, jit, sharedMem );
        if( object.getType() == ROOT_TYPE_ID )
        {
            bFoundRoot = true;
        }
    }
    VERIFY_RTE_MSG( bFoundRoot, "Failed to locate root in snapshot" );
}*/

reference LeafRequestConversation::NetworkToHeap( const reference& ref, const TimeStamp& lockCycle,
                                                  boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::NetworkToHeap: {} {}", ref, lockCycle );

    // caller has called this because they ALREADY have appropriate read or write lock
    reference heapAddress = ref;
    {
        if( heapAddress.isNetworkAddress() )
        {
            // if not already heap address then get or construct heap object
            auto llvm   = getLLVMCompiler( yield_ctx );
            heapAddress = m_leaf.m_pRemoteMemoryManager->networkToHeap( heapAddress, llvm );
        }
    }

    if( heapAddress.getLockCycle() != lockCycle )
    {
        if( heapAddress.getMP() != m_leaf.m_mp )
        {
            // re-aquire the object
            network::sim::Request_Encoder simRequest{
                [ leafRequest = getMPOUpSender( yield_ctx ), targetMPO = heapAddress.getMPO() ](
                    const network::Message& msg ) mutable { return leafRequest.MPOUp( msg, targetMPO ); },
                getID() };

            SPDLOG_TRACE( "LeafRequestConversation::NetworkToHeap: requesting snapshot for: {}",
                          heapAddress.getNetworkAddress() );
            Snapshot objectSnapshot = simRequest.SimObjectSnapshot( heapAddress.getNetworkAddress() );
            ASSERT( objectSnapshot.getTimeStamp() == lockCycle );
            SPDLOG_TRACE(
                "LeafRequestConversation::NetworkToHeap: got snapshot for: {}", heapAddress.getNetworkAddress() );
            {
                AddressTable& addressTable = objectSnapshot.getTable();
                for( AddressTable::Index objectIndex : objectSnapshot.getObjects() )
                {
                    reference remoteAddress = addressTable.indexToRef( objectIndex );
                    ASSERT( remoteAddress.isNetworkAddress() );

                    if( m_leaf.m_pRemoteMemoryManager->tryNetworkToHeap( remoteAddress ) )
                    {
                        addressTable.remap( objectIndex, remoteAddress );
                    }
                }
            }

            auto llvm   = getLLVMCompiler( yield_ctx );

            // NOTE: cannot used runtime function wrapper from leaf - not a MPOContext!
            // static thread_local mega::runtime::program::ObjectLoadBin objectLoadBin;
            // objectLoadBin( heapAddress.getType(), heapAddress.getHeap(), &archive );

            auto allocator = m_leaf.getJIT().getAllocator( llvm, heapAddress.getType() );
            BinLoadArchive archive( objectSnapshot );
            allocator->getLoadBin()( heapAddress.getHeap(), &archive );
        }
        heapAddress.setLockCycle( lockCycle );
    }

    return heapAddress;
}

} // namespace mega::service
