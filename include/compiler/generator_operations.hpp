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

// NOTE to use do something like this:

// namespace DerivationAnalysisView
// {
// using namespace DerivationAnalysisView::Interface;
// #include "generator_operations.hpp"
// } // namespace DerivationAnalysisView

#include "compiler/generator_utility.hpp"

class OperationsGen
{
public:
    class TemplateEngine
    {
        const mega::io::Environment&      m_environment;
        ::inja::Environment&              m_injaEnvironment;
        ::inja::Template                  m_operationsTemplate;
        std::optional< ::inja::Template > m_operationsExternsTemplate;

    public:
        template < typename EnvironmentType >
        TemplateEngine( const EnvironmentType& environment, ::inja::Environment& injaEnv )
            : m_environment( environment )
            , m_injaEnvironment( injaEnv )
            , m_operationsTemplate( m_injaEnvironment.parse_template( environment.OperationsTemplate().string() ) )
            , m_operationsExternsTemplate(
                  m_injaEnvironment.parse_template( environment.OperationsExternsTemplate().string() ) )

        {
        }

        TemplateEngine( const mega::io::Environment& environment, ::inja::Environment& injaEnv,
                        const boost::filesystem::path& operationsTemplatePath )
            : m_environment( environment )
            , m_injaEnvironment( injaEnv )
            , m_operationsTemplate( m_injaEnvironment.parse_template( operationsTemplatePath.string() ) )
        {
        }

        void renderOperations( const nlohmann::json& data, std::ostream& os ) const
        {
            m_injaEnvironment.render_to( os, m_operationsTemplate, data );
        }

        void renderOperationsExterns( const nlohmann::json& data, std::ostream& os ) const
        {
            VERIFY_RTE( m_operationsExternsTemplate.has_value() );
            m_injaEnvironment.render_to( os, m_operationsExternsTemplate.value(), data );
        }
    };

private:
    template < typename T >
    static nlohmann::json generateBlock( T* pNode, mega::U64 blockID, std::ostream& osBody )
    {
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

    static void generateAutomataRecurse( Automata::Block* pBlock, nlohmann::json& data, std::ostream& osBody )
    {
        data[ "blocks" ].push_back( generateBlock( pBlock, pBlock->get_id(), osBody ) );

        for( auto pNode : pBlock->get_nodes() )
        {
            if( auto pNestedBlock = db_cast< Automata::Block >( pNode ) )
            {
                generateAutomataRecurse( pNestedBlock, data, osBody );
            }
        }
    }

    static void recurse( IContext* pContext, nlohmann::json& data, CleverUtility::IDList& namespaces,
                         CleverUtility::IDList& types )
    {
        if( auto pNamespace = db_cast< Namespace >( pContext ) )
        {
            /*if ( pNamespace->get_is_global() )
            {
                CleverUtility c( namespaces, pNamespace->get_identifier() );
                for ( IContext* pNestedContext : pNamespace->get_children() )
                {
                    recurse( pNestedContext, data, namespaces, types );
                }
            }
            else*/
            {
                CleverUtility c( types, pNamespace->get_identifier() );
                for( IContext* pNestedContext : pNamespace->get_children() )
                {
                    recurse( pNestedContext, data, namespaces, types );
                }
            }
        }
        else if( auto pAbstract = db_cast< Abstract >( pContext ) )
        {
            CleverUtility c( types, pAbstract->get_identifier() );
            for( IContext* pNestedContext : pAbstract->get_children() )
            {
                recurse( pNestedContext, data, namespaces, types );
            }
        }
        else if( Interface::Action* pAction = db_cast< Interface::Action >( pContext ) )
        {
            CleverUtility c( types, pAction->get_identifier() );

            if( auto pStartState = db_cast< Automata::Start >( pAction ) )
            {
                nlohmann::json operation( {

                    { "automata", true },
                    { "return_type", "mega::ActionCoroutine" },
                    { "body", "" },
                    { "hash", "" },
                    { "typeID", pAction->get_interface_id().getSymbolID() },
                    { "has_namespaces", !namespaces.empty() },
                    { "namespaces", namespaces },
                    { "types", types },
                    { "params_string", "mega::U64 _blockID" },
                    { "params", nlohmann::json::array() },
                    { "blocks", nlohmann::json::array() } } );

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
                    { "return_type", "mega::ActionCoroutine" },
                    { "body", osBody.str() },
                    { "hash", common::Hash{ osBody.str() }.toHexString() },
                    { "typeID", pAction->get_interface_id().getSymbolID() },
                    { "has_namespaces", !namespaces.empty() },
                    { "namespaces", namespaces },
                    { "types", types },
                    { "params_string", "" },
                    { "params", nlohmann::json::array() } } );

                data[ "operations" ].push_back( operation );
            }

            for( IContext* pNestedContext : pAction->get_children() )
            {
                recurse( pNestedContext, data, namespaces, types );
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
                { "return_type", pFunction->get_return_type_trait()->get_str() },
                { "body", strBody },
                { "hash", common::Hash{ strBody }.toHexString() },
                { "typeID", pFunction->get_interface_id().getSymbolID() },
                { "has_namespaces", !namespaces.empty() },
                { "namespaces", namespaces },
                { "types", types },
                { "params_string", pFunction->get_arguments_trait()->get_str() },
                { "params", nlohmann::json::array() } } );
            {
                int iParamCounter = 1;
                for( const std::string& strParamType : pFunction->get_arguments_trait()->get_canonical_types() )
                {
                    std::ostringstream osParamName;
                    osParamName << "p_" << iParamCounter++;
                    nlohmann::json param( { { "type", strParamType }, { "name", osParamName.str() } } );
                    operation[ "params" ].push_back( param );
                }
            }

            data[ "operations" ].push_back( operation );
        }
        else if( auto pObject = db_cast< Object >( pContext ) )
        {
            CleverUtility c( types, pObject->get_identifier() );
            for( IContext* pNestedContext : pObject->get_children() )
            {
                recurse( pNestedContext, data, namespaces, types );
            }
        }
        else if( auto pLink = db_cast< Link >( pContext ) )
        {
            CleverUtility c( types, pLink->get_identifier() );
            for( IContext* pNestedContext : pLink->get_children() )
            {
                recurse( pNestedContext, data, namespaces, types );
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

public:
    static std::string generate( TemplateEngine& templateEngine, Interface::Root* pRoot, bool bRenderExterns )
    {
        nlohmann::json data( { { "operations", nlohmann::json::array() } } );

        CleverUtility::IDList namespaces, types;
        for( IContext* pContext : pRoot->get_children() )
        {
            recurse( pContext, data, namespaces, types );
        }

        std::ostringstream os;
        templateEngine.renderOperations( data, os );
        if( bRenderExterns )
        {
            templateEngine.renderOperationsExterns( data, os );
        }
        return os.str();
    }
};