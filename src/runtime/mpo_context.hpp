
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

#include "runtime/context.hpp"
#include "runtime/api.hpp"

#include "service/lock_tracker.hpp"
#include "service/protocol/common/header.hpp"

#include "service/protocol/model/sim.hxx"
#include "service/protocol/model/mpo.hxx"

#include "log/log.hpp"

#include <boost/filesystem.hpp>

#include <sstream>

namespace mega
{

class MPOContextImpl : public MPOContext
{
    boost::filesystem::path makeLogDirectory( const network::ConversationID& conversationID )
    {
        std::ostringstream os;
        os << "log_" << conversationID;
        return boost::filesystem::current_path() / os.str();
    }

protected:
    const network::ConversationID&            m_conversationIDRef;
    std::optional< mega::MPO >                m_mpo;
    log::Storage                              m_log;
    log::Storage::SchedulerIter               m_schedulerIter;
    log::Storage::MemoryIter                  m_memoryIter;
    std::shared_ptr< mega::runtime::MPORoot > m_pExecutionRoot;
    mega::service::LockTracker                m_lockTracker;
    boost::asio::yield_context*               m_pYieldContext = nullptr;

public:
    MPOContextImpl( const network::ConversationID& conversationID )
        : m_conversationIDRef( conversationID )
        , m_log( makeLogDirectory( conversationID ) )
        , m_schedulerIter( m_log.schedBegin() )
        , m_memoryIter( m_log.memoryBegin( log::MemoryTrackType::Simulation ) )
    {
    }

    virtual network::mpo::Request_Sender getMPRequest() = 0;

    network::sim::Request_Encoder getSimRequest( MPO mpo )
    {
        return { [ mpo, mpoRequest = getMPRequest() ]( const network::Message& msg ) mutable
                 { return mpoRequest.MPOUp( msg, mpo ); },
                 m_conversationIDRef };
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
    virtual mega::reference getRoot( MPO mpo ) override { return mega::runtime::get_root( mpo ); }
    virtual mega::reference getThisRoot() override { return m_pExecutionRoot->root(); }

    // log
    virtual log::Storage& getLog() override { return m_log; }

    // MPOContext
    // simulation locks
    virtual bool readLock( MPO mpo ) override
    {
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

    virtual bool writeLock( MPO mpo ) override
    {
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
    }

    virtual void cycleComplete() override
    {
        for ( auto iEnd = m_log.schedEnd(); m_schedulerIter != iEnd; ++m_schedulerIter )
        {
            const log::SchedulerRecordRead& record = *m_schedulerIter;
        }

        for ( auto iEnd = m_log.memoryEnd( log::MemoryTrackType::Simulation ); m_memoryIter != iEnd; ++m_memoryIter )
        {
            const log::MemoryRecordRead& record = *m_memoryIter;
        }

        for ( MPO writeLock : m_lockTracker.getWrites() )
        {
            network::sim::Request_Encoder request = getSimRequest( writeLock );
            request.SimLockRelease( m_mpo.value() );
            m_lockTracker.onRelease( writeLock );
        }

        for ( MPO readLock : m_lockTracker.getReads() )
        {
            network::sim::Request_Encoder request = getSimRequest( readLock );
            request.SimLockRelease( m_mpo.value() );
            m_lockTracker.onRelease( readLock );
        }
    }
};

} // namespace mega

#endif // GUARD_2022_October_14_mpo_context
