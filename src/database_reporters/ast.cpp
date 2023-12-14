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
/*
void addProperties( const std::vector< Interface::DimensionTrait* >& dimensions, mega::reports::Branch& branch )
{
    using namespace InterfaceStage::Interface;
    using namespace std::string_literals;
    using namespace mega::reports;

    if( !dimensions.empty() )
    {
        Table table{ { "Dimension"s, "Identifier"s, "Canon"s } };

        for( DimensionTrait* pDimension : dimensions )
        {
            if( auto pUser = db_cast< UserDimensionTrait >( pDimension ) )
            {
                table.m_rows.push_back( { Line{ "User "s }, Line{ getIdentifier( pDimension ) },
                                          Line{ pUser->get_canonical_type() }, Line{ pUser->get_mangled_name() } } );
            }
            else if( auto pCompiler = db_cast< CompilerDimensionTrait >( pDimension ) )
            {
                table.m_rows.push_back( { Line{ "Comp "s }, Line{ getIdentifier( pDimension ) },
                                          Line{ std::string{ mega::psz_bitset } },
                                          Line{ pCompiler->get_mangled_name() } } );
            }
            else
            {
                THROW_RTE( "Unknown dimension trait type" );
            }
        }
        branch.m_elements.emplace_back( std::move( table ) );
    }
}

void addLinks( const std::vector< Interface::LinkTrait* >& links, mega::reports::Branch& branch )
{
    using namespace InterfaceStage::Interface;
    using namespace std::string_literals;
    using namespace mega::reports;

    if( !links.empty() )
    {
        Table table{ { "Link"s, "Identifier"s, "Relation ID"s } };

        for( LinkTrait* pLink : links )
        {
            auto pRelation = pLink->get_relation();

            if( auto pUser = db_cast< UserLinkTrait >( pLink ) )
            {
                table.m_rows.push_back(
                    { Line{ "User "s }, Line{ getIdentifier( pLink ) }, Line{ pRelation->get_id() } } );
            }
            else if( auto pOwner = db_cast< OwnershipLinkTrait >( pLink ) )
            {
                table.m_rows.push_back(
                    { Line{ "Owner "s }, Line{ getIdentifier( pLink ) }, Line{ pRelation->get_id() } } );
            }
            else
            {
                THROW_RTE( "Unknown link trait type" );
            }
        }
        branch.m_elements.emplace_back( std::move( table ) );
    }
}

void addInheritance( std::optional< Interface::InheritanceTrait* > inheritance, mega::reports::Branch& branch )
{
    using namespace InterfaceStage::Interface;
    using namespace std::string_literals;
    using namespace mega::reports;
    if( inheritance.has_value() )
    {
        bool bFirst = true;
        for( auto pInherited : inheritance.value()->get_contexts() )
        {
            if( bFirst )
            {
                bFirst = false;
                branch.m_label.push_back( ":"s );
            }
            else
            {
                branch.m_label.push_back( ","s );
            }
            branch.m_label.push_back( Interface::printIContextFullType( pInherited ) );
        }
    }
}

void addEvent( Interface::EventTypeTrait* pEventTrait, mega::reports::Branch& branch )
{
    using namespace InterfaceStage::Interface;
    using namespace std::string_literals;
    using namespace reports;

    std::ostringstream os;
    os << "(" << pEventTrait->get_named_symbol_variant_path_sequence().str() << ")";
    // Interface::printSymbolVariantSequences( pEventTrait->get_symbol_variant_sequences(), os );
    branch.m_label.push_back( os.str() );
}

void addTransition( std::optional< Interface::TransitionTypeTrait* > pTransitionTraitOpt,
                    mega::reports::Branch&                           branch )
{
    using namespace InterfaceStage::Interface;
    using namespace std::string_literals;
    using namespace reports;

    if( pTransitionTraitOpt.has_value() )
    {
        auto pTransitionTrait = pTransitionTraitOpt.value();

        std::ostringstream os;

        if( pTransitionTrait->get_is_successor() )
        {
            os << " > ";
        }
        else if( pTransitionTrait->get_is_predecessor() )
        {
            os << " < ";
        }
        else
        {
            THROW_RTE( "Unknown transition type" );
        }

        os << pTransitionTrait->get_symbol_variant_path_sequence().str();

        branch.m_label.push_back( os.str() );
    }
}*/

