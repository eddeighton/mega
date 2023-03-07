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

#include "service/executor/request.hpp"

#include "clock.hpp"
#include "scheduler.hpp"

#include "service/executor/state_machine.hpp"
#include "service/network/sender_factory.hpp"
#include "service/protocol/common/conversation_id.hpp"

#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/stash.hxx"

#include "service/mpo_context.hpp"

#include "mega/reference.hpp"

namespace mega::service
{

class Executor;

class Simulation : public ExecutorRequestConversation, public MPOContext
{
public:
    using Ptr = std::shared_ptr< Simulation >;

    Simulation( Executor& executor, const network::ConversationID& conversationID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;

    virtual network::Message dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx ) override;
    virtual void             run( boost::asio::yield_context& yield_ctx ) override;

    // MPOContext
    virtual network::mpo::Request_Sender     getMPRequest() override;
    virtual network::enrole::Request_Encoder getRootEnroleRequest() override;
    virtual network::stash::Request_Encoder  getRootStashRequest() override;
    virtual network::memory::Request_Encoder getDaemonMemoryRequest() override;
    virtual network::sim::Request_Encoder    getMPOSimRequest( MPO mpo ) override;
    virtual network::memory::Request_Sender  getLeafMemoryRequest() override;
    virtual network::jit::Request_Sender     getLeafJITRequest() override;

    // network::sim::Impl
    virtual void SimErrorCheck(boost::asio::yield_context& yield_ctx) override;
    virtual Snapshot SimObjectSnapshot( const reference& object, boost::asio::yield_context& ) override;
    virtual reference SimAllocate( const TypeID& objectTypeID, boost::asio::yield_context& ) override;
    virtual Snapshot SimSnapshot( const MPO&, boost::asio::yield_context& ) override;
    virtual TimeStamp SimLockRead( const MPO&, const MPO&, boost::asio::yield_context& ) override;
    virtual TimeStamp SimLockWrite( const MPO&, const MPO&, boost::asio::yield_context& ) override;
    virtual void
    SimLockRelease( const MPO&, const MPO&, const network::Transaction&, boost::asio::yield_context& ) override;
    virtual void SimClock( boost::asio::yield_context& ) override;
    virtual MPO  SimCreate( boost::asio::yield_context& ) override;
    virtual void SimDestroy( boost::asio::yield_context& ) override;

    // network::leaf_exe::Impl
    virtual void RootSimRun( const MPO& mpo, boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual std::string     Ping( const std::string& strMsg, boost::asio::yield_context& yield_ctx ) override;

    // MPOContext
    // clock
    virtual TimeStamp cycle() override { return m_clock.cycle(); }
    virtual F32       ct() override { return m_clock.ct(); }
    virtual F32       dt() override { return m_clock.dt(); }

private:
    void issueClock();
    void clock();
    void runSimulation( boost::asio::yield_context& yield_ctx );

    auto getElapsedTime() const { return std::chrono::steady_clock::now() - m_startTime; }

private:
    network::Sender::Ptr                                 m_pRequestChannelSender;
    Scheduler                                            m_scheduler;
    StateMachine                                         m_stateMachine;
    boost::asio::steady_timer                            m_timer;
    std::chrono::time_point< std::chrono::steady_clock > m_startTime = std::chrono::steady_clock::now();
    StateMachine::MsgVector                              m_messageQueue;
    Clock                                                m_clock;
    std::string                                          m_strSimCreateError;
};

} // namespace mega::service

#endif // SIMULATION_22_JUNE_2022