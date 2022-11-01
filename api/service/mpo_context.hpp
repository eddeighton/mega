
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

#ifndef GUARD_2022_October_14_mpo_context
#define GUARD_2022_October_14_mpo_context

#include "mega/default_traits.hpp"
#include "mega/reference.hpp"

#include "jit/functions.hpp"

#include "service/context.hpp"
#include "service/lock_tracker.hpp"

#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"

#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/sim.hxx"
#include "service/protocol/model/memory.hxx"
#include "service/protocol/model/address.hxx"
#include "service/protocol/model/runtime.hxx"
#include "service/protocol/model/jit.hxx"
#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/stash.hxx"

#include "service/protocol/common/transaction.hpp"

#include "log/log.hpp"

#include <boost/filesystem.hpp>

#include <sstream>

namespace mega
{

class MPOContext : public Context
{
    boost::filesystem::path makeLogDirectory( const network::ConversationID& conversationID )
    {
        std::ostringstream os;
        os << "log_" << conversationID;
        return boost::filesystem::current_path() / os.str();
    }

protected:
    const network::ConversationID& m_conversationIDRef;
    std::optional< mega::MPO >     m_mpo;
    log::Storage                   m_log;
    log::Storage::SchedulerIter    m_schedulerIter;
    log::Storage::MemoryIters      m_memoryIters;
    mega::service::LockTracker     m_lockTracker;
    boost::asio::yield_context*    m_pYieldContext = nullptr;
    reference                      m_root;
    runtime::ManagedSharedMemory*  m_pSharedMemory = nullptr;

public:
    MPOContext( const network::ConversationID& conversationID )
        : m_conversationIDRef( conversationID )
        , m_log( makeLogDirectory( conversationID ) )
        , m_schedulerIter( m_log.schedBegin() )
        , m_memoryIters( m_log.memoryBegin() )
    {
    }

// runtime internal interface
#define FUNCTION_ARG_0( return_type, name ) return_type name();
#define FUNCTION_ARG_1( return_type, name, arg1_type, arg1_name ) return_type name( arg1_type arg1_name );
#define FUNCTION_ARG_2( return_type, name, arg1_type, arg1_name, arg2_type, arg2_name ) \
    return_type name( arg1_type arg1_name, arg2_type arg2_name );
#define FUNCTION_ARG_3( return_type, name, arg1_type, arg1_name, arg2_type, arg2_name, arg3_type, arg3_name ) \
    return_type name( arg1_type arg1_name, arg2_type arg2_name, arg3_type arg3_name );

#include "service/jit_interface.hxx"

#undef FUNCTION_ARG_0
#undef FUNCTION_ARG_1
#undef FUNCTION_ARG_2
#undef FUNCTION_ARG_3

    void* base();
    void* read( reference& ref );
    void* write( reference& ref );

    virtual network::mpo::Request_Sender      getMPRequest()           = 0;
    virtual network::address::Request_Encoder getRootAddressRequest()  = 0;
    virtual network::enrole::Request_Encoder  getRootEnroleRequest()   = 0;
    virtual network::stash::Request_Encoder   getRootStashRequest()    = 0;
    virtual network::memory::Request_Encoder  getDaemonMemoryRequest() = 0;
    virtual network::memory::Request_Sender   getLeafMemoryRequest()   = 0;
    virtual network::jit::Request_Sender      getLeafJITRequest()      = 0;

    network::sim::Request_Encoder getSimRequest( MPO mpo )
    {
        return { [ mpo, mpoRequest = getMPRequest() ]( const network::Message& msg ) mutable
                 { return mpoRequest.MPOUp( msg, mpo ); },
                 m_conversationIDRef };
    }

    void initSharedMemory( const mega::reference& root, network::JITMemoryPtr pMemory )
    {
        m_mpo           = root;
        m_root          = root;
        m_pSharedMemory = reinterpret_cast< runtime::ManagedSharedMemory* >( pMemory );
    }

    //////////////////////////
    // mega::MPOContext
    // queries
    virtual MPOContext::MachineIDVector getMachines() override
    {
        VERIFY_RTE( m_pYieldContext );
        return getRootEnroleRequest().EnroleGetDaemons();
    }
    virtual MPOContext::MachineProcessIDVector getProcesses( MachineID machineID ) override
    {
        VERIFY_RTE( m_pYieldContext );
        return getRootEnroleRequest().EnroleGetProcesses( machineID );
    }
    virtual MPOContext::MPOVector getMPO( MP machineProcess ) override
    {
        VERIFY_RTE( m_pYieldContext );
        return getRootEnroleRequest().EnroleGetMPO( machineProcess );
    }

    // mpo management
    virtual MPO getThisMPO() override { return m_mpo.value(); }
    virtual MPO constructMPO( MP machineProcess ) override
    {
        network::sim::Request_Encoder request(
            [ mpoRequest = getMPRequest(), machineProcess ]( const network::Message& msg ) mutable
            { return mpoRequest.MPRoot( msg, machineProcess ); },
            m_conversationIDRef );
        return request.SimCreate();
    }
    virtual mega::reference getRoot( MPO mpo ) override
    {
        THROW_TODO;
        // return mega::runtime::get_root( mpo );
    }

