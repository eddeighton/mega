
#ifndef SIMULATION_22_JUNE_2022
#define SIMULATION_22_JUNE_2022

#include "mega/common.hpp"
#include "mega/basic_scheduler.hpp"
#include "mega/execution_context.hpp"
#include "mega/root.hpp"

#include "request.hpp"
#include "service/network/sender.hpp"
#include "service/protocol/common/header.hpp"

#include "service/protocol/model/exe_sim.hxx"

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

    void simulationDeadline();
    void runSimulation( boost::asio::yield_context& yield_ctx );

    network::Sender& getRequestSender()
    {
        if ( !m_pRequestChannelSender )
        {
            m_pRequestChannelSender
                = network::make_current_channel_sender( m_requestChannel, network::ConnectionID{ "test" } );
        }
        return *m_pRequestChannelSender;
    }

    //  network::exe_sim::Impl
    virtual void ExeSimReadLockAcquire( const mega::network::ConversationID& simulationID,
                                        boost::asio::yield_context&          yield_ctx ) override;
    virtual void ExeSimWriteLockAcquire( const mega::network::ConversationID& simulationID,
                                         boost::asio::yield_context&          yield_ctx ) override;
    virtual void ExeSimLockRelease( const mega::network::ConversationID& simulationID,
                                    boost::asio::yield_context&          yield_ctx ) override;

    // mega::ExecutionContext
    virtual ExecutionIndex  getThisExecutionIndex() override;
    virtual mega::reference getRoot() override;
    virtual std::string     acquireMemory( ExecutionIndex executionIndex ) override;
    virtual LogicalAddress  allocateLogical( ExecutionIndex executionIndex, TypeID objectTypeID ) override;
    virtual void            deAllocateLogical( ExecutionIndex executionIndex, LogicalAddress logicalAddress ) override;
    virtual void            stash( const std::string& filePath, std::size_t determinant ) override;
    virtual bool            restore( const std::string& filePath, std::size_t determinant ) override;
    virtual void            readLock( ExecutionIndex executionIndex ) override;
    virtual void            writeLock( ExecutionIndex executionIndex ) override;
    virtual void            releaseLock( ExecutionIndex executionIndex ) override;

private:
    network::ConcurrentChannel                    m_requestChannel;
    network::Sender::Ptr                          m_pRequestChannelSender;
    boost::asio::yield_context*                   m_pYieldContext = nullptr;
    std::optional< mega::ExecutionIndex >         m_executionIndex;
    std::optional< mega::runtime::ExecutionRoot > m_executionRoot;
    mega::Scheduler                               m_scheduler;
};

} // namespace service
} // namespace mega

#endif // SIMULATION_22_JUNE_2022