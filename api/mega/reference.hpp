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

#ifdef DEBUG
#define ASSERT_IS_HEAP                                            \
    do                                                            \
    {                                                             \
        if( !this->isHeapAddress() )                              \
            throw "Mega Reference Error.  Expected heap address"; \
    } while( ( void )0, 0 )

#else // DEBUG
#define ASSERT_IS_HEAP
#endif

namespace mega
{

using HeapAddress                         = void*; // recheck numeric_limits if change
static constexpr HeapAddress NULL_ADDRESS = nullptr;
static_assert( sizeof( HeapAddress ) == 8U, "Invalid HeapAddress Size" );
static_assert( sizeof( HeapAddress ) == sizeof( U64 ), "Invalid HeapAddress Size" );

using ObjectID = U32;
using Flags    = U8;

constexpr static const ObjectID ROOT_OBJECT_ID = 0;

enum FlagsType : U8 // check reference_io if change
{
    HEAP_ADDRESS    = 0,
    NETWORK_ADDRESS = 1
};

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
        ObjectID     m_objectID;  // 4
        MachineID    m_machineID; // 4
        ProcessID    m_processID; // 2
        OwnerID      m_ownerID;   // 1
        Flags        m_flags;     // 1
        TypeInstance m_type;      // 4
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
            const reference& net = ref.getNetworkAddress();
            return net.getObjectID() + ( ( U64 )net.getMachineID() << 4 );
        }
    };

    // heap only
    constexpr inline HeapAddress getHeap() const
    {
        ASSERT_IS_HEAP;
        return prc.m_heap;
    }

    // heap only accessors - that use ObjectHeader
    inline const reference& getNetworkAddress() const;
    inline TimeStamp        getLockCycle() const;
    inline void             setLockCycle( TimeStamp lockCycle );

    // network - can access via heap header
    constexpr inline ObjectID  getObjectID() const;
    constexpr inline MPO       getMPO() const;
    constexpr inline MP        getMP() const;
    constexpr inline MachineID getMachineID() const;
    constexpr inline ProcessID getProcessID() const;

    // common to both
    constexpr inline OwnerID      getOwnerID() const { return prc.m_ownerID; }
    constexpr inline Flags        getFlags() const { return prc.m_flags; }
    constexpr inline TypeID       getType() const { return prc.m_type.type; }
    constexpr inline Instance     getInstance() const { return prc.m_type.instance; }
    constexpr inline TypeInstance getTypeInstance() const { return prc.m_type; }

    constexpr reference()
        : net{ 0U, 0U, 0U, 0U, NETWORK_ADDRESS, TypeInstance{ 0, 0 } }
    {
    }

    constexpr reference( TypeInstance typeInstance, OwnerID owner, HeapAddress heap )
        : prc{ heap, 0, owner, HEAP_ADDRESS, typeInstance }
    {
    }
    constexpr reference( TypeInstance typeInstance, MPO mpo, ObjectID object )
        : net{ object, mpo.getMachineID(), mpo.getProcessID(), mpo.getOwnerID(), NETWORK_ADDRESS, typeInstance }
    {
    }

    static constexpr reference make( const reference& other, TypeID typeID )
    {
        if( other.isHeapAddress() )
        {
            return { TypeInstance{ other.getInstance(), typeID }, other.getOwnerID(), other.getHeap() };
        }
        else
        {
            return { TypeInstance{ other.getInstance(), typeID }, other.getMPO(), other.getObjectID() };
        }
    }

    static constexpr reference make( const reference& other, TypeInstance typeInstance )
    {
        if( other.isHeapAddress() )
        {
            return { typeInstance, other.getOwnerID(), other.getHeap() };
        }
        else
        {
            return { typeInstance, other.getMPO(), other.getObjectID() };
        }
    }

    constexpr inline bool isHeapAddress() const { return prc.m_flags == HEAP_ADDRESS; }
    constexpr inline bool isNetworkAddress() const { return prc.m_flags == NETWORK_ADDRESS; }

    constexpr inline bool is_valid() const { return net.m_type.is_valid(); }

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
        if( isHeapAddress() && cmp.isHeapAddress() )
        {
            return ( prc.m_heap     != cmp.prc.m_heap  )    ?   ( prc.m_heap        < cmp.prc.m_heap        ) : 
                   ( prc.m_ownerID  != cmp.prc.m_ownerID )  ?   ( prc.m_ownerID     < cmp.prc.m_ownerID     ) : 
                   ( prc.m_flags    != cmp.prc.m_flags )    ?   ( prc.m_flags       < cmp.prc.m_flags       ) : 
                                                                ( prc.m_type        < cmp.prc.m_type        ) ;
        }
        else if( isNetworkAddress() && cmp.isNetworkAddress() )
        {
            // important to compare MPO first - since this is used by some algorithms 
            return ( net.m_machineID  != cmp.net.m_machineID )  ? ( net.m_machineID  < cmp.net.m_machineID ) :
                   ( net.m_processID  != cmp.net.m_processID )  ? ( net.m_processID  < cmp.net.m_processID ) :
                   ( net.m_ownerID    != cmp.net.m_ownerID )    ? ( net.m_ownerID    < cmp.net.m_ownerID ) :
                   ( net.m_objectID   != cmp.net.m_objectID )   ? ( net.m_objectID   < cmp.net.m_objectID ) :
                   ( net.m_flags      != cmp.net.m_flags )      ? ( net.m_flags      < cmp.net.m_flags ) :
                                                                  ( net.m_type       < cmp.net.m_type );
        }
        else
        {
            return isHeapAddress();
        }
        // clang-format on
    }
};

struct ObjectHeaderBase
{
    reference m_networkAddress;
    TimeStamp m_lockCycle;
};

inline TimeStamp reference::getLockCycle() const
{
    return reinterpret_cast< ObjectHeaderBase* >( getHeap() )->m_lockCycle;
}

inline void reference::setLockCycle( TimeStamp lockCycle )
{
    reinterpret_cast< ObjectHeaderBase* >( getHeap() )->m_lockCycle = lockCycle;
}

inline const reference& reference::getNetworkAddress() const
{
    if( isNetworkAddress() )
    {
        return *this;
    }
    else
    {
        return reinterpret_cast< ObjectHeaderBase* >( getHeap() )->m_networkAddress;
    }
}

constexpr inline ObjectID reference::getObjectID() const
{
    if( isNetworkAddress() )
    {
        return net.m_objectID;
    }
    else
    {
        return getNetworkAddress().getObjectID();
    }
}
constexpr inline MPO reference::getMPO() const
{
    if( isNetworkAddress() )
    {
        return MPO{ net.m_machineID, net.m_processID, net.m_ownerID };
    }
    else
    {
        return getNetworkAddress().getMPO();
    }
}
constexpr inline MP reference::getMP() const
{
    if( isNetworkAddress() )
    {
        return MP{ net.m_machineID, net.m_processID };
    }
    else
    {
        return getNetworkAddress().getMP();
    }
}
constexpr inline MachineID reference::getMachineID() const
{
    if( isNetworkAddress() )
    {
        return net.m_machineID;
    }
    else
    {
        return getNetworkAddress().getMachineID();
    }
}
constexpr inline ProcessID reference::getProcessID() const
{
    if( isNetworkAddress() )
    {
        return net.m_processID;
    }
    else
    {
        return getNetworkAddress().getProcessID();
    }
}

} // namespace mega

#endif // MEGA_REFERENCE_18_SEPT_2022
