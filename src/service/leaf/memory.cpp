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
void LeafRequestLogicalThread::MPODestroyed( const MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    if( m_leaf.m_pJIT.get() && m_leaf.m_pRemoteMemoryManager.get() )
    {
        m_leaf.m_pRemoteMemoryManager->MPODestroyed( mpo );
    }
}

reference LeafRequestLogicalThread::NetworkAllocate( const MPO& parent, const TypeID& objectTypeID,
                                                    const TimeStamp& lockCycle, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestLogicalThread::NetworkAllocate: {} {}", parent, objectTypeID );

    reference result;

    if( MP( parent ) != m_leaf.m_mp )
    {
        network::sim::Request_Encoder simRequest{
            [ leafRequest = getMPOUpSender( yield_ctx ), targetMPO = parent ]( const network::Message& msg ) mutable
            { return leafRequest.MPOUp( msg, targetMPO ); },
            getID() };

        result = simRequest.SimAllocate( objectTypeID );
    }
    else
    {
        network::sim::Request_Encoder simRequest{
            [ leafRequest = getMPODownSender( yield_ctx ), targetMPO = parent ]( const network::Message& msg ) mutable
            { return leafRequest.MPODown( msg, targetMPO ); },
            getID() };

        result = simRequest.SimAllocate( objectTypeID );
    }

    auto llvm = getLLVMCompiler( yield_ctx );
    return m_leaf.m_pRemoteMemoryManager->networkToHeap( result, llvm );
}

reference LeafRequestLogicalThread::NetworkToHeap( const reference& ref, const TimeStamp& lockCycle,
                                                  boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestLogicalThread::NetworkToHeap: {} {}", ref, lockCycle );

    // short circuit if already got it
    if( ref.isHeapAddress() && ref.getLockCycle() == lockCycle )
    {
        return ref;
    }

    auto llvm = getLLVMCompiler( yield_ctx );

    // caller has called this because they ALREADY have appropriate read or write lock
    reference heapAddress;
    {
        if( ref.isNetworkAddress() )
        {
            // if not already heap address then get or construct heap object
            heapAddress = m_leaf.m_pRemoteMemoryManager->networkToHeap( ref, llvm );
        }
        else
        {
            heapAddress = ref.getObjectAddress();
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

            SPDLOG_TRACE( "LeafRequestLogicalThread::NetworkToHeap: requesting snapshot for: {}",
                          heapAddress.getHeaderAddress() );
            Snapshot objectSnapshot = simRequest.SimObjectSnapshot( heapAddress.getHeaderAddress() );
            ASSERT( objectSnapshot.getTimeStamp() == lockCycle );
            SPDLOG_TRACE(
                "LeafRequestLogicalThread::NetworkToHeap: got snapshot for: {}", heapAddress.getHeaderAddress() );
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

            auto           allocator = m_leaf.getJIT().getAllocator( llvm, heapAddress.getType() );
            BinLoadArchive archive( objectSnapshot );
            allocator->getLoadBin()( heapAddress.getHeap(), &archive );
        }
        heapAddress.setLockCycle( lockCycle );
    }

    return reference::make( heapAddress, ref.getTypeInstance() );
}

} // namespace mega::service
