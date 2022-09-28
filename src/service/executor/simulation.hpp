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

#include "service/lock_tracker.hpp"
#include "service/state_machine.hpp"
#include "service/network/sender.hpp"
#include "service/protocol/common/header.hpp"
#include "service/protocol/model/sim.hxx"

#include "mega/common.hpp"
#include "mega/basic_scheduler.hpp"
#include "mega/root.hpp"

namespace mega
{
namespace service
{
class Executor;

class Simulation : public ExecutorRequestConversation, public mega::MPOContext
{
public:
    using Ptr = std::shared_ptr< Simulation >;

    Simulation( Executor& executor, const network::ConversationID& conversationID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;

    virtual network::Message dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx ) override;
    virtual void             run( boost::asio::yield_context& yield_ctx ) override;

    // network::sim::Impl
    virtual bool      SimLockRead( const mega::MPO&, boost::asio::yield_context& ) override;
    virtual bool      SimLockWrite( const mega::MPO&, boost::asio::yield_context& ) override;
    virtual void      SimLockRelease( const mega::MPO&, boost::asio::yield_context& ) override;
    virtual void      SimClock( boost::asio::yield_context& ) override;
    virtual mega::MPO SimCreate( boost::asio::yield_context& ) override;
    virtual void      SimDestroy( boost::asio::yield_context& ) override;

    // network::leaf_exe::Impl
    virtual void RootSimRun( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual std::string Ping( boost::asio::yield_context& yield_ctx ) override;

    // mega::MPOContext - native code interface
    virtual SimIDVector     getSimulationIDs() override;
    virtual SimID           createSimulation() override;
    virtual mega::reference getRoot( const SimID& simID ) override;
    virtual mega::reference getRoot() override;

    // mega::MPOContext - runtime internal interface
    virtual MPO            getThisMPO() override;
    virtual std::string    acquireMemory( MPO mpo ) override;
    virtual MPO            getNetworkAddressMPO( NetworkAddress networkAddress ) override;
    virtual NetworkAddress getRootNetworkAddress( MPO mpo ) override;
    virtual NetworkAddress allocateNetworkAddress( MPO mpo, TypeID objectTypeID ) override;
    virtual void           deAllocateNetworkAddress( MPO mpo, NetworkAddress networkAddress ) override;
    virtual void           stash( const std::string& filePath, mega::U64 determinant ) override;
    virtual bool           restore( const std::string& filePath, mega::U64 determinant ) override;
    virtual bool           readLock( MPO mpo ) override;
    virtual bool           writeLock( MPO mpo ) override;
    virtual void           releaseLock( MPO mpo ) override;

private:
    void issueClock();
    void clock();
    void cycle();
    void runSimulation( boost::asio::yield_context& yield_ctx );

    auto getElapsedTime() const { return std::chrono::steady_clock::now() - m_startTime; }

private:
    network::Sender::Ptr                                 m_pRequestChannelSender;
    boost::asio::yield_context*                          m_pYieldContext = nullptr;
    std::optional< mega::MPO >                           m_mpo;
    std::shared_ptr< mega::runtime::MPORoot >            m_pExecutionRoot;
    mega::Scheduler                                      m_scheduler;
    StateMachine                                         m_stateMachine;
    boost::asio::steady_timer                            m_timer;
    std::chrono::time_point< std::chrono::steady_clock > m_startTime = std::chrono::steady_clock::now();
    LockTracker                                          m_lockTracker;
    StateMachine::MsgVector                              m_messageQueue;
};

} // namespace service
} // namespace mega

#endif // SIMULATION_22_JUNE_2022