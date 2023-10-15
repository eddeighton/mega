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

#include "sub_type_instance.hpp"
#include "reference.hpp"

#include "type_id_io.hpp"

#include "common/string.hpp"

// the boost serialisation error handlers use iostream
// boost/archive/basic_text_oprimitive.hpp:130
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <iostream>
#include <iomanip>

inline std::istream& operator>>( std::istream& is, mega::MP& mp )
{
    mega::U32 m;
    mega::U32 p;
    char      c;
    is >> std::dec >> m >> c >> p;
    mp = mega::MP( m, p );
    return is;
}

inline std::ostream& operator<<( std::ostream& os, const mega::MP& mp )
{
    return os << std::dec << std::setw( 8 ) << std::setfill( '0' ) << static_cast< mega::U32 >( mp.getMachineID() )
              << '.' << std::setw( 4 ) << std::setfill( '0' ) << static_cast< mega::U32 >( mp.getProcessID() );
}

inline std::istream& operator>>( std::istream& is, mega::MPO& mpo )
{
    mega::U32 m;
    mega::U32 p;
    mega::U32 o;
    char      c;
    is >> std::dec >> m >> c >> p >> c >> o;
    mpo = mega::MPO( m, p, o );
    return is;
}

inline std::ostream& operator<<( std::ostream& os, const mega::MPO& mpo )
{
    return os <<

           std::dec << std::setw( 8 ) << std::setfill( '0' ) << static_cast< mega::U32 >( mpo.getMachineID() ) << '.'
              << std::setw( 4 ) << std::setfill( '0' ) << static_cast< mega::U32 >( mpo.getProcessID() ) << '.'
              << std::setw( 2 ) << std::setfill( '0' ) << static_cast< mega::U32 >( mpo.getOwnerID() );
}

inline std::ostream& operator<<( std::ostream& os, const mega::SubTypeInstance& subTypeInstance )
{
    using ::operator<<;

    return os << '<' << std::hex <<

           std::setw( 4 ) << std::setfill( '0' ) << static_cast< mega::U32 >( subTypeInstance.getSubType() )

              << '.' <<

           std::setw( 4 ) << std::setfill( '0' ) << static_cast< mega::U32 >( subTypeInstance.getInstance() )

              << std::dec << '>';
}
inline std::istream& operator>>( std::istream& is, mega::SubTypeInstance& subTypeInstance )
{
    using ::  operator>>;
    mega::U32 subType, instance;
    char      c;
    is >> c >> std::hex >> subType >> c >> instance >> c;
    subTypeInstance = mega::SubTypeInstance( subType, instance );
    return is;
}

inline std::ostream& operator<<( std::ostream& os, const mega::TypeInstance& typeInstance )
{
    using ::operator<<;
    return os << std::dec << '[' << typeInstance.type << "." << std::setw( 4 ) << std::setfill( '0' ) << std::hex
              << typeInstance.instance << std::dec << ']';
}
inline std::istream& operator>>( std::istream& is, mega::TypeInstance& typeInstance )
{
    using ::operator>>;
    char    c;
    return is >> std::dec >> c >> typeInstance.type >> c >> std::setw( 4 ) >> std::hex >> typeInstance.instance
           >> std::dec >> c;
}

inline std::ostream& operator<<( std::ostream& os, const mega::reference& ref )
{
    if( ref.isHeapAddress() )
    {
        return os << std::hex << "x" << std::setw( 16 ) << std::setfill( '0' )
                  << reinterpret_cast< mega::U64 >( ref.getHeap() ) << "." << ref.getTypeInstance();
    }
    else
    {
        return os << std::dec << std::setw( 8 ) << std::setfill( '0' ) << ref.getAllocationID() << "." << ref.getMPO()
                  << "." << ref.getTypeInstance();
    }
}

inline std::ostream& operator<<( std::ostream& os, const std::vector< mega::reference >& refVector )
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

