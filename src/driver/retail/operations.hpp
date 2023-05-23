
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

#ifndef GUARD_2023_May_23_operations
#define GUARD_2023_May_23_operations

#include "driver/retail/interface.hpp"

namespace driver::retail
{

template < typename T >
nlohmann::json generateBlock( T* pNode, mega::U64 blockID, std::ostream& osBody )
{
    using namespace FinalStage;
    std::ostringstream os;
    {
        for( auto pNode : pNode->get_nodes() )
        {
            if( auto pLit = db_cast< Automata::Literal >( pNode ) )
            {
                os << pLit->get_value();
                osBody << pLit->get_value();
            }
        }
    }
    return nlohmann::json( { { "id", blockID }, { "body", os.str() } } );
}

void generateAutomataRecurse( FinalStage::Automata::Block* pBlock, nlohmann::json& data, std::ostream& osBody )
{
    using namespace FinalStage;

    data[ "blocks" ].push_back( generateBlock( pBlock, pBlock->get_id(), osBody ) );

    for( auto pNode : pBlock->get_nodes() )
    {
        if( auto pNestedBlock = db_cast< Automata::Block >( pNode ) )
        {
            generateAutomataRecurse( pNestedBlock, data, osBody );
        }
    }
}

void recurseOperations( FinalStage::Interface::IContext* pContext,
                        nlohmann::json&                  data,
                        CleverUtility::IDList&           namespaces,
                        CleverUtility::IDList&           types )
{
    using namespace FinalStage;
    using namespace FinalStage::Interface;

    if( auto pNamespace = db_cast< Namespace >( pContext ) )
    {
        /*if ( pNamespace->get_is_global() )
        {
            CleverUtility c( namespaces, pNamespace->get_identifier() );
            for ( IContext* pNestedContext : pNamespace->get_children() )
            {
                recurseOperations( pNestedContext, data, namespaces, types );
            }
        }
        else*/
        {
            CleverUtility c( types, pNamespace->get_identifier() );
            for( IContext* pNestedContext : pNamespace->get_children() )
            {
                recurseOperations( pNestedContext, data, namespaces, types );
            }
        }
    }
    else if( auto pAbstract = db_cast< Abstract >( pContext ) )
    {
        CleverUtility c( types, pAbstract->get_identifier() );
        for( IContext* pNestedContext : pAbstract->get_children() )
        {
            recurseOperations( pNestedContext, data, namespaces, types );
        }
    }
    else if( Interface::Action* pAction = db_cast< Interface::Action >( pContext ) )
    {
        CleverUtility c( types, pAction->get_identifier() );

        if( auto pStartState = db_cast< Automata::Start >( pAction ) )
        {
            nlohmann::json operation( {
                
                { "automata", true },
                { "first_level_deep", true },
                { "return_type", "mega::ActionCoroutine" },
                { "has_args", true },
                { "has_args_explicit", false },
                { "body", "" },
                { "has_namespaces", !namespaces.empty() },
                { "namespaces", namespaces },
                { "types", types },
                { "params_string", "mega::U64 _blockID" },
                { "blocks", nlohmann::json::array() }

            } );

            {
                nlohmann::json param( { { "type", "mega::U64" }, { "name", "_blockID" } } );
                operation[ "params" ].push_back( param );
            }

            std::ostringstream osBody;
            generateAutomataRecurse( pStartState->get_sequence(), operation, osBody );
            operation[ "hash" ] = common::Hash{ osBody.str() }.toHexString();

            data[ "operations" ].push_back( operation );
        }
        else
        {
            std::ostringstream osBody;
            for( auto pDef : pAction->get_action_defs() )
            {
                if( !pDef->get_body().empty() )
                {
                    osBody << pDef->get_body();
                    break;
                }
            }

            osBody << "\nco_return mega::done();";

            nlohmann::json operation( {

                { "automata", false },
                { "first_level_deep", true },
                { "return_type", "mega::ActionCoroutine" },
                { "has_args", false },
                { "has_args_explicit", false },
                { "body", osBody.str() },
                { "has_namespaces", !namespaces.empty() },
                { "namespaces", namespaces },
                { "types", types },
                { "params_string", "" }

            } );

            data[ "operations" ].push_back( operation );
        }

        for( IContext* pNestedContext : pAction->get_children() )
        {
            recurseOperations( pNestedContext, data, namespaces, types );
        }
    }
    else if( auto pEvent = db_cast< Event >( pContext ) )
    {
    }
    else if( auto pFunction = db_cast< Function >( pContext ) )
    {
        CleverUtility c( types, pFunction->get_identifier() );

        std::string strBody;
        {
            for( auto pDef : pFunction->get_function_defs() )
            {
                if( !pDef->get_body().empty() )
                {
                    strBody = pDef->get_body();
                    break;
                }
            }
        }

        nlohmann::json operation( {

            { "automata", false },
            { "first_level_deep", true },
            { "return_type", pFunction->get_return_type_trait()->get_str() },
            { "has_args", true },
            { "has_args_explicit", true },
            { "body", strBody },
            { "has_namespaces", !namespaces.empty() },
            { "namespaces", namespaces },
            { "types", types },
            { "params_string", pFunction->get_arguments_trait()->get_str() }

        } );

        data[ "operations" ].push_back( operation );
    }
    else if( auto pObject = db_cast< Object >( pContext ) )
    {
        CleverUtility c( types, pObject->get_identifier() );
        for( IContext* pNestedContext : pObject->get_children() )
        {
            recurseOperations( pNestedContext, data, namespaces, types );
        }
    }
    else if( auto pLink = db_cast< Link >( pContext ) )
    {
        CleverUtility c( types, pLink->get_identifier() );
        for( IContext* pNestedContext : pLink->get_children() )
        {
            recurseOperations( pNestedContext, data, namespaces, types );
        }
    }
    else if( auto pBuffer = db_cast< Buffer >( pContext ) )
    {
    }
    else
    {
        THROW_RTE( "Unknown context type" );
    }
}
}

#endif //GUARD_2023_May_23_operations
