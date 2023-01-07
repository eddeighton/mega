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

#include "reference.hpp"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <iostream>

inline std::istream& operator>>( std::istream& is, mega::MP& mp )
{
    mega::U32 m;
    mega::U32 p;
    char      c;
    is >> m >> c >> p;
    mp = mega::MP( m, p );
    return is;
}

inline std::ostream& operator<<( std::ostream& os, const mega::MP& mp )
{
    return os << static_cast< mega::U32 >( mp.getMachineID() ) << '.' << static_cast< mega::U32 >( mp.getProcessID() );
}

inline std::istream& operator>>( std::istream& is, mega::MPO& mpo )
{
    mega::U32 m;
    mega::U32 p;
    mega::U32 o;
    char      c;
    is >> m >> c >> p >> c >> o;
    mpo = mega::MPO( m, p, o );
    return is;
}

inline std::ostream& operator<<( std::ostream& os, const mega::MPO& mpo )
{
    return os << static_cast< mega::U32 >( mpo.getMachineID() ) << '.' << static_cast< mega::U32 >( mpo.getProcessID() )
              << '.' << static_cast< mega::U32 >( mpo.getOwnerID() );
}

inline std::ostream& operator<<( std::ostream& os, const mega::TypeInstance& typeInstance )
{
    return os << '[' << typeInstance.type << "." << typeInstance.instance << ']';
}

inline std::ostream& operator<<( std::ostream& os, const mega::reference& ref )
{
    if( ref.isHeapAddress() )
    {
        return os << std::hex << reinterpret_cast< mega::U64 >( ref.getHeap() ) << "."
                  << static_cast< mega::U32 >( ref.getOwnerID() ) << "." << ref.getTypeInstance();
    }
    else
    {
        return os << ref.getObjectID() << "." << ref.getMPO() << "." << ref.getTypeInstance();
    }
}

