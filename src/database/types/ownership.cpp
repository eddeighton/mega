
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
            THROW_RTE( "Invalid compilation mode" );
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
