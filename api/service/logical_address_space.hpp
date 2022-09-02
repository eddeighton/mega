#ifndef LOGICAL_ADDRESS_SPACE_31_AUG_2022
#define LOGICAL_ADDRESS_SPACE_31_AUG_2022

#include "mega/common.hpp"

#include "service/protocol/common/header.hpp"
#include "service/network/log.hpp"

#include <boost/iterator/minimum_category.hpp>
#include <unordered_map>
#include <deque>

namespace mega
{
namespace service
{

class LogicalAddressSpace
{
    using FreeList            = std::deque< AddressStorage >;
    using LogicalOwnershipMap = std::unordered_map< mega::AddressStorage, mega::ExecutionIndex >;

public:
    Address allocate( const mega::ExecutionIndex executionIndex, const mega::TypeID objectTypeID )
    {
        AddressStorage nextFree;
        {
            if ( !m_freeList.empty() )
            {
                nextFree = m_freeList.back();
                m_freeList.pop_back();
            }
            else
            {
                VERIFY_RTE_MSG( m_capacity < MAX_OBJECTS, "Total capacity reached in logical address space" );
                nextFree = m_capacity;
                ++m_capacity;
            }
        }

        m_ownership.insert( { nextFree, executionIndex } );

        LogicalAddress logicalAddress;
        {
            logicalAddress.address = nextFree;
            logicalAddress.type    = LOGICAL_ADDRESS;
        }

        return Address{ logicalAddress };
    }

    void deAllocate( const mega::ExecutionIndex executionIndex, const mega::AddressStorage logicalAddress )
    {
        auto iFind = m_ownership.find( logicalAddress );
        VERIFY_RTE( iFind != m_ownership.end() );
        m_ownership.erase( iFind );
        m_freeList.push_back( logicalAddress );
    }

    mega::ExecutionIndex getOwnership( const mega::AddressStorage logicalAddress ) const
    {
        auto iFind = m_ownership.find( logicalAddress );
        if ( iFind != m_ownership.end() )
        {
            return iFind->second;
        }
        else
        {
            return MAX_SIMULATIONS;
        }
    }

    AddressStorage getCapacity() const { return m_capacity; }

    void defrag()
    {
        std::sort( m_freeList.begin(), m_freeList.end() );
        while ( !m_freeList.empty() && ( m_freeList.back() + 1 == m_capacity ) )
        {
            m_freeList.pop_back();
            --m_capacity;
        }
    }

private:
    AddressStorage      m_capacity = 0U;
    FreeList            m_freeList;
    LogicalOwnershipMap m_ownership;
};

} // namespace service
} // namespace mega

#endif // LOGICAL_ADDRESS_SPACE_31_AUG_2022
