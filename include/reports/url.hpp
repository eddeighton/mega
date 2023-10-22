
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

#ifndef GUARD_2023_October_19_url
#define GUARD_2023_October_19_url

#include "reporter_id.hpp"

#include "common/serialisation.hpp"
#include "common/assert_verify.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <ostream>
#include <optional>

namespace mega::reports
{

class URL
{
    friend class boost::serialization::access;
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& url;
    }

    friend std::ostream& operator<<( std::ostream& os, const URL& url );

public:
    inline std::string str() const
    {
        std::ostringstream os;
        os << *this;
        return os.str();
    }

    static inline URL makeFile( const boost::filesystem::path& filePath )
    {
        std::ostringstream osFileURL;
        
        osFileURL << "file:///" << filePath.string();

        return URL{ osFileURL.str() };
    }

    static inline URL makeWEB( const std::string& strReport )
    {
        std::ostringstream osWEB;
        
        osWEB << "http://0.0.0.0:8080/" << strReport;

        return URL{ osWEB.str() };
    }

    std::string url;
};

inline std::ostream& operator<<( std::ostream& os, const URL& url )
{
    return os << url.url;
}

} // namespace mega::reports

#endif // GUARD_2023_October_19_url
