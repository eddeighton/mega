
#ifndef SIMULATION_22_JUNE_2022
#define SIMULATION_22_JUNE_2022

#include "request.hpp"

#include "service/state_machine.hpp"
#include "service/network/sender.hpp"
#include "service/protocol/common/header.hpp"
#include "service/protocol/model/exe_sim.hxx"

#include "mega/common.hpp"
#include "mega/basic_scheduler.hpp"
#include "mega/execution_context.hpp"
#include "mega/root.hpp"

namespace mega
{
namespace service
{
class Executor;

class Simulation : public ExecutorRequestConversation, public network::exe_sim::Impl, public mega::ExecutionContext
{
public:
    using Ptr = std::shared_ptr< Simulation >;

    Simulation( Executor& executor, const network::ConversationID& conversationID );

    virtual bool dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx ) override;

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
    virtual void ExeSimDestroy( const mega::network::ConversationID& simulationID,
                                boost::asio::yield_context&          yield_ctx ) override;
    virtual void ExeSimReadLockAcquire( const mega::network::ConversationID& owningID,
                                        const mega::network::ConversationID& simulationID,
                                        boost::asio::yield_context&          yield_ctx ) override;
    virtual void ExeSimWriteLockAcquire( const mega::network::ConversationID& owningID,
                                         const mega::network::ConversationID& simulationID,
                                         boost::asio::yield_context&          yield_ctx ) override;
    virtual void ExeSimLockRelease( const mega::network::ConversationID& owningID,
                                    const mega::network::ConversationID& simulationID,
                                    boost::asio::yield_context&          yield_ctx ) override;

    // mega::ExecutionContext
    virtual MPE             getThisMPE() override;
    virtual mega::reference getRoot() override;
    virtual std::string     acquireMemory( MPE mpe ) override;
    virtual NetworkAddress  allocateNetworkAddress( MPE mpe, TypeID objectTypeID ) override;
    virtual void            deAllocateNetworkAddress( MPE mpe, NetworkAddress networkAddress ) override;
    virtual void            stash( const std::string& filePath, std::size_t determinant ) override;
    virtual bool            restore( const std::string& filePath, std::size_t determinant ) override;
    virtual bool            readLock( MPE mpe ) override;
    virtual bool            writeLock( MPE mpe ) override;
    virtual void            releaseLock( MPE mpe ) override;

private:
    void issueClock();
    void clock();
    void runSimulation( boost::asio::yield_context& yield_ctx );
    void acknowledgeMessage( const network::ChannelMsg&                            msg,
                             const std::optional< mega::network::ConversationID >& requestingID,
                             boost::asio::yield_context&                           yield_ctx );

    auto getElapsedTime() const { return std::chrono::steady_clock::now() - m_startTime; }

private:
    network::ConcurrentChannel                           m_requestChannel;
    network::Sender::Ptr                                 m_pRequestChannelSender;
    boost::asio::yield_context*                          m_pYieldContext = nullptr;
    std::optional< mega::MPE >                           m_executionIndex;
    std::shared_ptr< mega::runtime::ExecutionRoot >      m_pExecutionRoot;
    mega::Scheduler                                      m_scheduler;
    SimulationStateMachine                               m_stateMachine;
    boost::asio::steady_timer                            m_timer;
    std::chrono::time_point< std::chrono::steady_clock > m_startTime = std::chrono::steady_clock::now();
};

} // namespace service
} // namespace mega

#endif // SIMULATION_22_JUNE_2022