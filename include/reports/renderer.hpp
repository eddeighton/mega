
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

#ifndef GUARD_2023_October_19_renderer
#define GUARD_2023_October_19_renderer

#include "colours.hxx"
#include "reporter_id.hpp"
#include "reporter.hpp"
#include "url.hpp"

#include <memory>
#include <map>

#include "common/assert_verify.hpp"

#include <boost/filesystem/path.hpp>

namespace mega::reports
{

class Renderer
{
public:
    using ReporterMap = std::map< ReporterID, Reporter::Ptr >;

    Renderer( const boost::filesystem::path& templateDir );
    ~Renderer();

    inline void registerReporter( Reporter::Ptr pReporter )
    {
        // NOTE: avoid attempting to use after move in error msg
        auto tempID = pReporter->getID();
        VERIFY_RTE_MSG(
            m_reporters.insert( { tempID, std::move( pReporter ) } ).second, "Duplicate reported with ID: " << tempID );
    }

    inline void generate( const URL& url, std::ostream& os )
    {
        auto iFind1 = m_reporters.find( url.reportID );
        VERIFY_RTE_MSG( iFind1 != m_reporters.end(), "Failed to locate reporter of type: " << url.reportID );

        Reporter& reporter = *iFind1->second;
        Container report   = reporter.generate( url );

        if( url.reporterLinkTarget.has_value() )
        {
            auto iFind2 = m_reporters.find( url.reporterLinkTarget.value() );
            VERIFY_RTE_MSG( iFind2 != m_reporters.end(), "Failed to locate reporter of type: " << url.reportID );
            Reporter& linker = *iFind2->second;
            renderHTML( report, linker, os );
        }
        else
        {
            renderHTML( report, os );
        }
    }

private:
    void renderHTML( const Container& report, std::ostream& os );
    void renderHTML( const Container& report, Reporter& linker, std::ostream& os );

    ReporterMap m_reporters;

    // how to put Inja into anonymous namespace ??
    // class Inja;
    // std::unique_ptr< Inja > m_pInja;
    void* m_pInja;
};

} // namespace mega::reports

#endif // GUARD_2023_October_19_renderer
