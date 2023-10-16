
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

#include "symbol_utils.hpp"

#include "mega/common_strings.hpp"

namespace FinalStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
} // namespace FinalStage

namespace mega::runtime
{

struct Args
{
    const JITDatabase&                database;
    const CodeGenerator::VariableMap& variables;
    FunctionDeclarations&             functions;
    nlohmann::json&                   data;
    Indent&                           indent;
    Inja&                             inja;

    static std::string get( const CodeGenerator::VariableMap&                   variables,
                            const FinalStage::Invocations::Variables::Variable* pVar )
    {
        auto iFind = variables.find( pVar );
        VERIFY_RTE( iFind != variables.end() );
        return iFind->second;
    }
    std::string get( const FinalStage::Invocations::Variables::Variable* pVar )
    {
        auto iFind = variables.find( pVar );
        VERIFY_RTE( iFind != variables.end() );
        return iFind->second;
    }
};

namespace
{

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ParentDerivation
void gen( Args args, const FinalStage::Invocations::Instructions::ParentDerivation* pParentDerivation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    std::ostringstream os;
    os << args.indent << "// ParentDerivation\n";

    const Variables::Variable* pFrom      = pParentDerivation->get_from();
    const Variables::Stack*    pTo        = pParentDerivation->get_to();
    const Concrete::Context*   pToContext = db_cast< Concrete::Context >( pTo->get_concrete() );

    const std::string  s           = args.get( pFrom );
    const mega::TypeID targetType  = pToContext->get_concrete_id();
    const mega::U64    szLocalSize = pToContext->get_local_size();

    os << args.indent << args.get( pTo ) << " = mega::reference::make( " << args.get( pFrom )
       << ", mega::TypeInstance{ " << printTypeID( targetType ) << ", "
       << "static_cast< mega::Instance >( " << s << ".getInstance() / " << szLocalSize << " ) } );\n";

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ChildDerivation
void gen( Args args, const FinalStage::Invocations::Instructions::ChildDerivation* pChildDerivation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    std::ostringstream os;
    os << args.indent << "// ChildDerivation\n";

    const Variables::Variable* pFrom = pChildDerivation->get_from();
    const Variables::Stack*    pTo   = pChildDerivation->get_to();

    mega::TypeID targetType;
    {
        if( auto pContext = db_cast< Concrete::Context >( pTo->get_concrete() ) )
        {
            targetType = pContext->get_concrete_id();
        }
        else if( auto pDim = db_cast< Concrete::Dimensions::User >( pTo->get_concrete() ) )
        {
            targetType = pDim->get_concrete_id();
        }
        else if( auto pLink = db_cast< Concrete::Dimensions::Link >( pTo->get_concrete() ) )
        {
            targetType = pLink->get_concrete_id();
        }
        else
        {
            THROW_RTE( "Unknown child derivation target" );
        }
    }

    os << args.indent << args.get( pTo ) << " = mega::reference::make( " << args.get( pFrom )
       << ", mega::TypeInstance{ " << printTypeID( targetType ) << ", " << args.get( pFrom ) << ".getInstance() } );\n";

    args.data[ "assignments" ].push_back( os.str() );
}

//

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Dereference
void gen( Args args, const FinalStage::Invocations::Instructions::Dereference* pDereference )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    if( {{ instance }}.getMPO() != mega::runtime::getThisMPO() )
{{ indent }}    {
{{ indent }}        mega::runtime::readLock( {{ instance }} );
{{ indent }}    }
{{ indent }}    else if( {{ instance }}.isNetworkAddress() )
{{ indent }}    {
{{ indent }}        mega::runtime::networkToHeap( {{ instance }} );
{{ indent }}    }
{{ indent }}    {{ ref }} = *reinterpret_cast< mega::reference* >( reinterpret_cast< char* >( {{ instance }}.getHeap() )
{{ indent }}        + {{ part_offset }} + ( {{ part_size }} * {{ instance }}.getInstance() )
{{ indent }}        + {{ ref_offset }} );
{{ indent }}    {{ type }} = *reinterpret_cast< mega::TypeID* >( reinterpret_cast< char* >( {{ instance }}.getHeap() )
{{ indent }}        + {{ part_offset }} + ( {{ part_size }} * {{ instance }}.getInstance() )
{{ indent }}        + {{ type_offset }} );
{{ indent }}}
)TEMPLATE";

