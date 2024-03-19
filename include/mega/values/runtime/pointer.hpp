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

#include "mega/values/runtime/inline.h"

#include "mega/values/native_types.hpp"

#include "mega/values/compilation/concrete/type_id_instance.hpp"

#include "mega/values/runtime/allocation_id.hpp"
#include "mega/values/runtime/machine_id.hpp"
#include "mega/values/runtime/mp.hpp"
#include "mega/values/runtime/mpo.hpp"

#include "common/serialisation.hpp"
#include "common/assert_verify.hpp"

namespace mega::runtime
{

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

    inline AllocationID getAllocationID() const { return AllocationID{ m_allocationID }; }

    inline MachineID getMachineID() const { return MachineID{ m_machineID }; }

    inline ProcessID getProcessID() const { return ProcessID{ m_processID }; }

    inline OwnerID getOwnerID() const { return OwnerID{ m_ownerID }; }

    inline MP getMP() const { return MP{ m_machineID, m_processID }; }

    inline MPO getMPO() const { return MPO{ m_machineID, m_processID, m_ownerID }; }

    inline concrete::TypeID getTypeID() const { return concrete::TypeID{ m_type.type_id }; }

    inline concrete::Instance getInstance() const { return concrete::Instance{ m_type.instance }; }

    inline concrete::TypeIDInstance getTypeIDInstance() const { return concrete::TypeIDInstance{ m_type }; }

    bool operator<( const PointerNet& ) const { THROW_TODO; }
    bool operator==( const PointerNet& ) const { THROW_TODO; }

    template < class Archive >
    inline void serialize( Archive&, const unsigned int )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            THROW_TODO;
            // archive& boost::serialization::make_nvp( "value", m_value );
        }
        else
        {
            THROW_TODO;
            // archive& m_value;
        }
    }
};
inline std::ostream& operator<<( std::ostream&, const PointerNet& )
{
    THROW_TODO;
}

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

    inline bool valid() const
    {
        return false;
        THROW_TODO;
    }

    inline PointerNet getPointerNet() const { return PointerNet{ m_header->m_pointer_net }; }

    inline AllocationID getAllocationID() const { return AllocationID{ m_header->m_pointer_net.m_allocationID }; }

    inline MachineID getMachineID() const { return MachineID{ m_header->m_pointer_net.m_machineID }; }

    inline ProcessID getProcessID() const { return ProcessID{ m_header->m_pointer_net.m_processID }; }

    inline OwnerID getOwnerID() const { return OwnerID{ m_header->m_pointer_net.m_ownerID }; }

    inline MP getMP() const { return MP{ m_header->m_pointer_net.m_machineID, m_header->m_pointer_net.m_processID }; }

    inline MPO getMPO() const
    {
        return MPO{ m_header->m_pointer_net.m_machineID, m_header->m_pointer_net.m_processID,
                    m_header->m_pointer_net.m_ownerID };
    }

    inline concrete::TypeID getTypeID() const { return concrete::TypeID{ m_type.type_id }; }

    inline concrete::Instance getInstance() const { return concrete::Instance{ m_type.instance }; }

    inline concrete::TypeIDInstance getTypeIDInstance() const { return concrete::TypeIDInstance{ m_type }; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            THROW_TODO;
            // archive& boost::serialization::make_nvp( "value", m_value );
        }
        else
        {
            THROW_TODO;
            // archive& m_value;
        }
    }
};

inline std::ostream& operator<<( std::ostream&, const PointerHeap& )
{
    THROW_TODO;
}
class Pointer : public c_pointer
{
public:
    inline bool isHeap() const { return value.heap.m_flags == 0; }
    inline bool isNetwork() const { return !isHeap(); }

    PointerHeap& heap() { return *reinterpret_cast< PointerHeap* >( &value.heap ); }
    PointerNet&  net() { return *reinterpret_cast< PointerNet* >( &value.net ); }

    const PointerHeap& heap() const { return *reinterpret_cast< const PointerHeap* >( &value.heap ); }

    const PointerNet& net() const { return *reinterpret_cast< const PointerNet* >( &value.net ); }

    /*struct Hash
    {
        inline U64 operator()( const Pointer& ptr ) const noexcept
        {
            // THROW_TODO;
            if( ptr.isHeap() )
            {
                return PointerHeap::Hash()( ptr.heap() );
            }
            else
            {
                return PointerNet::Hash()( ptr.net() );
            }
        }
    };*/

    inline bool operator==( const Pointer& ) const { throw "TODO"; }
    inline bool operator<( const Pointer& ) const { throw "TODO"; }
    inline bool valid() const { throw "TODO"; }

    inline Pointer getNetworkAddress() const { THROW_TODO; }
    inline Pointer getHeapAddress() const { THROW_TODO; }

    template < class Archive >
    inline void serialize( Archive&, const unsigned int )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            THROW_TODO;
            // archive& boost::serialization::make_nvp( "value", m_value );
        }
        else
        {
            THROW_TODO;
            // archive& m_value;
        }
    }
};

inline std::istream& operator>>( std::istream&, Pointer& )
{
    THROW_TODO;
}
inline std::ostream& operator<<( std::ostream&, const Pointer& )
{
    THROW_TODO;
}

inline std::ostream& operator<<( std::ostream&, const std::vector< Pointer >& )
{
    THROW_TODO;
}
} // namespace mega::runtime

#endif // MEGA_REFERENCE_18_SEPT_2022
