
#ifndef VERSION_4_JUNE_2022
#define VERSION_4_JUNE_2022

#include "boost/serialization/nvp.hpp"

#include <string>
#include <cstddef>
#include <ostream>

namespace mega
{

struct Version
{
    static Version getVersion();

    int         major;
    int         minor;
    int         revision;
    std::string date;

    inline bool operator==( const Version& cmp ) const
    {
        return major == cmp.major && minor == cmp.minor && revision == cmp.revision && date == cmp.date;
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int v )
    {
        archive& boost::serialization::make_nvp( "major", major );
        archive& boost::serialization::make_nvp( "minor", minor );
        archive& boost::serialization::make_nvp( "revision", revision );
        archive& boost::serialization::make_nvp( "date", date );
    }
};

} // namespace mega

inline std::ostream& operator<<( std::ostream& os, const mega::Version& version )
{
    return os << version.major << "." << version.minor << "." << version.revision << "." << version.date;
}

#endif // VERSION_4_JUNE_2022