/*
void recurse( Interface::Node* pNode, reports::Branch& tree )
{
    using namespace InterfaceStage::Interface;
    using namespace std::string_literals;
    using namespace reports;

    Branch branch;
    branch.m_bookmark = pContext->get_interface_id();

    if( auto pNamespace = db_cast< Namespace >( pContext ) )
    {
        branch.m_label = { { "Namespace "s, Interface::getIdentifier( pContext ) } };
        addProperties( pNamespace->get_dimension_traits(), branch );
    }
    else if( auto pAbstract = db_cast< Abstract >( pContext ) )
    {
        branch.m_label
            = { { "Interface "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addInheritance( pAbstract->get_inheritance_trait_opt(), branch );
        addProperties( pAbstract->get_dimension_traits(), branch );
        addLinks( pAbstract->get_link_traits(), branch );
    }
    else if( auto pAction = db_cast< Action >( pContext ) )
    {
        branch.m_label
            = { { "Action "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addProperties( pAction->get_dimension_traits(), branch );
        addLinks( pAction->get_link_traits(), branch );
        addInheritance( pAction->get_inheritance_trait_opt(), branch );
        addTransition( pAction->get_transition_trait_opt(), branch );
    }
    else if( auto pComponent = db_cast< Component >( pContext ) )
    {
        branch.m_label
            = { { "Component "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addProperties( pComponent->get_dimension_traits(), branch );
        addLinks( pComponent->get_link_traits(), branch );
        addInheritance( pComponent->get_inheritance_trait_opt(), branch );
        addTransition( pComponent->get_transition_trait_opt(), branch );
    }
    else if( auto pState = db_cast< State >( pContext ) )
    {
        branch.m_label
            = { { "State "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addProperties( pState->get_dimension_traits(), branch );
        addLinks( pState->get_link_traits(), branch );
        addInheritance( pState->get_inheritance_trait_opt(), branch );
        addTransition( pState->get_transition_trait_opt(), branch );
    }
    else if( auto pEvent = db_cast< Event >( pContext ) )
    {
        branch.m_label
            = { { "Event "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addProperties( pEvent->get_dimension_traits(), branch );
        addInheritance( pEvent->get_inheritance_trait_opt(), branch );
    }
    else if( auto pInterupt = db_cast< Interupt >( pContext ) )
    {
        branch.m_label = { { "Interupt "s, Interface::getIdentifier( pContext ) } };
        if( auto eventOpt = pInterupt->get_events_trait_opt() )
        {
            addEvent( eventOpt.value(), branch );
        }
        addTransition( pInterupt->get_transition_trait_opt(), branch );
    }
    else if( auto pDecider = db_cast< Decider >( pContext ) )
    {
        branch.m_label = { { "Decider "s, Interface::getIdentifier( pContext ) } };
        addEvent( pDecider->get_events_trait(), branch );
    }
    else if( auto pFunction = db_cast< Function >( pContext ) )
    {
        branch.m_label = { { "Function "s, Interface::getIdentifier( pContext ) } };
        branch.m_label.push_back( "("s );
        bool bFirst = true;
        for( const auto& arg : pFunction->get_arguments_trait()->get_arguments() )
        {
            if( bFirst )
            {
                bFirst = false;
            }
            else
            {
                branch.m_label.push_back( ","s );
            }
            branch.m_label.push_back( arg.str() );
        }
        branch.m_label.push_back( ") : "s );
        branch.m_label.push_back( pFunction->get_return_type_trait()->get_str() );
    }
    else if( auto pObject = db_cast< Object >( pContext ) )
    {
        branch.m_label
            = { { "Object "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addProperties( pObject->get_dimension_traits(), branch );
        addLinks( pObject->get_link_traits(), branch );
        addInheritance( pObject->get_inheritance_trait_opt(), branch );
    }
    else
    {
        THROW_RTE( "Unknown context type" );
    }

    for( Interface::IContext* pChildContext : pContext->get_children() )
    {
        recurse( pChildContext, branch );
    }

    tree.m_elements.emplace_back( std::move( branch ) );
}*/

