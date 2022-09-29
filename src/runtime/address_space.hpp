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
    using AddressMappingAllocator = boost::interprocess::allocator< std::pair< const NetworkAddress, reference >,
                                                                    SharedMemoryType::segment_manager >;
    using AddressMapping
        = boost::interprocess::map< NetworkAddress, reference, std::less< NetworkAddress >, AddressMappingAllocator >;

    static constexpr const char* MEMORY_NAME = "MEGA_RUNTIME_ADDRESS_SPACE_MEMORY";

public:
    struct AddressSpaceMapLifetime // for the daemon
    {
        AddressSpaceMapLifetime() { boost::interprocess::shared_memory_object::remove( MEMORY_NAME ); }
        ~AddressSpaceMapLifetime() { boost::interprocess::shared_memory_object::remove( MEMORY_NAME ); }
    };

    using Lock = boost::interprocess::scoped_lock< boost::interprocess::named_mutex >;

    AddressSpace()
        : m_sharedMemory( boost::interprocess::open_or_create, MEMORY_NAME, 1024 * 1024 )
        , m_addressSpaceMutex( boost::interprocess::open_or_create, "MEGA_RUNTIME_ADDRESS_SPACE_MUTEX" )
        , m_addressMapping( *m_sharedMemory.find_or_construct< AddressMapping >( "MEGA_RUNTIME_ADDRESS_SPACE_MAP" )(
              std::less< AddressStorage >(), AddressMappingAllocator( m_sharedMemory.get_segment_manager() ) ) )
    {
    }

    boost::interprocess::named_mutex& mutex() { return m_addressSpaceMutex; }

    std::optional< reference > find( NetworkAddress networkAddress ) const
    {
        auto iFind = m_addressMapping.find( networkAddress );
        if ( iFind != m_addressMapping.end() )
        {
            return iFind->second;
        }
        else
        {
            return std::optional< reference >{};
        }
    }

    bool insert( NetworkAddress networkAddress, reference ref )
    {
        return m_addressMapping.insert( { networkAddress, ref } ).second;
    }

private:
    SharedMemoryType                 m_sharedMemory;
    boost::interprocess::named_mutex m_addressSpaceMutex;
    AddressMapping&                  m_addressMapping;
};
} // namespace runtime
} // namespace mega

#endif // ADDRESS_SPACE_4_SEPT_2022
