
#include "database/types/modes.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>

namespace
{
std::array< std::string, mega::CompilationMode::TOTAL_COMPILATION_MODES > g_pszModes
    = { "Interface", "Library", "Operations", "CPP" };
}

namespace mega
{
const char* CompilationMode::str() const
{
    switch ( m_value )
    {
        case eInterface:
        case eLibrary:
        case eOperations:
        case eCPP:
            return g_pszModes[ m_value ].c_str();
        case TOTAL_COMPILATION_MODES:
        default:
            THROW_RTE( "Invalid compilation mode" );
    }
}

CompilationMode CompilationMode::fromStr( const char* psz )
{
    std::array< std::string, TOTAL_COMPILATION_MODES >::const_iterator iFind
        = std::find( g_pszModes.begin(), g_pszModes.end(), psz );
    VERIFY_RTE_MSG( iFind != g_pszModes.end(), "Unknown compilation mode: " << psz );
    return CompilationMode( static_cast< CompilationMode::Value >( std::distance( g_pszModes.cbegin(), iFind ) ) );
}

} // namespace mega

std::ostream& operator<<( std::ostream& os, mega::CompilationMode compilationMode )
{
    return os << compilationMode.str();
}