    virtual mega::reference getThisRoot() override { return m_root; }

    // log
    virtual log::Storage& getLog() override { return m_log; }

private:
    // MPOContext
    // simulation locks
    /*
    virtual bool readLock( MPO mpo )
    {
        SPDLOG_TRACE( "readLock from: {} to: {}", m_mpo.value(), mpo );
        network::sim::Request_Encoder request = getSimRequest( mpo );

        if ( request.SimLockRead( m_mpo.value() ) )
        {
            m_lockTracker.onRead( mpo );
            return true;
        }
        else
        {
            return false;
        }
    }

    virtual bool writeLock( MPO mpo )
    {
        SPDLOG_TRACE( "writeLock from: {} to: {}", m_mpo.value(), mpo );
        network::sim::Request_Encoder request = getSimRequest( mpo );

        if ( request.SimLockWrite( m_mpo.value() ) )
        {
            m_lockTracker.onWrite( mpo );
            return true;
        }
        else
        {
            return false;
        }
    }*/

    // define temp data structures as members to reuse memory and avoid allocations
    using ShedulingMap   = std::map< MPO, std::vector< log::SchedulerRecordRead > >;
    using MemoryMap      = std::map< reference, std::string_view >;
    using MemoryMapArray = std::array< MemoryMap, log::toInt( log::TrackType::TOTAL ) >;
    ShedulingMap               m_schedulingMap;
    MemoryMapArray             m_memoryMaps;
    mega::network::Transaction m_transaction;

public:
    // called by Cycle dtor
    virtual void cycleComplete()
    {
        SPDLOG_TRACE( "cycleComplete {}", m_mpo.value() );

        U32 expectedSchedulingCount = 0;
        U32 expectedMemoryCount     = 0;
        {
            m_schedulingMap.clear();
            for ( auto iEnd = m_log.schedEnd(); m_schedulerIter != iEnd; ++m_schedulerIter )
            {
                const log::SchedulerRecordRead& record = *m_schedulerIter;
                m_schedulingMap[ record.getReference() ].push_back( record );
                SPDLOG_TRACE( "cycleComplete {} found scheduling record", m_mpo.value() );
                ++expectedSchedulingCount;
            }

            {
                for ( auto iTrack = 0; iTrack != log::toInt( log::TrackType::TOTAL ); ++iTrack )
                {
                    if ( iTrack == log::toInt( log::TrackType::Log )
                         || iTrack == log::toInt( log::TrackType::Scheduler ) )
                        continue;

                    MemoryMap& memoryMap = m_memoryMaps[ iTrack ];
                    memoryMap.clear();
                    for ( auto &iter = m_memoryIters[ iTrack ],
                               iEnd  = m_log.memoryEnd( log::MemoryTrackType( iTrack ) );
                          iter != iEnd;
                          ++iter )
                    {
                        SPDLOG_TRACE( "cycleComplete {} found memory record", m_mpo.value() );
                        const log::MemoryRecordRead& record = *iter;
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
                    if ( iTrack == log::toInt( log::TrackType::Log )
                         || iTrack == log::toInt( log::TrackType::Scheduler ) )
                        continue;

                    MemoryMap& memoryMap = m_memoryMaps[ iTrack ];
                    int        iCounter  = 0;
                    for ( auto i = memoryMap.lower_bound( reference( TypeInstance{}, writeLock ) );
                          i != memoryMap.end();
                          ++i )
                    {
                        if ( i->first.getMachineID() != writeLock.getMachineID()
                             || i->first.getProcessID() != writeLock.getProcessID()
                             || i->first.getOwnerID() != writeLock.getOwnerID() )
                        {
                            break;
                        }
                        m_transaction.addMemoryRecord( i->first, log::MemoryTrackType( iTrack ), i->second );
                        ++memoryCount;
                        ++iCounter;
                    }
                    SPDLOG_TRACE( "cycleComplete: {} sending: {} track: {} memory records to: {}", m_mpo.value(),
                                  iCounter, iTrack, writeLock );
                }

                network::sim::Request_Encoder request = getSimRequest( writeLock );
                request.SimLockRelease( m_mpo.value(), m_transaction );
            }
        }

        if ( expectedMemoryCount != memoryCount )
        {
            SPDLOG_WARN( "Expected memory records: {} not equal to actual: {} for: {}", expectedMemoryCount,
                         memoryCount, m_mpo.value() );
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
            network::sim::Request_Encoder request = getSimRequest( readLock );
            request.SimLockRelease( m_mpo.value(), m_transaction );
        }

        m_lockTracker.reset();
    }
};

} // namespace mega

#endif // GUARD_2022_October_14_mpo_context
