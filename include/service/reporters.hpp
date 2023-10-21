
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

#ifndef GUARD_2023_October_19_reporters
#define GUARD_2023_October_19_reporters

#include "reports/report.hpp"
#include "reports/reporter.hpp"

#include "service/memory_manager.hpp"
#include "environment/mpo_database.hpp"

namespace mega::reports
{

class MemoryReporter : public mega::reports::Reporter
{
    mega::runtime::MemoryManager& m_memoryManager;
    runtime::MPODatabase& m_database;
public:
    MemoryReporter( mega::runtime::MemoryManager& memoryManager, runtime::MPODatabase& database );
    mega::reports::ReporterID    getID() override;
    std::optional< std::string > link( const mega::reports::Value& value ) override;
    mega::reports::Container     generate( const mega::reports::URL& url ) override;
};

} // namespace mega::reports

#endif // GUARD_2023_October_19_reporters
