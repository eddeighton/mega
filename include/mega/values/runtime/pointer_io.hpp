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

#ifndef REFERENCE_IO_24_SEPT_2022
#define REFERENCE_IO_24_SEPT_2022

#include "mega/values/compilation/concrete/type_id.hpp"

#include "mega/values/runtime/pointer.hpp"

#include "common/string.hpp"

// the boost serialisation error handlers use iostream
// boost/archive/basic_text_oprimitive.hpp:130
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <iostream>
#include <iomanip>

namespace mega
{
    /*
inline std::istream& operator>>( std::istream& is, MP& mp )
{
    U32 m;
    U32 p;
    char      c;
    is >> std::dec >> m >> c >> p;
    mp = MP( m, p );
    return is;
}

inline std::ostream& operator<<( std::ostream& os, const MP& mp )
{
    return os << std::dec << std::setw( 8 ) << std::setfill( '0' ) << static_cast< U32 >( mp.getMachineID() )
              << '.' << std::setw( 4 ) << std::setfill( '0' ) << static_cast< U32 >( mp.getProcessID() );
}

inline std::istream& operator>>( std::istream& is, MPO& mpo )
{
    U32 m;
    U32 p;
    U32 o;
    char      c;
    is >> std::dec >> m >> c >> p >> c >> o;
    mpo = MPO( m, p, o );
    return is;
}

inline std::ostream& operator<<( std::ostream& os, const MPO& mpo )
{
    return os <<

           std::dec << std::setw( 8 ) << std::setfill( '0' ) << static_cast< U32 >( mpo.getMachineID() ) << '.'
              << std::setw( 4 ) << std::setfill( '0' ) << static_cast< U32 >( mpo.getProcessID() ) << '.'
              << std::setw( 2 ) << std::setfill( '0' ) << static_cast< U32 >( mpo.getOwnerID() );
}

inline std::ostream& operator<<( std::ostream& os, const Pointer& ref )
{
    if( ref.isHeapAddress() )
    {
        return os << std::hex << "x" << std::setw( 16 ) << std::setfill( '0' )
                  << reinterpret_cast< U64 >( ref.getHeap() ) << "." << ref.getTypeInstance();
    }
    else
    {
        return os << std::dec << std::setw( 8 ) << std::setfill( '0' ) << ref.getAllocationID() << "." << ref.getMPO()
                  << "." << ref.getTypeInstance();
    }
}

inline std::ostream& operator<<( std::ostream& os, const std::vector< Pointer >& refVector )
{
    for( auto p = refVector.begin(), pNext = refVector.begin(); p != refVector.end(); ++p )
    {
        ++pNext;
        if( pNext == refVector.end() )
        {
            os << *p;
        }
        else
        {
            os << *p << " ";
        }
    }
    // common::delimit(  refVector.begin(), refVector.end(), " ", os );
    return os;
}

inline std::istream& operator>>( std::istream& is, Pointer& ref )
{
    char c;
    if( is.peek() == 'x' )
    {
        HeapAddress  heapAddress;
        TypeInstance typeInstance;

        is >> std::dec >> c >> std::setw( 16 ) >> heapAddress >> std::setw( 1 ) >> c >> typeInstance;
        ref = Pointer( typeInstance, heapAddress );
    }
    else
    {
        AllocationID allocationID;
        MachineID    machineID;
        U32          processID;
        U32          ownerID;
        TypeInstance typeInstance;

        is >> std::dec >> allocationID >> c >> std::setw( 8 ) >> machineID >> c >> std::setw( 4 ) >> processID >> c
            >> std::setw( 2 ) >> ownerID >> c >> typeInstance;
        ref = Pointer( typeInstance, MPO( machineID, processID, ownerID ), allocationID );
    }
    return is;
}*/
} // namespace mega

