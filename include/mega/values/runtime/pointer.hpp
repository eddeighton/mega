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

#include "mega/values/runtime/pointer.h"

#include "mega/values/native_types.hpp"

#include "mega/values/compilation/concrete/type_id_instance.hpp"

#include "mega/values/runtime/mpo.hpp"

#include "common/assert_verify.hpp"

#ifdef DEBUG
// #define ASSERT_IS_HEAP                                            \
//     do                                                            \
//     {                                                             \
//         if( !this->isHeapAddress() )                              \
//             throw "Mega Reference Error.  Expected heap address"; \
//     } while( ( void )0, 0 )

#else // DEBUG
#define ASSERT_IS_HEAP
#endif

namespace mega
{

// using HeapAddress                         = void*; // recheck numeric_limits if change
// static constexpr HeapAddress NULL_ADDRESS = nullptr;
// 
// #ifndef MEGAJIT
// static_assert( sizeof( HeapAddress ) == 8U, "Invalid HeapAddress Size" );
// static_assert( sizeof( HeapAddress ) == sizeof( U64 ), "Invalid HeapAddress Size" );
// #endif
// 
// using AllocationID = U16;
// using Flags        = U8;

// constexpr static const AllocationID ROOT_OBJECT_ID = 0;

// enum FlagsType : U8 // check reference_io if change
// {
//     HEAP_ADDRESS    = 0,
//     NETWORK_ADDRESS = 1
// };
/*
struct ObjectHeaderBase
{
    c_pointer_net m_networkAddress;
    TimeStamp m_lockCycle = 0U;
    RefCount  m_refCount  = 0U;
};
 */
 
class PointerHeap : public c_pointer_heap
{
public:

    struct Hash
    {
        inline U64 operator()( const c_pointer_heap& ptr ) const noexcept
        {
            const U64* p = reinterpret_cast< const U64* >( &ptr );
            return *p + *( p + 1 );
        }
    };
};

class PointerNet : public c_pointer_net
{
public:
    struct Hash
    {
        inline U64 operator()( const c_pointer_net& ptr ) const noexcept
        {
            const U64* p = reinterpret_cast< const U64* >( &ptr );
            return *p + *( p + 1 );
        }
    };
};

class Pointer : public c_pointer
{

public:

    inline bool isHeap() const
    {
        return value.heap.m_flags == 0;
    }
    inline bool isNetwork() const
    {
        return !isHeap();
    }

    PointerHeap& heap()
    {
        return *reinterpret_cast< PointerHeap* >( &value.heap );
    }
    PointerNet& net()
    {
        return *reinterpret_cast< PointerNet* >( &value.net );
    }

    const PointerHeap& heap() const
    {
        return *reinterpret_cast< const PointerHeap* >( &value.heap );
    }

    const PointerNet& net() const
    {
        return *reinterpret_cast< const PointerNet* >( &value.net );
    }

    struct Hash
    {
        inline U64 operator()( const Pointer& ptr ) const noexcept
        {
            if( ptr.isHeap() )
            {
                return PointerHeap::Hash()( ptr.heap() );
            }
            else
            {
                return PointerNet::Hash()( ptr.net() );
            }
        }
    };
    
    inline bool operator==( const Pointer& cmp ) const
    {
        throw "TODO";
    }
    inline bool operator<( const Pointer& cmp ) const
    {
        throw "TODO";
    }
    inline bool valid() const
    {
        throw "TODO";
    }
/*
    const PointerHeap& getHeapPointer() const
    {
        if( isHeap() )
        {
            return *this;
        }
        else
        {
            return networkToHeap( *this );
        }
    }

    const PointerNet& getNetworkPointer() const
    {
        if( isNetwork() )
        {
            return *this;
        }
        else if( getHeap() != nullptr )
        {
            return reinterpret_cast< ObjectHeaderBase* >( getHeap() )->m_networkAddress;
        }
        else
        {
            static const PointerNet nullref;
            return nullref;
        }
    }
*/
    // heap only
    // constexpr inline HeapAddress getHeap() const
    // {
    //     ASSERT_IS_HEAP;
    //     return prc.m_heap;
    // }

    // inline const reference& getHeaderAddress() const;
    // inline reference        getNetworkAddress() const;
    // inline reference        getObjectAddress() const;
    // inline RefCount getRefCount() const;
    // inline void     decRefCount() const;
    // inline void     incRefCount() const;
    // inline TimeStamp getLockCycle() const;
    // inline void      setLockCycle( TimeStamp lockCycle );

    // network - can access via heap header
    // constexpr inline AllocationID getAllocationID() const;
    // constexpr inline MPO          getMPO() const;
    // constexpr inline MP           getMP() const;
    // constexpr inline MachineID    getMachineID() const;
    // constexpr inline ProcessID    getProcessID() const;

    // common to both
    // constexpr inline Flags        getFlags() const { return prc.m_flags; }
    // constexpr inline TypeID       getType() const { return prc.m_type.type; }
    // constexpr inline Instance     getInstance() const { return prc.m_type.instance; }
    // constexpr inline TypeInstance getTypeInstance() const { return prc.m_type; }
    // constexpr inline bool         isHeapAddress() const { return prc.m_flags == HEAP_ADDRESS; }
    // constexpr inline bool         isNetworkAddress() const { return prc.m_flags == NETWORK_ADDRESS; }
    // constexpr inline bool         valid() const { return getTypeInstance().valid(); }