void recurse( Interface::Aggregate* pNode, reports::Branch& tree )
{
    using namespace InterfaceStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch branch;
    if( auto pParsedAggregate = db_cast< Interface::ParsedAggregate >( pNode ) )
    {
        auto pParserAggregate = pParsedAggregate->get_aggregate();

        if( auto pDimension = db_cast< Parser::Dimension >( pParserAggregate ) )
        {
            branch.m_label = { { "Dim "s, Interface::getIdentifier( pNode ) } };
        }
        else if( auto pLink = db_cast< Parser::Link >( pParserAggregate ) )
        {
            branch.m_label = { { "Link "s, Interface::getIdentifier( pNode ) } };
        }
        else if( auto pAlias = db_cast< Parser::Alias >( pParserAggregate ) )
        {
            branch.m_label = { { "Alias "s, Interface::getIdentifier( pNode ) } };
        }
        else if( auto pUsing = db_cast< Parser::Using >( pParserAggregate ) )
        {
            branch.m_label = { { "Using "s, Interface::getIdentifier( pNode ) } };
        }
        else
        {
            THROW_RTE( "Unknown ParsedAggregate type" );
        }
    }
    else if( auto pGeneratedAggregate = db_cast< Interface::GeneratedAggregate >( pNode ) )
    {
        THROW_TODO;
    }
    else
    {
        THROW_RTE( "Unknown aggregate type" );
    }
    tree.m_elements.emplace_back( std::move( branch ) );
}

void recurse( Interface::Node* pNode, reports::Branch& tree );

void recurse( Interface::IContext* pNode, reports::Branch& tree )
{
    using namespace InterfaceStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch branch;

    if( auto pNamespace = db_cast< Interface::Namespace >( pNode ) )
    {
        branch.m_label = { { "Namespace "s, Interface::getIdentifier( pNode ) } };
    }
    else if( auto pAbstract = db_cast< Interface::Abstract >( pNode ) )
    {
        branch.m_label = { { "Interface "s, Interface::getIdentifier( pNode ) } };
    }
    else if( auto pEvent = db_cast< Interface::Event >( pNode ) )
    {
        branch.m_label = { { "Event "s, Interface::getIdentifier( pNode ) } };
    }
    else if( auto pObject = db_cast< Interface::Object >( pNode ) )
    {
        branch.m_label = { { "Object "s, Interface::getIdentifier( pNode ) } };
    }
    else if( auto pInterupt = db_cast< Interface::Interupt >( pNode ) )
    {
        branch.m_label = { { "Interupt "s, Interface::getIdentifier( pNode ) } };
    }
    else if( auto pRequirement = db_cast< Interface::Requirement >( pNode ) )
    {
        branch.m_label = { { "Requirement "s, Interface::getIdentifier( pNode ) } };
    }
    else if( auto pDecider = db_cast< Interface::Decider >( pNode ) )
    {
        branch.m_label = { { "Decider "s, Interface::getIdentifier( pNode ) } };
    }
    else if( auto pFunction = db_cast< Interface::Function >( pNode ) )
    {
        branch.m_label = { { "Function "s, Interface::getIdentifier( pNode ) } };
    }
    else if( auto pAction = db_cast< Interface::Action >( pNode ) )
    {
        branch.m_label = { { "Action "s, Interface::getIdentifier( pNode ) } };
    }
    else if( auto pComponent = db_cast< Interface::Component >( pNode ) )
    {
        branch.m_label = { { "Component "s, Interface::getIdentifier( pNode ) } };
    }
    else if( auto pState = db_cast< Interface::State >( pNode ) )
    {
        branch.m_label = { { "State "s, Interface::getIdentifier( pNode ) } };
    }
    else
    {
        branch.m_label = { { "IContext "s, Interface::getIdentifier( pNode ) } };
    }

    for( auto pChild : pNode->get_children() )
    {
        recurse( pChild, branch );
    }

    tree.m_elements.emplace_back( std::move( branch ) );
}


void recurse( Interface::Node* pNode, reports::Branch& tree )
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

mega::reports::Container ASTReporter::generate( const mega::reports::URL& url )
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
