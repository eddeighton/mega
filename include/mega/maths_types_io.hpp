
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

#ifndef GUARD_2023_April_14_maths_types_io
#define GUARD_2023_April_14_maths_types_io

#include "mega/maths_types.hpp"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/array.hpp>

#include <ostream>

namespace boost::serialization
{

template < typename Archive, typename TMathType >
inline void serializeMathType( Archive& ar, TMathType& mathType )
{
    ar& mathType.data;
}

template < class Archive >
inline void serialize( Archive& ar, ::F2& v, const unsigned int version )
{
    serializeMathType( ar, v );
}

template < class Archive >
inline void serialize( Archive& ar, ::F3& v, const unsigned int version )
{
    serializeMathType( ar, v );
}

template < class Archive >
inline void serialize( Archive& ar, ::F4& v, const unsigned int version )
{
    serializeMathType( ar, v );
}

template < class Archive >
inline void serialize( Archive& ar, ::Quat& v, const unsigned int version )
{
    serializeMathType( ar, v );
}

template < class Archive >
inline void serialize( Archive& ar, ::F33& v, const unsigned int version )
{
    serializeMathType( ar, v );
}

} // namespace boost::serialization
static constexpr char MATHS_DELIM = ',';
static constexpr char MATHS_OPEN = '(';
static constexpr char MATHS_CLOSE = ')';

inline std::ostream& operator<<( std::ostream& os, const F2& v )
{
    return os << MATHS_OPEN << v.x() << MATHS_DELIM << v.y() << MATHS_CLOSE;
}
inline std::ostream& operator<<( std::ostream& os, const F3& v )
{
    return os << MATHS_OPEN << v.x() << MATHS_DELIM << v.y() << MATHS_DELIM << v.z() << MATHS_CLOSE;
}
inline std::ostream& operator<<( std::ostream& os, const F4& v )
{
    return os << MATHS_OPEN << v.x() << MATHS_DELIM << v.y() << MATHS_DELIM << v.z() << MATHS_DELIM << v.w() << MATHS_CLOSE;
}
inline std::ostream& operator<<( std::ostream& os, const Quat& v )
{
    return os << MATHS_OPEN << v.x() << MATHS_DELIM << v.y() << MATHS_DELIM << v.z() << MATHS_DELIM << v.w() << MATHS_CLOSE;
}
inline std::ostream& operator<<( std::ostream& os, const F33& v )
{
    bool bOuterFirst = true;
    os << MATHS_OPEN;
    for( const auto& row : v.data )
    {   
        if( bOuterFirst ) bOuterFirst = false;
        else os << MATHS_DELIM;
        os << MATHS_OPEN;
        bool bFirst = true;
        for( auto value : row )
        {
            if( bFirst ) bFirst = false;
            else os << MATHS_DELIM;
            os << value;
        }
        os << MATHS_CLOSE;
    }
    os << MATHS_CLOSE;
    return os;
}

#endif // GUARD_2023_April_14_maths_types_io
