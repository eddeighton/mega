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

#ifndef FACTORY_24_OCT_2023
#define FACTORY_24_OCT_2023

#include "compilation_reporter_args.hpp"

#include "reports/report.hpp"
#include "reports/reporter_id.hpp"

#include "mega/values/service/url.hpp"

#include <optional>

namespace mega::reporters
{

bool isCompilationReportType( const mega::reports::URL& url );
void getDatabaseReporterIDs( std::vector< reports::ReporterID >& reportIDs );

std::optional< mega::reports::Container > generateCompilationReport( const mega::reports::URL& url, CompilationReportArgs args );

} // namespace mega::reporters

#endif // FACTORY_24_OCT_2023
