
#ifndef SIMULATION_22_JUNE_2022
#define SIMULATION_22_JUNE_2022

#include "request.hpp"

#include "service/lock_tracker.hpp"
#include "service/state_machine.hpp"
#include "service/network/sender.hpp"
#include "service/protocol/common/header.hpp"
#include "service/protocol/model/exe_sim.hxx"

#include "mega/common.hpp"
#include "mega/basic_scheduler.hpp"
#include "mega/root.hpp"

namespace mega
{
namespace service
{
class Executor;

class Simulation : public ExecutorRequestConversation, public network::exe_sim::Impl, public mega::MPOContext
{
public:
    using Ptr = std::shared_ptr< Simulation >;

    Simulation( Executor& executor, const network::ConversationID& conversationID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;
    virtual void             dispatchResponse( const network::ConnectionID& connectionID,
                                               const network::Message&      msg,
                                               boost::asio::yield_context&  yield_ctx ) override;

    virtual void error( const network::ConnectionID& connectionID, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx ) override;

    virtual void run( boost::asio::yield_context& yield_ctx ) override;

    network::Sender& getRequestSender()
    {
        if ( !m_pRequestChannelSender )
        {
            m_pRequestChannelSender
                = network::make_current_channel_sender( m_requestChannel, network::ConnectionID{ "sim" } );
        }
        return *m_pRequestChannelSender;
    }

    //  network::exe_sim::Impl
    /*virtual void ExeSimDestroy( const mega::network::ConversationID& simulationID,
                                boost::asio::yield_context&          yield_ctx ) override;
    virtual void ExeSimReadLockAcquire( const mega::network::ConversationID& owningID,
                                        const mega::network::ConversationID& simulationID,
                                        boost::asio::yield_context&          yield_ctx ) override;
    virtual void ExeSimWriteLockAcquire( const mega::network::ConversationID& owningID,
                                         const mega::network::ConversationID& simulationID,
                                         boost::asio::yield_context&          yield_ctx ) override;
    virtual void ExeSimLockRelease( const mega::network::ConversationID& owningID,
                                    const mega::network::ConversationID& simulationID,
                                    boost::asio::yield_context&          yield_ctx ) override;*/

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
    void acknowledgeMessage( const network::ChannelMsg&                            msg,
                             const std::optional< mega::network::ConversationID >& requestingID,
                             boost::asio::yield_context&                           yield_ctx );

    auto getElapsedTime() const { return std::chrono::steady_clock::now() - m_startTime; }

private:
    network::ConcurrentChannel                           m_requestChannel;
    network::Sender::Ptr                                 m_pRequestChannelSender;
    boost::asio::yield_context*                          m_pYieldContext = nullptr;
    std::optional< mega::MPO >                           m_mpo;
    std::shared_ptr< mega::runtime::MPORoot >            m_pExecutionRoot;
    mega::Scheduler                                      m_scheduler;
    SimulationStateMachine                               m_stateMachine;
    boost::asio::steady_timer                            m_timer;
    std::chrono::time_point< std::chrono::steady_clock > m_startTime = std::chrono::steady_clock::now();
    LockTracker                                          m_lockTracker;
};

} // namespace service
} // namespace mega

#endif // SIMULATION_22_JUNE_2022