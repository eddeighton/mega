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
#include "database/FinalStage.hxx"

#include "reports/renderer_html.hpp"

#include "mega/values/service/url.hpp"
#include "mega/values/service/project.hpp"
#include "mega/values/compilation/type_id.hpp"

#include "mega/mangle/traits.hpp"
#include "mega/common_strings.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace FinalStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/interface.hpp"

namespace Concrete
{
using ContextVector = std::vector< const Context* >;
static ContextVector getSubTypes( const Concrete::Graph::Vertex* pVertex )
{
    auto pContext       = db_cast< const Context >( pVertex );
    using ContextVector = std::vector< const Context* >;
    ContextVector path;
    while( pContext )
    {
        if( db_cast< const Object >( pContext ) )
        {
            break;
        }
        path.push_back( pContext );
        pContext = db_cast< const Context >( pContext->get_parent() );
    }
    std::reverse( path.begin(), path.end() );
    return path;
}
} // namespace Concrete

} // namespace FinalStage

namespace mega::reporters
{
using namespace FinalStage;

mega::reports::Container TruthTableReporter::generate( const mega::reports::URL& url )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch branch{ { ID } };

    for( const mega::io::megaFilePath& sourceFilePath : m_args.manifest.getMegaSourceFiles() )
    {
        Branch sourceBranch{ { sourceFilePath.path() } };

        Database database( m_args.environment, sourceFilePath );

        for( Concrete::Object* pObject : database.many< Concrete::Object >( sourceFilePath ) )
        {
            auto variables = pObject->get_variable_vertices();
            auto table     = pObject->get_truth_table();

            Table truthTable;
            {
                {
                    std::vector< Concrete::ContextVector > headings;
                    std::size_t                            szLargest = 0;
                    for( auto pVar : variables )
                    {
                        headings.push_back( Concrete::getSubTypes( pVar->get_context() ) );
                        szLargest = std::max( headings.back().size(), szLargest );
                    }

                    // add cascading header rows
                    for( std::size_t sz = 0; sz != szLargest; ++sz )
                    {
                        ContainerVector row;
                        for( const auto& c : headings )
                        {
                            if( c.size() > sz )
                            {
                                row.push_back( Line{ Concrete::getIdentifier( c[ sz ] ) } );
                            }
                            else
                            {
                                row.push_back( Line{} );
                            }
                        }
                        truthTable.m_rows.push_back( row );
                    }
                }

                for( auto pTruthAssignment : table )
                {
                    ContainerVector row;
                    {
                        auto trueVertex = pTruthAssignment->get_true_vertices();
                        for( auto pVar : variables )
                        {
                            if( std::find( trueVertex.begin(), trueVertex.end(), pVar ) != trueVertex.end() )
                            {
                                row.push_back(
                                    Line{ "T"s, std::nullopt, std::nullopt, Colour::black, Colour::lightgreen } );
                            }
                            else
                            {
                                row.push_back(
                                    Line{ "F"s, std::nullopt, std::nullopt, Colour::black, Colour::lightgray } );
                            }
                        }
                    }
                    truthTable.m_rows.push_back( row );
                }
            }

            Branch branch{ { Concrete::printContextFullType( pObject ), " Total: "s, std::to_string( table.size() ) } };
            branch.m_elements.push_back( truthTable );
            sourceBranch.m_elements.push_back( branch );
        }

        branch.m_elements.push_back( sourceBranch );
    }

    return branch;
}

} // namespace mega::reporters
