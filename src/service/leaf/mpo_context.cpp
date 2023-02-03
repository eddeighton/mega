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

void MPOContext::cycleComplete()
{
    m_log.cycle();

    if( m_log.getTimeStamp() % 60 == 0 )
    {
        SPDLOG_TRACE( "MPOContext: cycleComplete {} {}", m_mpo.value(), m_log.getTimeStamp() );
    }
/*
    U32 expectedSchedulingCount = 0;
    U32 expectedMemoryCount     = 0;
    {
        m_schedulingMap.clear();
        for( auto iEnd = m_log.schedEnd(); m_schedulerIter != iEnd; ++m_schedulerIter )
        {
            const log::SchedulerRecordRead& record = *m_schedulerIter;

            if( record.getReference().getMPO() != m_mpo.value() )
            {
                m_schedulingMap[ record.getReference().getMPO() ].push_back( record );
                SPDLOG_TRACE( "MPOContext: cycleComplete {} found scheduling record", m_mpo.value() );
                ++expectedSchedulingCount;
            }
        }

        {
            for( auto iTrack = 0; iTrack != log::toInt( log::TrackType::TOTAL ); ++iTrack )
            {
                if( iTrack == log::toInt( log::TrackType::Log ) || iTrack == log::toInt( log::TrackType::Scheduler ) )
                    continue;

                MemoryMap& memoryMap = m_memoryMaps[ iTrack ];
                memoryMap.clear();
                for( auto &iter = m_memoryIters[ iTrack ], iEnd = m_log.memoryEnd( log::MemoryTrackType( iTrack ) );
                     iter != iEnd; ++iter )
                {
                    const log::MemoryRecordRead& record = *iter;
                    SPDLOG_TRACE( "MPOContext: cycleComplete {} found memory record for: {}", m_mpo.value(),
                                  record.getReference() );

                    if( record.getReference().getMPO() != m_mpo.value() )
                    {
                        // only record the last write for each reference
                        auto ib = memoryMap.insert( { record.getReference(), record.getData() } );
                        if( ib.second )
                        {
                            ++expectedMemoryCount;
                        }
                        else
                        {
                            ib.first->second = record.getData();
                        }
                    }
                }
            }
        }
    }

    U32 schedulingCount = 0;
    U32 memoryCount     = 0;
    {
        for( const auto& [ writeLockMPO, lockCycle ] : m_lockTracker.getWrites() )
        {
            SPDLOG_TRACE( "MPOContext: cycleComplete: write lock: {} with cycle: {}", writeLockMPO, lockCycle );
            m_transaction.reset(); // reuse memory

            {
                auto iFind = m_schedulingMap.find( writeLockMPO );
                if( iFind != m_schedulingMap.end() )
                {
                    SPDLOG_TRACE( "MPOContext: cycleComplete: {} sending: {} scheduling records to: {}", m_mpo.value(),
                                  iFind->second.size(), writeLockMPO );
                    m_transaction.addSchedulingRecords( iFind->second );
                    schedulingCount += iFind->second.size();
                }
            }

            for( auto iTrack = 0; iTrack != log::toInt( log::TrackType::TOTAL ); ++iTrack )
            {
                if( iTrack == log::toInt( log::TrackType::Log ) || iTrack == log::toInt( log::TrackType::Scheduler ) )
                    continue;

                MemoryMap& memoryMap = m_memoryMaps[ iTrack ];
                int        iCounter  = 0;
                // here lower_bound relies on the reference::operator< comparing the MPO first in lexigraphical
                // comparison
                for( auto i = memoryMap.lower_bound( reference( TypeInstance{}, writeLockMPO, 0U ) );
                     i != memoryMap.end();
                     ++i )
                {
                    if( i->first.getMPO() != writeLockMPO )
                        break;
                    m_transaction.addMemoryRecord( i->first, log::MemoryTrackType( iTrack ), i->second );
                    ++memoryCount;
                    ++iCounter;
                }
                SPDLOG_TRACE( "MPOContext: cycleComplete: {} sending: {} memory record to track: {} for target: {}",
                              m_mpo.value(), iCounter, iTrack, writeLockMPO );
            }

            getMPOSimRequest( writeLockMPO ).SimLockRelease( m_mpo.value(), writeLockMPO, m_transaction );
        }
    }

    if( expectedMemoryCount != memoryCount )
    {
        SPDLOG_WARN( "Expected memory records: {} not equal to actual: {} for: {}", expectedMemoryCount, memoryCount,
                     m_mpo.value() );
    }
    if( expectedSchedulingCount != schedulingCount )
    {
        SPDLOG_WARN( "Expected scheduling records: {} not equal to actual: {} for: {}", expectedSchedulingCount,
                     schedulingCount, m_mpo.value() );
    }

    for( const auto& [ readLockMPO, lockCycle ] : m_lockTracker.getReads() )
    {
        SPDLOG_TRACE( "MPOContext: cycleComplete: {} sending: read release to: {}", m_mpo.value(), readLockMPO );
        m_transaction.reset();
        getMPOSimRequest( readLockMPO ).SimLockRelease( m_mpo.value(), readLockMPO, m_transaction );
    }
*/
    m_lockTracker.reset();
}

} // namespace mega
