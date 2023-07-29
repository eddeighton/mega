
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

#ifndef GUARD_2023_May_23_interface
#define GUARD_2023_May_23_interface

#include "driver/retail/invocations.hpp"

namespace driver::retail
{

template < typename TContextType >
static std::vector< nlohmann::json >
getDimensionTraits( CleverUtility::IDList& typenames, TContextType* pContext,
                    const std::vector< FinalStage::Interface::DimensionTrait* >& dimensionTraits )
{
    using namespace FinalStage;
    using namespace FinalStage::Interface;

    std::vector< nlohmann::json > traits;
    for( DimensionTrait* pDimensionTrait : dimensionTraits )
    {
        const std::string& strType = pDimensionTrait->get_type();

        std::ostringstream osName;
        osName << pDimensionTrait->get_id()->get_str();

        nlohmann::json traitNames = typenames;
        traitNames.push_back( osName.str() );

        nlohmann::json trait_struct(
            { { "name", osName.str() }, { "types", traitNames }, { "traits", nlohmann::json::array() } } );

        {
            {
                std::ostringstream osTrait;
                osTrait << "using Type = " << strType;
                trait_struct[ "traits" ].push_back( osTrait.str() );
            }
            {
                std::ostringstream osTrait;
                osTrait << "using Read = mega::DimensionTraits< " << strType << " >::Read";
                trait_struct[ "traits" ].push_back( osTrait.str() );
            }
            {
                std::ostringstream osTrait;
                osTrait << "using Write = mega::DimensionTraits< " << strType << " >::Write";
                trait_struct[ "traits" ].push_back( osTrait.str() );
            }
            {
                std::ostringstream osTrait;
                osTrait << "using Erased = mega::DimensionTraits< " << strType << " >::Erased";
                trait_struct[ "traits" ].push_back( osTrait.str() );
            }
        }
        traits.push_back( trait_struct );
    }

    return traits;
}

void recurseInterface( const InvocationInfo& invocationInfo, FinalStage::Symbols::SymbolTable* pSymbolTable,
                       TemplateEngine& templateEngine, CleverUtility::IDList& namespaces, CleverUtility::IDList& types,
                       FinalStage::Interface::IContext* pContext, std::ostream& os, nlohmann::json& interfaceOperations,
                       nlohmann::json& traitStructs )
{
    using namespace FinalStage;

    CleverUtility c( types, pContext->get_identifier() );

    std::ostringstream osNested;
    for( Interface::IContext* pChildContext : pContext->get_children() )
    {
        recurseInterface( invocationInfo, pSymbolTable, templateEngine, namespaces, types, pChildContext, osNested,
                          interfaceOperations, traitStructs );
    }

    nlohmann::json contextInvocations = nlohmann::json::array();
    {
        for( const auto& [ _, pInvocation ] : invocationInfo.contextInvocations[ pContext ] )
        {
            nlohmann::json invocation = { { "name", generateInvocationName( invocationInfo, pInvocation ) },
                                          { "result_type_id", invocationInfo.generateResultTypeID( pInvocation ) }

            };
            contextInvocations.push_back( invocation );
        }
    }

    nlohmann::json context( {

        { "name", pContext->get_identifier() },
        { "typeid", toHex( pContext->get_interface_id() ) },
        { "has_operation", false },
        { "operation_params_string", "" },
        { "invocations", contextInvocations },
        { "nested", osNested.str() },
        { "trait_structs", nlohmann::json::array() }

    } );

    if( Interface::Namespace* pNamespace = db_cast< Interface::Namespace >( pContext ) )
    {
        for( const nlohmann::json& trait : getDimensionTraits( types, pNamespace, pNamespace->get_dimension_traits() ) )
        {
            context[ "trait_structs" ].push_back( trait );
            traitStructs.push_back( trait );
        }
    }
    else if( Interface::Abstract* pAbstract = db_cast< Interface::Abstract >( pContext ) )
    {
        for( const nlohmann::json& trait : getDimensionTraits( types, pAbstract, pAbstract->get_dimension_traits() ) )
        {
            context[ "trait_structs" ].push_back( trait );
            traitStructs.push_back( trait );
        }
    }
    else if( Interface::Action* pAction = db_cast< Interface::Action >( pContext ) )
    {
        for( const nlohmann::json& trait : getDimensionTraits( types, pAction, pAction->get_dimension_traits() ) )
        {
            context[ "trait_structs" ].push_back( trait );
            traitStructs.push_back( trait );
        }

        context[ "has_operation" ] = true;

        if( auto pStartState = db_cast< Meta::Automata >( pAction ) )
        {
            context[ "operation_return_type" ]   = "mega::ActionCoroutine";
            context[ "operation_params_string" ] = "mega::U64 _blockID";
        }
        else
        {
            context[ "operation_return_type" ] = "mega::ActionCoroutine";
        }
    }
    else if( Interface::Event* pEvent = db_cast< Interface::Event >( pContext ) )
    {
        for( const nlohmann::json& trait : getDimensionTraits( types, pEvent, pEvent->get_dimension_traits() ) )
        {
            context[ "trait_structs" ].push_back( trait );
            traitStructs.push_back( trait );
        }
    }
    else if( Interface::Function* pFunction = db_cast< Interface::Function >( pContext ) )
    {
        context[ "has_operation" ]           = true;
        context[ "operation_return_type" ]   = pFunction->get_return_type_trait()->get_str();
        context[ "operation_params_string" ] = pFunction->get_arguments_trait()->get_str();
    }
    else if( Interface::Object* pObject = db_cast< Interface::Object >( pContext ) )
    {
        for( const nlohmann::json& trait : getDimensionTraits( types, pObject, pObject->get_dimension_traits() ) )
        {
            context[ "trait_structs" ].push_back( trait );
            traitStructs.push_back( trait );
        }
    }
    else if( Interface::Link* pLink = db_cast< Interface::Link >( pContext ) )
    {
    }

    templateEngine.renderContext( context, os );
}

} // namespace driver::retail

#endif // GUARD_2023_May_23_interface
