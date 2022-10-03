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

#ifndef LOGICAL_ADDRESS_SPACE_31_AUG_2022
#define LOGICAL_ADDRESS_SPACE_31_AUG_2022

#include "mega/common.hpp"

#include "service/protocol/common/header.hpp"
#include "service/network/log.hpp"

#include <boost/iterator/minimum_category.hpp>
#include <unordered_map>
#include <deque>

namespace mega::service
{

class LogicalAddressSpace
{
    using FreeList            = std::deque< NetworkAddress >;
    using LogicalOwnershipMap = std::unordered_map< NetworkAddress, MPO >;
    using RootMPOMap          = std::unordered_map< MPO, NetworkAddress, MPO::Hash >;
    using MPORootMap          = std::unordered_map< NetworkAddress, MPO >;

public:
    MPO getNetworkAddressMPO( NetworkAddress networkAddress )
    {
        auto iFind = m_ownership.find( networkAddress );
        if ( iFind != m_ownership.end() )
        {
            return iFind->second;
        }
        else
        {
            THROW_RTE( "Failed to locate owner of network address: " << networkAddress );
        }
    }

    NetworkAddress getRootNetworkAddress( MPO mpo ) const
    {
        RootMPOMap::const_iterator iFind = m_rootMPOMap.find( mpo );
        if ( iFind != m_rootMPOMap.end() )
        {
            return iFind->second;
        }
        THROW_RTE( "getRootNetworkAddress Failed to locate network address" );
    }

    NetworkAddress allocateNetworkAddress( MPO mpo, TypeID objectTypeID )
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

        if ( objectTypeID == ROOT_TYPE_ID )
        {
            m_rootMPOMap.insert( { mpo, nextFree } );
            m_MPORootMap.insert( { nextFree, mpo } );
        }

        return nextFree;
    }

    void deAllocateNetworkAddress( MPO mpo, NetworkAddress networkAddress )
    {
        {
            auto iFind = m_ownership.find( networkAddress );
            VERIFY_RTE( iFind != m_ownership.end() );
            m_ownership.erase( iFind );
            m_freeList.push_back( networkAddress );
        }

        // use reverse mapping to remove if was root object
        {
            auto iFind = m_MPORootMap.find( networkAddress );
            if ( iFind != m_MPORootMap.end() )
            {
                m_rootMPOMap.erase( iFind->second );
                m_MPORootMap.erase( iFind );
            }
        }
    }

    NetworkAddress getCapacity() const { return m_capacity; }

private:
    NetworkAddress      m_capacity = 1U; // start at 1
    FreeList            m_freeList;
    LogicalOwnershipMap m_ownership;
    RootMPOMap          m_rootMPOMap;
    MPORootMap          m_MPORootMap;
};

} // namespace mega::service

#endif // LOGICAL_ADDRESS_SPACE_31_AUG_2022
