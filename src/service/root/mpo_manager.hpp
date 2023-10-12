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

#include "mega/ring_allocator.hpp"
#include "mega/reference_io.hpp"

#include "service/protocol/common/logical_thread_id.hpp"
#include "service/network/log.hpp"

#include "common/assert_verify.hpp"

#include <unordered_map>
#include <unordered_set>
#include <array>

namespace mega::service
{

class MPOManager
{
    class MachineAllocators
    {
        static constexpr auto MAX_PROCESS_PER_MACHINE = 256U;
        static constexpr auto MAX_OWNER_PER_PROCESS = 256U;

        using ProcessAllocator = mega::RingAllocator< mega::ProcessID, MAX_PROCESS_PER_MACHINE >;
        using OwnerAllocator   = mega::RingAllocator< mega::OwnerID, MAX_OWNER_PER_PROCESS >;
        using OwnerArray       = std::array< OwnerAllocator, MAX_PROCESS_PER_MACHINE >;

        MachineID        m_machineID;
        ProcessAllocator m_processes;
        OwnerArray       m_owners;

    public:
        MachineAllocators( MachineID machineID )
            : m_machineID( machineID )
        {
        }

        MP newLeaf()
        {
            if( m_processes.full() )
            {
                THROW_RTE( "MPOMgr: Maximum machine capacity reached on: " << m_machineID );
            }

            const MP mp( m_machineID, m_processes.allocate() );
            SPDLOG_TRACE( "MPOMGR: newLeaf: {}", mp );
            return mp;
        }

        std::vector< MPO > leafDisconnected( MP mp )
        {
            SPDLOG_TRACE( "MPOMGR: leafDisconnected: {}", mp );

            m_processes.free( mp.getProcessID() );

            std::vector< MPO > allocated;
            {
                // free all owners for the process
                OwnerAllocator& owners = m_owners[ mp.getProcessID() ];
                for( OwnerID id : owners.getAllocated() )
                {
                    allocated.emplace_back( MPO( mp.getMachineID(), mp.getProcessID(), id ) );
                }
                owners.reset();
            }

            return allocated;
        }

        MPO newOwner( MP leafMP, const network::LogicalThreadID& logicalthreadID )
        {
            OwnerAllocator& alloc = m_owners[ leafMP.getProcessID() ];
            if( alloc.full() )
            {
                THROW_RTE( "MPOMgr: Maximum machine capacity reached" );
            }
            const MPO mpo( leafMP.getMachineID(), leafMP.getProcessID(), alloc.allocate() );
            SPDLOG_TRACE( "MPOMgr: newOwner: {} {}", mpo, logicalthreadID );

            return mpo;
        }

        void release( MPO mpo ) { m_owners[ mpo.getProcessID() ].free( mpo.getOwnerID() ); }

        std::vector< MP > getProcesses() const
        {
            std::vector< MP > processes;
            for( auto id : m_processes.getAllocated() )
            {
                processes.emplace_back( MP( m_machineID, id ) );
            }
            return processes;
        }

        std::vector< MPO > getOwners( MP mp ) const
        {
            std::vector< MPO > owners;
            for( auto id : m_owners[ mp.getProcessID() ].getAllocated() )
            {
                owners.emplace_back( MPO( mp, id ) );
            }
            return owners;
        }
    };

    using MPOMap            = std::unordered_map< MachineID, MachineAllocators >;
    using MachineIDFreeList = std::unordered_set< MachineID >;

    MPOMap            m_mpoMap;
    MachineIDFreeList m_freeList;

    MachineAllocators& get( MachineID machineID )
    {
        auto iFind = m_mpoMap.find( machineID );
        VERIFY_RTE_MSG( iFind != m_mpoMap.end(), "Failed to locate machineID: " << machineID );
        return iFind->second;
    }

    const MachineAllocators& get( MachineID machineID ) const
    {
        auto iFind = m_mpoMap.find( machineID );
        VERIFY_RTE_MSG( iFind != m_mpoMap.end(), "Failed to locate machineID: " << machineID );
        return iFind->second;
    }

public:
    MachineID newDaemon()
    {
        MachineID machineID;
        if( !m_freeList.empty() )
        {
            machineID = *m_freeList.begin();
            m_freeList.erase( m_freeList.begin() );
        }
        else
        {
            machineID = m_mpoMap.size();
        }
        auto ib = m_mpoMap.insert( { machineID, MachineAllocators( machineID ) } );
        VERIFY_RTE_MSG( ib.second, "Failed to create machineID" << machineID );

        SPDLOG_TRACE( "MPOMGR: newDaemon: {}", machineID );
        return machineID;
    }

    void daemonDisconnect( MachineID machineID )
    {
        auto iFind = m_mpoMap.find( machineID );
        VERIFY_RTE_MSG( iFind != m_mpoMap.end(), "Failed to locate machineID: " << machineID );
        m_mpoMap.erase( iFind );
        SPDLOG_TRACE( "MPOMGR: daemonDisconnect: {}", machineID );
        m_freeList.insert( machineID );
    }

    MP newLeaf( MachineID machineID ) { return get( machineID ).newLeaf(); }

    std::vector< MPO > leafDisconnected( MP mp ) { return get( mp.getMachineID() ).leafDisconnected( mp ); }

    MPO newOwner( MP leafMP, const network::LogicalThreadID& logicalthreadID )
    {
        return get( leafMP.getMachineID() ).newOwner( leafMP, logicalthreadID );
    }

    void release( MPO mpo ) { return get( mpo.getMachineID() ).release( mpo ); }

    std::vector< MachineID > getMachines() const
    {
        std::vector< MachineID > machines;
        for( const auto& [ machineID, state ] : m_mpoMap )
        {
            machines.push_back( machineID );
        }
        std::sort( machines.begin(), machines.end() );
        return machines;
    }

    std::vector< MP > getMachineProcesses( MachineID machineID ) const
    {
        std::vector< MP > result = get( machineID ).getProcesses();
        std::sort( result.begin(), result.end() );
        return result;
    }

    std::vector< MPO > getMPO( MP machineProcess ) const
    {
        std::vector< MPO > result = get( machineProcess.getMachineID() ).getOwners( machineProcess );
        std::sort( result.begin(), result.end() );
        return result;
    }
};

} // namespace mega::service

#endif // MPO_MANAGER_25_AUG_2022
