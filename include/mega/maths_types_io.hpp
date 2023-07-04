
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
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/array.hpp>

namespace boost::serialization
{

template < typename Archive, typename TMathType >
inline void serializeMathType( Archive& ar, TMathType& mathType )
{
    if constexpr( mega::ArchiveTraits< Archive >::is_xml )
    {
        ar& boost::serialization::make_nvp( "data", mathType.data );
    }
    else // if constexpr ( !mega::ArchiveTraits< Archive >::is_xml )
    {
        ar& mathType.data;
    }
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

#endif // GUARD_2023_April_14_maths_types_io
