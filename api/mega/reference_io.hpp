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

#include <iostream>

inline std::istream& operator>>( std::istream& is, mega::MP& mp )
{
    bool bIsDaemon = true;
    int  m, p = 0;
    is >> m;

    if ( is.peek() == '.' )
    {
        char c;
        is >> c;
        bIsDaemon = false;
        is >> p;
    }

    mp = mega::MP( m, p, bIsDaemon );
    return is;
}

inline std::ostream& operator<<( std::ostream& os, const mega::MP& mp )
{
    if ( mp.getIsDaemon() )
    {
        return os << ( int )mp.getMachineID();
    }
    else
    {
        return os << ( int )mp.getMachineID() << '.' << ( int )mp.getProcessID();
    }
}

inline std::istream& operator>>( std::istream& is, mega::MPO& mpo )
{
    int  m, p, o;
    char c;
    is >> m >> c >> p >> c >> o;
    mpo = mega::MPO( m, p, o );
    return is;
}

inline std::ostream& operator<<( std::ostream& os, const mega::MPO& mpo )
{
    return os << ( int )mpo.getMachineID() << '.' << ( int )mpo.getProcessID() << '.' << ( int )mpo.getOwnerID();
}


namespace boost::serialization
{
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

inline void serialize( boost::archive::binary_iarchive& ar, mega::NetworkOrProcessAddress& value, const unsigned int version )
{
    ar& value.nop_storage;
}

inline void serialize( boost::archive::binary_oarchive& ar, mega::NetworkOrProcessAddress& value, const unsigned int version )
{
    ar& value.nop_storage;
}

inline void serialize( boost::archive::binary_iarchive& ar, mega::MP& value, const unsigned int version )
{
    ar& value.mp_storage;
}

inline void serialize( boost::archive::binary_oarchive& ar, mega::MP& value, const unsigned int version )
{
    ar& value.mp_storage;
}

inline void serialize( boost::archive::binary_iarchive& ar, mega::MPO& value, const unsigned int version )
{
    ar& value.mpo_storage;
}

inline void serialize( boost::archive::binary_oarchive& ar, mega::MPO& value, const unsigned int version )
{
    ar& value.mpo_storage;
}

inline void serialize( boost::archive::binary_iarchive& ar, mega::reference& value, const unsigned int version )
{
    ar& static_cast< mega::TypeInstance& >( value );
    ar& static_cast< mega::MPO& >( value );
    ar& static_cast< mega::NetworkOrProcessAddress& >( value );
}

inline void serialize( boost::archive::binary_oarchive& ar, mega::reference& value, const unsigned int version )
{
    ar& static_cast< mega::TypeInstance& >( value );
    ar& static_cast< mega::MPO& >( value );
    ar& static_cast< mega::NetworkOrProcessAddress& >( value );
}

}

#endif // REFERENCE_IO_24_SEPT_2022
