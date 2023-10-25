
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

#include "reporter_id.hpp"
#include "linker.hpp"
#include "report.hpp"

#include "common/serialisation.hpp"

#include <boost/filesystem/path.hpp>

#include <ostream>
#include <vector>

namespace mega::reports
{

class HTMLRenderer
{
public:
    struct JavascriptShortcuts
    {
        friend class boost::serialization::access;
        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& boost::serialization::make_nvp( "shortcuts", m_shortcuts );
        }

    public:
        struct Shortcut
        {
            std::string strAction;
            char        key;

            using Vector = std::vector< Shortcut >;
        };

        void add( Shortcut shortcut ) { m_shortcuts.emplace_back( std::move( shortcut ) ); }
        const Shortcut::Vector& get() const { return m_shortcuts; }

    private:
        Shortcut::Vector m_shortcuts;
    };

    HTMLRenderer( const boost::filesystem::path& templateDir, JavascriptShortcuts shortcuts, bool bClearTempFiles );
    ~HTMLRenderer();

    using ReporterIDs = std::vector< reports::ReporterID >;

    void render( const Container& report, std::ostream& os );
    void render( const Container& report, Linker& linker, std::ostream& os );

private:
    void*               m_pInja;
    JavascriptShortcuts m_shortcuts;
};

} // namespace mega::reports

#endif // GUARD_2023_October_19_renderer
