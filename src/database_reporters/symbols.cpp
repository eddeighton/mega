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

#include "reporters.hpp"

#include "database_reporters/factory.hpp"

#include "environment/environment_archive.hpp"
#include "database/SymbolRollout.hxx"

#include "reports/renderer_html.hpp"

#include "mega/values/service/url.hpp"
#include "mega/values/service/project.hpp"

#include "mega/common_strings.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace SymbolRollout
{
#include "compiler/interface_printer.hpp"
// #include "compiler/concrete_printer.hpp"
#include "compiler/interface.hpp"
} // namespace SymbolRollout

namespace mega::reporters
{
using namespace SymbolRollout;

mega::reports::Container SymbolsReporter::generate( const mega::reports::URL& url )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch branch{ { ID } };

    Database database( m_args.environment, m_args.environment.project_manifest() );

    auto pSymbolTable = database.one< Symbols::SymbolTable >( m_args.environment.project_manifest() );

    {
        Table symbols{ { "Symbol ID"s, "Token"s } };
        for( auto [ symbolID, pSymbolID ] : pSymbolTable->get_symbol_ids() )
        {
            symbols.m_rows.push_back( ContainerVector{
                Line{ symbolID, std::nullopt, symbolID }, Line{ pSymbolID->get_token() }

            } );
        }
        branch.m_elements.push_back( std::move( symbols ) );
    }

    return branch;
}

mega::reports::Container InterfaceTypeIDReporter::generate( const mega::reports::URL& url )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    Database database( m_args.environment, m_args.environment.project_manifest() );

    Branch branch{ { ID } };

    auto pSymbolTable = database.one< Symbols::SymbolTable >( m_args.environment.project_manifest() );

    {
        Table interface {
            {
                "Interface Type ID"s, "Full Type Name"s
            }
        };
        for( auto [ typeID, pInterfaceTypeID ] : pSymbolTable->get_interface_type_ids() )
        {
            auto pNode = pInterfaceTypeID->get_node();

            // clang-format off
            interface.m_rows.push_back( ContainerVector{
                Line{ typeID, std::nullopt, typeID }, 
                Line{ Interface::fullTypeName( pNode ) }

            } );
        }
        branch.m_elements.push_back( std::move( interface ) );
    }

    return branch;
}

} // namespace mega::reporters
