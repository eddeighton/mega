
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
