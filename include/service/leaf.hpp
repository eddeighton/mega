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

#ifndef LEAF_16_JUNE_2022
#define LEAF_16_JUNE_2022

#include "service/remote_memory_manager.hpp"

#include "service/network/client.hpp"
#include "service/network/logical_thread_manager.hpp"
#include "service/network/sender_factory.hpp"
#include "service/network/receiver_channel.hpp"

#include "service/protocol/common/logical_thread_id.hpp"
#include "service/protocol/common/node.hpp"

#include "jit/jit.hpp"

#include <boost/asio/io_service.hpp>

#include <memory>
#include <vector>
#include <thread>

namespace mega::service
{

class HeapMemory;

class Leaf : public network::LogicalThreadManager
{
    friend class LeafRequestLogicalThread;
    friend class LeafEnrole;
    friend class MPOEntry;

public:
    Leaf( network::Sender::Ptr pSender, network::Node::Type nodeType, short daemonPortNumber );
    ~Leaf();

    void startup();

    // void shutdown();
    bool running() { return !m_io_context.stopped(); }

    // network::LogicalThreadManager
    virtual network::LogicalThreadBase::Ptr joinLogicalThread( const network::Message& msg );

    network::Node::Type  getType() const { return m_nodeType; }
    network::Sender::Ptr getDaemonSender() { return m_client.getSender(); }
    network::Sender::Ptr getNodeChannelSender() { return m_pSender; }
    network::Sender::Ptr getLeafSender() { return m_pSelfSender; }

    // network::Sender
    void setActiveProject( const Project& project );

    const MegastructureInstallation& getMegastructureInstallation() const
    {
        VERIFY_RTE_MSG( m_megastructureInstallationOpt.has_value(), "Megastructure Installation not found" );
        return m_megastructureInstallationOpt.value();
    }
    const Project& getActiveProject() const
    {
        VERIFY_RTE_MSG( m_activeProject.has_value(), "No active project" );
        return m_activeProject.value();
    }
    std::optional< task::FileHash > getUnityDBHashCode() const { return m_unityDatabaseHashCode; }

    HeapMemory&            getHeapMemory();
    std::set< mega::MPO >& getMPOs() { return m_mpos; }
    runtime::JIT&          getJIT() { return *m_pJIT; }

private:
    network::Sender::Ptr     m_pSender;
    network::Sender::Ptr     m_pSelfSender;
    network::Node::Type      m_nodeType;
    boost::asio::io_context  m_io_context;
    network::ReceiverChannel m_receiverChannel;
    network::Client          m_client;
    using ExecutorType = decltype( m_io_context.get_executor() );
    boost::asio::executor_work_guard< ExecutorType > m_work_guard;
    std::thread                                      m_io_thread;
    mega::MP                                         m_mp;
    std::set< mega::MPO >                            m_mpos;
    std::unique_ptr< runtime::JIT >                  m_pJIT;
    std::optional< MegastructureInstallation >       m_megastructureInstallationOpt;
    std::optional< Project >                         m_activeProject;
    std::optional< task::FileHash >                  m_unityDatabaseHashCode;
    std::unique_ptr< runtime::RemoteMemoryManager >  m_pRemoteMemoryManager;
};

} // namespace mega::service

#endif // LEAF_16_JUNE_2022