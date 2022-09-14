#ifndef EXECUTION_CONTEXT_MANAGER_25_AUG_2022
#define EXECUTION_CONTEXT_MANAGER_25_AUG_2022

#include "mega/common.hpp"
#include "mega/allocator.hpp"

#include "service/protocol/common/header.hpp"
#include "service/network/log.hpp"

#include <set>
#include <map>
#include <tuple>

namespace mega
{
namespace service
{
class ExecutionContextManager
{
    using ConversationGroup = std::tuple< network::ConnectionID, network::ConversationID, network::ConversationID >;

    using MachineAllocator  = mega::RingAllocator< mega::MachineID, mega::MAX_MACHINES >;
    using ProcessAllocator  = mega::RingAllocator< mega::ProcessID, mega::MAX_PROCESS_PER_MACHINE >;
    using ExecutorAllocator = mega::RingAllocator< mega::ExecutorID, mega::MAX_EXECUTOR_PER_PROCESS >;

public:
    MachineProcessExecutor newDaemon()
    {
        if ( m_machines.full() )
        {
            THROW_RTE( "Maximum machine capacity reached" );
        }

        MachineProcessExecutor mpe;
        {
            mpe.executor     = 0U;
            mpe.process      = 0U;
            mpe.machine      = m_machines.nextFree();
            mpe.address_type = MACHINE_ADDRESS;
        }
        m_machines.allocate( mpe.machine );

        SPDLOG_TRACE( "ExecutionContextManager: newDaemon: {}", mpe );

        return mpe;
    }
    MachineProcessExecutor newLeaf( MPEStorage storage )
    {
        MachineProcessExecutor mpe{ storage };
        ProcessAllocator&      alloc = m_processes[ mpe.machine ];
        if ( alloc.full() )
        {
            THROW_RTE( "Maximum machine capacity reached" );
        }

        {
            mpe.executor = 0U;
            mpe.process  = alloc.nextFree();
            // mpe.machine      = mpe.machine;
            mpe.address_type = MACHINE_ADDRESS;
        }
        alloc.allocate( mpe.process );

        SPDLOG_TRACE( "ExecutionContextManager: newLeaf: {}", mpe );

        return mpe;
    }

    MachineProcessExecutor newExecutor( MPEStorage storage, const network::ConversationID& conversationID )
    {
        MachineProcessExecutor mpe{ storage };
        ExecutorAllocator&     alloc = m_executors[ mpe.machine ][ mpe.process ];
        if ( alloc.full() )
        {
            THROW_RTE( "Maximum machine capacity reached" );
        }

        {
            mpe.executor = alloc.nextFree();
            // mpe.process      = mpe.process;
            // mpe.machine      = mpe.machine;
            mpe.address_type = MACHINE_ADDRESS;
        }
        alloc.allocate( mpe.executor );

        SPDLOG_TRACE( "ExecutionContextManager: newExecutor: {}", mpe );

        m_simulations.insert( { mpe.mpe_storage, conversationID } );
        m_conversations.insert( { conversationID, mpe.mpe_storage } );

        return mpe;
    }

    void release( const network::ConversationID& conversationID )
    {
        /*auto iFind = m_conversations.find( conversationID );
        if ( iFind != m_conversations.end() )
        {
            const auto address = iFind->second;
            SPDLOG_TRACE( "Releasing execution context: {} for conversation: {}", address, conversationID );
            m_conversations.erase( iFind );
            m_allocations.free( address );
            m_simulations.erase( address );
        }*/
    }

    void release( mega::MPEStorage address )
    {
        // m_allocations.free( address );
        // m_simulations.erase( address );
    }

    const network::ConversationID& get( mega::MPEStorage mpe ) const
    {
        auto iFind = m_simulations.find( mpe );
        VERIFY_RTE_MSG( iFind != m_simulations.end(), "Failed to locate execution index: " << mpe );
        // return iFind->second;
        THROW_RTE( "not implemented" );
    }

private:
    MachineAllocator  m_machines;
    ProcessAllocator  m_processes[ mega::MAX_MACHINES ];
    ExecutorAllocator m_executors[ mega::MAX_MACHINES ][ mega::MAX_PROCESS_PER_MACHINE ];

    std::map< mega::MPEStorage, network::ConversationID > m_simulations;
    std::map< network::ConversationID, mega::MPEStorage > m_conversations;
};
} // namespace service
} // namespace mega

#endif // EXECUTION_CONTEXT_MANAGER_25_AUG_2022
