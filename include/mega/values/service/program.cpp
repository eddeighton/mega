
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

#include "mega/values/service/program.hpp"

#include "common/string.hpp"
#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>

#include <iomanip>
#include <algorithm>

namespace mega::service
{

std::optional< Program > Program::parse( const std::string& str )
{
    const auto strings = common::simpleTokenise( str, "." );
    if( strings.size() == 2 )
    {
        Version::Value versionValue;
        try
        {
            std::istringstream is( strings.back() );
            is >> versionValue;
            return Program( Project( strings.front() ), Program::Version( versionValue ) );
        }
        catch( std::exception& )
        {
        }
    }
    return {};
}

std::optional< Program > Program::latest( const Project& project, const boost::filesystem::path& workBin )
{
    using namespace boost::filesystem;
    std::optional< Program > latest;
    VERIFY_RTE_MSG( boost::filesystem::exists( workBin ), "Failed to locate work bin at: " << workBin.string() );
    for( directory_iterator iter( workBin ); iter != directory_iterator(); ++iter )
    {
        boost::filesystem::path pth = *iter;
        if( is_regular_file( pth ) )
        {
            // pth.filename()
            if( auto programOpt = Program::parse( pth.filename().string() ) )
            {
                if( programOpt.value().project() == project )
                {
                    if( latest.has_value() )
                    {
                        latest = std::max( programOpt.value(), latest.value() );
                    }
                    else
                    {
                        latest = programOpt.value();
                    }
                }
            }
        }
    }
    return latest;
}

Program Program::next( const Project& project, const boost::filesystem::path& workBin )
{
    if( std::optional< Program > latestOpt = Program::latest( project, workBin ) )
    {
        return { project, latestOpt.value().version().next() };
    }
    else
    {
        return { project };
    }
}

std::ostream& operator<<( std::ostream& os, const Program& program )
{
    return os << program.project() << '.' << std::hex << std::setw( 8 ) << std::setfill( '0' )
              << program.version().value();
}

} // namespace mega::service