inline std::istream& operator>>( std::istream& is, mega::reference& ref )
{
    char c;
    if( is.peek() == 'x' )
    {
        mega::HeapAddress  heapAddress;
        mega::TypeInstance typeInstance;

        is >> std::dec >> c >> std::setw( 16 ) >> heapAddress >> std::setw( 1 ) >> c >> typeInstance;
        ref = mega::reference( typeInstance, heapAddress );
    }
    else
    {
        mega::AllocationID allocationID;
        mega::MachineID    machineID;
        mega::U32          processID;
        mega::U32          ownerID;
        mega::TypeInstance typeInstance;

        is >> std::dec >> allocationID >> c >> std::setw( 8 ) >> machineID >> c >> std::setw( 4 ) >> processID >> c
            >> std::setw( 2 ) >> ownerID >> c >> typeInstance;
        ref = mega::reference( typeInstance, mega::MPO( machineID, processID, ownerID ), allocationID );
    }
    return is;
}

namespace boost::serialization
{

inline void serialize( boost::archive::xml_iarchive& ar, ::mega::TypeInstance& value, const unsigned int version )
{
    ar& boost::serialization::make_nvp( "instance", value.instance );
    ar& boost::serialization::make_nvp( "type", value.type );
}

inline void serialize( boost::archive::xml_oarchive& ar, ::mega::TypeInstance& value, const unsigned int version )
{
    ar& boost::serialization::make_nvp( "instance", value.instance );
    ar& boost::serialization::make_nvp( "type", value.type );
}

inline void serialize( boost::archive::xml_iarchive& ar, ::mega::SubTypeInstance& subTypeInstance,
                       const unsigned int version )
{
    ::mega::U32 value;
    ar&         boost::serialization::make_nvp( "value", value );
    subTypeInstance = ::mega::SubTypeInstance( value );
}

inline void serialize( boost::archive::xml_oarchive& ar, ::mega::SubTypeInstance& subTypeInstance,
                       const unsigned int version )
{
    ::mega::U32 value = subTypeInstance.getValue();
    ar&         boost::serialization::make_nvp( "value", value );
}

inline void serialize( boost::archive::xml_iarchive& ar, ::mega::MP& value, const unsigned int version )
{
    ::mega::U32 machine{};
    ::mega::U32 process{};
    ar&         boost::serialization::make_nvp( "machine", machine );
    ar&         boost::serialization::make_nvp( "process", process );
    value = ::mega::MP( machine, process );
}

inline void serialize( boost::archive::xml_oarchive& ar, ::mega::MP& value, const unsigned int version )
{
    ::mega::U32 machine = value.getMachineID();
    ::mega::U32 process = value.getProcessID();
    ar&         boost::serialization::make_nvp( "machine", machine );
    ar&         boost::serialization::make_nvp( "process", process );
}

inline void serialize( boost::archive::xml_iarchive& ar, ::mega::MPO& value, const unsigned int version )
{
    ::mega::U32 machine{};
    ::mega::U32 process{};
    ::mega::U32 owner{};
    ar&         boost::serialization::make_nvp( "machine", machine );
    ar&         boost::serialization::make_nvp( "process", process );
    ar&         boost::serialization::make_nvp( "owner", owner );
    value = ::mega::MPO( machine, process, owner );
}

inline void serialize( boost::archive::xml_oarchive& ar, ::mega::MPO& value, const unsigned int version )
{
    ::mega::U32 machine = value.getMachineID();
    ::mega::U32 process = value.getProcessID();
    ::mega::U32 owner   = value.getOwnerID();
    ar&         boost::serialization::make_nvp( "machine", machine );
    ar&         boost::serialization::make_nvp( "process", process );
    ar&         boost::serialization::make_nvp( "owner", owner );
}

inline void serialize( boost::archive::xml_iarchive& ar, ::mega::reference& ref, const unsigned int version )
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

