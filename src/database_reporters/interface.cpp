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
#include "compiler/interface.hpp"
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/concrete.hpp"
#include "compiler/symbol_variant_printer.hpp"
} // namespace FinalStage

namespace mega::reporters
{

using namespace FinalStage;

namespace
{

template < typename T >
reports::ValueVector fromConcrete( std::vector< T* > concrete )
{
    using namespace std::string_literals;
    reports::ValueVector result{ { "Concrete: "s } };
    for( auto pContext : concrete )
    {
        result.push_back( pContext->get_concrete_id() );
    }
    return result;
}

void addProperties( mega::reports::Branch& typeIDs, mega::reports::Branch& parentBranch,
                    reports::Branch& concreteTypeIDs, const std::vector< Interface::DimensionTrait* >& dimensions )
{
    using namespace FinalStage::Interface;
    using namespace std::string_literals;
    using namespace mega::reports;

    if( !dimensions.empty() )
    {
        Table tableInterfaceTypeIDs{ { "Data"s } };
        Table table{ { "Dimension"s, "Identifier"s, "Canon"s } };
        Table tableConcreteTypeIDs{ { "Data"s } };

        for( DimensionTrait* pDimension : dimensions )
        {
            tableInterfaceTypeIDs.m_rows.push_back( { Line{ pDimension->get_interface_id() } } );
            tableConcreteTypeIDs.m_rows.push_back( { Multiline{ fromConcrete( pDimension->get_concrete() ) } } );

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
        typeIDs.m_elements.push_back( std::move( tableInterfaceTypeIDs ) );
        parentBranch.m_elements.emplace_back( std::move( table ) );
        concreteTypeIDs.m_elements.push_back( std::move( tableConcreteTypeIDs ) );
    }
}

void addLinks( mega::reports::Branch& typeIDs, mega::reports::Branch& parentBranch, reports::Branch& concreteTypeIDs,
               const std::vector< Interface::LinkTrait* >& links )
{
    using namespace FinalStage::Interface;
    using namespace std::string_literals;
    using namespace mega::reports;

    if( !links.empty() )
    {
        Table tableInterfaceTypeIDs{ { "Data"s } };
        Table table{ { "Link"s, "Identifier"s, "Relation ID"s } };
        Table tableConcreteTypeIDs{ { "Data"s } };

        for( LinkTrait* pLink : links )
        {
            tableInterfaceTypeIDs.m_rows.push_back( { Line{ pLink->get_interface_id() } } );
            tableConcreteTypeIDs.m_rows.push_back( { Multiline{ fromConcrete( pLink->get_concrete() ) } } );

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
        typeIDs.m_elements.push_back( std::move( tableInterfaceTypeIDs ) );
        parentBranch.m_elements.emplace_back( std::move( table ) );
        concreteTypeIDs.m_elements.push_back( std::move( tableConcreteTypeIDs ) );
    }
}

void addInheritance( std::optional< Interface::InheritanceTrait* > inheritance, mega::reports::Branch& branch )
{
    using namespace FinalStage::Interface;
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
    using namespace FinalStage::Interface;
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
    using namespace FinalStage::Interface;
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
}

void recurse( reports::Branch& interfaceTypeIDs, reports::Branch& parentBranch, reports::Branch& concreteTypeIDs,
              Interface::IContext* pContext )
{
    using namespace FinalStage::Interface;
    using namespace std::string_literals;
    using namespace reports;

    Branch branch;
    branch.m_bookmark = pContext->get_interface_id();

    interfaceTypeIDs.m_elements.push_back( Line{ pContext->get_interface_id() } );
    concreteTypeIDs.m_elements.push_back( Multiline{ fromConcrete( pContext->get_concrete() ) } );

    if( auto pNamespace = db_cast< Namespace >( pContext ) )
    {
        branch.m_label = { { "Namespace "s, Interface::getIdentifier( pContext ) } };
        addProperties( interfaceTypeIDs, branch, concreteTypeIDs, pNamespace->get_dimension_traits() );
    }
    else if( auto pAbstract = db_cast< Abstract >( pContext ) )
    {
        branch.m_label
            = { { "Interface "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addInheritance( pAbstract->get_inheritance_trait_opt(), branch );
        addProperties( interfaceTypeIDs, branch, concreteTypeIDs, pAbstract->get_dimension_traits() );
        addLinks( interfaceTypeIDs, branch, concreteTypeIDs, pAbstract->get_link_traits() );
    }
    else if( auto pAction = db_cast< Action >( pContext ) )
    {
        branch.m_label
            = { { "Action "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addProperties( interfaceTypeIDs, branch, concreteTypeIDs, pAction->get_dimension_traits() );
        addLinks( interfaceTypeIDs, branch, concreteTypeIDs, pAction->get_link_traits() );
        addInheritance( pAction->get_inheritance_trait_opt(), branch );
        addTransition( pAction->get_transition_trait_opt(), branch );
    }
    else if( auto pComponent = db_cast< Component >( pContext ) )
    {
        branch.m_label
            = { { "Component "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addProperties( interfaceTypeIDs, branch, concreteTypeIDs, pComponent->get_dimension_traits() );
        addLinks( interfaceTypeIDs, branch, concreteTypeIDs, pComponent->get_link_traits() );
        addInheritance( pComponent->get_inheritance_trait_opt(), branch );
        addTransition( pComponent->get_transition_trait_opt(), branch );
    }
    else if( auto pState = db_cast< State >( pContext ) )
    {
        branch.m_label
            = { { "State "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addProperties( interfaceTypeIDs, branch, concreteTypeIDs, pState->get_dimension_traits() );
        addLinks( interfaceTypeIDs, branch, concreteTypeIDs, pState->get_link_traits() );
        addInheritance( pState->get_inheritance_trait_opt(), branch );
        addTransition( pState->get_transition_trait_opt(), branch );
    }
    else if( auto pEvent = db_cast< Event >( pContext ) )
    {
        branch.m_label
            = { { "Event "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addProperties( interfaceTypeIDs, branch, concreteTypeIDs, pEvent->get_dimension_traits() );
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
        addProperties( interfaceTypeIDs, branch, concreteTypeIDs, pObject->get_dimension_traits() );
        addLinks( interfaceTypeIDs, branch, concreteTypeIDs, pObject->get_link_traits() );
        addInheritance( pObject->get_inheritance_trait_opt(), branch );
    }
    else
    {
        THROW_RTE( "Unknown context type" );
    }

    for( Interface::IContext* pChildContext : pContext->get_children() )
    {
        recurse( interfaceTypeIDs, branch, concreteTypeIDs, pChildContext );
    }

    parentBranch.m_elements.emplace_back( std::move( branch ) );
}
} // namespace

mega::reports::Container InterfaceReporter::generate( const mega::reports::URL& url )
{
    using namespace FinalStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    Table root{ { "Interface TypeID"s, ID, "Concrete TypeIDs"s } };

    for( const mega::io::megaFilePath& sourceFilePath : m_args.manifest.getMegaSourceFiles() )
    {
        Database database( m_args.environment, sourceFilePath );

        Branch interfaceTypeIDs( { { sourceFilePath.path() } } );
        Branch fileBranch( { { sourceFilePath.path() } } );
        Branch concreteTypeIDs( { { sourceFilePath.path() } } );
        for( Interface::Root* pRoot : database.many< Interface::Root >( sourceFilePath ) )
        {
            for( Interface::IContext* pContext : pRoot->get_children() )
            {
                recurse( interfaceTypeIDs, fileBranch, concreteTypeIDs, pContext );
            }
        }
        root.m_rows.push_back( { interfaceTypeIDs, fileBranch, concreteTypeIDs } );
    }

    return root;
}

} // namespace mega::reporters
