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

#ifndef MEGA_REFERENCE_18_SEPT_2022
#define MEGA_REFERENCE_18_SEPT_2022

#include "mega/native_types.hpp"
#include "mega/type_instance.hpp"
#include "mega/mpo.hpp"

namespace mega
{

class reference
{
    struct HeapAddressData
    {
        HeapAddress m_heap;    // 8
        U16         m_padding; // 2

        OwnerID      m_ownerID; // 1
        Flags        m_flags;   // 1
        TypeInstance m_type;    // 4
    };
    static_assert( sizeof( HeapAddressData ) == 16U, "Invalid HeapAddressData Size" );

    struct NetworkAddressData
    {
        ObjectID  m_objectID;  // 4
        MachineID m_machineID; // 4
        ProcessID m_processID; // 2

        OwnerID      m_ownerID; // 1
        Flags        m_flags;   // 1
        TypeInstance m_type;    // 4
    };
    static_assert( sizeof( NetworkAddressData ) == 16U, "Invalid NetworkAddressData Size" );

    union
    {
        HeapAddressData    prc;
        NetworkAddressData net;
    };

public:
    struct Hash
    {
        inline U64 operator()( const reference& ref ) const noexcept
        {
            if( ref.isHeapAddress() )
                return reinterpret_cast< U64 >( ref.getHeap() );
            else
                return ref.getObjectID() + ( (U64)ref.getMachineID() << 4 );
        }
    };

    constexpr inline HeapAddress getHeap() const { return prc.m_heap; }

    constexpr inline ObjectID  getObjectID() const { return net.m_objectID; }
    constexpr inline MPO       getMPO() const { return MPO{ net.m_machineID, net.m_processID, net.m_ownerID }; }
    constexpr inline MP        getMP() const { return MP{ net.m_machineID, net.m_processID }; }
    constexpr inline MachineID getMachineID() const { return net.m_machineID; }
    constexpr inline ProcessID getProcessID() const { return net.m_processID; }

    constexpr inline OwnerID      getOwnerID() const { return prc.m_ownerID; }
    constexpr inline Flags        getFlags() const { return prc.m_flags; }
    constexpr inline TypeID       getType() const { return prc.m_type.type; }
    constexpr inline Instance     getInstance() const { return prc.m_type.instance; }
    constexpr inline TypeInstance getTypeInstance() const { return prc.m_type; }

    constexpr reference()
        : prc{ NULL_ADDRESS, 0, OwnerID{}, HEAP_ADDRESS, TypeInstance{ 0, 0 } }
    {
    }

    constexpr reference( TypeInstance typeInstance, OwnerID owner, HeapAddress heap )
        : prc{ heap, 0, owner, HEAP_ADDRESS, typeInstance }
    {
    }
    constexpr reference( TypeInstance typeInstance, MPO mpo, ObjectID object )
        : net{ object, mpo.getMachineID(), mpo.getProcessID(), mpo.getOwnerID(), HEAP_ADDRESS, typeInstance }
    {
    }

    constexpr inline bool isHeapAddress() const { return prc.m_flags == HEAP_ADDRESS; }
    constexpr inline bool isNetworkAddress() const { return prc.m_flags == NETWORK_ADDRESS; }

    constexpr inline bool is_valid() const
    {
        if( isHeapAddress() )
            return ( prc.m_heap != NULL_ADDRESS ) && prc.m_type.is_valid();
        else
            return net.m_type.is_valid();
    }

    constexpr inline bool operator==( const reference& cmp ) const
    {
        // clang-format off
        if( isNetworkAddress() && cmp.isNetworkAddress() )
        {
            return prc.m_heap == cmp.prc.m_heap &&
                   prc.m_ownerID == cmp.prc.m_ownerID &&
                   prc.m_flags == cmp.prc.m_flags &&
                   prc.m_type == cmp.prc.m_type;
        }
        else if( !isNetworkAddress() && !cmp.isNetworkAddress() )
        {
            return net.m_objectID == cmp.net.m_objectID &&
                   net.m_machineID == cmp.net.m_machineID &&
                   net.m_processID == cmp.net.m_processID &&
                   net.m_ownerID == cmp.net.m_ownerID &&
                   net.m_flags == cmp.net.m_flags &&
                   net.m_type == cmp.net.m_type;
        }
        else
        {
            return false;
        }
        // clang-format on
    }
    constexpr inline bool operator!=( const reference& cmp ) const { return !( *this == cmp ); }

    constexpr inline bool operator<( const reference& cmp ) const
    {
        // clang-format off
        if( isNetworkAddress() && cmp.isNetworkAddress() )
        {
            return ( prc.m_heap     != cmp.prc.m_heap  )    ?   ( prc.m_heap        < cmp.prc.m_heap        ) : 
                   ( prc.m_ownerID  != cmp.prc.m_ownerID )  ?   ( prc.m_ownerID     < cmp.prc.m_ownerID     ) : 
                   ( prc.m_flags    != cmp.prc.m_flags )    ?   ( prc.m_flags       < cmp.prc.m_flags       ) : 
                                                                ( prc.m_type        < cmp.prc.m_type        ) ;
        }
        else if( !isNetworkAddress() && !cmp.isNetworkAddress() )
        {
            return ( net.m_objectID   != cmp.net.m_objectID )   ? ( net.m_objectID   < cmp.net.m_objectID ) :
                   ( net.m_machineID  != cmp.net.m_machineID )  ? ( net.m_machineID  < cmp.net.m_machineID ) :
                   ( net.m_processID  != cmp.net.m_processID )  ? ( net.m_processID  < cmp.net.m_processID ) :
                   ( net.m_ownerID    != cmp.net.m_ownerID )    ? ( net.m_ownerID    < cmp.net.m_ownerID ) :
                   ( net.m_flags      != cmp.net.m_flags )      ? ( net.m_flags      < cmp.net.m_flags ) :
                                                                  ( net.m_type       < cmp.net.m_type );
        }
        else
        {
            return false;
        }
        // clang-format on
    }
};

} // namespace mega

#endif // MEGA_REFERENCE_18_SEPT_2022
