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

using AllocationID = U16;
using Flags        = U8;

constexpr static const AllocationID ROOT_OBJECT_ID = 0;

enum FlagsType : U8 // check reference_io if change
{
    HEAP_ADDRESS    = 0,
    NETWORK_ADDRESS = 1
};

class reference
{
#pragma pack( 1 )
    struct HeapAddressData
    {
        HeapAddress  m_heap;    // 8
        mega::U8     _padding;  // 1
        Flags        m_flags;   // 1
        TypeInstance m_type;    // 6
    };
#pragma pack()
    static_assert( sizeof( HeapAddressData ) == 16U, "Invalid HeapAddressData Size" );

#pragma pack( 1 )
    struct NetworkAddressData
    {
        AllocationID m_allocationID; // 2
        MachineID    m_machineID;    // 4
        ProcessID    m_processID;    // 2
        OwnerID      m_ownerID;      // 1
        Flags        m_flags;        // 1
        TypeInstance m_type;         // 6
    };
#pragma pack()
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
            const reference& r = ref.getNetworkAddress();
            const U64* p = reinterpret_cast< const U64* >( &r );
            return *p + *( p + 1 );
        }
    };

    // heap only
    constexpr inline HeapAddress getHeap() const
    {
        ASSERT_IS_HEAP;
        return prc.m_heap;
    }

    inline const reference& getHeaderAddress() const;
    inline reference        getNetworkAddress() const;
    inline reference        getObjectAddress() const;
    inline TimeStamp        getLockCycle() const;
    inline void             setLockCycle( TimeStamp lockCycle );

    // network - can access via heap header
    constexpr inline AllocationID getAllocationID() const;
    constexpr inline MPO          getMPO() const;
    constexpr inline MP           getMP() const;
    constexpr inline MachineID    getMachineID() const;
    constexpr inline ProcessID    getProcessID() const;

    // common to both
    constexpr inline Flags        getFlags() const { return prc.m_flags; }
    constexpr inline TypeID       getType() const { return prc.m_type.type; }
    constexpr inline Instance     getInstance() const { return prc.m_type.instance; }
    constexpr inline TypeInstance getTypeInstance() const { return prc.m_type; }
    constexpr inline bool         isHeapAddress() const { return prc.m_flags == HEAP_ADDRESS; }
    constexpr inline bool         isNetworkAddress() const { return prc.m_flags == NETWORK_ADDRESS; }
    constexpr inline bool         is_valid() const { return getTypeInstance().is_valid(); }

    constexpr reference()
        : net{ 0U, 0U, 0U, 0U, NETWORK_ADDRESS, TypeInstance{} }
    {
    }

    constexpr reference( TypeInstance typeInstance, HeapAddress heap )
        : prc{ heap, 0U, HEAP_ADDRESS, typeInstance }
    {
    }
    constexpr reference( TypeInstance typeInstance, MPO mpo, AllocationID allocationID )
        : net{ allocationID, mpo.getMachineID(), mpo.getProcessID(), mpo.getOwnerID(), NETWORK_ADDRESS, typeInstance }
    {
    }

    static constexpr inline reference make_root( MPO mpo ) { return { TypeInstance::make_root(), mpo, ROOT_OBJECT_ID }; }

    static constexpr inline reference make( const reference& other, TypeID typeID )
    {
        if( other.isHeapAddress() )
        {
            return { TypeInstance{ typeID, other.getInstance() }, other.getHeap() };
        }
        else
        {
            return { TypeInstance{ typeID, other.getInstance() }, other.getMPO(), other.getAllocationID() };
        }
    }

    static constexpr inline reference make( const reference& other, TypeID::ValueType typeID )
    {
        return reference::make( other, TypeID{ typeID } );
    }

    static constexpr inline reference make( const reference& other, TypeInstance typeInstance )
    {
        if( other.isHeapAddress() )
        {
            return { typeInstance, other.getHeap() };
        }
        else
        {
            return { typeInstance, other.getMPO(), other.getAllocationID() };
        }
    }

    constexpr inline bool operator==( const reference& cmp ) const
    {
        // clang-format off
        // if( isHeapAddress() && cmp.isHeapAddress() )
        // {
        //     return prc.m_heap == cmp.prc.m_heap &&
        //            prc.m_ownerID == cmp.prc.m_ownerID &&
        //            prc.m_flags == cmp.prc.m_flags &&
        //            prc.m_type == cmp.prc.m_type;
        // }
        if( isNetworkAddress() && cmp.isNetworkAddress() )
        {
            return net.m_allocationID == cmp.net.m_allocationID &&
                   net.m_machineID == cmp.net.m_machineID &&
                   net.m_processID == cmp.net.m_processID &&
                   net.m_ownerID == cmp.net.m_ownerID &&
                   net.m_flags == cmp.net.m_flags &&
                   net.m_type == cmp.net.m_type;
        }
        else if( isNetworkAddress() )
        {
            return this->operator==( cmp.getNetworkAddress() );
        }
        else
        {
            return getNetworkAddress().operator==( cmp );
        }
        // clang-format on
    }
    constexpr inline bool operator!=( const reference& cmp ) const { return !( *this == cmp ); }

    constexpr inline bool operator<( const reference& cmp ) const
    {
        // clang-format off
        // if( isHeapAddress() && cmp.isHeapAddress() )
        // {
        //     return ( prc.m_heap     != cmp.prc.m_heap  )    ?   ( prc.m_heap        < cmp.prc.m_heap        ) : 
        //            ( prc.m_ownerID  != cmp.prc.m_ownerID )  ?   ( prc.m_ownerID     < cmp.prc.m_ownerID     ) : 
        //            ( prc.m_flags    != cmp.prc.m_flags )    ?   ( prc.m_flags       < cmp.prc.m_flags       ) : 
        //                                                         ( prc.m_type        < cmp.prc.m_type        ) ;
        // }
        if( isNetworkAddress() && cmp.isNetworkAddress() )
        {
            // important to compare MPO first - since this is used by some algorithms 
            return ( net.m_machineID      != cmp.net.m_machineID )      ? ( net.m_machineID     < cmp.net.m_machineID ) :
                   ( net.m_processID      != cmp.net.m_processID )      ? ( net.m_processID     < cmp.net.m_processID ) :
                   ( net.m_ownerID        != cmp.net.m_ownerID )        ? ( net.m_ownerID       < cmp.net.m_ownerID ) :
                   ( net.m_allocationID   != cmp.net.m_allocationID )   ? ( net.m_allocationID  < cmp.net.m_allocationID ) :
                   ( net.m_flags          != cmp.net.m_flags )          ? ( net.m_flags         < cmp.net.m_flags ) :
                                                                          ( net.m_type          < cmp.net.m_type );
        }
        else if( isNetworkAddress() )
        {
            return this->operator<( cmp.getNetworkAddress() );
        }
        else
        {
            return getNetworkAddress().operator<( cmp );
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

inline const reference& reference::getHeaderAddress() const
{
    if( isNetworkAddress() )
    {
        return *this;
    }
    else if( getHeap() != nullptr )
    {
        return reinterpret_cast< ObjectHeaderBase* >( getHeap() )->m_networkAddress;
    }
    else
    {
        static const reference nullref;
        return nullref;
    }
}

inline reference reference::getNetworkAddress() const
{
    if( isNetworkAddress() )
    {
        return *this;
    }
    else if( getHeap() != nullptr )
    {
        return make( reinterpret_cast< ObjectHeaderBase* >( getHeap() )->m_networkAddress, getTypeInstance() );
    }
    else
    {
        return make( reference{}, getTypeInstance() );
    }
}

inline reference reference::getObjectAddress() const
{
    return reference::make( *this, TypeID::make_object_type( getType() ) );
}

constexpr inline AllocationID reference::getAllocationID() const
{
    if( isNetworkAddress() )
    {
        return net.m_allocationID;
    }
    else
    {
        return getHeaderAddress().getAllocationID();
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
        return getHeaderAddress().getMPO();
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
        return getHeaderAddress().getMP();
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
        return getHeaderAddress().getMachineID();
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
        return getHeaderAddress().getProcessID();
    }
}

} // namespace mega

#endif // MEGA_REFERENCE_18_SEPT_2022
