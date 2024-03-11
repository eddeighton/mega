
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

#include "mega/values/runtime/pointer.hpp"

#include "environment/mpo_database.hpp"

#include "runtime/context.hpp"

#include "service/memory_manager.hpp"
#include "service/lock_tracker.hpp"

#include "log/log.hpp"

#include "mega/values/service/logical_thread_id.hpp"

#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/sim.hxx"
#include "service/protocol/model/memory.hxx"
#include "service/protocol/model/jit.hxx"
#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/stash.hxx"

#include "service/protocol/common/transaction.hpp"

#include "event/file_log.hpp"

#include <boost/filesystem.hpp>

#include <sstream>

namespace mega::runtime
{

class MPOContext : public runtime::Context
{
protected:
    const network::LogicalThreadID&                 m_logicalthreadIDRef;
    std::optional< mega::runtime::MPO >             m_mpo;
    std::unique_ptr< event::FileStorage >           m_pLog;
    mega::service::LockTracker                      m_lockTracker;
    std::unique_ptr< network::TransactionProducer > m_pTransactionProducer;
    boost::asio::yield_context*                     m_pYieldContext = nullptr;
    runtime::PointerHeap                            m_root;
    std::unique_ptr< runtime::MPODatabase >         m_pDatabase;
    // std::unique_ptr< runtime::MemoryManager >       m_pMemoryManager;
    network::TransactionProducer::MovedObjects m_movedObjects; // dependency to SimMoveMachine

    std::chrono::time_point< std::chrono::system_clock > m_systemStartTime = std::chrono::system_clock::now();
    std::chrono::time_point< std::chrono::steady_clock > m_startTime       = std::chrono::steady_clock::now();

public:
    MPOContext( const network::LogicalThreadID& logicalthreadID )
        : m_logicalthreadIDRef( logicalthreadID )
    {
    }

    virtual network::mpo::Request_Sender     getMPRequest()                       = 0;
    virtual network::enrole::Request_Encoder getRootEnroleRequest()               = 0;
    virtual network::stash::Request_Encoder  getRootStashRequest()                = 0;
    virtual network::memory::Request_Encoder getDaemonMemoryRequest()             = 0;
    virtual network::sim::Request_Encoder    getMPOSimRequest( runtime::MPO mpo ) = 0;
    virtual network::memory::Request_Sender  getLeafMemoryRequest()               = 0;
    virtual network::jit::Request_Sender     getLeafJITRequest()                  = 0;

    //////////////////////////
    // mega::MPOContext
    // queries
    virtual MPOContext::MachineIDVector getMachines() override
    {
        VERIFY_RTE( m_pYieldContext );
        return getRootEnroleRequest().EnroleGetDaemons();
    }
    virtual MPOContext::MachineProcessIDVector getProcesses( runtime::MachineID machineID ) override
    {
        VERIFY_RTE( m_pYieldContext );
        return getRootEnroleRequest().EnroleGetProcesses( machineID );
    }
    virtual MPOContext::MPOVector getMPO( runtime::MP machineProcess ) override
    {
        VERIFY_RTE( m_pYieldContext );
        return getRootEnroleRequest().EnroleGetMPO( machineProcess );
    }

    // mpo management
    runtime::PointerHeap allocate( concrete::ObjectID objectType );
    runtime::PointerNet  allocateRemote( const runtime::MPO& remote, concrete::ObjectID objectType );
    runtime::PointerHeap networkToHeap( const runtime::PointerNet& ref );

    /*
    void                 readLock( runtime::PointerHeap& ref );
    void                 writeLock( runtime::PointerHeap& ref );
    */

    virtual runtime::MPO         getThisMPO() override { return m_mpo.value(); }
    virtual runtime::PointerHeap getThisRoot() override { return m_root; }
    virtual runtime::PointerNet  getRoot( runtime::MPO mpo ) override;
    virtual runtime::MPO         constructMPO( runtime::MP machineProcess ) override;
    virtual runtime::MP          constructExecutor( runtime::MachineID daemonMachineID ) override;
    virtual void                 destroyExecutor( runtime::MP mp ) override;
    virtual void                 jit( runtime::RuntimeFunctor func ) override;
    virtual void                 yield() override;

    // event
    virtual event::FileStorage& getLog() override
    {
        ASSERT( m_pLog );
        return *m_pLog;
    }

    // called by Cycle dtor
    void applyTransaction( const network::Transaction& transaction );
    void cycleComplete();

    void getBasicReport( const report::URL& url, mega::reports::Table& table );
    auto getElapsedTime() const { return std::chrono::steady_clock::now() - m_startTime; }

protected:
    void createRoot( const runtime::MPO& mpo );
};

} // namespace mega

#endif // GUARD_2022_October_14_mpo_context
