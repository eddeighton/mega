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

#include "mega/values/compilation/modes.hpp"

#include "common/assert_verify.hpp"

#include <algorithm>
#include <array>

namespace mega
{
namespace
{
constexpr std::array< std::string, CompilationMode::TOTAL_COMPILATION_MODES > g_pszModes
    = { "Normal", "Traits", "Interface", "Library", "Operations", "CPP" };
} // namespace

const char* CompilationMode::str() const
{
    static_assert( g_pszModes.size() == mega::CompilationMode::TOTAL_COMPILATION_MODES,
                   "CompilationMode strings are out of date!" );
    switch( m_value )
    {
        case eNormal:
        case eTraits:
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
    auto iFind = std::find_if(
        g_pszModes.cbegin(), g_pszModes.cend(), [ psz ]( const std::string& str ) { return str == psz; } );
    VERIFY_RTE_MSG( iFind != g_pszModes.end(), "Unknown compilation mode: " << psz );
    return CompilationMode( static_cast< CompilationMode::Value >( std::distance( g_pszModes.cbegin(), iFind ) ) );
}

std::ostream& operator<<( std::ostream& os, mega::CompilationMode compilationMode )
{
    return os << compilationMode.str();
}

} // namespace mega
