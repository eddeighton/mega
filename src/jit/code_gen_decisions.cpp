
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

#include "code_generator.hpp"

#include "database/FinalStage.hxx"

#include "symbol_utils.hpp"

namespace mega::runtime
{
using namespace FinalStage;

std::string recurseStep( Inja& inja, Indent& indent, const TypeID deciderTypeID, Decision::DecisionProcedure* pDecision,
                         Decision::Step* pStep )
{
    std::ostringstream os;

    auto optDecider = pStep->get_decider();

    if( auto pAssignments = db_cast< Decision::Assignments >( pStep ) )
    {
        VERIFY_RTE( !optDecider.has_value() );

        static const char* szTemplate =
            R"TEMPLATE(
{% for assignment in assignments %}
{% if assignment.is_true %}
{{ indent }}mega::mangle::bitset_set( pBitset, {{ assignment.bit_offset }} + 
{{ indent }}    ( ( ref.getInstance() / {{ assignment.divider }} ) * {{ assignment.multiplier }} ), {{ assignment.multiplier }} );
{% else %}
{{ indent }}mega::mangle::bitset_unset( pBitset, {{ assignment.bit_offset }} + 
{{ indent }}    ( ( ref.getInstance() / {{ assignment.divider }} ) * {{ assignment.multiplier }} ), {{ assignment.multiplier }} );
{% endif %}
{% endfor %}
{{ indent }}
)TEMPLATE";

        nlohmann::json templateData( {

            { "indent", indent.str() }, { "assignments", nlohmann::json::array() }

        } );

        for( auto pAssignment : pAssignments->get_assignments() )
        {
            auto optBit = pAssignment->get_vertex()->get_bitset_range_start();
            VERIFY_RTE( optBit.has_value() );

            nlohmann::json assignment( {

                { "is_true", pAssignment->get_is_true() },
                { "bit_offset", optBit.value() },
                { "divider", pDecision->get_instance_divider() },
                { "multiplier", pAssignment->get_instance_multiplier() }

            } );
            templateData[ "assignments" ].push_back( assignment );
        }

        os << inja.render( szTemplate, templateData );
    }
    else if( auto pBoolean = db_cast< Decision::Boolean >( pStep ) )
    {
        if( optDecider.has_value() )
        {
            auto pDecider = optDecider.value();

            static const char* szTemplate =
                R"TEMPLATE(
{{ indent }} static thread_local mega::runtime::object::CallGetter function( g_pszModuleName, {{ decider_interface_type_id }} );
{{ indent }} using Boolean = I32 (*)();
{{ indent }} const Boolean pDeciderFunction = (const Boolean)( function() );
{{ indent }} if( pDeciderFunction() )
{{ indent }} {
{{ indent }}     {{ body_true }}
{{ indent }} }
{{ indent }} else
{{ indent }} {
{{ indent }}     {{ body_false }}
{{ indent }} }
)TEMPLATE";
            auto children = pBoolean->get_children();
            VERIFY_RTE( children.size() == 2 );
            auto pTrueStep  = children[ 0 ];
            auto pFalseStep = children[ 1 ];

            nlohmann::json templateData( {

                { "indent", indent.str() },
                { "decider_type_id", printTypeID( deciderTypeID ) },
                { "decider_interface_type_id", printTypeID( pDecider->get_interface_decider()->get_interface_id() ) },
                { "body_true", "" },
                { "body_false", "" }

            } );

            ++indent;
            templateData[ "body_true" ]  = recurseStep( inja, indent, deciderTypeID, pDecision, pTrueStep );
            templateData[ "body_false" ] = recurseStep( inja, indent, deciderTypeID, pDecision, pFalseStep );
            --indent;

            os << inja.render( szTemplate, templateData );
        }
        else
        {
            static const char* szTemplate =
                R"TEMPLATE(
{{ indent }}{{ body }}
)TEMPLATE";

            auto children = pStep->get_children();
            VERIFY_RTE( children.size() == 1 );

            nlohmann::json templateData( {

                { "indent", indent.str() },
                { "body", recurseStep( inja, indent, deciderTypeID, pDecision, children.front() ) }

            } );

