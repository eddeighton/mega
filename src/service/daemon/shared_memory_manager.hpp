
#ifndef SHARED_MEMORY_MANAGER_3_SEPT_2022
#define SHARED_MEMORY_MANAGER_3_SEPT_2022

#include "mega/common.hpp"
#include "mega/reference.hpp"
#include "mega/reference_io.hpp"
#include "mega/default_traits.hpp"

#include "service/protocol/common/header.hpp"

#include "service/network/log.hpp"

#include <string>
#include <sstream>
#include <map>

namespace mega
{
namespace service
{
class SharedMemoryManager
{
    class SharedMemory
    {
        static constexpr mega::U64 SIZE = 1024 * 1024 * 4;

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

    public:
        using Ptr = std::unique_ptr< SharedMemory >;

        SharedMemory( const mega::MPO& mpo, const std::string& strName )
            : m_strName( strName )
            , m_memoryLifetime( m_strName )
            , m_memory( boost::interprocess::create_only, m_strName.c_str(), SIZE )
        {
        }

        const std::string& getName() const { return m_strName; }

    private:
        const std::string            m_strName;
        AddressSpaceMapLifetime      m_memoryLifetime;
        runtime::ManagedSharedMemory m_memory;
    };

    using SharedMemoryMap = std::map< mega::MPO, SharedMemory::Ptr >;

    std::string memoryName( const mega::MPO& mpo ) const
    {
        std::ostringstream os;
        os << m_strDaemonPrefix << "_mem_" << mpo;
        return os.str();
    }

public:
    SharedMemoryManager( const std::string& daemonPrefix )
        : m_strDaemonPrefix( daemonPrefix )
    {
    }

    const std::string& acquire( const mega::MPO& mpo )
    {
        auto iFind = m_memory.find( mpo );
        if ( iFind != m_memory.end() )
        {
            return iFind->second->getName();
        }
        else
        {
            SharedMemory::Ptr  pNewMemory = std::make_unique< SharedMemory >( mpo, memoryName( mpo ) );
            const std::string& strName    = pNewMemory->getName();
            m_memory.insert( { mpo, std::move( pNewMemory ) } );
            return strName;
        }
    }

    void release( const mega::MPO& mpo )
    {
        auto iFind = m_memory.find( mpo );
        if ( iFind != m_memory.end() )
        {
            m_memory.erase( iFind );
            SPDLOG_INFO( "Daemon released memory for {}", mpo );
        }
    }

private:
    const std::string m_strDaemonPrefix;
    SharedMemoryMap   m_memory;
};
} // namespace service
} // namespace mega

#endif // SHARED_MEMORY_MANAGER_3_SEPT_2022
