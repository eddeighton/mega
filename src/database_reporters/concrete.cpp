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

namespace ConcreteStage
{
#include "compiler/interface.hpp"
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
} // namespace ConcreteStage

namespace mega::reporters
{

using namespace ConcreteStage;

namespace
{

void recurse( Concrete::Node* pNode, mega::reports::Branch& tree )
{
    using namespace ConcreteStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch branch;

    branch.m_label = { { Interface::getKind( pNode->get_node() ), ": "s, Concrete::getIdentifier( pNode ) } };

    if( auto pContext = db_cast< Concrete::Context >( pNode ) )
    {
        branch.m_label.push_back( { pContext->get_icontext()->get_flags() } );
    }

    // if( auto pContext = db_cast< Interface::IContext >( pNode->get_node() ) )
    // {
    //     branch.m_label.push_back( { pContext->get_flags() } );
    // }

    for( auto pChild : pNode->get_children() )
    {
        recurse( pChild, branch );
    }
    tree.m_elements.emplace_back( std::move( branch ) );
}

} // namespace

mega::reports::Container ConcreteReporter::generate( const mega::reports::URL& url )
{
    using namespace ConcreteStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch root{ { ID } };

    Database database( m_args.environment, m_args.environment.project_manifest(), true );

    Branch tree{ { m_args.manifest.getManifestFilePath().path() } };

    auto pRoot = database.one< Concrete::Root >( m_args.manifest.getManifestFilePath() );

    for( Concrete::Node* pNode : pRoot->get_children() )
    {
        recurse( pNode, tree );
    }
    root.m_elements.push_back( { tree } );

    return root;
}

} // namespace mega::reporters
