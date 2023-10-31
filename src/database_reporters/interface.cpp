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
} // namespace FinalStage

namespace mega::reporters
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

void InterfaceReporter::addProperties( mega::reports::Branch& typeIDs, mega::reports::Branch& parentBranch,
                                       reports::Branch&                                             concreteTypeIDs,
                                       const std::vector< FinalStage::Interface::DimensionTrait* >& dimensions )
{
    using namespace FinalStage;
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
                table.m_rows.push_back(
                    { Line{ "User "s }, Line{ getIdentifier( pDimension ) }, Line{ pUser->get_canonical_type() } } );
            }
            else if( auto pCompiler = db_cast< CompilerDimensionTrait >( pDimension ) )
            {
                table.m_rows.push_back( { Line{ "Comp "s }, Line{ getIdentifier( pDimension ) },
                                          Line{ std::string{ mega::psz_bitset } } } );
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

void InterfaceReporter::addInheritance( std::optional< FinalStage::Interface::InheritanceTrait* > inheritance,
                                        mega::reports::Branch&                                    branch )
{
    using namespace FinalStage;
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

void InterfaceReporter::recurse( reports::Branch& interfaceTypeIDs, reports::Branch& parentBranch,
                                 reports::Branch& concreteTypeIDs, FinalStage::Interface::IContext* pContext )
{
    using namespace FinalStage;
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
        addInheritance( pAbstract->get_inheritance_trait(), branch );

        addProperties( interfaceTypeIDs, branch, concreteTypeIDs, pAbstract->get_dimension_traits() );

        // pAbstract->get_concrete()
        // pAbstract->get_link_traits()
    }
    else if( auto pAction = db_cast< Action >( pContext ) )
    {
        branch.m_label
            = { { "Action "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addProperties( interfaceTypeIDs, branch, concreteTypeIDs, pAction->get_dimension_traits() );
        addInheritance( pAction->get_inheritance_trait(), branch );

        // addTransition( pAction->get_transition_trait(), node );
    }
    else if( auto pComponent = db_cast< Component >( pContext ) )
    {
        branch.m_label
            = { { "Component "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addProperties( interfaceTypeIDs, branch, concreteTypeIDs, pComponent->get_dimension_traits() );
        addInheritance( pComponent->get_inheritance_trait(), branch );
        // addTransition( pComponent->get_transition_trait(), node );
    }
    else if( auto pState = db_cast< State >( pContext ) )
    {
        branch.m_label
            = { { "State "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addProperties( interfaceTypeIDs, branch, concreteTypeIDs, pState->get_dimension_traits() );
        addInheritance( pState->get_inheritance_trait(), branch );

        // addTransition( pState->get_transition_trait(), node );
    }
    else if( auto pEvent = db_cast< Event >( pContext ) )
    {
        branch.m_label
            = { { "Event "s, Interface::getIdentifier( pContext ), "["s, getLocalDomainSize( pContext ), "]"s } };
        addProperties( interfaceTypeIDs, branch, concreteTypeIDs, pEvent->get_dimension_traits() );
        addInheritance( pEvent->get_inheritance_trait(), branch );
    }
    else if( auto pInterupt = db_cast< Interupt >( pContext ) )
    {
        branch.m_label = { { "Interupt "s, Interface::getIdentifier( pContext ) } };

        // addTransition( pInterupt->get_transition_trait(), node );
        // addEvent( pInterupt->get_events_trait(), node );
    }
    else if( auto pDecider = db_cast< Decider >( pContext ) )
    {
        branch.m_label = { { "Decider "s, Interface::getIdentifier( pContext ) } };

        // addTransition( pInterupt->get_transition_trait(), node );
        // addEvent( pInterupt->get_events_trait(), node );
    }
    else if( auto pFunction = db_cast< Function >( pContext ) )
    {
        branch.m_label = { { "Function "s, Interface::getIdentifier( pContext ) } };
        branch.m_label.push_back( "("s );
        bool bFirst = true;
        for( const auto& arg : pFunction->get_arguments_trait()->get_args() )
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
        addInheritance( pObject->get_inheritance_trait(), branch );
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
