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

#include "service/clock.hpp"

#include "service/executor/message_traits.hpp"
#include "service/executor/state_machine.hpp"
#include "service/executor/sim_move_machine.hpp"
#include "service/executor/transaction_machine.hpp"
#include "service/executor/request.hpp"

#include "service/mpo_context.hpp"

#include "service/network/sender_factory.hpp"

#include "mega/values/service/logical_thread_id.hpp"
#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/stash.hxx"

#include "mega/values/runtime/pointer.hpp"

namespace mega::service
{

class Executor;

class Simulation : public ExecutorRequestLogicalThread, public runtime::MPOContext
{
public:
    using Ptr = std::shared_ptr< Simulation >;

    Simulation( Executor& executor, const network::LogicalThreadID& logicalthreadID, ProcessClock& processClock );

    virtual network::Message dispatchInBoundRequest( const network::Message&     msg,
                                                     boost::asio::yield_context& yield_ctx ) override;
    virtual void             unqueue() override;
    virtual bool             queue( const network::ReceivedMessage& msg ) override;
    virtual void             run( boost::asio::yield_context& yield_ctx ) override;

    // MPOContext
    virtual network::mpo::Request_Sender     getMPRequest() override;
    virtual network::enrole::Request_Encoder getRootEnroleRequest() override;
    virtual network::stash::Request_Encoder  getRootStashRequest() override;
    virtual network::memory::Request_Encoder getDaemonMemoryRequest() override;
    virtual network::sim::Request_Encoder    getMPOSimRequest( runtime::MPO mpo ) override;
    virtual network::memory::Request_Sender  getLeafMemoryRequest() override;
    virtual network::jit::Request_Sender     getLeafJITRequest() override;

    // network::sim::Impl
    virtual void     SimErrorCheck( boost::asio::yield_context& yield_ctx ) override;
    virtual Snapshot SimObjectSnapshot( const runtime::PointerNet& object, boost::asio::yield_context& ) override;
    virtual runtime::PointerHeap SimAllocate( const concrete::ObjectID& objectTypeID,
                                              boost::asio::yield_context& ) override;
    virtual Snapshot             SimSnapshot( const runtime::MPO&, boost::asio::yield_context& ) override;
    virtual runtime::TimeStamp
    SimLockRead( const runtime::MPO&, const runtime::MPO&, boost::asio::yield_context& ) override;
    virtual runtime::TimeStamp
                         SimLockWrite( const runtime::MPO&, const runtime::MPO&, boost::asio::yield_context& ) override;
    virtual void         SimLockRelease( const runtime::MPO&,
                                         const runtime::MPO&,
                                         const network::Transaction&,
                                         boost::asio::yield_context& ) override;
    virtual runtime::MPO SimCreate( boost::asio::yield_context& ) override;
    virtual void         SimDestroy( boost::asio::yield_context& ) override;
    virtual void         SimDestroyBlocking( boost::asio::yield_context& ) override;

    // network::leaf_exe::Impl
    virtual void RootSimRun( const runtime::MPO& mpo, boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual std::string     Ping( const std::string& strMsg, boost::asio::yield_context& yield_ctx ) override;

    // network::report::Impl
    virtual Report GetReport( const URL&           url,
                              const std::vector< Report >& report,
                              boost::asio::yield_context&  yield_ctx ) override;

private:
    void runSimulation( boost::asio::yield_context& yield_ctx );

    friend class TransactionMachine< Simulation >;
    // Simulation concept for state machine
    bool unrequestClock();

private:
    ProcessClock&        m_processClock;
    network::Sender::Ptr m_pRequestChannelSender;
    MsgTraits::AckVector m_ackVector;
    MsgTraits::MsgVector m_messageQueue;

    TransactionMachine< Simulation > m_transactionMachine;
    SimMoveMachine< Simulation >     m_simMoveMachine;
    using SM = StateMachine< Simulation >;
    SM m_stateMachine;

    int m_queueStack = 0;
    struct QueueStackDepth
    {
        int& stackDepth;
        QueueStackDepth( int& st )
            : stackDepth( st )
        {
            ++stackDepth;
        }
        ~QueueStackDepth() { --stackDepth; }
    };
    std::string                               m_strSimCreateError;
    std::optional< network::ReceivedMessage > m_simCreateMsgOpt;
    bool                                      m_bShuttingDown = false;
    std::optional< MsgTraits::Msg >           m_blockDestroyMsgOpt;
};

} // namespace mega::service

#endif // SIMULATION_22_JUNE_2022