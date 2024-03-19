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
#include "database/ConcreteStage.hxx"

#include "report/url.hpp"
#include "mega/values/service/project.hpp"

#include "mega/common_strings.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace ConcreteStage
{
#include "compiler/interface.hpp"
#include "compiler/interface_printer.hpp"
#include "compiler/concrete.hpp"
#include "compiler/concrete_printer.hpp"
} // namespace ConcreteStage

namespace mega::reporters
{

using namespace ConcreteStage;

namespace
{

struct TableTree
{
    Branch concreteKind{ { { ConcreteReporter::ID } } };
    Branch concreteTree{ { { ConcreteReporter::ID } } };
    Branch directRealiser{ { { ConcreteReporter::ID } } };
    Branch interfaceType{ { { ConcreteReporter::ID } } };
    Branch interfaceTypeID{ { { ConcreteReporter::ID } } };
    Branch flags{ { { ConcreteReporter::ID } } };
};

const std::string& bool_to_string( bool b )
{
    using namespace std::string_literals;
    static const std::string strTrue  = "True"s;
    static const std::string strFalse = "False"s;
    return b ? strTrue : strFalse;
}

void recurse( Concrete::Node* pNode, TableTree& table, Branch& tree )
{
    using namespace ConcreteStage;
    using namespace std::string_literals;

    Colour color = Colour::blue;
    if( auto pContext = db_cast< Concrete::Context >( pNode ) )
    {
        table.flags.m_elements.push_back( Line{ pContext->get_icontext()->get_flags() } );
        color = Colour::blue;
    }
    else
    {
        table.flags.m_elements.push_back( Line{ " "s } );
        color = Colour::lightgreen;
    }

    if( pNode->get_is_direct_realiser() )
    {
        VERIFY_RTE( pNode->get_node_opt().has_value() );
        VERIFY_RTE( pNode->get_node_opt().value()->get_direct_realiser_opt().has_value() );
        VERIFY_RTE( pNode->get_node_opt().value()->get_direct_realiser_opt().value() == pNode );
    }

    if( auto interfaceNodeOpt = pNode->get_node_opt() )
    {
        auto pINode = interfaceNodeOpt.value();
        table.interfaceType.m_elements.push_back( Line{ Interface::fullTypeName( pINode ) } );
        table.interfaceTypeID.m_elements.push_back( Line{ pINode->get_interface_id()->get_type_id() } );
    }
    else
    {
        table.interfaceType.m_elements.push_back( Line{ " "s } );
        table.interfaceTypeID.m_elements.push_back( Line{ interface::NULL_TYPE_ID } );
    }

    table.concreteKind.m_elements.push_back( Line{ Concrete::getKind( pNode ), std::nullopt, std::nullopt, color } );
    table.directRealiser.m_elements.push_back( Line{ bool_to_string( pNode->get_is_direct_realiser() ) } );

    Branch branch{ { Concrete::getIdentifier( pNode ) } };
    for( auto pChild : pNode->get_children() )
    {
        recurse( pChild, table, branch );
    }
    tree.m_elements.emplace_back( std::move( branch ) );
}

} // namespace

Report ConcreteReporter::generate( const URL&  )
{
    using namespace ConcreteStage;
    using namespace std::string_literals;

    Table root{ { "Kind"s, ID, "Direct Realiser"s, "Interface Type"s, "Interface Type ID"s, "Flags"s } };

    TableTree tree;

    Database database( m_args.environment, m_args.environment.project_manifest(), true );

    auto pRoot = database.one< Concrete::Root >( m_args.manifest.getManifestFilePath() );

    for( Concrete::Node* pNode : pRoot->get_children() )
    {
        recurse( pNode, tree, tree.concreteTree );
    }
    root.m_rows.push_back( { tree.concreteKind, tree.concreteTree, tree.directRealiser, tree.interfaceType,
                             tree.interfaceTypeID, tree.flags } );

    return root;
}

} // namespace mega::reporters
