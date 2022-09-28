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


#include "database/types/component_type.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>
#include <array>

namespace
{
std::array< std::string, mega::ComponentType::TOTAL_COMPONENT_TYPES > g_pszModes = { "INTERFACE", "LIBRARY" };
}

namespace mega
{
const char* ComponentType::str() const
{
    switch ( m_value )
    {
        case eInterface:
        case eLibrary:
            return g_pszModes[ m_value ].c_str();
        case TOTAL_COMPONENT_TYPES:
        default:
            THROW_RTE( "Invalid component type" );
    }
}

ComponentType ComponentType::fromStr( const char* psz )
{
    std::array< std::string, TOTAL_COMPONENT_TYPES >::const_iterator iFind
        = std::find( g_pszModes.begin(), g_pszModes.end(), psz );
    VERIFY_RTE_MSG( iFind != g_pszModes.end(), "Unknown component type: " << psz );
    return ComponentType( static_cast< ComponentType::Value >( std::distance( g_pszModes.cbegin(), iFind ) ) );
}

} // namespace mega

std::ostream& operator<<( std::ostream& os, mega::ComponentType componentType ) { return os << componentType.str(); }