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
} // namespace FinalStage

namespace mega::reporters
{

mega::reports::Container InheritanceReporter::generate( const mega::reports::URL& url )
{
    using namespace FinalStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    Graph graph;

    std::map< TypeID, std::size_t >      objectIDs;
    std::multimap< TypeID, std::size_t > objectGroups;
    for( const mega::io::megaFilePath& sourceFilePath : m_args.manifest.getMegaSourceFiles() )
    {
        Database database( m_args.environment, sourceFilePath );

        for( Interface::IContext* pContext : database.many< Interface::IContext >( sourceFilePath ) )
        {
            if( pContext->get_is_meta_type() )
            {
                continue;
            }

            auto index = graph.m_nodes.size();
            VERIFY_RTE( objectIDs.insert( { pContext->get_interface_id(), index } ).second );

            std::optional< TypeID > parentType;
            for( auto pIter = pContext; pIter; pIter = db_cast< Interface::IContext >( pIter->get_parent() ) )
            {
                if( auto pObject = db_cast< Interface::Object >( pIter ) )
                {
                    parentType = pObject->get_interface_id();
                    break;
                }
            }

            if( parentType.has_value() )
            {
                objectGroups.insert( { parentType.value(), index } );
            }
            else
            {
                objectGroups.insert( { TypeID{}, index } );
            }

            graph.m_nodes.push_back(
                Graph::Node{ { { "Type: "s, Interface::printIContextFullType( pContext ) },
                               { "TypeID: "s, pContext->get_interface_id() } },
                             ( parentType == pContext->get_interface_id() ) ? Colour::lightblue : Colour::lightskyblue,
                             std::nullopt,
                             pContext->get_interface_id() } );
        }
    }

    // construct subgraphs for each object
    for( auto i = objectGroups.begin(), iEnd = objectGroups.end(); i != iEnd; )
    {
        auto            iNext = objectGroups.upper_bound( i->first );
        Graph::Subgraph subgraph;
        for( ; i != iNext; ++i )
        {
            subgraph.m_nodes.push_back( i->second );
        }
        graph.m_subgraphs.push_back( subgraph );
    }

    for( const mega::io::megaFilePath& sourceFilePath : m_args.manifest.getMegaSourceFiles() )
    {
        Database database( m_args.environment, sourceFilePath );

        for( Interface::IContext* pContext : database.many< Interface::IContext >( sourceFilePath ) )
        {
            if( pContext->get_is_meta_type() )
            {
                continue;
            }

            auto iFind = objectIDs.find( pContext->get_interface_id() );
            VERIFY_RTE( iFind != objectIDs.end() );
            const auto index = iFind->second;

            std::optional< Interface::InheritanceTrait* > inheritance;
            {
                if( auto pAbstract = db_cast< Interface::Abstract >( pContext ) )
                {
                    inheritance = pAbstract->get_inheritance_trait_opt();
                }
                else if( auto pState = db_cast< Interface::State >( pContext ) )
                {
                    inheritance = pState->get_inheritance_trait_opt();
                }
                else if( auto pEvent = db_cast< Interface::Event >( pContext ) )
                {
                    inheritance = pEvent->get_inheritance_trait_opt();
                }
                else if( auto pObject = db_cast< Interface::Object >( pContext ) )
                {
                    inheritance = pObject->get_inheritance_trait_opt();
                }
            }

            if( inheritance.has_value() )
            {
                for( auto pInherited : inheritance.value()->get_contexts() )
                {
                    if( pInherited->get_is_meta_type() )
                    {
                        continue;
                    }
                    auto jFind = objectIDs.find( pInherited->get_interface_id() );
                    VERIFY_RTE( jFind != objectIDs.end() );
                    graph.m_edges.push_back( Graph::Edge{ index, jFind->second, Colour::red } );
                }
            }

            if( auto pParent = db_cast< Interface::IContext >( pContext->get_parent() ) )
            {
                auto jFind = objectIDs.find( pParent->get_interface_id() );
                VERIFY_RTE( jFind != objectIDs.end() );
                graph.m_edges.push_back( Graph::Edge{ jFind->second, index, Colour::blue } );
            }
        }
    }

    return graph;
}

} // namespace mega::reporters
