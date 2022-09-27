#ifndef MPO_MANAGER_25_AUG_2022
#define MPO_MANAGER_25_AUG_2022

#include "mega/common.hpp"
#include "mega/allocator.hpp"

#include "service/protocol/common/header.hpp"
#include "service/network/log.hpp"

#include <set>
#include <unordered_map>

namespace mega
{
namespace service
{
class MPOManager
{
    using MachineAllocator = mega::RingAllocator< mega::MachineID, mega::MAX_MACHINES >;
    using ProcessAllocator = mega::RingAllocator< mega::ProcessID, mega::MAX_PROCESS_PER_MACHINE >;
    using OwnerAllocator   = mega::RingAllocator< mega::OwnerID, mega::MAX_OWNER_PER_PROCESS >;

public:
    MP newDaemon()
    {
        if ( m_machines.full() )
        {
            THROW_RTE( "MPOMgr: Maximum machine capacity reached" );
        }

        const MP mp( m_machines.allocate(), 0U, true );
        SPDLOG_TRACE( "MPOMGR: newDaemon: {}", mp );
        return mp;
    }

    void daemonDisconnect( MP mp )
    {
        MachineID m = mp.getMachineID();

        m_machines.free( m );
        m_processes[ m ].reset();
        for ( OwnerAllocator& alloc : m_owners[ m ] )
        {
            alloc.reset();
        }
        SPDLOG_TRACE( "MPOMGR: daemonDisconnect: {}", mp );
    }

    MP newLeaf( MP daemonMP )
    {
        ProcessAllocator& alloc = m_processes[ daemonMP.getMachineID() ];
        if ( alloc.full() )
        {
            THROW_RTE( "MPOMgr: Maximum machine capacity reached" );
        }

        const MP mp( daemonMP.getMachineID(), alloc.allocate(), false );
        SPDLOG_TRACE( "MPOMGR: newLeaf: {}", mp );
        return mp;
    }

    void leafDisconnected( MP mp )
    {
        m_processes[ mp.getMachineID() ].free( mp.getProcessID() );
        m_owners[ mp.getMachineID() ][ mp.getProcessID() ].reset();
        SPDLOG_TRACE( "MPOMGR: leafDisconnected: {}", mp );

        // TODO - how to broadcast disconnects ??
        // THROW_RTE( "TODO" );

        for( const auto& [ simMPO, simID ] : m_simulations )
        {
            if( simMPO.getMachineID() == mp.getMachineID() )
            {
                if( simMPO.getProcessID() == mp.getProcessID() )
                {
                    // 
                }
            }
        }
    }

    MPO newOwner( MP leafMP, const network::ConversationID& conversationID )
    {
        OwnerAllocator& alloc = m_owners[ leafMP.getMachineID() ][ leafMP.getProcessID() ];
        if ( alloc.full() )
        {
            THROW_RTE( "MPOMgr: Maximum machine capacity reached" );
        }
        const MPO mpo( leafMP.getMachineID(), leafMP.getProcessID(), alloc.allocate() );
        SPDLOG_TRACE( "MPOMgr: newOwner: {} {}", mpo, conversationID );

        m_simulations.insert( { mpo, conversationID } );
        m_conversations.insert( { conversationID, mpo } );

        return mpo;
    }

    void release( const network::ConversationID& conversationID )
    {
        // called after graceful close of conversation
        auto iFind = m_conversations.find( conversationID );
        if ( iFind != m_conversations.end() )
        {
            mega::MPO mpo = iFind->second;
            m_conversations.erase( iFind );
            {
                auto jFind = m_simulations.find( mpo );
                VERIFY_RTE( jFind != m_simulations.end() );
                m_simulations.erase( jFind );
            }
            m_owners[ mpo.getMachineID() ][ mpo.getProcessID() ].free( mpo.getOwnerID() );
        }
    }

    const network::ConversationID& get( mega::MPO mpo ) const
    {
        auto iFind = m_simulations.find( mpo );
        VERIFY_RTE_MSG( iFind != m_simulations.end(), "MPOMgr: Could not find mpo" );
        return iFind->second;
    }

    mega::MPO get( const network::ConversationID& simID ) const
    {
        auto iFind = m_conversations.find( simID );
        VERIFY_RTE_MSG( iFind != m_conversations.end(), "MPOMgr: Could not find simID: " << simID );
        return iFind->second;
    }

private:
    MachineAllocator m_machines;
    ProcessAllocator m_processes[ mega::MAX_MACHINES ];
    OwnerAllocator   m_owners[ mega::MAX_MACHINES ][ mega::MAX_PROCESS_PER_MACHINE ];

    std::unordered_map< mega::MPO, network::ConversationID, mega::MPO::Hash >               m_simulations;
    std::unordered_map< network::ConversationID, mega::MPO, network::ConversationID::Hash > m_conversations;
};
} // namespace service
} // namespace mega

#endif // MPO_MANAGER_25_AUG_2022
