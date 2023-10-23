
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

#include "common/serialisation.hpp"
#include "common/assert_verify.hpp"

#include <boost/url.hpp>

#include <vector>
#include <string>
#include <sstream>
#include <ostream>
#include <optional>

namespace mega::reports
{

using URL = boost::url;

URL makeFileURL( const URL& url, const boost::filesystem::path& filePath );

} // namespace mega::reports


namespace boost::serialization
{

template < class Archive >
inline void serialize( Archive& ar, ::mega::reports::URL& url, const unsigned int version )
{
    if constexpr( Archive::is_saving::value )
    {
        std::string view = url.buffer();
        ar & view;
    }

    if constexpr( Archive::is_loading::value )
    {
        std::string str;
        ar & str;
        url = ::mega::reports::URL( str );
    }
}

}

#endif // GUARD_2023_October_19_url
