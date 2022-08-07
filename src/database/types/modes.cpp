
#include "database/types/modes.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>

namespace
{
constexpr std::array g_pszModes = { "Normal", "Interface", "Library", "Operations", "CPP" };
} // namespace

namespace mega
{
const char* CompilationMode::str() const
{
    static_assert( g_pszModes.size() == mega::CompilationMode::TOTAL_COMPILATION_MODES,
                   "CompilationMode strings are out of date!" );
    switch ( m_value )
    {
        case eNormal:
        case eInterface:
        case eLibrary:
        case eOperations:
        case eCPP:
            return g_pszModes[ m_value ];
        case TOTAL_COMPILATION_MODES:
        default:
            THROW_RTE( "Invalid compilation mode" );
    }
}

CompilationMode CompilationMode::fromStr( const char* psz )
{
    auto iFind = std::find_if(
        g_pszModes.cbegin(), g_pszModes.cend(), [ psz ]( const char* p ) { return 0 == strcmp( psz, p ); } );
    VERIFY_RTE_MSG( iFind != g_pszModes.end(), "Unknown compilation mode: " << psz );
    return CompilationMode( static_cast< CompilationMode::Value >( std::distance( g_pszModes.cbegin(), iFind ) ) );
}

} // namespace mega

std::ostream& operator<<( std::ostream& os, mega::CompilationMode compilationMode )
{
    return os << compilationMode.str();
}