    // clang-format on
    std::ostringstream os;
    {
        os << args.indent << "// Dereference\n";

        const Variables::Variable*        pFrom = pDereference->get_instance();
        const Variables::Memory*          pRef  = pDereference->get_ref();
        const Variables::LinkType*        pType = pDereference->get_link_type();
        const Concrete::Dimensions::Link* pLink = pDereference->get_link_dimension();
        MemoryLayout::Part*               pPart = pLink->get_part();

        std::ostringstream osIndent;
        osIndent << args.indent;

        VERIFY_RTE( pLink->get_part() == pLink->get_link_type()->get_part() );

        nlohmann::json templateData( { { "indent", osIndent.str() },
                                       { "part_offset", pPart->get_offset() },
                                       { "part_size", pPart->get_size() },
                                       { "ref_offset", pLink->get_offset() },
                                       { "type_offset", pLink->get_link_type()->get_offset() },
                                       { "instance", args.get( pFrom ) },
                                       { "ref", args.get( pRef ) },
                                       { "type", args.get( pType ) }

        } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Call
void gen( Args args, const FinalStage::Operations::Call* pCall,
          const FinalStage::Invocations::Operations::Operation* pOperation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    static thread_local mega::runtime::object::CallGetter function( g_pszModuleName, {{ interface_type_id }}
);
{{ indent }}    return mega::runtime::CallResult{ function(), {{ instance }}, mega::TypeID( {{ interface_type_id }} ) };
{{ indent }}}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        auto pFunction = db_cast< Concrete::Function >( pOperation->get_context() );
        VERIFY_RTE( pFunction );

        std::ostringstream osIndent;
        osIndent << args.indent;

        nlohmann::json templateData(
            { { "indent", osIndent.str() },
              { "interface_type_id", printTypeID( pFunction->get_interface()->get_interface_id() ) },
              { "instance", args.get( pOperation->get_variable() ) } } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
    args.data[ "return_type" ] = "mega::runtime::CallResult";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Signal
void gen( Args args, const FinalStage::Operations::Signal* pSignal,
          const FinalStage::Invocations::Operations::Operation* pOperation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    mega::mangle::event_signal( {{ instance }} );
{{ indent }}    return {{ instance }};
{{ indent }}}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        // auto pState = db_cast< Concrete::State >( pOperation->get_context() );
        // VERIFY_RTE( pState );

        std::ostringstream osIndent;
        osIndent << args.indent;

        nlohmann::json templateData(
            { { "indent", osIndent.str() }, { "instance", args.get( pOperation->get_variable() ) } } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
    args.data[ "return_type" ] = "mega::reference";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Start
void gen( Args args, const FinalStage::Operations::Start* pStart,
          const FinalStage::Invocations::Operations::Operation* pOperation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    mega::mangle::action_start( {{ instance }} );
{{ indent }}    return {{ instance }};
{{ indent }}}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        // auto pState = db_cast< Concrete::State >( pOperation->get_context() );
        // VERIFY_RTE( pState );

        std::ostringstream osIndent;
        osIndent << args.indent;

        nlohmann::json templateData(
            { { "indent", osIndent.str() }, { "instance", args.get( pOperation->get_variable() ) } } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
    args.data[ "return_type" ] = "mega::reference";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// GetContext
void gen( Args args, const FinalStage::Operations::GetContext* pGet,
          const FinalStage::Invocations::Operations::Operation* pOperation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    return {{ instance }};
{{ indent }}}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        // auto pContext = db_cast< Concrete::Context >( pOperation->get_context() );

        std::ostringstream osIndent;
        osIndent << args.indent;

        nlohmann::json templateData(
            { { "indent", osIndent.str() }, { "instance", args.get( pOperation->get_variable() ) } } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
    args.data[ "return_type" ] = "mega::reference";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Read
void gen( Args args, const FinalStage::Operations::Read* pRead,
          const FinalStage::Invocations::Operations::Operation* pOperation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    // Read
{{ indent }}    if( {{ instance }}.getMPO() != mega::runtime::getThisMPO() )
{{ indent }}    {
{{ indent }}        mega::runtime::readLock( {{ instance }} );
{{ indent }}    }
{{ indent }}    else if( {{ instance }}.isNetworkAddress() )
{{ indent }}    {
{{ indent }}        mega::runtime::networkToHeap( {{ instance }} );
{{ indent }}    }
{{ indent }}    return reinterpret_cast< char* >( {{ instance }}.getHeap() )
{{ indent }}        + {{ part_offset }} + ( {{ part_size }} * {{ instance }}.getInstance() )
{{ indent }}        + {{ dimension_offset }};
{{ indent }}}
)TEMPLATE";
    // clang-format on
    std::ostringstream os;
    {
        auto pDimension = db_cast< Concrete::Dimensions::User >( pOperation->get_context() );
        VERIFY_RTE( pDimension );

        Variables::Variable* pVariable = pOperation->get_variable();
        MemoryLayout::Part*  pPart     = pDimension->get_part();

        std::ostringstream osIndent;
        osIndent << args.indent;

        nlohmann::json templateData( { { "indent", osIndent.str() },
                                       { "part_offset", pPart->get_offset() },
                                       { "part_size", pPart->get_size() },
                                       { "dimension_offset", pDimension->get_offset() },
                                       { "instance", args.get( pVariable ) } } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
    args.data[ "return_type" ] = "void*";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Write
void gen( Args args, const FinalStage::Operations::Write* pWrite,
          const FinalStage::Invocations::Operations::Operation* pOperation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    // Write
{{ indent }}    if( {{ instance }}.getMPO() != mega::runtime::getThisMPO() )
{{ indent }}    {
{{ indent }}        mega::runtime::writeLock( {{ instance }} );
{{ indent }}    }
{{ indent }}    else if( {{ instance }}.isNetworkAddress() )
{{ indent }}    {
{{ indent }}        mega::runtime::networkToHeap( {{ instance }} );
{{ indent }}    }
{{ indent }}    void* pTarget =
{{ indent }}        reinterpret_cast< char* >( {{ instance }}.getHeap() )
{{ indent }}        + {{ part_offset }} + ( {{ part_size }} * {{ instance }}.getInstance() )
{{ indent }}        + {{ dimension_offset }};
{{ indent }}
{{ indent }}    mega::mangle::copy_{{ mangled_type_name }}( pData, pTarget );
{{ indent }}    mega::mangle::save_record_{{ mangled_type_name }}( {{ instance }}, pTarget );
{{ indent }}    return {{ instance }};
{{ indent }}}
)TEMPLATE";
    // clang-format on
    std::ostringstream os;
    {
        auto pDimension = db_cast< Concrete::Dimensions::User >( pOperation->get_context() );
        VERIFY_RTE( pDimension );

        Variables::Variable* pVariable  = pOperation->get_variable();
        MemoryLayout::Part*  pPart      = pDimension->get_part();
        const bool           bSimple    = pDimension->get_interface_dimension()->get_simple();
        const std::string    strMangled = megaMangle( pDimension->get_interface_dimension()->get_erased_type() );

        std::ostringstream osIndent;
        osIndent << args.indent;

        nlohmann::json templateData( { { "indent", osIndent.str() },
                                       { "concrete_type_id", printTypeID( pDimension->get_concrete_id() ) },
                                       { "part_offset", pPart->get_offset() },
                                       { "part_size", pPart->get_size() },
                                       { "dimension_offset", pDimension->get_offset() },
                                       { "mangled_type_name", strMangled },
                                       { "instance", args.get( pVariable ) }

        } );

        os << args.inja.render( szTemplate, templateData );

        args.functions.copySet.insert( strMangled );
        args.functions.eventSet.insert( strMangled );
    }

    args.data[ "assignments" ].push_back( os.str() );
    args.data[ "return_type" ]        = "mega::reference";
    args.data[ "has_parameter_data" ] = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// LinkRead
void gen( Args args, const FinalStage::Operations::LinkRead* pLinkRead,
          const FinalStage::Invocations::Operations::Operation* pOperation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    // LinkRead
{{ indent }}    if( {{ instance }}.getMPO() != mega::runtime::getThisMPO() )
{{ indent }}    {
{{ indent }}        mega::runtime::readLock( {{ instance }} );
{{ indent }}    }
{{ indent }}    else if( {{ instance }}.isNetworkAddress() )
{{ indent }}    {
{{ indent }}        mega::runtime::networkToHeap( {{ instance }} );
{{ indent }}    }
{{ indent }}    return reinterpret_cast< char* >( {{ instance }}.getHeap() )
{{ indent }}        + {{ part_offset }} + ( {{ part_size }} * {{ instance }}.getInstance() )
{{ indent }}        + {{ dimension_offset }};
{{ indent }}}
)TEMPLATE";
    // clang-format on
    std::ostringstream os;
    {
        auto pLink = db_cast< Concrete::Dimensions::Link >( pOperation->get_context() );
        VERIFY_RTE( pLink );

        Variables::Variable* pVariable = pOperation->get_variable();
        MemoryLayout::Part*  pPart     = pLink->get_part();

        std::ostringstream osIndent;
        osIndent << args.indent;

        nlohmann::json templateData( { { "indent", osIndent.str() },
                                       { "part_offset", pPart->get_offset() },
                                       { "part_size", pPart->get_size() },
                                       { "dimension_offset", pLink->get_offset() },
                                       { "instance", args.get( pVariable ) } } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
    args.data[ "return_type" ] = "void*";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// LinkAdd
void gen( Args args, const FinalStage::Operations::LinkAdd* pLinkAdd,
          const FinalStage::Invocations::Operations::Operation* pOperation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    static thread_local mega::runtime::relation::LinkMake function( g_pszModuleName,
mega::RelationID{ {{ relation_id_lower }}, {{ relation_id_upper }} } );
{{ indent }}    function( {{ instance }}, target );
{{ indent }}}
{{ indent }}return {{ instance }};
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        auto pLink = db_cast< Concrete::Dimensions::Link >( pOperation->get_context() );
        VERIFY_RTE( pLink );

        RelationID relationID = pLink->get_relation()->get_id();

        std::ostringstream osIndent;
        osIndent << args.indent;

        nlohmann::json templateData( { { "indent", osIndent.str() },
                                       { "relation_id_lower", printTypeID( relationID.getLower() ) },
                                       { "relation_id_upper", printTypeID( relationID.getUpper() ) },
                                       { "instance", args.get( pOperation->get_variable() ) }

        } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
    args.data[ "return_type" ]           = "mega::reference";
    args.data[ "has_parameter_context" ] = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// LinkRemove
void gen( Args args, const FinalStage::Operations::LinkRemove* pLinkRemove,
          const FinalStage::Invocations::Operations::Operation* pOperation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    static thread_local mega::runtime::relation::LinkBreak function( g_pszModuleName,
mega::RelationID{ {{ relation_id_lower }}, {{ relation_id_upper }} } );
{{ indent }}    function( {{ instance }}, target );
{{ indent }}}
{{ indent }}return {{ instance }};
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        auto pLink = db_cast< Concrete::Dimensions::Link >( pOperation->get_context() );
        VERIFY_RTE( pLink );

        RelationID relationID = pLink->get_relation()->get_id();

        std::ostringstream osIndent;
        osIndent << args.indent;

        nlohmann::json templateData( { { "indent", osIndent.str() },
                                       { "relation_id_lower", printTypeID( relationID.getLower() ) },
                                       { "relation_id_upper", printTypeID( relationID.getUpper() ) },
                                       { "instance", args.get( pOperation->get_variable() ) }

        } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
    args.data[ "return_type" ]           = "mega::reference";
    args.data[ "has_parameter_context" ] = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// LinkClear
void gen( Args args, const FinalStage::Operations::LinkClear* pLinkClear,
          const FinalStage::Invocations::Operations::Operation* pOperation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    static thread_local mega::runtime::relation::LinkReset function( g_pszModuleName,
mega::RelationID{ {{ relation_id_lower }}, {{ relation_id_upper }} } );
{{ indent }}    function( {{ instance }} );
{{ indent }}}
{{ indent }}return {{ instance }};
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        auto pLink = db_cast< Concrete::Dimensions::Link >( pOperation->get_context() );
        VERIFY_RTE( pLink );

        RelationID relationID = pLink->get_relation()->get_id();

        std::ostringstream osIndent;
        osIndent << args.indent;

        nlohmann::json templateData( { { "indent", osIndent.str() },
                                       { "relation_id_lower", printTypeID( relationID.getLower() ) },
                                       { "relation_id_upper", printTypeID( relationID.getUpper() ) },
                                       { "instance", args.get( pOperation->get_variable() ) }

        } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
    args.data[ "return_type" ] = "mega::reference";
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Range
void gen( Args args, const FinalStage::Operations::Range* pRange,
          const FinalStage::Invocations::Operations::Operation* pOperation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    THROW_TODO;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Move
void gen( Args args, const FinalStage::Operations::Move* pMove,
          const FinalStage::Invocations::Operations::Operation* pOperation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    THROW_TODO;
    /*
        // clang-format off
    static const char* szTemplate =
    R"TEMPLATE(
    {{ indent }}{
    {{ indent }}    if( {{ instance }}.getMPO() != mega::runtime::getThisMPO() )
    {{ indent }}    {
    {{ indent }}        mega::runtime::readLock( {{ instance }} );
    {{ indent }}    }
    {{ indent }}    else if( {{ instance }}.isNetworkAddress() )
    {{ indent }}    {
    {{ indent }}        mega::runtime::networkToHeap( {{ instance }} );
    {{ indent }}    }
    {{ indent }}}
    {{ indent }}
    {% if unparent_all %}
    {{ indent }} static thread_local mega::runtime::object::ObjectUnparent unparent( g_pszModuleName, {{
    concrete_type_id }}
    );
    {{ indent }} unparent( {{ instance }}.getObjectAddress() );
    {% else %}
    {{ indent }} static thread_local mega::runtime::relation::LinkReset unparent( g_pszModuleName, mega::RelationID{ {{
    relation_id }} } );
    {{ indent }} unparent( {{ instance }} );
    {% endif %}
    {{ indent }}
    {{ indent }} if( {{ instance }}.getMPO() == target.getMPO() )
    {{ indent }} {
    {{ indent }}    static thread_local mega::runtime::relation::LinkMake reparent( g_pszModuleName, mega::RelationID{
    {{ relation_id }} } );
    {{ indent }}    reparent( {{ instance }}, target );
    {{ indent }} }
    {{ indent }} else
    {{ indent }} {
    {{ indent }}    mega::mangle::structure_move( {{ instance }}, target, {{ relation_id_as_int }} );
    {{ indent }} }
    {{ indent }}
    )TEMPLATE";
        // clang-format on

        THROW_TODO;
        std::ostringstream os;
        {
            Concrete::Context*   pConcreteTarget = pMove->get_concrete_target();
            Variables::Variable* pVariable       = pMove->get_variable();

            bool               bUnparentAll = false;
            std::ostringstream osRelationID;
            std::ostringstream osRelationIDAsInt;

            Concrete::Context* pMoveContext = pInstance->get_concrete();
            if( Concrete::Link* pMoveLinkContext = db_cast< Concrete::Link >( pMoveContext ) )
            {
                auto pMovingObjectOpt = pMoveLinkContext->get_concrete_object();
                VERIFY_RTE_MSG( pMovingObjectOpt.has_value(), "Attempting to move link context that has no object" );
                auto pMovingObject = pMovingObjectOpt.value();

                auto owningLinks = pMovingObject->get_owning_links();
                VERIFY_RTE_MSG( std::find( owningLinks.begin(), owningLinks.end(), pMoveLinkContext ) !=
    owningLinks.end(), "Specified move link does NOT own object" );

                auto pLinkInterface = pMoveLinkContext->get_link_interface();
                auto pRelation      = pLinkInterface->get_relation();
                bool bSource        = true;

                Interface::LinkInterface* pOtherLinkInterface = nullptr;
                if( pRelation->get_source_interface() == pLinkInterface )
                {
                    VERIFY_RTE( pRelation->get_ownership().get() == mega::Ownership::eOwnSource );
                    pOtherLinkInterface = pRelation->get_target_interface();
                }
                else if( pRelation->get_target_interface() == pLinkInterface )
                {
                    VERIFY_RTE( pRelation->get_ownership().get() == mega::Ownership::eOwnTarget );
                    pOtherLinkInterface = pRelation->get_source_interface();
                    bSource             = false;
                }
                else
                {
                    THROW_RTE( "Invalid relation does not match source or target link interface" );
                }

                // calculate the relation ID
                {
                    const mega::RelationID& relationID = pRelation->get_id();
                    osRelationID << relationID.getLower() << ',' << relationID.getUpper();
                    using ::operator<<;
                    osRelationIDAsInt << relationID;
                }
                // is the move WITHIN the same MPO ?
            }
            // TODO: use pObject->get_owning_links() on object instead
            else
            {
                bUnparentAll = true;
                THROW_RTE( "Move context is not a link interface" );
            }

            std::ostringstream osIndent;
            osIndent << args.indent;

            nlohmann::json templateData( { { "indent", osIndent.str() },
                                           { "concrete_type_id", printTypeID( pConcreteTarget->get_concrete_id() ) },
                                           { "instance", args.get( pInstance ) },
                                           { "unparent_all", bUnparentAll },
                                           { "relation_id", osRelationID.str() },
                                           { "relation_id_as_int", osRelationIDAsInt.str() }

            } );

            os << args.inja.render( szTemplate, templateData );
        }

        args.data[ "assignments" ].push_back( os.str() );
        args.data[ "has_parameter_context" ] = true;*/
}
} // namespace

void CodeGenerator::generateInstructions( const JITDatabase&                                        database,
                                          const FinalStage::Operations::Invocation*                 pInvocation,
                                          const FinalStage::Invocations::Instructions::Instruction* pInstruction,
                                          const VariableMap& variables, FunctionDeclarations& functions,
                                          nlohmann::json& data, Indent& indent ) const
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    if( auto pInstructionGroup = db_cast< Instructions::InstructionGroup >( pInstruction ) )
    {
        bool bTailRecursion = true;
        if( auto pParentDerivation = db_cast< Instructions::ParentDerivation >( pInstructionGroup ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pParentDerivation );
        }
        else if( auto pChildDerivation = db_cast< Instructions::ChildDerivation >( pInstructionGroup ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pChildDerivation );
        }
        else if( auto pDereference = db_cast< Instructions::Dereference >( pInstructionGroup ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pDereference );
        }
        else if( auto pLinkBranch = db_cast< Instructions::LinkBranch >( pInstructionGroup ) )
        {
            bTailRecursion = false;

            {
                std::ostringstream os;
                os << indent << "// LinkBranch\n";
                os << indent << "switch( " << Args::get( variables, pLinkBranch->get_link_type() ) << " )\n";
                os << indent << "{";
                data[ "assignments" ].push_back( os.str() );
            }

            {
                ++indent;
                for( auto pChildInstruction : pInstructionGroup->get_children() )
                {
                    generateInstructions(
                        database, pInvocation, pChildInstruction, variables, functions, data, indent );
                }
                --indent;
            }

            {
                using ::operator<<;
                std::ostringstream os;
                os << indent << "   case 0x00000000:\n";
                os << indent << "   default:\n";
                os << indent << "   {\n";
                os << indent
                   << "       throw mega::runtime::JITException{ \"Null reference in poly branch for invocation: "
                   << pInvocation->get_id() << "\" };\n";
                os << indent << "   }\n";
                os << indent << "   break;\n";
                os << indent << "}";
                data[ "assignments" ].push_back( os.str() );
            }
        }
        else if( auto pPolyReference = db_cast< Instructions::PolyBranch >( pInstructionGroup ) )
        {
            bTailRecursion = false;

            {
                std::ostringstream os;
                os << indent << "// PolyBranch\n";
                os << indent << "switch( " << Args::get( variables, pPolyReference->get_parameter() )
                   << ".getType() )\n";
                os << indent << "{";
                data[ "assignments" ].push_back( os.str() );
            }

            {
                ++indent;
                for( auto pChildInstruction : pInstructionGroup->get_children() )
                {
                    generateInstructions(
                        database, pInvocation, pChildInstruction, variables, functions, data, indent );
                }
                --indent;
            }

            {
                std::ostringstream os;
                os << indent << "   case 0x00000000:\n";
                os << indent << "   default:\n";
                os << indent << "   {\n";
                os << indent << "       throw mega::runtime::JITException{ \"Null reference in poly branch\" };\n";
                os << indent << "   }\n";
                os << indent << "   break;\n";
                os << indent << "}";
                data[ "assignments" ].push_back( os.str() );
            }
        }
        else if( auto pPolyCase = db_cast< Instructions::PolyCase >( pInstructionGroup ) )
        {
            bTailRecursion = false;

            {
                std::ostringstream os;
                os << indent << "// PolyCase\n";
                Concrete::Graph::Vertex* pType = pPolyCase->get_type();
                if( auto pUserDim = db_cast< Concrete::Dimensions::User >( pType ) )
                {
                    os << indent << "case " << printTypeID( pUserDim->get_concrete_id() ) << " :\n";
                }
                else if( auto pLinkDim = db_cast< Concrete::Dimensions::Link >( pType ) )
                {
                    os << indent << "case " << printTypeID( pLinkDim->get_concrete_id() ) << " :\n";
                }
                else if( auto pContext = db_cast< Concrete::Context >( pType ) )
                {
                    os << indent << "case " << printTypeID( pContext->get_concrete_id() ) << " :\n";
                }
                else
                {
                    THROW_RTE( "Unknown poly case vertex type" );
                }

                os << indent << "{\n";
                ++indent;
                data[ "assignments" ].push_back( os.str() );
            }

            {
                for( auto pChildInstruction : pInstructionGroup->get_children() )
                {
                    generateInstructions(
                        database, pInvocation, pChildInstruction, variables, functions, data, indent );
                }
            }

            {
                --indent;
                std::ostringstream os;
                os << indent << "}\n";
                os << indent << "break;\n";
                data[ "assignments" ].push_back( os.str() );
            }
        }
        else
        {
            THROW_RTE( "Unknown instruction type" );
        }

        if( bTailRecursion )
        {
            ++indent;
            for( auto pChildInstruction : pInstructionGroup->get_children() )
            {
                generateInstructions( database, pInvocation, pChildInstruction, variables, functions, data, indent );
            }
            --indent;
        }
    }
    else if( auto pOperation = db_cast< FinalStage::Invocations::Operations::Operation >( pInstruction ) )
    {
        using namespace FinalStage::Operations;

        if( auto pStart = db_cast< Start >( pInvocation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pStart, pOperation );
        }
        else if( auto pCall = db_cast< Call >( pInvocation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pCall, pOperation );
        }
        else if( auto pSignal = db_cast< Signal >( pInvocation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pSignal, pOperation );
        }
        else if( auto pMove = db_cast< Move >( pInvocation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pMove, pOperation );
        }
        else if( auto pGetContext = db_cast< GetContext >( pInvocation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pGetContext, pOperation );
        }
        else if( auto pRead = db_cast< Read >( pInvocation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pRead, pOperation );
        }
        else if( auto pWrite = db_cast< Write >( pInvocation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pWrite, pOperation );
        }
        else if( auto pLinkRead = db_cast< LinkRead >( pInvocation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pLinkRead, pOperation );
        }
        else if( auto pLinkAdd = db_cast< LinkAdd >( pInvocation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pLinkAdd, pOperation );
        }
        else if( auto pLinkRemove = db_cast< LinkRemove >( pInvocation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pLinkRemove, pOperation );
        }
        else if( auto pLinkClear = db_cast< LinkClear >( pInvocation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pLinkClear, pOperation );
        }
        else if( auto pRange = db_cast< Range >( pInvocation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pRange, pOperation );
        }
        else
        {
            THROW_RTE( "Unknown operation type" );
        }
    }
    else
    {
        THROW_RTE( "Unknown instruction type" );
    }
}

CodeGenerator::VariableMap CodeGenerator::generateVariables(
    const std::vector< ::FinalStage::Invocations::Variables::Variable* >& invocationVariables,
    nlohmann::json&                                                       data,
    Indent&                                                               indent ) const
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    VariableMap variables;

    int iVariableCounter = 0;
    for( auto pVariable : invocationVariables )
    {
        if( auto pInstanceVar = db_cast< Variables::Stack >( pVariable ) )
        {
            std::ostringstream osName;
            {
                osName << "var_";
                Concrete::printContextFullType( pInstanceVar->get_concrete(), osName, "_" );
                osName << "_" << iVariableCounter++;
            }
            variables.insert( { pVariable, osName.str() } );

            std::ostringstream osVar;
            {
                osVar << indent << "mega::reference " << osName.str() << ";";
            }
            data[ "variables" ].push_back( osVar.str() );
        }
        else if( auto pDimensionVar = db_cast< Variables::Memory >( pVariable ) )
        {
            std::ostringstream osName;
            {
                osName << "var";
                for( auto pType : pDimensionVar->get_types() )
                {
                    osName << '_';
                    Concrete::printContextFullType( pType, osName, "_" );
                }
                osName << "_" << iVariableCounter++;
            }
            variables.insert( { pVariable, osName.str() } );

            std::ostringstream osVar;
            {
                osVar << indent << "mega::reference " << osName.str() << ";";
            }
            data[ "variables" ].push_back( osVar.str() );
        }
        else if( auto pContextVar = db_cast< Variables::Parameter >( pVariable ) )
        {
            variables.insert( { pContextVar, "context" } );
        }
        else if( auto pLinkType = db_cast< Variables::LinkType >( pVariable ) )
        {
            std::ostringstream osName;
            {
                osName << "var";
                osName << "_" << iVariableCounter++;
            }
            variables.insert( { pVariable, osName.str() } );

            std::ostringstream osVar;
            {
                osVar << indent << "mega::TypeID " << osName.str() << ";";
            }
            data[ "variables" ].push_back( osVar.str() );
        }
        else
        {
            THROW_RTE( "Unknown variable type" );
        }
    }
    return variables;
}
} // namespace mega::runtime
