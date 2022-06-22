
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