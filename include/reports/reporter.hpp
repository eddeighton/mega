
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

#ifndef GUARD_2023_October_19_reporter
#define GUARD_2023_October_19_reporter

#include "value.hpp"
#include "reporter_id.hpp"
#include "url.hpp"
#include "report.hpp"

#include <string>
#include <optional>
#include <memory>

namespace mega::reports
{

class Reporter
{
public:
    using ID  = std::string;
    using Ptr = std::unique_ptr< Reporter >;

    virtual ~Reporter()                                             = default;
    virtual ReporterID                   getID()                    = 0;
    virtual std::optional< std::string > link( const Value& value ) = 0;
    virtual Container                    generate( const URL& url ) = 0;
};

} // namespace mega::reports

#endif // GUARD_2023_October_19_reporter
