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

#include "database_reporters/factory.hpp"

#include "reporters.hpp"

namespace mega::reporters
{

// generate static ReporterID strings for each type
#define REPORTER( reportname ) const mega::reports::ReporterID reportname##Reporter::ID = #reportname;
#include "reporters.hxx"
#undef REPORTER

bool isCompilationReportType( const mega::reports::URL& url )
{
    static std::set< std::string > reportedIDMap;
    if( reportedIDMap.empty() )
    {
#define REPORTER( reportname ) \
    VERIFY_RTE_MSG( reportedIDMap.insert( #reportname ).second, "Invalid reported first char" );
#include "reporters.hxx"
#undef REPORTER
    }

    auto reportType = mega::reports::getReportType( url );

    if( reportType.has_value() )
    {
        return reportedIDMap.contains( reportType.value() );
    }
    return false;
}

void getDatabaseReporterIDs( std::vector< reports::ReporterID >& reportIDs )
{
#define REPORTER( reportname ) reportIDs.push_back( #reportname );
#include "reporters.hxx"
#undef REPORTER
}

mega::reports::Container generateCompilationReport( const mega::reports::URL& url, CompilationReportArgs args )
{
    if( mega::reporters::isCompilationReportType( url ) )
    {
        auto reportType = mega::reports::getReportType( url ).value();
        if( reportType.empty() )
        {
            return {};
        }
#define REPORTER( reportname )                           \
    else if( reportType == #reportname )                 \
    {                                                    \
        mega::reporters::reportname##Reporter r{ args }; \
        return r.generate( url );                        \
    }
#include "reporters.hxx"
#undef REPORTER
    }
    return {};
}

} // namespace mega::reporters