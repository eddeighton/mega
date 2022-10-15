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

#ifndef SIMULATION_22_JUNE_2022
#define SIMULATION_22_JUNE_2022

#include "request.hpp"
#include "clock.hpp"
#include "scheduler.hpp"

#include "service/state_machine.hpp"
#include "service/network/sender.hpp"
#include "service/protocol/common/header.hpp"

#include "runtime/mpo_context.hpp"

#include "mega/reference.hpp"

namespace mega::service
{

class Executor;

class Simulation : public ExecutorRequestConversation, public mega::MPOContextImpl
{
public:
    using Ptr = std::shared_ptr< Simulation >;

    Simulation( Executor& executor, const network::ConversationID& conversationID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;

    virtual network::Message dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx ) override;
    virtual void             run( boost::asio::yield_context& yield_ctx ) override;

    // MPOContextImpl
    virtual network::mpo::Request_Sender getMPRequest() override;

    // network::sim::Impl
    virtual bool SimLockRead( const mega::MPO&, boost::asio::yield_context& ) override;
    virtual bool SimLockWrite( const mega::MPO&, boost::asio::yield_context& ) override;
    virtual void SimLockRelease( const mega::MPO&, const network::Transaction&, boost::asio::yield_context& ) override;
    virtual void SimClock( boost::asio::yield_context& ) override;
    virtual mega::MPO SimCreate( boost::asio::yield_context& ) override;
    virtual void      SimDestroy( boost::asio::yield_context& ) override;

    // network::leaf_exe::Impl
    virtual void RootSimRun( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual std::string     Ping( boost::asio::yield_context& yield_ctx ) override;

    // mega::MPOContext - native code interface
    // queries
    virtual MPOContext::MachineIDVector        getMachines() override;
    virtual MPOContext::MachineProcessIDVector getProcesses( MachineID machineID ) override;
    virtual MPOContext::MPOVector              getMPO( MP machineProcess ) override;

    // mega::MPOContext
    // clock
    virtual TimeStamp cycle() override { return m_clock.cycle(); }
    virtual F32       ct() override { return m_clock.ct(); }
    virtual F32       dt() override { return m_clock.dt(); }

    // mega::MPOContext
    // memory management
    virtual std::string    acquireMemory( MPO mpo ) override;
    virtual MPO            getNetworkAddressMPO( NetworkAddress networkAddress ) override;
    virtual NetworkAddress getRootNetworkAddress( MPO mpo ) override;
    virtual NetworkAddress allocateNetworkAddress( MPO mpo, TypeID objectTypeID ) override;
    virtual void           deAllocateNetworkAddress( MPO mpo, NetworkAddress networkAddress ) override;

    // mega::MPOContext
    // stash
    virtual void stash( const std::string& filePath, mega::U64 determinant ) override;
    virtual bool restore( const std::string& filePath, mega::U64 determinant ) override;

private:
    void issueClock();
    void clock();
    void runSimulation( boost::asio::yield_context& yield_ctx );

    auto getElapsedTime() const { return std::chrono::steady_clock::now() - m_startTime; }

private:
    network::Sender::Ptr                                 m_pRequestChannelSender;
    mega::Scheduler                                      m_scheduler;
    StateMachine                                         m_stateMachine;
    boost::asio::steady_timer                            m_timer;
    std::chrono::time_point< std::chrono::steady_clock > m_startTime = std::chrono::steady_clock::now();
    StateMachine::MsgVector                              m_messageQueue;
    mega::Clock                                          m_clock;
};

} // namespace mega::service

#endif // SIMULATION_22_JUNE_2022