namespace boost::serialization
{

// xml
inline void serialize( boost::archive::xml_iarchive& ar, mega::TypeInstance& value, const unsigned int version )
{
    ar& boost::serialization::make_nvp( "instance", value.instance );
    ar& boost::serialization::make_nvp( "type", value.type );
}

inline void serialize( boost::archive::xml_oarchive& ar, mega::TypeInstance& value, const unsigned int version )
{
    ar& boost::serialization::make_nvp( "instance", value.instance );
    ar& boost::serialization::make_nvp( "type", value.type );
}

inline void serialize( boost::archive::xml_iarchive& ar, mega::MP& value, const unsigned int version )
{
    mega::U32 machine{};
    mega::U32 process{};
    ar&       boost::serialization::make_nvp( "machine", machine );
    ar&       boost::serialization::make_nvp( "process", process );
    value = mega::MP( machine, process );
}

inline void serialize( boost::archive::xml_oarchive& ar, mega::MP& value, const unsigned int version )
{
    mega::U32 machine = value.getMachineID();
    mega::U32 process = value.getProcessID();
    ar&       boost::serialization::make_nvp( "machine", machine );
    ar&       boost::serialization::make_nvp( "process", process );
}

inline void serialize( boost::archive::xml_iarchive& ar, mega::MPO& value, const unsigned int version )
{
    mega::U32 machine{};
    mega::U32 process{};
    mega::U32 owner{};
    ar&       boost::serialization::make_nvp( "machine", machine );
    ar&       boost::serialization::make_nvp( "process", process );
    ar&       boost::serialization::make_nvp( "owner", owner );
    value = mega::MPO( machine, process, owner );
}

inline void serialize( boost::archive::xml_oarchive& ar, mega::MPO& value, const unsigned int version )
{
    mega::U32 machine = value.getMachineID();
    mega::U32 process = value.getProcessID();
    mega::U32 owner   = value.getOwnerID();
    ar&       boost::serialization::make_nvp( "machine", machine );
    ar&       boost::serialization::make_nvp( "process", process );
    ar&       boost::serialization::make_nvp( "owner", owner );
}

inline void serialize( boost::archive::xml_iarchive& ar, mega::reference& ref, const unsigned int version )
{
    mega::Flags flags{};
    ar&         boost::serialization::make_nvp( "flags", flags );

    if( flags == mega::HEAP_ADDRESS )
    {
        mega::U64          heap{};
        mega::U32          owner{};
        mega::TypeInstance typeInstance{};

        // NOTE reloading heap pointer!
        ar& boost::serialization::make_nvp( "heap", heap );
        ar& boost::serialization::make_nvp( "owner", owner );
        ar& boost::serialization::make_nvp( "type_instance", typeInstance );

        ref = mega::reference( typeInstance, owner, reinterpret_cast< mega::HeapAddress >( heap ) );
    }
    else
    {
        mega::ObjectID     object{};
        mega::MPO          mpo{};
        mega::Flags        flags{};
        mega::TypeInstance typeInstance{};

        ar& boost::serialization::make_nvp( "object", object );
        ar& boost::serialization::make_nvp( "mpo", mpo );
        ar& boost::serialization::make_nvp( "flags", flags );
        ar& boost::serialization::make_nvp( "type_instance", typeInstance );

        ref = mega::reference( typeInstance, mpo, object );
    }
}

inline void serialize( boost::archive::xml_oarchive& ar, mega::reference& ref, const unsigned int version )
{
    mega::Flags flags = ref.getFlags();
    ar&         boost::serialization::make_nvp( "flags", flags );

    if( ref.isHeapAddress() )
    {
        auto               heap = reinterpret_cast< mega::U64 >( ref.getHeap() );
        ar&                boost::serialization::make_nvp( "heap", heap );
        mega::U32          owner = ref.getOwnerID();
        ar&                boost::serialization::make_nvp( "owner", owner );
        mega::TypeInstance typeInstance = ref.getTypeInstance();
        ar&                boost::serialization::make_nvp( "type_instance", typeInstance );
    }
    else
    {
        mega::ObjectID     object = ref.getObjectID();
        ar&                boost::serialization::make_nvp( "object", object );
        mega::MPO          mpo = ref.getMPO();
        ar&                boost::serialization::make_nvp( "mpo", mpo );
        mega::TypeInstance typeInstance = ref.getTypeInstance();
        ar&                boost::serialization::make_nvp( "type_instance", typeInstance );
    }
}

// binary
inline void serialize( boost::archive::binary_iarchive& ar, mega::TypeInstance& value, const unsigned int version )
{
    ar& value.instance;
    ar& value.type;
}

inline void serialize( boost::archive::binary_oarchive& ar, mega::TypeInstance& value, const unsigned int version )
{
    ar& value.instance;
    ar& value.type;
}

inline void serialize( boost::archive::binary_iarchive& ar, mega::MP& value, const unsigned int version )
{
    mega::U32 machine{};
    mega::U32 process{};
    ar&       machine;
    ar&       process;
    value = mega::MP( machine, process );
}

inline void serialize( boost::archive::binary_oarchive& ar, mega::MP& value, const unsigned int version )
{
    ar& (mega::U32)value.getMachineID();
    ar& (mega::U32)value.getProcessID();
}

inline void serialize( boost::archive::binary_iarchive& ar, mega::MPO& value, const unsigned int version )
{
    mega::U32 machine{};
    mega::U32 process{};
    mega::U32 owner{};
    ar&       machine;
    ar&       process;
    ar&       owner;
    value = mega::MPO( machine, process, owner );
}

inline void serialize( boost::archive::binary_oarchive& ar, mega::MPO& value, const unsigned int version )
{
    ar& (mega::U32)value.getMachineID();
    ar& (mega::U32)value.getProcessID();
    ar& (mega::U32)value.getOwnerID();
}

inline void serialize( boost::archive::binary_iarchive& ar, mega::reference& ref, const unsigned int version )
{
    mega::Flags flags{};
    ar&         flags;

    if( flags == mega::HEAP_ADDRESS )
    {
        mega::U64          heap{};
        mega::U32          owner{};
        mega::TypeInstance typeInstance{};

        ar& heap;
        ar& owner;
        ar& typeInstance;

        ref = mega::reference( typeInstance, owner, reinterpret_cast< mega::HeapAddress >( heap ) );
    }
    else
    {
        mega::ObjectID     object{};
        mega::MPO          mpo{};
        mega::TypeInstance typeInstance{};

        ar& object;
        ar& mpo;
        ar& typeInstance;

        ref = mega::reference( typeInstance, mpo, object );
    }
}

inline void serialize( boost::archive::binary_oarchive& ar, mega::reference& ref, const unsigned int version )
{
    ar& ref.getFlags();
    if( ref.isHeapAddress() )
    {
        ar& reinterpret_cast< mega::U64 >( ref.getHeap() );
        ar& (mega::U32)ref.getOwnerID();
        ar& ref.getTypeInstance();
    }
    else
    {
        ar& ref.getObjectID();
        ar& ref.getMPO();
        ar& ref.getTypeInstance();
    }
}

} // namespace boost::serialization

#endif // REFERENCE_IO_24_SEPT_2022
