
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

#ifndef GUARD_2023_February_08_type_id_io
#define GUARD_2023_February_08_type_id_io

#include "type_id.hpp"

// the boost serialisation error handlers use iostream
// boost/archive/basic_text_oprimitive.hpp:130
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <istream>
#include <ostream>
#include <sstream>

inline std::ostream& operator<<( std::ostream& os, const mega::TypeID& typeID )
{
    if( typeID.isContextID() )
    {
        // clang-format off
        return os << '[' << std::hex
                        << std::setw( 4 ) << std::setfill( '0' ) << static_cast< mega::U16 >( typeID.getObjectID() ) << '.'
                        << std::setw( 4 ) << std::setfill( '0' ) << static_cast< mega::U16 >( typeID.getSubObjectID() ) 
                << std::dec << ']';
        // clang-format on
    }
    else
    {
        return os << std::dec << typeID.getSymbolID();
    }
}
inline std::ostream& operator<<( std::ostream& os, const std::vector< mega::TypeID >& typeID )
{
    for( auto p = typeID.begin(), pNext = typeID.begin(); p != typeID.end(); ++p )
    {
        ++pNext;
        if( pNext == typeID.end() )
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
inline std::istream& operator>>( std::istream& is, mega::TypeID& typeID )
{
    char c;
    if( is.peek() == '[' )
    {
        mega::TypeID::ValueType object, subObject;
        is >> std::dec >> c >> std::hex >> std::setw( 4 ) >> object >> std::dec >> c >> std::hex >> std::setw( 4 )
            >> subObject >> std::dec >> c;
        typeID = mega::TypeID::make_context( object, subObject );
        return is;
    }
    else
    {
        mega::TypeID::ValueType symbolID;
        is >> std::dec >> symbolID;
        typeID = mega::TypeID( symbolID );
        return is;
    }
}

namespace boost::serialization
{
// xml
inline void serialize( boost::archive::xml_iarchive& ar, ::mega::TypeID& typeID, const unsigned int version )
{
    ::mega::TypeID::ValueType value;
    ar&                       boost::serialization::make_nvp( "symbolID", value );
    typeID = ::mega::TypeID( value );
}

inline void serialize( boost::archive::xml_oarchive& ar, ::mega::TypeID& typeID, const unsigned int version )
{
    ::mega::TypeID::ValueType value = typeID;
    ar&                       boost::serialization::make_nvp( "symbolID", value );
}

// binary
inline void serialize( boost::archive::binary_iarchive& ar, ::mega::TypeID& typeID, const unsigned int version )
{
    ::mega::TypeID::ValueType value;
    ar&                       value;
    typeID = ::mega::TypeID( value );
}

inline void serialize( boost::archive::binary_oarchive& ar, ::mega::TypeID& typeID, const unsigned int version )
{
    ::mega::TypeID::ValueType value = typeID;
    ar&                       value;
}

} // namespace boost::serialization

#endif // GUARD_2023_February_08_type_id_io
