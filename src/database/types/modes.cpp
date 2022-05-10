
#include "database/types/modes.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>

namespace mega
{
    std::array< std::string, TOTAL_COMPILATION_MODES > g_pszModes =
    {
        "Interface",
        "Operations",
        "Implementation",
        "CPP"
    };

    const char* toStr( CompilationMode mode )
    {
        switch( mode )
        {
            case eInterface:
            case eOperations:
            case eImplementation:
            case eCPP:
                return g_pszModes[ mode ].c_str();
            case TOTAL_COMPILATION_MODES:
            default:
                THROW_RTE( "Invalid compilation mode" );
        }
    }

    CompilationMode fromStr( const char* psz )
    {
        std::array< std::string, TOTAL_COMPILATION_MODES >::const_iterator iFind = 
            std::find( g_pszModes.begin(), g_pszModes.end(), psz );
        VERIFY_RTE_MSG( iFind != g_pszModes.end(), "Unknown compilation mode: " << psz );
        return static_cast< CompilationMode >( std::distance( g_pszModes.cbegin(), iFind ) );
    }

}