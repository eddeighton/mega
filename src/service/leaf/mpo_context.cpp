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

#include "service/mpo_context.hpp"

#include "common/assert_verify.hpp"

namespace
{
thread_local mega::MPOContext* g_pMPOContext = nullptr;
} // namespace

namespace mega
{

Cycle::~Cycle() { getMPOContext()->cycleComplete(); }

Context* Context::get() { return g_pMPOContext; }

MPOContext* getMPOContext() { return g_pMPOContext; }
void        resetMPOContext() { g_pMPOContext = nullptr; }
void        setMPOContext( MPOContext* pMPOContext )
{
    VERIFY_RTE( g_pMPOContext == nullptr );
    g_pMPOContext = pMPOContext;
}

void* MPOContext::base() { return m_pSharedMemory->get_address(); }

void* MPOContext::read( reference& ref )
{
    SPDLOG_TRACE( "MPOContext::read {}", ref );
    network::MemoryBaseReference result = getLeafMemoryRequest().Read( m_root, ref );
    ref                                 = result.machineRef;
    m_lockTracker.onRead( ref );
    return result.getBaseAddress();
}

void* MPOContext::write( reference& ref )
{
    SPDLOG_TRACE( "MPOContext::write {}", ref );
    network::MemoryBaseReference result = getLeafMemoryRequest().Write( m_root, ref );
    ref                                 = result.machineRef;
    m_lockTracker.onWrite( ref );
    return result.getBaseAddress();
}

MPO MPOContext::constructMPO( MP machineProcess )
{
    network::sim::Request_Encoder request(
        [ mpoRequest = getMPRequest(), machineProcess ]( const network::Message& msg ) mutable
        { return mpoRequest.MPRoot( msg, machineProcess ); },
        m_conversationIDRef );
    return request.SimCreate();
}

reference MPOContext::getRoot( MPO mpo )
{
    const NetworkAddress netAddress = getRootAddressRequest().GetRootNetworkAddress( mpo );
    return { TypeInstance::Root(), mpo, netAddress };
}

reference MPOContext::allocate( const reference& context, TypeID objectTypeID )
{
    return getLeafMemoryRequest().Allocate( context, objectTypeID );
}
reference MPOContext::networkToMachine( const reference& ref )
{
    return getLeafMemoryRequest().NetworkToMachine( ref );
}

void MPOContext::get_save_object( const char* pszUnitName, TypeID objectTypeID,
                                  runtime::SaveObjectFunction* ppFunction )
{
    getLeafJITRequest().GetSaveObject( network::convert( pszUnitName ), objectTypeID, network::convert( ppFunction ) );
}
void MPOContext::get_load_object( const char* pszUnitName, TypeID objectTypeID,
                                  runtime::LoadObjectFunction* ppFunction )
{
    getLeafJITRequest().GetLoadObject( network::convert( pszUnitName ), objectTypeID, network::convert( ppFunction ) );
}

void MPOContext::get_call_getter( const char* pszUnitName, TypeID objectTypeID,
                                  runtime::TypeErasedFunction* ppFunction )
{
    getLeafJITRequest().GetCallGetter( network::convert( pszUnitName ), objectTypeID, network::convert( ppFunction ) );
}
void MPOContext::get_allocate( const char* pszUnitName, const InvocationID& invocationID,
                               runtime::AllocateFunction* ppFunction )
{
    getLeafJITRequest().GetAllocate( network::convert( pszUnitName ), invocationID, network::convert( ppFunction ) );
}
void MPOContext::get_read( const char* pszUnitName, const InvocationID& invocationID,
                           runtime::ReadFunction* ppFunction )
{
    getLeafJITRequest().GetRead( network::convert( pszUnitName ), invocationID, network::convert( ppFunction ) );
}
void MPOContext::get_write( const char* pszUnitName, const InvocationID& invocationID,
                            runtime::WriteFunction* ppFunction )
{
    getLeafJITRequest().GetWrite( network::convert( pszUnitName ), invocationID, network::convert( ppFunction ) );
}
void MPOContext::get_call( const char* pszUnitName, const InvocationID& invocationID,
                           runtime::CallFunction* ppFunction )
{
    getLeafJITRequest().GetCall( network::convert( pszUnitName ), invocationID, network::convert( ppFunction ) );
}
void MPOContext::get_start( const char* pszUnitName, const InvocationID& invocationID,
                            runtime::StartFunction* ppFunction )
{
    getLeafJITRequest().GetStart( network::convert( pszUnitName ), invocationID, network::convert( ppFunction ) );
}
void MPOContext::get_stop( const char* pszUnitName, const InvocationID& invocationID,
                           runtime::StopFunction* ppFunction )
{
    getLeafJITRequest().GetStop( network::convert( pszUnitName ), invocationID, network::convert( ppFunction ) );
}
void MPOContext::get_save( const char* pszUnitName, const InvocationID& invocationID,
                           runtime::SaveFunction* ppFunction )
{
    getLeafJITRequest().GetSave( network::convert( pszUnitName ), invocationID, network::convert( ppFunction ) );
}
void MPOContext::get_load( const char* pszUnitName, const InvocationID& invocationID,
                           runtime::LoadFunction* ppFunction )
{
    getLeafJITRequest().GetLoad( network::convert( pszUnitName ), invocationID, network::convert( ppFunction ) );
}

void MPOContext::cycleComplete()
{
    m_log.cycle();

    // SPDLOG_TRACE( "cycleComplete {}", m_mpo.value() );

    U32 expectedSchedulingCount = 0;
    U32 expectedMemoryCount     = 0;
    {
        m_schedulingMap.clear();
        for ( auto iEnd = m_log.schedEnd(); m_schedulerIter != iEnd; ++m_schedulerIter )
        {
            const log::SchedulerRecordRead& record = *m_schedulerIter;

            if ( MPO( record.getReference() ) != m_mpo.value() )
            {
                m_schedulingMap[ record.getReference() ].push_back( record );
                SPDLOG_TRACE( "cycleComplete {} found scheduling record", m_mpo.value() );
                ++expectedSchedulingCount;
            }
        }

        {
            for ( auto iTrack = 0; iTrack != log::toInt( log::TrackType::TOTAL ); ++iTrack )
            {
                if ( iTrack == log::toInt( log::TrackType::Log ) || iTrack == log::toInt( log::TrackType::Scheduler ) )
                    continue;

                MemoryMap& memoryMap = m_memoryMaps[ iTrack ];
                memoryMap.clear();
                for ( auto &iter = m_memoryIters[ iTrack ], iEnd = m_log.memoryEnd( log::MemoryTrackType( iTrack ) );
                      iter != iEnd; ++iter )
                {
                    const log::MemoryRecordRead& record = *iter;
                    SPDLOG_TRACE(
                        "cycleComplete {} found memory record for: {}", m_mpo.value(), record.getReference() );

                    if ( MPO( record.getReference() ) != m_mpo.value() )
                    {
                        // only record the last write for each reference
                        auto ib = memoryMap.insert( { record.getReference(), record.getData() } );
                        if ( ib.second )
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
        for ( MPO writeLock : m_lockTracker.getWrites() )
        {
            m_transaction.reset(); // reuse memory

            {
                auto iFind = m_schedulingMap.find( writeLock );
                if ( iFind != m_schedulingMap.end() )
                {
                    SPDLOG_TRACE( "cycleComplete: {} sending: {} scheduling records to: {}", m_mpo.value(),
                                  iFind->second.size(), writeLock );
                    m_transaction.addSchedulingRecords( iFind->second );
                    schedulingCount += iFind->second.size();
                }
            }

            for ( auto iTrack = 0; iTrack != log::toInt( log::TrackType::TOTAL ); ++iTrack )
            {
                if ( iTrack == log::toInt( log::TrackType::Log ) || iTrack == log::toInt( log::TrackType::Scheduler ) )
                    continue;

                MemoryMap& memoryMap = m_memoryMaps[ iTrack ];
                int        iCounter  = 0;
                for ( auto i = memoryMap.lower_bound(
                          reference( TypeInstance{}, writeLock, std::numeric_limits< U64 >::min() ) );
                      i != memoryMap.end();
                      ++i )
                {
                    if ( MPO( i->first ) != writeLock )
                        break;
                    m_transaction.addMemoryRecord( i->first, log::MemoryTrackType( iTrack ), i->second );
                    ++memoryCount;
                    ++iCounter;
                }
                SPDLOG_TRACE( "cycleComplete: {} sending: {} track: {} memory records to: {}", m_mpo.value(), iCounter,
                              iTrack, writeLock );
            }

            getLeafMemoryRequest().Release( m_mpo.value(), writeLock, m_transaction );
        }
    }

    if ( expectedMemoryCount != memoryCount )
    {
        SPDLOG_WARN( "Expected memory records: {} not equal to actual: {} for: {}", expectedMemoryCount, memoryCount,
                     m_mpo.value() );
    }
    if ( expectedSchedulingCount != schedulingCount )
    {
        SPDLOG_WARN( "Expected scheduling records: {} not equal to actual: {} for: {}", expectedSchedulingCount,
                     schedulingCount, m_mpo.value() );
    }

    for ( MPO readLock : m_lockTracker.getReads() )
    {
        SPDLOG_TRACE( "cycleComplete: {} sending: read release to: {}", m_mpo.value(), readLock );
        m_transaction.reset();
        getLeafMemoryRequest().Release( m_mpo.value(), readLock, m_transaction );
    }

    m_lockTracker.reset();
}
} // namespace mega
