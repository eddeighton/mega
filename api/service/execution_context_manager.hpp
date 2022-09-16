#ifndef EXECUTION_CONTEXT_MANAGER_25_AUG_2022
#define EXECUTION_CONTEXT_MANAGER_25_AUG_2022

#include "mega/common.hpp"
#include "mega/allocator.hpp"

#include "service/protocol/common/header.hpp"
#include "service/network/log.hpp"

#include <set>
#include <unordered_map>
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
    MPE newDaemon()
    {
        if ( m_machines.full() )
        {
            THROW_RTE( "Maximum machine capacity reached" );
        }

        const MPE mpe( m_machines.allocate(), 0U, 0U );
        SPDLOG_TRACE( "ECM: newDaemon: {}", mpe );
        return mpe;
    }

    void daemonDisconnect( MPE mpe )
    {
        MachineID m = mpe.getMachineID();

        m_machines.free( m );
        m_processes[ m ].reset();
        for ( ExecutorAllocator& alloc : m_executors[ m ] )
        {
            alloc.reset();
        }
        SPDLOG_TRACE( "ECM: daemonDisconnect: {}", mpe );
    }

    MPE newLeaf( MPE daemonMPE )
    {
        ProcessAllocator& alloc = m_processes[ daemonMPE.getMachineID() ];
        if ( alloc.full() )
        {
            THROW_RTE( "Maximum machine capacity reached" );
        }

        const MPE mpe( daemonMPE.getMachineID(), alloc.allocate(), 0U );
        SPDLOG_TRACE( "ECM: newLeaf: {}", mpe );
        return mpe;
    }

    void leafDisconnected( MPE mpe )
    {
        m_processes[ mpe.getMachineID() ].free( mpe.getProcessID() );
        m_executors[ mpe.getMachineID() ][ mpe.getProcessID() ].reset();
        SPDLOG_TRACE( "ECM: leafDisconnected: {}", mpe );
    }

    MPE newExecutor( MPE leafMPE, const network::ConversationID& conversationID )
    {
        ExecutorAllocator& alloc = m_executors[ leafMPE.getMachineID() ][ leafMPE.getProcessID() ];
        if ( alloc.full() )
        {
            THROW_RTE( "Maximum machine capacity reached" );
        }
        const MPE mpe( leafMPE.getMachineID(), leafMPE.getProcessID(), alloc.allocate() );
        SPDLOG_TRACE( "ECM: newExecutor: {}", mpe );

        m_simulations.insert( { mpe, conversationID } );
        m_conversations.insert( { conversationID, mpe } );

        return mpe;
    }

    void release( const network::ConversationID& conversationID )
    {
        // called after graceful close of conversation
        auto iFind = m_conversations.find( conversationID );
        if ( iFind != m_conversations.end() )
        {
            mega::MPE mpe = iFind->second;
            m_conversations.erase( iFind );
            {
                auto jFind = m_simulations.find( mpe );
                VERIFY_RTE( jFind != m_simulations.end() );
                m_simulations.erase( jFind );
            }
            m_executors[ mpe.getMachineID() ][ mpe.getProcessID() ].free( mpe.getExecutorID() );
        }
    }

    const network::ConversationID& get( mega::MPE mpe ) const
    {
        auto iFind = m_simulations.find( mpe );
        VERIFY_RTE_MSG( iFind != m_simulations.end(), "Could not find mpe" );
        return iFind->second;
    }

    mega::MPE get( const network::ConversationID& simID ) const
    {
        auto iFind = m_conversations.find( simID );
        VERIFY_RTE_MSG( iFind != m_conversations.end(), "Could not find simID: " << simID );
        return iFind->second;
    }

private:
    MachineAllocator  m_machines;
    ProcessAllocator  m_processes[ mega::MAX_MACHINES ];
    ExecutorAllocator m_executors[ mega::MAX_MACHINES ][ mega::MAX_PROCESS_PER_MACHINE ];

    std::unordered_map< mega::MPE, network::ConversationID, mega::MPE::Hash >               m_simulations;
    std::unordered_map< network::ConversationID, mega::MPE, network::ConversationID::Hash > m_conversations;
};
} // namespace service
} // namespace mega

#endif // EXECUTION_CONTEXT_MANAGER_25_AUG_2022
