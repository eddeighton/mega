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


#include "database/types/derivation.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>

namespace
{
std::array< std::string, mega::DerivationDirection::TOTAL_DERIVATION_MODES > g_pszModes
    = { "DeriveNone", "DeriveSource", "DeriveTarget" };
}

namespace mega
{

const char* DerivationDirection::str() const
{
    switch ( m_value )
    {
        case eDeriveNone:
        case eDeriveSource:
        case eDeriveTarget:
            return g_pszModes[ m_value ].c_str();
        case TOTAL_DERIVATION_MODES:
        default:
            THROW_RTE( "Invalid compilation mode" );
    }
}

DerivationDirection DerivationDirection::fromStr( const char* psz )
{
    std::array< std::string, TOTAL_DERIVATION_MODES >::const_iterator iFind
        = std::find( g_pszModes.begin(), g_pszModes.end(), psz );
    VERIFY_RTE_MSG( iFind != g_pszModes.end(), "Unknown derivation mode: " << psz );
    return DerivationDirection( static_cast< DerivationDirection::Value >( std::distance( g_pszModes.cbegin(), iFind ) ) );
}
} // namespace mega

std::ostream& operator<<( std::ostream& os, mega::DerivationDirection derivation ) { return os << derivation.str(); }
