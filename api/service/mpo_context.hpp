
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
#include "service/protocol/common/header.hpp"

#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/sim.hxx"
#include "service/protocol/model/memory.hxx"
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
    const network::ConversationID&            m_conversationIDRef;
    std::optional< mega::MPO >                m_mpo;
    log::Storage                              m_log;
    log::Storage::SchedulerIter               m_schedulerIter;
    log::Storage::MemoryIters                 m_memoryIters;
    mega::service::LockTracker                m_lockTracker;
    boost::asio::yield_context*               m_pYieldContext = nullptr;
    reference                                 m_root;
    std::unique_ptr< runtime::MemoryManager > m_pMemoryManager;

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
#include "service/component_interface.hxx"

#undef FUNCTION_ARG_0
#undef FUNCTION_ARG_1
#undef FUNCTION_ARG_2
#undef FUNCTION_ARG_3

    virtual network::mpo::Request_Sender     getMPRequest()              = 0;
    virtual network::enrole::Request_Encoder getRootEnroleRequest()      = 0;
    virtual network::stash::Request_Encoder  getRootStashRequest()       = 0;
    virtual network::memory::Request_Encoder getDaemonMemoryRequest()    = 0;
    virtual network::sim::Request_Encoder    getMPOSimRequest( MPO mpo ) = 0;
    virtual network::memory::Request_Sender  getLeafMemoryRequest()      = 0;
    virtual network::jit::Request_Sender     getLeafJITRequest()         = 0;

    void createRoot( const mega::MPO& mpo );

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
    virtual MPO             constructMPO( MP machineProcess ) override;
    virtual mega::reference getRoot( MPO mpo ) override;
    virtual mega::reference getThisRoot() override { return m_root; }

    // log
    virtual log::Storage& getLog() override { return m_log; }

private:
    // define temp data structures as members to reuse memory and avoid allocations
    using ShedulingMap   = std::map< MPO, std::vector< log::SchedulerRecordRead > >;
    using MemoryMap      = std::map< reference, std::string_view >;
    using MemoryMapArray = std::array< MemoryMap, log::toInt( log::TrackType::TOTAL ) >;
    ShedulingMap               m_schedulingMap;
    MemoryMapArray             m_memoryMaps;
    mega::network::Transaction m_transaction;

public:
    // called by Cycle dtor
    virtual void cycleComplete();
};

} // namespace mega

#endif // GUARD_2022_October_14_mpo_context
