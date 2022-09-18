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
    using FreeList            = std::deque< NetworkAddress >;
    using LogicalOwnershipMap = std::unordered_map< mega::NetworkAddress, mega::MPO >;

public:
    NetworkAddress allocateNetworkAddress( const mega::MPO mpo, const mega::TypeID objectTypeID )
    {
        NetworkAddress nextFree;
        {
            if ( !m_freeList.empty() )
            {
                nextFree = m_freeList.back();
                m_freeList.pop_back();
            }
            else
            {
                nextFree = m_capacity;
                ++m_capacity;
            }
        }

        m_ownership.insert( { nextFree, mpo } );
        return nextFree;
    }

    void deAllocateNetworkAddress( const mega::MPO mpo, const mega::NetworkAddress networkAddress )
    {
        auto iFind = m_ownership.find( networkAddress );
        VERIFY_RTE( iFind != m_ownership.end() );
        m_ownership.erase( iFind );
        m_freeList.push_back( networkAddress );
    }

    mega::MPO getOwnership( const mega::NetworkAddress networkAddress ) const
    {
        auto iFind = m_ownership.find( networkAddress );
        if ( iFind != m_ownership.end() )
        {
            return iFind->second;
        }
        else
        {
            THROW_RTE( "Failed to locate owner of network address: " << networkAddress );
            // return  mega::MPO{};
        }
    }

    NetworkAddress getCapacity() const { return m_capacity; }

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
    NetworkAddress      m_capacity = 1U; // start at 1
    FreeList            m_freeList;
    LogicalOwnershipMap m_ownership;
};

} // namespace service
} // namespace mega

#endif // LOGICAL_ADDRESS_SPACE_31_AUG_2022
