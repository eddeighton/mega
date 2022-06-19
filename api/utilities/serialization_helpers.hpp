#ifndef SERIALIZATION_HELPERS_3_JUNE_2022
#define SERIALIZATION_HELPERS_3_JUNE_2022

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/path_traits.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

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