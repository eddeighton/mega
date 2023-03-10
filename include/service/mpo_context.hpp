
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

#include "mega/types/traits.hpp"
#include "mega/reference.hpp"

#include "jit/functions.hpp"

#include "service/memory_manager.hpp"
#include "service/lock_tracker.hpp"

#include "service/network/log.hpp"

#include "service/protocol/common/context.hpp"
#include "service/protocol/common/conversation_id.hpp"

#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/sim.hxx"
#include "service/protocol/model/memory.hxx"
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
protected:
    const network::ConversationID&            m_conversationIDRef;
    std::optional< mega::MPO >                m_mpo;
    log::Storage                              m_log;
    mega::service::LockTracker                m_lockTracker;
    network::TransactionProducer              m_transactionProducer;
    boost::asio::yield_context*               m_pYieldContext = nullptr;
    reference                                 m_root;
    std::unique_ptr< runtime::MemoryManager > m_pMemoryManager;

public:
    MPOContext( const network::ConversationID& conversationID )
        : m_conversationIDRef( conversationID )
        , m_log( makeLogDirectory( conversationID ) )
        , m_transactionProducer( m_log )
    {
    }

    virtual network::mpo::Request_Sender     getMPRequest()              = 0;
    virtual network::enrole::Request_Encoder getRootEnroleRequest()      = 0;
    virtual network::stash::Request_Encoder  getRootStashRequest()       = 0;
    virtual network::memory::Request_Encoder getDaemonMemoryRequest()    = 0;
    virtual network::sim::Request_Encoder    getMPOSimRequest( MPO mpo ) = 0;
    virtual network::memory::Request_Sender  getLeafMemoryRequest()      = 0;
    virtual network::jit::Request_Sender     getLeafJITRequest()         = 0;

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
    reference allocate( const reference& parent, TypeID objectTypeID );
    void      networkToHeap( reference& ref );
    void      readLock( reference& ref );
    void      writeLock( reference& ref );

    virtual MPO             getThisMPO() override { return m_mpo.value(); }
    virtual mega::reference getThisRoot() override { return m_root; }
    virtual mega::reference getRoot( MPO mpo ) override;
    virtual MPO             constructMPO( MP machineProcess ) override;
    virtual void            jit( runtime::JITFunctor func ) override;
    virtual void            yield() override;

    // log
    virtual log::Storage& getLog() override { return m_log; }

    // called by Cycle dtor
    void applyTransaction( const network::Transaction& transaction );
    void cycleComplete();

protected:
    void createRoot( const mega::MPO& mpo );

private:
    boost::filesystem::path makeLogDirectory( const network::ConversationID& conversationID )
    {
        std::ostringstream os;
        os << "log_" << conversationID;
        return boost::filesystem::current_path() / os.str();
    }
};

} // namespace mega

#endif // GUARD_2022_October_14_mpo_context
