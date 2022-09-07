
#ifndef ADDRESS_SPACE_4_SEPT_2022
#define ADDRESS_SPACE_4_SEPT_2022

#include "mega/common.hpp"
#include "mega/default_traits.hpp"

#include <boost/interprocess/interprocess_fwd.hpp>
#include "boost/interprocess/managed_shared_memory.hpp"
#include "boost/interprocess/sync/scoped_lock.hpp"
#include "boost/interprocess/sync/named_mutex.hpp"
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <optional>

namespace mega
{
namespace runtime
{

class AddressSpace
{
    using SharedMemoryType        = boost::interprocess::managed_shared_memory;
    using AddressMappingAllocator = boost::interprocess::allocator< std::pair< const AddressStorage, AddressStorage >,
                                                                    SharedMemoryType::segment_manager >;
    using AddressMapping = boost::interprocess::map< AddressStorage, AddressStorage, std::less< AddressStorage >,
                                                     AddressMappingAllocator >;

    static constexpr const char* MEMORY_NAME = "MEGA_RUNTIME_ADDRESS_SPACE_MEMORY";

    struct AddressSpaceMapLifetime
    {
        AddressSpaceMapLifetime() { boost::interprocess::shared_memory_object::remove( MEMORY_NAME ); }
        ~AddressSpaceMapLifetime() { boost::interprocess::shared_memory_object::remove( MEMORY_NAME ); }
    };

public:
    using Lock = boost::interprocess::scoped_lock< boost::interprocess::named_mutex >;

    AddressSpace()
        : m_sharedMemory( boost::interprocess::open_or_create, MEMORY_NAME, 1024 * 1024 )
        , m_addressSpaceMutex( boost::interprocess::open_or_create, "MEGA_RUNTIME_ADDRESS_SPACE_MUTEX" )
        , m_addressMapping( *m_sharedMemory.find_or_construct< AddressMapping >( "MEGA_RUNTIME_ADDRESS_SPACE_MAP" )(
              std::less< AddressStorage >(), AddressMappingAllocator( m_sharedMemory.get_segment_manager() ) ) )
    {
    }

    boost::interprocess::named_mutex& mutex() { return m_addressSpaceMutex; }

    std::optional< PhysicalAddress > find( LogicalAddress logicalAddress ) const
    {
        std::optional< PhysicalAddress > result;

        auto iFind = m_addressMapping.find( Address( logicalAddress ) );
        if ( iFind != m_addressMapping.end() )
        {
            return Address{ iFind->second }.physical;
        }

        return result;
    }

    void insert( LogicalAddress logicalAddress, PhysicalAddress physicalAddress )
    {
        m_addressMapping.insert( { Address( logicalAddress ), Address( physicalAddress ) } );
    }

private:
    AddressSpaceMapLifetime          m_memoryLifetime;
    SharedMemoryType                 m_sharedMemory;
    boost::interprocess::named_mutex m_addressSpaceMutex;
    AddressMapping&                  m_addressMapping;
};
} // namespace runtime
} // namespace mega

#endif // ADDRESS_SPACE_4_SEPT_2022
