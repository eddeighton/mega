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

#ifndef REPORTERS_24_OCT_2023
#define REPORTERS_24_OCT_2023

#include "database_reporters/compilation_reporter_args.hpp"

#include "mega/reports.hpp"

#include "report/reporter_id.hpp"
#include "report/url.hpp"

namespace mega::reporters
{

#define REPORTER( reportname )                                              \
    struct reportname##Reporter                                             \
    {                                                                       \
        CompilationReportArgs           m_args;                             \
        static const report::ReporterID ID;                                 \
        Report                          generate( const URL& url ); \
    };
#include "reporters.hxx"
#undef REPORTER

} // namespace mega::reporters

#endif // REPORTERS_24_OCT_2023
