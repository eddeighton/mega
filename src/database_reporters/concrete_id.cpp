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
#include "database/ConcreteTypeAnalysis.hxx"

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

namespace ConcreteTypeAnalysis
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/interface.hpp"
// #include "compiler/concrete.hpp"
} // namespace ConcreteTypeAnalysis

namespace mega::reporters
{
using namespace ConcreteTypeAnalysis;

mega::reports::Container ConcreteIDReporter::generate( const mega::reports::URL& url )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch branch{ { ID } };

    Database database( m_args.environment, m_args.environment.project_manifest(), true );

    auto pSymbolTable = database.one< Symbols::SymbolTable >( m_args.environment.project_manifest() );

    {
        Table concrete{ { "Concrete Type ID"s, "Interface Type ID"s, "Full Type Name"s } };
        const auto concreteTypeIDs = pSymbolTable->get_concrete_type_ids();
        for( const auto& [ typeID, pConcreteID ] : concreteTypeIDs )
        {
            auto pNode = pConcreteID->get_node();

            // clang-format off
            concrete.m_rows.push_back( ContainerVector{
                Line{ typeID, std::nullopt, typeID }
                , Line{ pNode->get_node()->get_interface_id()->get_type_id() }
                , Line{ Concrete::fullTypeName( pNode ) }
                }

            );
            // clang-format on
        }
        branch.m_elements.push_back( std::move( concrete ) );
    }

    return branch;
}
} // namespace mega::reporters
