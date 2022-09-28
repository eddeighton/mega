


#ifndef REFERENCE_IO_24_SEPT_2022
#define REFERENCE_IO_24_SEPT_2022

#include "reference.hpp"

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

#endif // REFERENCE_IO_24_SEPT_2022
