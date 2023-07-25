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

#ifndef LOGICALTHREAD_MANAGER_26_MAY_2022
#define LOGICALTHREAD_MANAGER_26_MAY_2022

#include "service/protocol/common/logical_thread_id.hpp"

#include "service/network/logical_thread.hpp"

#include <shared_mutex>
#include <mutex>

namespace mega::network
{

class LogicalThreadManager
{
    using LogicalThreadPtrMap = std::map< LogicalThreadID, LogicalThreadBase::Ptr >;
    using LogicalThreadIDSet  = std::set< LogicalThreadID >;

public:
    LogicalThreadManager( const std::string& strProcessName, boost::asio::io_context& ioContext );
    virtual ~LogicalThreadManager() = 0;

    const std::string&       getProcessName() const { return m_strProcessName; }
    boost::asio::io_context& getIOContext() const;

    void onDisconnect( network::Sender::Ptr pConnectionSender );

    std::vector< LogicalThreadID > reportLogicalThreads() const;
    LogicalThreadID                createLogicalThreadID() const;
    void                           externalLogicalThreadInitiated( ExternalLogicalThread::Ptr pLogicalThread );
    void                           logicalthreadInitiated( LogicalThread::Ptr pLogicalThread );
    void                           logicalthreadJoined( LogicalThread::Ptr pLogicalThread );
    virtual void                   logicalthreadCompleted( LogicalThreadBase::Ptr pLogicalThread );

    LogicalThreadBase::Ptr     findExistingLogicalThread( const network::LogicalThreadID& logicalthreadID ) const;
    ExternalLogicalThread::Ptr getExternalLogicalThread() const;

    virtual LogicalThreadBase::Ptr joinLogicalThread( const network::Message& msg ) = 0;
    virtual void                   dispatch( const ReceivedMessage& msg );

protected:
    void spawnInitiatedLogicalThread( LogicalThread::Ptr pLogicalThread );

protected:
    boost::asio::io_context&   m_ioContext;
    std::string                m_strProcessName;
    LogicalThreadPtrMap        m_logicalthreads;
    ExternalLogicalThread::Ptr m_pExternalLogicalThread;
    mutable std::shared_mutex  m_mutex;
    using WriteLock = std::unique_lock< std::shared_mutex >;
    using ReadLock  = std::shared_lock< std::shared_mutex >;
};

} // namespace mega::network

#endif // LOGICALTHREAD_MANAGER_26_MAY_2022
