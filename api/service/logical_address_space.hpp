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
    using LogicalOwnershipMap = std::unordered_map< mega::NetworkAddress, mega::MPEStorage >;

public:
    NetworkAddress allocateNetworkAddress( const mega::MPEStorage mpe, const mega::TypeID objectTypeID )
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

        m_ownership.insert( { nextFree, mpe } );
        return nextFree;
    }

    void deAllocateNetworkAddress( const mega::MPEStorage mpe, const mega::NetworkAddress networkAddress )
    {
        auto iFind = m_ownership.find( networkAddress );
        VERIFY_RTE( iFind != m_ownership.end() );
        m_ownership.erase( iFind );
        m_freeList.push_back( networkAddress );
    }

    mega::MPEStorage getOwnership( const mega::NetworkAddress networkAddress ) const
    {
        auto iFind = m_ownership.find( networkAddress );
        if ( iFind != m_ownership.end() )
        {
            return iFind->second;
        }
        else
        {
            return TOTAL_EXECUTORS - 1U;
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
