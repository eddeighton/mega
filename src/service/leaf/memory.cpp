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

#include "shared_memory.hpp"
#include "heap_memory.hpp"

#include "service/network/log.hpp"

#include "service/protocol/model/memory.hxx"
#include "service/protocol/model/sim.hxx"

#include "common/assert_verify.hpp"
#include "common/optimisation.hpp"

#pragma DISABLE_GCC_OPTIMIZATIONS

namespace mega::service
{

// network::memory::Impl
void LeafRequestConversation::MPODestroyed( const MPO& mpo, const bool& bDeleteShared,
                                            boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE_MSG( m_leaf.m_pJIT.get(), "JIT not initialised" );
    auto& jit      = m_leaf.getJIT();
    auto  compiler = getLLVMCompiler( yield_ctx );

    auto memoryAccess = [ &conversationBase = *this, &leaf = m_leaf, &yield_ctx = yield_ctx ]( MPO mpo ) -> std::string
    { return network::memory::Request_Sender( conversationBase, leaf.getDaemonSender(), yield_ctx ).Acquire( mpo ); };

    if ( bDeleteShared )
    {
        m_leaf.getSharedMemory().freeAll( compiler, jit, mpo, memoryAccess );
    }

    m_leaf.getHeapMemory().freeAll( compiler, jit, mpo );
}

reference LeafRequestConversation::Allocate( const MPO& mpo, const TypeID& objectTypeID,
                                             boost::asio::yield_context& yield_ctx )
{
    auto& jit      = m_leaf.getJIT();
    auto  compiler = getLLVMCompiler( yield_ctx );

    auto daemon = network::memory::Request_Sender( *this, m_leaf.getDaemonSender(), yield_ctx );

    reference result = daemon.Allocate( mpo, objectTypeID );

    void* pSharedMemoryBuffer = m_leaf.getSharedMemory().construct(
        compiler, jit, result, [ &daemon ]( MPO mpo ) -> std::string { return daemon.Acquire( mpo ); } );

    m_leaf.getHeapMemory().allocate( compiler, jit, pSharedMemoryBuffer, result );

    return result;
}

network::MemoryBaseReference LeafRequestConversation::Read( const MPO& requestingMPO, const reference& ref,
                                                            boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::Read {} {}", requestingMPO, ref );

    auto& jit      = m_leaf.getJIT();
    auto  compiler = getLLVMCompiler( yield_ctx );

    auto daemon = network::memory::Request_Sender( *this, m_leaf.getDaemonSender(), yield_ctx );

    network::MemoryBaseReference result{ nullptr, ref };

    if ( ref.getMachineID() == m_leaf.m_mp.getMachineID() )
    {
        if ( ref.getProcessID() == m_leaf.m_mp.getProcessID() )
        {
            reference machineRef = ref;
            if ( machineRef.isNetwork() )
            {
                machineRef = daemon.NetworkToMachine( ref );
            }
            ASSERT( machineRef.isMachine() );

            network::sim::Request_Sender rq( *this, m_leaf.getNodeChannelSender(), yield_ctx );
            if ( const TimeStamp timestamp = rq.SimLockReadMPO( requestingMPO, MPO( ref ) ); timestamp > 0 )
            {
                result = m_leaf.getSharedMemory().getOrConstruct( compiler, jit, machineRef,
                                                                  [ &daemon ]( MPO mpo ) -> std::string
                                                                  { return daemon.Acquire( mpo ); } );
                m_leaf.getHeapMemory().ensureAllocated( compiler, jit, result );
            }
            else
            {
                THROW_TODO;
            }
        }
        else
        {
            // same machine but different process
            reference machineRef = ref;
            if ( machineRef.isNetwork() )
            {
                machineRef = daemon.NetworkToMachine( ref );
            }
            ASSERT( machineRef.isMachine() );

            network::sim::Request_Sender rq( *this, m_leaf.getDaemonSender(), yield_ctx );

            Snapshot snapshot = rq.SimLockRead( requestingMPO, machineRef );
            if ( snapshot.getTimeStamp() > 0 )
            {
                // construct all heap buffers in snapshot objects
                const reference mpoRoot( TypeInstance::Root(), machineRef, machineRef.pointer );
                bool            bFoundRoot = false;
                for ( const auto& object : snapshot.getObjects() )
                {
                    ASSERT_MSG( object.isMachine(), "Snapshot object not machine ref" );
                    auto sharedMem = m_leaf.getSharedMemory().getOrConstruct( compiler, jit, object,
                                                                              [ &daemon ]( MPO mpo ) -> std::string
                                                                              { return daemon.Acquire( mpo ); } );
                    m_leaf.getHeapMemory().ensureAllocated( compiler, jit, sharedMem );
                    if ( object.type == ROOT_TYPE_ID )
                    {
                        bFoundRoot = true;
                    }
                }
                VERIFY_RTE_MSG( bFoundRoot, "Failed to locate root in snapshot" );

                // now load the snapshot
                m_leaf.getJIT().load( compiler, mpoRoot, snapshot, false );

                // finally get the actually requested reference
                result = m_leaf.getSharedMemory().getOrConstruct( compiler, jit, machineRef,
                                                                  [ &daemon ]( MPO mpo ) -> std::string
                                                                  { return daemon.Acquire( mpo ); } );
            }
        }
    }
    else
    {
    }

    return result;
}

network::MemoryBaseReference LeafRequestConversation::Write( const MPO& requestingMPO, const reference& ref,
                                                             boost::asio::yield_context& yield_ctx )
{
    network::MemoryBaseReference result{ nullptr, ref };

    return result;
}

void LeafRequestConversation::Release( const MPO&                  requestingMPO,
                                       const MPO&                  targetMPO,
                                       const network::Transaction& transaction,
                                       boost::asio::yield_context& yield_ctx )
{
    if ( targetMPO.getMachineID() == m_leaf.m_mp.getMachineID() )
    {
        if ( targetMPO.getProcessID() == m_leaf.m_mp.getProcessID() )
        {
            network::sim::Request_Sender rq( *this, m_leaf.getNodeChannelSender(), yield_ctx );
            rq.SimLockRelease( requestingMPO, targetMPO, transaction );
        }
        else
        {
            network::sim::Request_Sender rq( *this, m_leaf.getDaemonSender(), yield_ctx );
            rq.SimLockRelease( requestingMPO, targetMPO, transaction );
        }
    }
    else
    {
        network::sim::Request_Sender rq( *this, m_leaf.getDaemonSender(), yield_ctx );
        rq.SimLockRelease( requestingMPO, targetMPO, transaction );
    }
}

reference LeafRequestConversation::NetworkToMachine( const reference& ref, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::NetworkToMachine {}", ref );

    if ( ref.isNetwork() )
    {
        auto daemon = network::memory::Request_Sender( *this, m_leaf.getDaemonSender(), yield_ctx );
        return daemon.NetworkToMachine( ref );
    }

    return ref;
}

Snapshot LeafRequestConversation::SimLockRead( const MPO& requestingMPO, const MPO& targetMPO,
                                               boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::SimLockRead {} {}", requestingMPO, targetMPO );
    VERIFY_RTE_MSG( m_leaf.m_pJIT.get(), "JIT not initialised in SimLockRead" );

    switch ( m_leaf.m_nodeType )
    {
        case network::Node::Executor:
        case network::Node::Tool:
        {
            network::sim::Request_Sender rq{ *this, m_leaf.getNodeChannelSender(), yield_ctx };
            TimeStamp                    timestamp = rq.SimLockReadMPO( requestingMPO, targetMPO );

            if ( m_leaf.m_mp.getMachineID() == targetMPO.getMachineID() )
            {
                if ( m_leaf.m_mp.getProcessID() == targetMPO.getProcessID() )
                {
                    // same process - just return timestamp
                    return Snapshot{ timestamp };
                }
                else
                {
                    // NOTE: should ALWAYS find the mpo root in the shared memory hash table since this leaf owns it
                    const reference& mpoRoot = m_leaf.getSharedMemory().getMPORoot( targetMPO );
                    Snapshot result = m_leaf.getJIT().save( getLLVMCompiler( yield_ctx ), mpoRoot, timestamp, false );
                    SPDLOG_TRACE( "Created snapshot with objects: {} and size: {}", result.getObjects().size(),
                                  result.getBuffer().size() );
                    return result;
                }
            }
            else
            {
                // NOTE: should ALWAYS find the mpo root in the shared memory hash table since this leaf owns it
                const reference& mpoRoot = m_leaf.getSharedMemory().getMPORoot( targetMPO );
                Snapshot result = m_leaf.getJIT().save( getLLVMCompiler( yield_ctx ), mpoRoot, timestamp, true );
                SPDLOG_TRACE( "Created snapshot with objects: {} and size: {}", result.getObjects().size(),
                              result.getBuffer().size() );
                return result;
            }
        }
        case network::Node::Terminal:
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::Leaf:
        case network::Node::TOTAL_NODE_TYPES:
            THROW_RTE( "Unreachable" );
        default:
            THROW_RTE( "Unknown node type" );
    }
}

Snapshot LeafRequestConversation::SimLockWrite( const MPO& requestingMPO, const MPO& targetMPO,
                                                boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::SimLockRead {} {}", requestingMPO, targetMPO );
    VERIFY_RTE_MSG( m_leaf.m_pJIT.get(), "JIT not initialised in SimLockRead" );
    switch ( m_leaf.m_nodeType )
    {
        case network::Node::Executor:
        case network::Node::Tool:
        {
            network::sim::Request_Sender rq{ *this, m_leaf.getNodeChannelSender(), yield_ctx };
            TimeStamp                    timestamp = rq.SimLockWriteMPO( requestingMPO, targetMPO );

            if ( m_leaf.m_mp.getMachineID() == targetMPO.getMachineID() )
            {
                if ( m_leaf.m_mp.getProcessID() == targetMPO.getProcessID() )
                {
                    // same process - just return timestamp
                    return Snapshot{ timestamp };
                }
                else
                {
                    // NOTE: should ALWAYS find the mpo root in the shared memory hash table since this leaf owns it
                    const reference& mpoRoot = m_leaf.getSharedMemory().getMPORoot( targetMPO );
                    return m_leaf.getJIT().save( getLLVMCompiler( yield_ctx ), mpoRoot, timestamp, false );
                }
            }
            else
            {
                // NOTE: should ALWAYS find the mpo root in the shared memory hash table since this leaf owns it
                const reference& mpoRoot = m_leaf.getSharedMemory().getMPORoot( targetMPO );
                return m_leaf.getJIT().save( getLLVMCompiler( yield_ctx ), mpoRoot, timestamp, true );
            }
        }
        case network::Node::Terminal:
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::Leaf:
        case network::Node::TOTAL_NODE_TYPES:
            THROW_RTE( "Unreachable" );
        default:
            THROW_RTE( "Unknown node type" );
    }
}

void LeafRequestConversation::SimLockRelease( const MPO&                  requestingMPO,
                                              const MPO&                  targetMPO,
                                              const network::Transaction& transaction,
                                              boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestConversation::SimLockRelease {} {}", requestingMPO, targetMPO );
    VERIFY_RTE_MSG( m_leaf.m_pJIT.get(), "JIT not initialised in SimLockRead" );
    switch ( m_leaf.m_nodeType )
    {
        case network::Node::Executor:
        case network::Node::Tool:
        {
            // apply transaction...

            SPDLOG_TRACE( "SimLockRelease got {} memory records",
                          transaction.getMemoryRecords( log::MemoryTrackType::Simulation ).size() );

            network::sim::Request_Sender rq{ *this, m_leaf.getNodeChannelSender(), yield_ctx };
            rq.SimLockRelease( requestingMPO, targetMPO, transaction );
            return;
        }
        case network::Node::Terminal:
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::Leaf:
        case network::Node::TOTAL_NODE_TYPES:
            THROW_RTE( "Unreachable" );
        default:
            THROW_RTE( "Unknown node type" );
    }
}

} // namespace mega::service
