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
            if ( const TimeStamp timeStamp = rq.SimLockRead( requestingMPO ); timeStamp > 0 )
            {
                result = m_leaf.getSharedMemory().getOrConstruct( compiler, jit, machineRef,
                                                                  [ &daemon ]( MPO mpo ) -> std::string
                                                                  { return daemon.Acquire( mpo ); } );
                {
                    // m_leaf.getHeapMemory().allocate( compiler, jit, , reference &ref)
                }
            }
        }
        else
        {
            reference machineRef = ref;
            if ( machineRef.isNetwork() )
            {
                machineRef = daemon.NetworkToMachine( ref );
            }
            ASSERT( machineRef.isMachine() );

            network::sim::Request_Sender rq( *this, m_leaf.getDaemonSender(), yield_ctx );

            // if ( const TimeStamp timeStamp = rq.SimLockRead( requestingMPO ); timeStamp > 0 )
            /*{
                result = m_leaf.getSharedMemory().getOrConstruct( compiler, jit, machineRef,
                                                                [ &daemon ]( MPO mpo ) -> std::string
                                                                { return daemon.Acquire( mpo ); } );
                if( bShared )
                {
                    // TODO - replicate the shared buffer
                    THROW_TODO;
                }
                else
                {
                    // TODO replicate the heap buffer
                    THROW_TODO;
                }
            }*/
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

} // namespace mega::service
