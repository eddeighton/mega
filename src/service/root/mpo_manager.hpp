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

#include "mega/values/service/logical_thread_id.hpp"
#include "log/log.hpp"

#include "common/assert_verify.hpp"

#include <unordered_map>
#include <unordered_set>
#include <array>
#include <limits>

namespace mega::service
{

class MPOManager
{
    class MachineAllocators
    {
        static constexpr auto MAX_PROCESS_PER_MACHINE = std::numeric_limits< runtime::ProcessID::ValueType >::max();
        static constexpr auto MAX_OWNER_PER_PROCESS   = std::numeric_limits< runtime::OwnerID::ValueType >::max();

        using ProcessAllocator = RingAllocator< runtime::ProcessID, MAX_PROCESS_PER_MACHINE >;
        using OwnerAllocator   = RingAllocator< runtime::OwnerID, MAX_OWNER_PER_PROCESS >;
        using OwnerArray       = std::array< OwnerAllocator, MAX_PROCESS_PER_MACHINE >;

        runtime::MachineID m_machineID;
        ProcessAllocator   m_processes;
        OwnerArray         m_owners;

    public:
        MachineAllocators( runtime::MachineID machineID )
            : m_machineID( machineID )
        {
        }

        runtime::MP newLeaf()
        {
            if( m_processes.full() )
            {
                THROW_RTE( "MPOMgr: Maximum machine capacity reached on: " << m_machineID );
            }

            const runtime::MP mp( m_machineID, m_processes.allocate() );
            SPDLOG_TRACE( "MPOMGR: newLeaf: {}", mp );
            return mp;
        }

        std::vector< runtime::MPO > leafDisconnected( runtime::MP mp )
        {
            SPDLOG_TRACE( "MPOMGR: leafDisconnected: {}", mp );

            m_processes.free( mp.getProcessID() );

            std::vector< runtime::MPO > allocated;
            {
                // free all owners for the process
                OwnerAllocator& owners = m_owners[ mp.getProcessID().getValue() ];
                for( runtime::OwnerID id : owners.getAllocated() )
                {
                    allocated.emplace_back( runtime::MPO( mp.getMachineID(), mp.getProcessID(), id ) );
                }
                owners.reset();
            }

            return allocated;
        }

        runtime::MPO newOwner( runtime::MP leafMP, const network::LogicalThreadID& logicalthreadID )
        {
            OwnerAllocator& alloc = m_owners[ leafMP.getProcessID().getValue() ];
            if( alloc.full() )
            {
                THROW_RTE( "MPOMgr: Maximum machine capacity reached" );
            }
            const runtime::MPO mpo( leafMP.getMachineID(), leafMP.getProcessID(), alloc.allocate() );
            SPDLOG_TRACE( "MPOMgr: newOwner: {} {}", mpo, logicalthreadID );

            return mpo;
        }

        void release( runtime::MPO mpo ) { m_owners[ mpo.getProcessID().getValue() ].free( mpo.getOwnerID() ); }

        std::vector< runtime::MP > getProcesses() const
        {
            std::vector< runtime::MP > processes;
            for( auto id : m_processes.getAllocated() )
            {
                processes.emplace_back( runtime::MP( m_machineID, id ) );
            }
            return processes;
        }

        std::vector< runtime::MPO > getOwners( runtime::MP mp ) const
        {
            std::vector< runtime::MPO > owners;
            for( auto id : m_owners[ mp.getProcessID().getValue() ].getAllocated() )
            {
                owners.emplace_back( runtime::MPO( mp, id ) );
            }
            return owners;
        }
    };

    using MPOMap            = std::unordered_map< runtime::MachineID, MachineAllocators, runtime::MachineID::Hash >;
    using MachineIDFreeList = std::unordered_set< runtime::MachineID, runtime::MachineID::Hash >;

    MPOMap            m_mpoMap;
    MachineIDFreeList m_freeList;

    MachineAllocators& get( runtime::MachineID machineID )
    {
        auto iFind = m_mpoMap.find( machineID );
        VERIFY_RTE_MSG( iFind != m_mpoMap.end(), "Failed to locate machineID: " << machineID );
        return iFind->second;
    }

    const MachineAllocators& get( runtime::MachineID machineID ) const
    {
        auto iFind = m_mpoMap.find( machineID );
        VERIFY_RTE_MSG( iFind != m_mpoMap.end(), "Failed to locate machineID: " << machineID );
        return iFind->second;
    }

public:
    runtime::MachineID newDaemon()
    {
        runtime::MachineID machineID;
        if( !m_freeList.empty() )
        {
            machineID = *m_freeList.begin();
            m_freeList.erase( m_freeList.begin() );
        }
        else
        {
            machineID = runtime::MachineID{ static_cast< runtime::MachineID::ValueType >( m_mpoMap.size() ) };
        }
        auto ib = m_mpoMap.insert( { machineID, MachineAllocators( machineID ) } );
        VERIFY_RTE_MSG( ib.second, "Failed to create machineID" << machineID );

        SPDLOG_TRACE( "MPOMGR: newDaemon: {}", machineID );
        return machineID;
    }

    void daemonDisconnect( runtime::MachineID machineID )
    {
        auto iFind = m_mpoMap.find( machineID );
        VERIFY_RTE_MSG( iFind != m_mpoMap.end(), "Failed to locate machineID: " << machineID );
        m_mpoMap.erase( iFind );
        SPDLOG_TRACE( "MPOMGR: daemonDisconnect: {}", machineID );
        m_freeList.insert( machineID );
    }

    runtime::MP newLeaf( runtime::MachineID machineID ) { return get( machineID ).newLeaf(); }

    std::vector< runtime::MPO > leafDisconnected( runtime::MP mp )
    {
        return get( mp.getMachineID() ).leafDisconnected( mp );
    }

    runtime::MPO newOwner( runtime::MP leafMP, const network::LogicalThreadID& logicalthreadID )
    {
        return get( leafMP.getMachineID() ).newOwner( leafMP, logicalthreadID );
    }

    void release( runtime::MPO mpo ) { return get( mpo.getMachineID() ).release( mpo ); }

    std::vector< runtime::MachineID > getMachines() const
    {
        std::vector< runtime::MachineID > machines;
        for( const auto& [ machineID, state ] : m_mpoMap )
        {
            machines.push_back( machineID );
        }
        std::sort( machines.begin(), machines.end() );
        return machines;
    }

    std::vector< runtime::MP > getMachineProcesses( runtime::MachineID machineID ) const
    {
        std::vector< runtime::MP > result = get( machineID ).getProcesses();
        std::sort( result.begin(), result.end() );
        return result;
    }

    std::vector< runtime::MPO > getMPO( runtime::MP machineProcess ) const
    {
        std::vector< runtime::MPO > result = get( machineProcess.getMachineID() ).getOwners( machineProcess );
        std::sort( result.begin(), result.end() );
        return result;
    }
};

} // namespace mega::service

#endif // MPO_MANAGER_25_AUG_2022
