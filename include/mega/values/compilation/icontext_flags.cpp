
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

#include "mega/values/compilation/icontext_flags.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>
#include <array>

namespace
{
static const std::array< std::string, mega::IContextFlags::TOTAL_FLAGS > g_pszModes = { "Meta", "OR", "Historical" };
}

namespace mega
{

std::string IContextFlags::str() const
{
    std::ostringstream os;
    os << "[";
    bool bFirst = true;
    for( std::size_t sz = 0; sz != TOTAL_FLAGS; ++sz )
    {
        if( bFirst )
        {
            bFirst = false;
        }
        else
        {
            os << ",";
        }
        if( m_value.test( sz ) )
        {
            os << g_pszModes[ sz ].c_str() << "=1";
        }
        else
        {
            os << g_pszModes[ sz ].c_str() << "=0";
        }
    }
    os << "]";
    return os.str();
}

const char* IContextFlags::str( Value bit )
{
    return g_pszModes[ bit ].c_str();
}
const std::array< std::string, IContextFlags::TOTAL_FLAGS >& IContextFlags::strings()
{
    return g_pszModes;
}

std::ostream& operator<<( std::ostream& os, IContextFlags value )
{
    return os << value.str();
}

} // namespace mega