        ref = ::mega::reference( typeInstance, reinterpret_cast< ::mega::HeapAddress >( heap ) );
    }
    else
    {
        ::mega::AllocationID allocationID{};
        ::mega::MPO          mpo{};
        ::mega::TypeInstance typeInstance{};

        ar& boost::serialization::make_nvp( "allocationID", allocationID );
        ar& boost::serialization::make_nvp( "mpo", mpo );
        ar& boost::serialization::make_nvp( "type_instance", typeInstance );

        ref = ::mega::reference( typeInstance, mpo, allocationID );
    }
}

inline void serialize( boost::archive::xml_oarchive& ar, ::mega::reference& ref, const unsigned int version )
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
        ::mega::MPO          mpo = ref.getMPO();
        ar&                  boost::serialization::make_nvp( "mpo", mpo );
        ::mega::TypeInstance typeInstance = ref.getTypeInstance();
        ar&                  boost::serialization::make_nvp( "type_instance", typeInstance );
    }
}

// binary

inline void serialize( boost::archive::binary_iarchive& ar, ::mega::TypeInstance& value, const unsigned int version )
{
    ar& value.type;
    ar& value.instance;
}

inline void serialize( boost::archive::binary_oarchive& ar, ::mega::TypeInstance& value, const unsigned int version )
{
    ar& value.type;
    ar& value.instance;
}

inline void serialize( boost::archive::binary_iarchive& ar, ::mega::SubTypeInstance& subTypeInstance,
                       const unsigned int version )
{
    ::mega::U32 value;
    ar&         value;
    subTypeInstance = ::mega::SubTypeInstance( value );
}

inline void serialize( boost::archive::binary_oarchive& ar, ::mega::SubTypeInstance& subTypeInstance,
                       const unsigned int version )
{
    ::mega::U32 value  = subTypeInstance.getValue();
    ar&         value;
}

inline void serialize( boost::archive::binary_iarchive& ar, ::mega::MP& value, const unsigned int version )
{
    ::mega::U32 machine{};
    ::mega::U32 process{};
    ar&         machine;
    ar&         process;
    value = ::mega::MP( machine, process );
}

inline void serialize( boost::archive::binary_oarchive& ar, ::mega::MP& value, const unsigned int version )
{
    ar&( ::mega::U32 )value.getMachineID();
    ar&( ::mega::U32 )value.getProcessID();
}

inline void serialize( boost::archive::binary_iarchive& ar, ::mega::MPO& value, const unsigned int version )
{
    ::mega::U32 machine{};
    ::mega::U32 process{};
    ::mega::U32 owner{};
    ar&         machine;
    ar&         process;
    ar&         owner;
    value = ::mega::MPO( machine, process, owner );
}

inline void serialize( boost::archive::binary_oarchive& ar, ::mega::MPO& value, const unsigned int version )
{
    ar&( ::mega::U32 )value.getMachineID();
    ar&( ::mega::U32 )value.getProcessID();
    ar&( ::mega::U32 )value.getOwnerID();
}

inline void serialize( boost::archive::binary_iarchive& ar, ::mega::reference& ref, const unsigned int version )
{
    ::mega::Flags flags{};
    ar&           flags;

    if( flags == ::mega::HEAP_ADDRESS )
    {
        ::mega::U64          heap{};
        ::mega::TypeInstance typeInstance{};

        ar& heap;
        ar& typeInstance;

        ref = ::mega::reference( typeInstance, reinterpret_cast< ::mega::HeapAddress >( heap ) );
    }
    else
    {
        ::mega::AllocationID allocationID{};
        ::mega::MPO          mpo{};
        ::mega::TypeInstance typeInstance{};

        ar& allocationID;
        ar& mpo;
        ar& typeInstance;

        ref = ::mega::reference( typeInstance, mpo, allocationID );
    }
}

inline void serialize( boost::archive::binary_oarchive& ar, ::mega::reference& ref, const unsigned int version )
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
}

} // namespace boost::serialization

#endif // REFERENCE_IO_24_SEPT_2022