namespace boost::serialization
{
/*
inline void serialize( boost::archive::xml_iarchive& ar, ::mega::runtime::MP& value, const unsigned int version )
{
    ::mega::U32 machine{};
    ::mega::U32 process{};
    ar&         boost::serialization::make_nvp( "machine", machine );
    ar&         boost::serialization::make_nvp( "process", process );
    value = ::mega::runtime::MP( machine, process );
}

inline void serialize( boost::archive::xml_oarchive& ar, ::mega::runtime::MP& value, const unsigned int version )
{
    ::mega::U32 machine = value.getMachineID();
    ::mega::U32 process = value.getProcessID();
    ar&         boost::serialization::make_nvp( "machine", machine );
    ar&         boost::serialization::make_nvp( "process", process );
}

inline void serialize( boost::archive::xml_iarchive& ar, ::mega::runtime::MPO& value, const unsigned int version )
{
    ::mega::U32 machine{};
    ::mega::U32 process{};
    ::mega::U32 owner{};
    ar&         boost::serialization::make_nvp( "machine", machine );
    ar&         boost::serialization::make_nvp( "process", process );
    ar&         boost::serialization::make_nvp( "owner", owner );
    value = ::mega::runtime::MPO( machine, process, owner );
}

inline void serialize( boost::archive::xml_oarchive& ar, ::mega::runtime::MPO& value, const unsigned int version )
{
    ::mega::U32 machine = value.getMachineID();
    ::mega::U32 process = value.getProcessID();
    ::mega::U32 owner   = value.getOwnerID();
    ar&         boost::serialization::make_nvp( "machine", machine );
    ar&         boost::serialization::make_nvp( "process", process );
    ar&         boost::serialization::make_nvp( "owner", owner );
}

inline void serialize( boost::archive::xml_iarchive& ar, ::mega::runtime::Pointer& ref, const unsigned int version )
{
    ::mega::Flags flags{};
    ar&           boost::serialization::make_nvp( "flags", flags );

    if( flags == ::mega::HEAP_ADDRESS )
    {
        ::mega::U64          heap{};
        ::mega::TypeInstance typeInstance{};

        // NOTE reloading heap pointer!
        ar& boost::serialization::make_nvp( "heap", heap );
        ar& boost::serialization::make_nvp( "type_instance", typeInstance );

        ref = ::mega::runtime::Pointer( typeInstance, reinterpret_cast< ::mega::HeapAddress >( heap ) );
    }
    else
    {
        ::mega::AllocationID allocationID{};
        ::mega::runtime::MPO          mpo{};
        ::mega::TypeInstance typeInstance{};

        ar& boost::serialization::make_nvp( "allocationID", allocationID );
        ar& boost::serialization::make_nvp( "mpo", mpo );
        ar& boost::serialization::make_nvp( "type_instance", typeInstance );

        ref = ::mega::runtime::Pointer( typeInstance, mpo, allocationID );
    }
}

inline void serialize( boost::archive::xml_oarchive& ar, ::mega::runtime::Pointer& ref, const unsigned int version )
{
    ::mega::Flags flags = ref.getFlags();
    ar&           boost::serialization::make_nvp( "flags", flags );

    if( ref.isHeapAddress() )
    {
        auto                 heap = reinterpret_cast< ::mega::U64 >( ref.getHeap() );
        ar&                  boost::serialization::make_nvp( "heap", heap );
        ::mega::TypeInstance typeInstance = ref.getTypeInstance();
        ar&                  boost::serialization::make_nvp( "type_instance", typeInstance );
    }
    else
    {
        ::mega::AllocationID allocationID = ref.getAllocationID();
        ar&                  boost::serialization::make_nvp( "allocationID", allocationID );
        ::mega::runtime::MPO          mpo = ref.getMPO();
        ar&                  boost::serialization::make_nvp( "mpo", mpo );
        ::mega::TypeInstance typeInstance = ref.getTypeInstance();
        ar&                  boost::serialization::make_nvp( "type_instance", typeInstance );
    }
}

// binary

inline void serialize( boost::archive::binary_iarchive& ar, ::mega::runtime::MP& value, const unsigned int version )
{
    ::mega::U32 machine{};
    ::mega::U32 process{};
    ar&         machine;
    ar&         process;
    value = ::mega::runtime::MP( machine, process );
}

inline void serialize( boost::archive::binary_oarchive& ar, ::mega::runtime::MP& value, const unsigned int version )
{
    ar&( ::mega::U32 )value.getMachineID();
    ar&( ::mega::U32 )value.getProcessID();
}

inline void serialize( boost::archive::binary_iarchive& ar, ::mega::runtime::MPO& value, const unsigned int version )
{
    ::mega::U32 machine{};
    ::mega::U32 process{};
    ::mega::U32 owner{};
    ar&         machine;
    ar&         process;
    ar&         owner;
    value = ::mega::runtime::MPO( machine, process, owner );
}

inline void serialize( boost::archive::binary_oarchive& ar, ::mega::runtime::MPO& value, const unsigned int version )
{
    ar&( ::mega::U32 )value.getMachineID();
    ar&( ::mega::U32 )value.getProcessID();
    ar&( ::mega::U32 )value.getOwnerID();
}

inline void serialize( boost::archive::binary_iarchive& ar, ::mega::runtime::Pointer& ref, const unsigned int version )
{
    ::mega::Flags flags{};
    ar&           flags;

    if( flags == ::mega::HEAP_ADDRESS )
    {
        ::mega::U64          heap{};
        ::mega::TypeInstance typeInstance{};

        ar& heap;
        ar& typeInstance;

        ref = ::mega::runtime::Pointer( typeInstance, reinterpret_cast< ::mega::HeapAddress >( heap ) );
    }
    else
    {
        ::mega::AllocationID allocationID{};
        ::mega::runtime::MPO          mpo{};
        ::mega::TypeInstance typeInstance{};

        ar& allocationID;
        ar& mpo;
        ar& typeInstance;

        ref = ::mega::runtime::Pointer( typeInstance, mpo, allocationID );
    }
}

inline void serialize( boost::archive::binary_oarchive& ar, ::mega::runtime::Pointer& ref, const unsigned int version )
{
    ar& ref.getFlags();
    if( ref.isHeapAddress() )
    {
        ar& reinterpret_cast< ::mega::U64 >( ref.getHeap() );
        ar& ref.getTypeInstance();
    }
    else
    {
        ar& ref.getAllocationID();
        ar& ref.getMPO();
        ar& ref.getTypeInstance();
    }
}*/

} // namespace boost::serialization

#endif // REFERENCE_IO_24_SEPT_2022