            os << inja.render( szTemplate, templateData );
        }
    }
    else if( auto pSelection = db_cast< Decision::Selection >( pStep ) )
    {
        if( optDecider.has_value() )
        {
            auto pDecider = optDecider.value();

            static const char* szTemplate =
                R"TEMPLATE(
{{ indent }} static thread_local mega::runtime::object::CallGetter function( g_pszModuleName, {{ decider_interface_type_id }} );
{{ indent }} using Selector = I32 (*)();
{{ indent }} const Selector pDeciderFunction = (const Selector)( function() );
{{ indent }} switch( pDeciderFunction() )
{{ indent }} {
{% for result in results %}
{{ indent }}    case {{ result.type_id }}:
{{ indent }}    {
{{ indent }}{{ result.body }}
{{ indent }}    }
{{ indent }}    break;
{% endfor %}
{{ indent }}    default:
{{ indent }}    {
{{ indent }}        throw mega::runtime::JITException{ "decider_{{ decider_type_id }} decider return value did not match any type" };
{{ indent }}    }
{{ indent }}    break;
{{ indent }} }
)TEMPLATE";

            nlohmann::json templateData( {

                { "indent", indent.str() },
                { "decider_type_id", printTypeID( deciderTypeID ) },
                { "decider_interface_type_id", printTypeID( pDecider->get_interface_decider()->get_interface_id() ) },
                { "results", nlohmann::json::array() }

            } );

            auto alternatives = pSelection->get_variable_alternatives();
            auto ordering     = pSelection->get_variable_ordering();

            ++indent;
            int iVar = 0;
            for( auto pNestedStep : pStep->get_children() )
            {
                VERIFY_RTE( alternatives.size() > iVar );
                const Automata::Vertex* pVar = alternatives[ iVar ];

                nlohmann::json result( {

                    { "type_id", printTypeID( pVar->get_context()->get_interface()->get_interface_id() ) },
                    { "body", recurseStep( inja, indent, deciderTypeID, pDecision, pNestedStep ) }

                } );

                templateData[ "results" ].push_back( result );

                ++iVar;
            }
            --indent;

            os << inja.render( szTemplate, templateData );
        }
        else
        {
            auto alternatives = pSelection->get_variable_alternatives();
            VERIFY_RTE( alternatives.size() == 1 );

            static const char* szTemplate =
                R"TEMPLATE(
{{ indent }}{{ body }}
)TEMPLATE";

            auto children = pStep->get_children();
            VERIFY_RTE( children.size() == 1 );

            nlohmann::json templateData( {

                { "indent", indent.str() },
                { "body", recurseStep( inja, indent, deciderTypeID, pDecision, children.front() ) }

            } );

            os << inja.render( szTemplate, templateData );
        }
    }
    else
    {
        THROW_RTE( "Unknown step type" );
    }

    return os.str();
}

void CodeGenerator::generate_decision( const LLVMCompiler& compiler, const JITDatabase& database, TypeID concreteTypeID,
                                       std::ostream& os )
{
    // clang-format off
static const char* szTemplate =
R"TEMPLATE(

#include "mega/values/native_types.hpp"
#include "mega/values/runtime/reference.hpp"
#include "jit/object_functions.hxx"
#include "jit/jit_exception.hpp"

static const char* g_pszModuleName = "decider_{{ type_id }}";

namespace mega::mangle
{
    void bitset_set( void*, mega::U32, mega::U32 );
    void bitset_unset( void*, mega::U32, mega::U32 );
}

void decision_{{ type_id }}( const mega::reference& ref )
{
    void* pBitset = reinterpret_cast< char* >( ref.getHeap() ) + {{ bitset_offset }};

{{ body }}
}

)TEMPLATE";
    // clang-format on

    FinalStage::Concrete::Object* pObject   = database.getObject( TypeID::make_object_from_typeID( concreteTypeID ) );
    Decision::DecisionProcedure*  pDecision = database.getDecision( concreteTypeID );

    std::ostringstream osCPPCode;
    try
    {
        Indent indent;
        ++indent;

        nlohmann::json templateData( {

            { "type_id", printTypeID( concreteTypeID ) },
            { "bitset_offset",
              pObject->get_activation()->get_part()->get_offset() + pObject->get_activation()->get_offset() },
            { "body", recurseStep( *m_pInja, indent, concreteTypeID, pDecision, pDecision->get_root() ) }

        } );

        osCPPCode << m_pInja->render( szTemplate, templateData );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate_decision: {} for {}", ex.what(), concreteTypeID );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate_decision: " << ex.what() );
    }
    std::ostringstream osModule;
    osModule << "decision_" << printTypeID( concreteTypeID );
    compiler.compileToLLVMIR( osModule.str(), osCPPCode.str(), os, std::nullopt );
}

} // namespace mega::runtime
