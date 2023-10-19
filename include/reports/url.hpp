
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

#include <vector>
#include <string>
#include <sstream>
#include <ostream>
#include <optional>

namespace mega::reports
{

class URL
{
    friend std::ostream& operator<<( std::ostream& os, const URL& url );
public:
    static URL parse( const std::string& strURL );

    inline std::string str() const
    {
        std::ostringstream os;
        os << *this;
        return os.str();
    }

    ReporterID                   reportID;
    std::vector< std::string >   path;
    std::optional< std::string > bookmark;
    std::optional< ReporterID >  reporterLinkTarget;
};

inline std::ostream& operator<<( std::ostream& os, const URL& url )
{
    // os << "http://0.0.0.0:8080/";

    bool bFirst = true;
    for( const auto& p : url.path )
    {
        if( bFirst )
        {
            bFirst = false;
        }
        else
        {
            os << '/';
        }
        os << p;
    }

    os << url.reportID;

    if( url.bookmark.has_value() )
    {
        os << '/' << url.bookmark.value();
    }
    if( url.reporterLinkTarget.has_value() )
    {
        os << '/' << url.reporterLinkTarget.value();
    }

    return os;
}

} // namespace mega::reports

#endif // GUARD_2023_October_19_url