    // constexpr reference()
    //     : net{ 0U, 0U, 0U, 0U, NETWORK_ADDRESS, TypeInstance{} }
    // {
    // }
    // constexpr explicit reference( TypeInstance typeInstance, HeapAddress heap )
    //     : prc{ heap, 0U, HEAP_ADDRESS, typeInstance }
    // {
    // }
    // constexpr explicit reference( TypeInstance typeInstance, MPO mpo, AllocationID allocationID )
    //     : net{ allocationID, mpo.getMachineID(), mpo.getProcessID(), mpo.getOwnerID(), NETWORK_ADDRESS, typeInstance }
    // {
    // }
    // static constexpr inline reference make_root( MPO mpo )
    // {
    //     return reference{ TypeInstance::make_root(), mpo, ROOT_OBJECT_ID };
    // }
    // static constexpr inline reference make( const reference& other, TypeInstance typeInstance )
    // {
    //     if( other.isHeapAddress() )
    //     {
    //         return reference{ typeInstance, other.getHeap() };
    //     }
    //     else
    //     {
    //         return reference{ typeInstance, other.getMPO(), other.getAllocationID() };
    //     }
    // }
    // constexpr inline bool operator==( const reference& cmp ) const
    // {
    //     // clang-format off
    //     if( isHeapAddress() && cmp.isHeapAddress() )
    //     {
    //         return prc.m_heap == cmp.prc.m_heap &&
    //                prc.m_flags == cmp.prc.m_flags &&
    //                prc.m_type == cmp.prc.m_type;
    //     }
    //     else if( isNetworkAddress() && cmp.isNetworkAddress() )
    //     {
    //         return net.m_allocationID == cmp.net.m_allocationID &&
    //                net.m_machineID == cmp.net.m_machineID &&
    //                net.m_processID == cmp.net.m_processID &&
    //                net.m_ownerID == cmp.net.m_ownerID &&
    //                net.m_flags == cmp.net.m_flags &&
    //                net.m_type == cmp.net.m_type;
    //     }
    //     else if( isNetworkAddress() )
    //     {
    //         return this->operator==( cmp.getNetworkAddress() );
    //     }
    //     else
    //     {
    //         return getNetworkAddress().operator==( cmp );
    //     }
    //     // clang-format on
    // }
    // constexpr inline bool operator!=( const reference& cmp ) const { return !( *this == cmp ); }

    // constexpr inline bool operator<( const reference& cmp ) const
    // {
    //     // clang-format off
    //     //if( isHeapAddress() && cmp.isHeapAddress() )
    //     //{
    //     //    return ( prc.m_heap     != cmp.prc.m_heap  )    ?   ( prc.m_heap        < cmp.prc.m_heap        ) : 
    //     //           ( prc.m_flags    != cmp.prc.m_flags )    ?   ( prc.m_flags       < cmp.prc.m_flags       ) : 
    //     //                                                        ( prc.m_type        < cmp.prc.m_type        ) ;
    //     //}
    //     //else 
    //     if( isNetworkAddress() && cmp.isNetworkAddress() )
    //     {
    //         // important to compare MPO first - since this is used by some algorithms 
    //         return ( net.m_machineID      != cmp.net.m_machineID )      ? ( net.m_machineID     < cmp.net.m_machineID ) :
    //                ( net.m_processID      != cmp.net.m_processID )      ? ( net.m_processID     < cmp.net.m_processID ) :
    //                ( net.m_ownerID        != cmp.net.m_ownerID )        ? ( net.m_ownerID       < cmp.net.m_ownerID ) :
    //                ( net.m_allocationID   != cmp.net.m_allocationID )   ? ( net.m_allocationID  < cmp.net.m_allocationID ) :
    //                ( net.m_flags          != cmp.net.m_flags )          ? ( net.m_flags         < cmp.net.m_flags ) :
    //                                                                       ( net.m_type          < cmp.net.m_type );
    //     }
    //     else if( isNetworkAddress() )
    //     {
    //         return this->operator<( cmp.getNetworkAddress() );
    //     }
    //     else
    //     {
    //         return getNetworkAddress().operator<( cmp );
    //     }
    //     // clang-format on
    // }
};


inline std::istream& operator>>( std::istream& is, Pointer& ref )
{
    THROW_TODO;
}
inline std::ostream& operator<<( std::ostream& os, const Pointer& ref )
{
    THROW_TODO;
}

inline std::ostream& operator<<( std::ostream& os, const std::vector< Pointer >& refVector )
{
    THROW_TODO;
}
/*
struct ObjectHeaderBase
{
    reference m_networkAddress;
    TimeStamp m_lockCycle = 0U;
    RefCount  m_refCount  = 0U;
};

inline RefCount reference::getRefCount() const
{
    return reinterpret_cast< ObjectHeaderBase* >( getHeap() )->m_refCount;
}

inline void reference::decRefCount() const
{
    --reinterpret_cast< ObjectHeaderBase* >( getHeap() )->m_refCount;
}

inline void reference::incRefCount() const
{
    ++reinterpret_cast< ObjectHeaderBase* >( getHeap() )->m_refCount;
}

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
    return reference::make( *this, TypeInstance::make_object( getType() ) );
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
}*/

} // namespace mega

#endif // MEGA_REFERENCE_18_SEPT_2022
