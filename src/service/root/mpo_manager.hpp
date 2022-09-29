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

    template < typename T, T max, typename FreeList >
    inline std::vector< T > inverseOfFree( const FreeList& free ) const
    {
        std::vector< T >       allocated;
        std::vector< OwnerID > freeOwners( free.begin(), free.end() );
        std::sort( freeOwners.begin(), freeOwners.end() );
        auto iter = freeOwners.begin();
        // calculate the inverse of the free
        for ( T id = 0U; id != max; ++id )
        {
            if ( ( iter != freeOwners.end() ) && ( *iter == id ) )
            {
                ++iter;
            }
            else
            {
                allocated.push_back( id );
            }
        }
        return allocated;
    }

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

    std::vector< MPO > leafDisconnected( MP mp )
    {
        SPDLOG_TRACE( "MPOMGR: leafDisconnected: {}", mp );

        // free the process within the machine
        m_processes[ mp.getMachineID() ].free( mp.getProcessID() );

        std::vector< MPO > allocated;
        {
            // free all owners for the process
            OwnerAllocator& owners = m_owners[ mp.getMachineID() ][ mp.getProcessID() ];
            for ( OwnerID id : inverseOfFree< OwnerID, mega::MAX_PROCESS_PER_MACHINE >( owners.getFree() ) )
            {
                allocated.push_back( MPO( mp.getMachineID(), mp.getProcessID(), id ) );
            }
            owners.reset();
        }

        return allocated;
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

        return mpo;
    }

    void release( MPO mpo ) { m_owners[ mpo.getMachineID() ][ mpo.getProcessID() ].free( mpo.getOwnerID() ); }

    std::vector< MachineID > getMachines() const
    {
        std::vector< MachineID > machines;
        for ( auto id : inverseOfFree< MachineID, mega::MAX_MACHINES >( m_machines.getFree() ) )
        {
            machines.push_back( id );
        }
        return machines;
    }
    std::vector< MP > getMachineProcesses( MachineID machineID ) const
    {
        std::vector< MP > processes;
        for ( auto id :
              inverseOfFree< ProcessID, mega::MAX_PROCESS_PER_MACHINE >( m_processes[ machineID ].getFree() ) )
        {
            processes.push_back( MP( machineID, id, false ) );
        }
        return processes;
    }
    std::vector< MPO > getMPO( MP machineProcess ) const
    {
        std::vector< MPO > mpos;
        for ( auto id : inverseOfFree< OwnerID, mega::MAX_PROCESS_PER_MACHINE >(
                  m_owners[ machineProcess.getMachineID() ][ machineProcess.getProcessID() ].getFree() ) )
        {
            mpos.push_back( MPO( machineProcess.getMachineID(), machineProcess.getProcessID(), id ) );
        }
        return mpos;
    }

private:
    MachineAllocator m_machines;
    ProcessAllocator m_processes[ mega::MAX_MACHINES ];
    OwnerAllocator   m_owners[ mega::MAX_MACHINES ][ mega::MAX_PROCESS_PER_MACHINE ];
};
} // namespace service
} // namespace mega

#endif // MPO_MANAGER_25_AUG_2022
