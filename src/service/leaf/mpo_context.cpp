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

#include <memory>

#include "jit/program_functions.hxx"

#include "service/mpo_context.hpp"
#include "service/cycle.hpp"

#include "service/protocol/common/type_erase.hpp"

#include "common/assert_verify.hpp"

#include "service/network/log.hpp"

namespace mega
{
namespace runtime
{
MPO getThisMPO()
{
    return getMPOContext()->getThisMPO();
}
void networkToHeap( reference& ref )
{
    getMPOContext()->networkToHeap( ref );
}
void readLock( reference& ref )
{
    getMPOContext()->readLock( ref );
}
void writeLock( reference& ref )
{
    getMPOContext()->writeLock( ref );
}
reference allocate( const reference& parent, TypeID typeID )
{
    return getMPOContext()->allocate( parent, typeID );
}
void* log()
{
    return &getMPOContext()->getLog();
}
} // namespace runtime

Cycle::~Cycle()
{
    if( auto pContext = getMPOContext() )
    {
        pContext->cycleComplete();
    }
}

reference MPOContext::getRoot( MPO mpo )
{
    if( m_root.getMPO() == mpo )
    {
        return m_root;
    }
    else
    {
        return reference{ TypeInstance{ 0U, ROOT_TYPE_ID }, mpo, ROOT_OBJECT_ID };
    }
}

MPO MPOContext::constructMPO( MP machineProcess )
{
    network::sim::Request_Encoder request(
        [ mpoRequest = getMPRequest(), machineProcess ]( const network::Message& msg ) mutable
        { return mpoRequest.MPRoot( msg, machineProcess ); },
        m_conversationIDRef );
    return request.SimCreate();
}

reference MPOContext::allocate( const reference& context, TypeID objectTypeID )
{
    return m_pMemoryManager->New( objectTypeID );
}

// networkToHeap ONLY called when MPO matches
void MPOContext::networkToHeap( reference& ref )
{
    SPDLOG_TRACE( "MPOContext::networkToHeap: {}", ref );

    VERIFY_RTE_MSG( ref.getMPO() == getThisMPO(), "networkToHeap used when not matching MPO" );

    if( ref.isNetworkAddress() )
    {
        ASSERT( m_pMemoryManager );
        ref = m_pMemoryManager->networkToHeap( ref );
    }
}

void MPOContext::readLock( reference& ref )
{
    SPDLOG_TRACE( "MPOContext::readLock: {}", ref );

    VERIFY_RTE_MSG( ref.getMPO() != getThisMPO(), "readLock used when matching MPO" );

    // acquire lock if required and get lock cycle
    TimeStamp lockCycle = m_lockTracker.isRead( ref.getMPO() );
    if( lockCycle == 0U )
    {
        lockCycle = getMPOSimRequest( ref.getMPO() ).SimLockRead( getThisMPO(), ref.getMPO() );
        VERIFY_RTE_MSG( lockCycle != 0U, "Failed to acquire write lock on: " << ref );
        m_lockTracker.onRead( ref.getMPO(), lockCycle );
    }

    if( ref.isNetworkAddress() || ( ref.getLockCycle() != lockCycle ) )
    {
        ref = getLeafMemoryRequest().NetworkToHeap( ref, lockCycle );
    }
}

void MPOContext::writeLock( reference& ref )
{
    SPDLOG_TRACE( "MPOContext::writeLock: {}", ref );

    VERIFY_RTE_MSG( ref.getMPO() != getThisMPO(), "writeLock used when matching MPO" );

    TimeStamp lockCycle = m_lockTracker.isWrite( ref.getMPO() );
    if( lockCycle == 0U )
    {
        lockCycle = getMPOSimRequest( ref.getMPO() ).SimLockWrite( getThisMPO(), ref.getMPO() );
        VERIFY_RTE_MSG( lockCycle != 0U, "Failed to acquire write lock on: " << ref );
        m_lockTracker.onWrite( ref.getMPO(), lockCycle );
    }

    if( ref.isNetworkAddress() || ( ref.getLockCycle() != lockCycle ) )
    {
        ref = getLeafMemoryRequest().NetworkToHeap( ref, lockCycle );
    }
}

void MPOContext::createRoot( const mega::MPO& mpo )
{
    m_mpo = mpo;

    m_pMemoryManager.reset();

    m_pMemoryManager = std::make_unique< runtime::MemoryManager >(
        mpo,
        [ jitRequest = getLeafJITRequest() ]( TypeID typeID ) mutable -> runtime::Allocator::Ptr
        {
            runtime::Allocator::Ptr pAllocator;
            jitRequest.GetAllocator( typeID, type_erase( &pAllocator ) );
            return pAllocator;
        } );

    // instantiate the root
    m_root = m_pMemoryManager->New( ROOT_TYPE_ID );
}

void MPOContext::jit( runtime::JITFunctor func )
{
    getLeafJITRequest().ExecuteJIT( func );
}
void MPOContext::yield()
{
    boost::asio::post( *m_pYieldContext );
}

void MPOContext::applyTransaction( const network::Transaction& transaction )
{
    // NOTE: can context switch when call get_load_record
    static thread_local mega::runtime::program::RecordLoadBin recordLoadBin;

    const network::Transaction::In& data = transaction.m_in.value();
    {
        for( const log::Structure::DataIO& structure : data.m_structure )
        {
            SPDLOG_TRACE( "SIM::SimLockRelease Got structure record: {} {} {}", structure.m_data.m_Source,
                          structure.m_data.m_Target, log::Structure::toString( structure.m_data.m_Type ) );
        }
    }
    {
        for( const log::Scheduling::DataIO& scheduling : data.m_scheduling )
        {
            SPDLOG_TRACE( "SIM::SimLockRelease Got scheduling record: {} {}", scheduling.m_data.m_Ref,
                          log::Scheduling::toString( scheduling.m_data.m_Type ) );
        }
    }
    {
        for( const log::Memory::DataIO& memory : data.m_memory )
        {
            SPDLOG_TRACE( "SIM::SimLockRelease Got memory record: {} {}", memory.m_data.m_Ref, memory.m_Data );
            recordLoadBin( memory.m_data.m_Ref, ( void* )memory.m_Data.data(), memory.m_Data.size() );
        }
    }
}

void MPOContext::cycleComplete()
{
    m_log.cycle();

    if( m_log.getTimeStamp() % 60 == 0 )
    {
        SPDLOG_TRACE( "MPOContext: cycleComplete {} {}", m_mpo.value(), m_log.getTimeStamp() );
    }

    network::TransactionProducer::MPOTransactions transactions;
    m_transactionProducer.generate( transactions );

    for( const auto& [ writeLockMPO, lockCycle ] : m_lockTracker.getWrites() )
    {
        SPDLOG_TRACE( "MPOContext: cycleComplete: {} sending: write release to: {}", m_mpo.value(), writeLockMPO );
        getMPOSimRequest( writeLockMPO )
            .SimLockRelease( m_mpo.value(), writeLockMPO, network::Transaction{ transactions[ writeLockMPO ] } );
    }

    for( const auto& [ readLockMPO, lockCycle ] : m_lockTracker.getReads() )
    {
        SPDLOG_TRACE( "MPOContext: cycleComplete: {} sending: read release to: {}", m_mpo.value(), readLockMPO );
        getMPOSimRequest( readLockMPO ).SimLockRelease( m_mpo.value(), readLockMPO, network::Transaction{} );
    }

    m_lockTracker.reset();
}

} // namespace mega
