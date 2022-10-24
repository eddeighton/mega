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

#ifndef SHARED_MEMORY_MANAGER_3_SEPT_2022
#define SHARED_MEMORY_MANAGER_3_SEPT_2022

#include "database/database.hpp"

#include "mega/common.hpp"
#include "mega/reference.hpp"
#include "mega/reference_io.hpp"
#include "mega/default_traits.hpp"

#include "database/types/shared_memory_header.hpp"

#include "service/protocol/common/header.hpp"
#include "service/network/log.hpp"

#include <string>
#include <sstream>
#include <unordered_map>

namespace mega::service
{

class SharedMemoryManager
{
    struct AddressSpaceMapLifetime
    {
        const std::string& strName;
        AddressSpaceMapLifetime( const std::string& strName )
            : strName( strName )
        {
            boost::interprocess::shared_memory_object::remove( strName.c_str() );
        }
        ~AddressSpaceMapLifetime() { boost::interprocess::shared_memory_object::remove( strName.c_str() ); }
    };

    class SharedMemory
    {
        static constexpr mega::U64 SIZE = 1024 * 1024 * 4;

    public:
        using Ptr = std::unique_ptr< SharedMemory >;

        SharedMemory( const mega::MPO& mpo, const std::string& strName )
            : m_strName( strName )
            , m_memoryLifetime( m_strName )
            , m_memory( boost::interprocess::create_only, m_strName.c_str(), SIZE )
        {
        }

        const std::string&            getName() const { return m_strName; }
        runtime::ManagedSharedMemory& get() { return m_memory; }

    private:
        const std::string            m_strName;
        AddressSpaceMapLifetime      m_memoryLifetime;
        runtime::ManagedSharedMemory m_memory;
    };

    using SharedMemoryMap = std::unordered_map< mega::MPO, SharedMemory::Ptr, mega::MPO::Hash >;

    std::string memoryName( const mega::MPO& mpo ) const
    {
        std::ostringstream os;
        os << m_strDaemonPrefix << "_mem_" << mpo;
        return os.str();
    }

    mega::network::MemoryConfig calculateSharedConfig() const
    {
        mega::network::MemoryConfig config;
        {
            std::ostringstream os;
            os << m_strDaemonPrefix << "_address_space_mem";
            config.setMemory( os.str() );
        }
        {
            std::ostringstream os;
            os << m_strDaemonPrefix << "_address_space_mutex";
            config.setMutex( os.str() );
        }
        {
            std::ostringstream os;
            os << m_strDaemonPrefix << "_address_space_map";
            config.setMap( os.str() );
        }
        return config;
    }

    using ReferenceMap = std::unordered_map< reference, reference, reference::Hash >;

public:
    SharedMemoryManager( runtime::DatabaseInstance& database, const std::string& daemonPrefix )
        : m_database( database )
        , m_strDaemonPrefix( daemonPrefix )
        , m_config( calculateSharedConfig() )
        , m_memoryLifetime( m_config.getMemory() )
    {
    }

    SharedMemory& getOrCreate( const mega::MPO& mpo )
    {
        auto iFind = m_memory.find( mpo );
        if ( iFind != m_memory.end() )
        {
            return *iFind->second;
        }
        else
        {
            SPDLOG_TRACE( "SharedMemoryManager allocated: {}", mpo );
            SharedMemory::Ptr  pNewMemory = std::make_unique< SharedMemory >( mpo, memoryName( mpo ) );
            SharedMemory&      memory     = *pNewMemory;
            const std::string& strName    = pNewMemory->getName();
            m_memory.insert( { mpo, std::move( pNewMemory ) } );
            return memory;
        }
    }

    const std::string& acquire( const mega::MPO& mpo ) { return getOrCreate( mpo ).getName(); }

    void release( const mega::MPO& mpo )
    {
        auto iFind = m_memory.find( mpo );
        if ( iFind != m_memory.end() )
        {
            m_memory.erase( iFind );
            SPDLOG_INFO( "Daemon released memory for {}", mpo );
        }
    }

    const mega::network::MemoryConfig& getConfig() const { return m_config; }

    void allocated( reference network, reference machine )
    {
        m_networkToMachine.insert( { network, machine } );
        m_machineToNetwork.insert( { machine, network } );
    }

    void freed( reference ref )
    {
        if ( ref.isMachine() )
        {
            auto iFind = m_machineToNetwork.find( ref );
            if ( iFind != m_machineToNetwork.end() )
            {
                auto net = iFind->second;
                m_machineToNetwork.erase( iFind );
                m_networkToMachine.erase( net );
            }
            else
            {
                THROW_RTE( "Failed to locate reference: " << ref );
            }
        }
        else if ( ref.isNetwork() )
        {
            auto iFind = m_networkToMachine.find( ref );
            if ( iFind != m_networkToMachine.end() )
            {
                auto mac = iFind->second;
                m_networkToMachine.erase( iFind );
                m_machineToNetwork.erase( mac );
            }
            else
            {
                THROW_RTE( "Failed to locate reference: " << ref );
            }
        }
        else
        {
            THROW_RTE( "Unreachable" );
        }
    }

    reference allocate( MPO mpo, const TypeID& objectTypeID, const NetworkAddress& networkAddress )
    {
        const auto size = m_database.getObjectSize( objectTypeID );

        auto&         memory        = getOrCreate( mpo ).get();
        void*         pSharedMemory = memory.allocate_aligned( size.shared_size, size.shared_alignment );
        SharedHeader& hd            = makeSharedHeader( pSharedMemory );

        const reference machine{
            TypeInstance( 0u, objectTypeID ), mpo, toProcessAddress( memory.get_address(), pSharedMemory ) };
        const reference network{ TypeInstance( 0u, objectTypeID ), mpo, networkAddress };

        allocated( network, machine );

        return machine;
    }

    reference allocateRoot( MPO mpo, const NetworkAddress& networkAddress )
    {
        return allocate( mpo, ROOT_TYPE_ID, networkAddress );
    }

    reference networkToMachine( const reference& ref )
    {
        auto iFind = m_networkToMachine.find( ref );
        if ( iFind != m_networkToMachine.end() )
        {
            return iFind->second;
        }
        return allocate( ref, ref.type, ref.network );
    }

private:
    runtime::DatabaseInstance&  m_database;
    const std::string           m_strDaemonPrefix;
    mega::network::MemoryConfig m_config;
    SharedMemoryMap             m_memory;
    AddressSpaceMapLifetime     m_memoryLifetime;
    ReferenceMap                m_networkToMachine;
    ReferenceMap                m_machineToNetwork;
};

} // namespace mega::service

#endif // SHARED_MEMORY_MANAGER_3_SEPT_2022
