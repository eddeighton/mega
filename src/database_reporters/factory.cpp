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

bool isCompilationReportType( const mega::reports::URL& url )
{
    auto reportType = mega::reports::getReportType( url );

    if( reportType.has_value() )
    {
        if( reportType.value() == "interface" )
        {
            return true;
        }
        else if( reportType.value() == "symbols" )
        {
            return true;
        }
    }
    return false;
}

mega::reports::Container generateCompilationReport( const mega::reports::URL& url, CompilationReportArgs args )
{
    if( mega::reporters::isCompilationReportType( url ) )
    {
        auto reportType = mega::reports::getReportType( url ).value();
        if( reportType == "symbols" )
        {
            mega::reporters::SymbolsReporter reporter( args.environment, args.database );
            return reporter.generate( url );
        }
        else if( reportType == "interface" )
        {
            mega::reporters::InterfaceReporter reporter( args.manifest, args.environment, args.database );
            return reporter.generate( url );
        }
    }
    return {};
}

} // namespace mega::reporters