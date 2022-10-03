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





#ifndef VERSION_4_JUNE_2022
#define VERSION_4_JUNE_2022

#include <boost/serialization/nvp.hpp>

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
