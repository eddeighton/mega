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
#include "database/InterfaceStage.hxx"

#include "mega/values/service/url.hpp"
#include "mega/values/service/project.hpp"

#include "mega/common_strings.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace InterfaceStage
{
#include "compiler/interface.hpp"
#include "compiler/interface_printer.hpp"
} // namespace InterfaceStage

namespace mega::reporters
{

using namespace InterfaceStage;

namespace
{
    
void recurse( Interface::Aggregate* pNode, Branch& tree )
{
    using namespace InterfaceStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch branch{ { Interface::getKind( pNode ), " "s, Interface::getIdentifier( pNode ) } };
    tree.m_elements.emplace_back( std::move( branch ) );
}

void recurse( Interface::Node* pNode, Branch& tree );

void recurse( Interface::IContext* pNode, Branch& tree )
{
    using namespace InterfaceStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch branch{ { Interface::getKind( pNode ), " "s, Interface::getIdentifier( pNode ) } };

    for( auto pChild : pNode->get_children() )
    {
        recurse( pChild, branch );
    }

    tree.m_elements.emplace_back( std::move( branch ) );
}

void recurse( Interface::Node* pNode, Branch& tree )
{
    using namespace InterfaceStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    if( auto pAggregate = db_cast< Interface::Aggregate >( pNode ) )
    {
        recurse( pAggregate, tree );
    }
    else if( auto pIContext = db_cast< Interface::IContext >( pNode ) )
    {
        recurse( pIContext, tree );
    }
    else
    {
        THROW_RTE( "Unknown node type" );
    }
}

} // namespace

Report ASTReporter::generate( const report::URL& url )
{
    using namespace InterfaceStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch root{ { ID } };

    Database database( m_args.environment, m_args.environment.project_manifest(), true );

    Branch tree{ { m_args.manifest.getManifestFilePath().path() } };
    auto   pRoot = database.one< Interface::Root >( m_args.manifest.getManifestFilePath() );
    for( Interface::Node* pNode : pRoot->get_children() )
    {
        recurse( pNode, tree );
    }
    root.m_elements.push_back( { tree } );

    return root;
}

} // namespace mega::reporters
