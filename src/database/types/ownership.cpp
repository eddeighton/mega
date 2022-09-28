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


#include "database/types/ownership.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>

namespace
{
std::array< std::string, mega::Ownership::TOTAL_OWNERSHIP_MODES > g_pszModes
    = { "OwnNothing", "OwnSource", "OwnTarget" };
}

namespace mega
{

const char* Ownership::str() const
{
    switch ( m_value )
    {
        case eOwnNothing:
        case eOwnSource:
        case eOwnTarget:
            return g_pszModes[ m_value ].c_str();
        case TOTAL_OWNERSHIP_MODES:
        default:
            THROW_RTE( "Invalid Ownership type" );
    }
}

Ownership Ownership::fromStr( const char* psz )
{
    std::array< std::string, TOTAL_OWNERSHIP_MODES >::const_iterator iFind
        = std::find( g_pszModes.begin(), g_pszModes.end(), psz );
    VERIFY_RTE_MSG( iFind != g_pszModes.end(), "Unknown ownership mode: " << psz );
    return Ownership( static_cast< Ownership::Value >( std::distance( g_pszModes.cbegin(), iFind ) ) );
}
} // namespace mega

std::ostream& operator<<( std::ostream& os, mega::Ownership ownership ) { return os << ownership.str(); }
