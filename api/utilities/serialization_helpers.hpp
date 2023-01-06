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




#ifndef SERIALIZATION_HELPERS_3_JUNE_2022
#define SERIALIZATION_HELPERS_3_JUNE_2022

#include <boost/filesystem/path.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/utility.hpp>

#include <optional>

namespace boost
{
namespace serialization
{
template < class Archive >
inline void serialize( Archive& ar, boost::filesystem::path& p, const unsigned int version )
{
    std::string s;
    if ( Archive::is_saving::value )
        s = p.generic_string();
    ar& boost::serialization::make_nvp( "filepath", s );
    if ( Archive::is_loading::value )
        p = s;
}

template < class Archive >
inline void serialize( Archive& ar, std::optional< boost::filesystem::path >& p, const unsigned int version )
{
    std::string s;
    if ( Archive::is_saving::value )
    {
        if ( p.has_value() )
            s = p.value().generic_string();
        else
            s = "";
    }
    ar& boost::serialization::make_nvp( "string", s );
    if ( Archive::is_loading::value )
    {
        if ( s.empty() )
            p = std::optional< boost::filesystem::path >();
        else
            p = s;
    }
}

template < class Archive, class T >
inline void serialize( Archive& ar, std::optional< T >& optionalValue, const unsigned int version )
{
    if ( Archive::is_saving::value )
    {
        if ( optionalValue.has_value() )
        {
            bool bTrueIsTrue = true;
            ar&  bTrueIsTrue;
            ar&  optionalValue.value();
        }
        else
        {
            bool bFalseIsFalse = false;
            ar&  bFalseIsFalse;
        }
    }

    if ( Archive::is_loading::value )
    {
        bool bHasValue = false;
        ar&  bHasValue;
        if ( bHasValue )
        {
            // MUST ensure the ObjectInfo passed to archive can have address captured
            optionalValue.emplace( T{} );
            ar& optionalValue.value();
        }
    }
}
} // namespace serialization
} // namespace boost

#endif // SERIALIZATION_HELPERS_3_JUNE_2022