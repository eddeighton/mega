
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
}

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
void gen( Args args, FinalStage::Invocations::Instructions::ParentDerivation* pParentDerivation )
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
    const mega::U64    szLocalSize = args.database.getLocalDomainSize( targetType );

    os << args.indent << args.get( pTo ) << " = mega::reference::make( " << args.get( pFrom )
       << ", mega::TypeInstance{ " << printTypeID( targetType ) << ", "
       << "static_cast< mega::Instance >( " << s << ".getInstance() / " << szLocalSize << " ) } );\n";

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ChildDerivation
void gen( Args args, FinalStage::Invocations::Instructions::ChildDerivation* pChildDerivation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    std::ostringstream os;
    os << args.indent << "// ChildDerivation\n";

    const Variables::Variable* pFrom      = pChildDerivation->get_from();
    const Variables::Stack*    pTo        = pChildDerivation->get_to();
    const Concrete::Context*   pToContext = db_cast< Concrete::Context >( pTo->get_concrete() );

    const std::string  s           = args.get( pFrom );
    const mega::TypeID targetType  = pToContext->get_concrete_id();
    const mega::U64    szLocalSize = args.database.getLocalDomainSize( targetType );

    os << args.indent << args.get( pTo ) << " = mega::reference::make( " << args.get( pFrom )
       << ", mega::TypeInstance{ " << printTypeID( targetType ) << ", " << s << ".getInstance() } );\n";

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// EnumDerivation
/*
void gen( Args args, FinalStage::Invocations::Instructions::EnumDerivation* pEnumDerivation )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    THROW_TODO;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Enumeration
void gen( Args args, FinalStage::Invocations::Instructions::Enumeration* pEnumeration )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    THROW_TODO;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// DimensionReferenceRead
void gen( Args args, FinalStage::Invocations::Instructions::DimensionReferenceRead* pDimensionReferenceRead )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    THROW_TODO;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations
/*
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Allocate
void gen( Args args, FinalStage::Invocations::Operations::Allocate* pAllocate )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    mega::reference allocatedRef = mega::runtime::allocate( {{ instance }}, mega::TypeID{ {{
concrete_type_id }} } );
{% if has_owning_link %}
{{ indent }}    static thread_local mega::runtime::relation::LinkMake function( g_pszModuleName, mega::RelationID{ {{
owning_relation_id }} } );
{{ indent }}    function( {{ link_source }}, {{ link_target }} );
{% endif %}
{{ indent }}    return allocatedRef;
{{ indent }}}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        Variables::Variable* pVariable       = pAllocate->get_variable();
        Concrete::Context*   pConcreteTarget = pAllocate->get_concrete_target();
        std::ostringstream   osIndent;
        osIndent << args.indent;

        nlohmann::json templateData( { { "indent", osIndent.str() },
                                       { "concrete_type_id", printTypeID( pConcreteTarget->get_concrete_id() ) },
                                       { "instance", args.get( pInstance ) },
                                       { "has_owning_link", false },
                                       { "link_source", "" },
                                       { "link_target", "" },
                                       { "owning_relation_id", "" } } );

        auto pConstructedObject = db_cast< Concrete::Object >( pConcreteTarget );
        VERIFY_RTE( pConstructedObject );
        std::vector< Concrete::Link* > owningLinks = pConstructedObject->get_owning_links();
        if( !owningLinks.empty() )
        {
            Concrete::Context* pParentContext = pInstance->get_concrete();
            if( Concrete::Link* pParentLink = db_cast< Concrete::Link >( pParentContext ) )
            {
                Interface::LinkInterface* pParentLinkInterface = pParentLink->get_link_interface();
                Interface::LinkInterface* pChildLinkInterface  = nullptr;
                HyperGraph::Relation*     pRelation            = pParentLinkInterface->get_relation();

                if( pRelation->get_source_interface() == pParentLinkInterface )
                {
                    pChildLinkInterface = pRelation->get_target_interface();
                }
                else if( pRelation->get_target_interface() == pParentLinkInterface )
                {
                    pChildLinkInterface = pRelation->get_source_interface();
                }
                else
                {
                    THROW_RTE( "Bad relation" );
                }

                Concrete::Link* pOwningLink = nullptr;
                for( Concrete::Link* pLink : owningLinks )
                {
                    if( pLink->get_link_interface() == pChildLinkInterface )
                    {
                        VERIFY_RTE_MSG( !pOwningLink, "Duplicate owning links" );
                        pOwningLink = pLink;
                    }
                }
                VERIFY_RTE_MSG( pOwningLink, "Failed to locate owning link interface for allocation" );
                std::ostringstream osLinkReference;
                osLinkReference << "mega::reference::make( allocatedRef, mega::TypeInstance{ "
                                << printTypeID( pOwningLink->get_concrete_id() )
                                << ", allocatedRef.getInstance() } )";

                Interface::LinkTrait* pLinkTrait = pChildLinkInterface->get_link_trait();
                if( pRelation->get_source_interface() == pChildLinkInterface )
                {
                    templateData[ "link_source" ] = osLinkReference.str();
                    templateData[ "link_target" ] = args.get( pInstance );
                }
                else if( pRelation->get_target_interface() == pChildLinkInterface )
                {
                    templateData[ "link_source" ] = args.get( pInstance );
                    templateData[ "link_target" ] = osLinkReference.str();
                }
                else
                {
                    THROW_RTE( "Invalid relation" );
                }

                const mega::RelationID& relationID = pRelation->get_id();
                std::ostringstream      osRelationID;
                osRelationID << relationID.getLower() << ',' << relationID.getUpper();

                templateData[ "owning_relation_id" ] = osRelationID.str();
                templateData[ "has_owning_link" ]    = true;
            }
            else
            {
                THROW_RTE( "Invalid allocation - no owning links" );
            }
        }

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Move
void gen( Args args, FinalStage::Invocations::Operations::Move* pMove )
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
{{ indent }}}
{{ indent }}
{% if unparent_all %}
{{ indent }} static thread_local mega::runtime::object::ObjectUnparent unparent( g_pszModuleName, {{ concrete_type_id }}
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
{{ indent }}    static thread_local mega::runtime::relation::LinkMake reparent( g_pszModuleName, mega::RelationID{ {{
relation_id }} } );
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
            VERIFY_RTE_MSG( std::find( owningLinks.begin(), owningLinks.end(), pMoveLinkContext ) != owningLinks.end(),
                            "Specified move link does NOT own object" );

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
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Call
void gen( Args args, FinalStage::Invocations::Operations::Call* pCall )
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
        Concrete::Context*   pConcreteTarget = pCall->get_concrete_target();
        Variables::Variable* pVariable       = pCall->get_variable();
        std::ostringstream   osIndent;
        osIndent << args.indent;

        nlohmann::json templateData(
            { { "indent", osIndent.str() },
              //{ "concrete_type_id", printTypeID( pConcreteTarget->get_concrete_id() ) },
              { "interface_type_id", printTypeID( pConcreteTarget->get_interface()->get_interface_id() ) },
              { "instance", args.get( pVariable ) } } );

        os << args.inja.render( szTemplate, templateData );

        args.functions.callSet.insert( pConcreteTarget );
    }

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Start
void gen( Args args, FinalStage::Invocations::Operations::Start* pStart )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    if( {{ instance }}.getMPO() != mega::runtime::getThisMPO() )
{{ indent }}    {
{{ indent }}        mega::runtime::writeLock( {{ instance }} );
{{ indent }}    }
{{ indent }}    else if( {{ instance }}.isNetworkAddress() )
{{ indent }}    {
{{ indent }}        mega::runtime::networkToHeap( {{ instance }} );
{{ indent }}    }
{{ indent }}    mega::reference action = mega::reference::make( {{ instance }}, mega::TypeInstance{ {{ concrete_type_id
}}, {{ instance }}.getInstance() } );
{{ indent }}    mega::mangle::action_start( action );
{{ indent }}    return action;
{{ indent }}}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        Concrete::Context*   pConcreteTarget = pStart->get_concrete_target();
        Variables::Variable* pVariable       = pStart->get_variable();
        std::ostringstream   osIndent;
        osIndent << args.indent;

        nlohmann::json templateData( { { "indent", osIndent.str() },
                                       { "concrete_type_id", printTypeID( pConcreteTarget->get_concrete_id() ) },
                                       { "instance", args.get( pVariable ) } } );

        os << args.inja.render( szTemplate, templateData );

        args.functions.callSet.insert( pConcreteTarget );
    }

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Stop
void gen( Args args, FinalStage::Invocations::Operations::Stop* pStop )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    if( {{ instance }}.getMPO() != mega::runtime::getThisMPO() )
{{ indent }}    {
{{ indent }}        mega::runtime::writeLock( {{ instance }} );
{{ indent }}    }
{{ indent }}    else if( {{ instance }}.isNetworkAddress() )
{{ indent }}    {
{{ indent }}        mega::runtime::networkToHeap( {{ instance }} );
{{ indent }}    }
{{ indent }}    mega::reference action = mega::reference::make( {{ instance }}, mega::TypeInstance{ {{ concrete_type_id
}}, {{ instance }}.getInstance() } );
{{ indent }}    mega::mangle::action_stop( action );
{{ indent }}    return action;
{{ indent }}}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        Concrete::Context*   pConcreteTarget = pStop->get_concrete_target();
        Variables::Variable* pVariable       = pStop->get_variable();
        std::ostringstream   osIndent;
        osIndent << args.indent;

        nlohmann::json templateData( { { "indent", osIndent.str() },
                                       { "concrete_type_id", printTypeID( pConcreteTarget->get_concrete_id() ) },
                                       { "instance", args.get( pVariable ) } } );

        os << args.inja.render( szTemplate, templateData );

        args.functions.callSet.insert( pConcreteTarget );
    }

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// GetContext
void gen( Args args, FinalStage::Invocations::Operations::GetContext* pGet )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    return mega::reference::make( {{ instance }}, mega::TypeInstance{ {{ concrete_type_id }}, {{ instance
}}.getInstance() } );
{{ indent }}}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        Variables::Variable* pVariable = pGet->get_variable();

        std::ostringstream osIndent;
        osIndent << args.indent;

        nlohmann::json templateData(
            { { "indent", osIndent.str() },
              { "instance", args.get( pVariable ) },
              { "concrete_type_id", printTypeID( pGet->get_concrete_target()->get_concrete_id() ) } } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// GetDimension
void gen( Args args, FinalStage::Invocations::Operations::GetDimension* pGetDimension )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    return mega::reference::make( {{ instance }}, mega::TypeInstance{ {{ concrete_type_id }}, {{ instance
}}.getInstance() } );
{{ indent }}}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        Variables::Variable* pVariable = pGetDimension->get_variable();

        std::ostringstream osIndent;
        osIndent << args.indent;

        nlohmann::json templateData(
            { { "indent", osIndent.str() },
              { "instance", args.get( pVariable ) },
              { "concrete_type_id", printTypeID( pGetDimension->get_concrete_dimension()->get_concrete_id() ) } }

        );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Read
void gen( Args args, FinalStage::Invocations::Operations::Read* pRead )
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
        Concrete::Dimensions::User* pDimension = pRead->get_concrete_dimension();
        Variables::Variable*        pVariable  = pRead->get_variable();
        MemoryLayout::Part*         pPart      = pDimension->get_part();

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
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Write
void gen( Args args, FinalStage::Invocations::Operations::Write* pWrite )
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
{{ indent }}    mega::mangle::save_record_{{ mangled_type_name }}
{{ indent }}    (
{{ indent }}        mega::reference
{{ indent }}        (
{{ indent }}            mega::TypeInstance
{{ indent }}            (
{{ indent }}                {{ concrete_type_id }},
{{ indent }}                {{ instance }}.getInstance()
{{ indent }}            ),
{{ indent }}            {{ instance }}.getHeap()
{{ indent }}        ),
{{ indent }}        pTarget
{{ indent }}    );
{{ indent }}    return {{ instance }};
{{ indent }}}
)TEMPLATE";
    // clang-format on
    std::ostringstream os;
    {
        Concrete::Dimensions::User* pDimension = pWrite->get_concrete_dimension();
        Variables::Variable*        pVariable  = pWrite->get_variable();
        MemoryLayout::Part*         pPart      = pDimension->get_part();
        const bool                  bSimple    = pDimension->get_interface_dimension()->get_simple();
        const std::string           strMangled = megaMangle( pDimension->get_interface_dimension()->get_erased_type() );

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
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ReadLink
void gen( Args args, FinalStage::Invocations::Operations::ReadLink* pReadLink )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    // ReadLink
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
        THROW_TODO;
        Concrete::Link*      pLink          = pReadLink->get_concrete_link();
        auto                 pLinkReference = pLink->get_link_reference();
        Variables::Variable* pVariable       = pReadLink->get_variable();
        MemoryLayout::Part*  pPart          = pLinkReference->get_part();

        std::ostringstream osIndent;
        osIndent << args.indent;

        nlohmann::json templateData( { { "indent", osIndent.str() },
                                       { "part_offset", pPart->get_offset() },
                                       { "part_size", pPart->get_size() },
                                       { "dimension_offset", pLinkReference->get_offset() },
                                       { "instance", args.get( pVariable ) } } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// WriteLink
void gen( Args args, FinalStage::Invocations::Operations::WriteLink* pWriteLink )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{{ indent }}{
{{ indent }}    switch( overload )
{{ indent }}    {
{{ indent }}        case WriteOperation::DEFAULT:
{{ indent }}        {
{{ indent }}            static thread_local mega::runtime::relation::LinkMake function( g_pszModuleName,
mega::RelationID{ {{ relation_id_lower }}, {{ relation_id_upper }} } );
{{ indent }}            function( {{ instance }}, target );
{{ indent }}        }
{{ indent }}        break;
{{ indent }}        case WriteOperation::REMOVE:
{{ indent }}        {
{{ indent }}            static thread_local mega::runtime::relation::LinkBreak function( g_pszModuleName,
mega::RelationID{ {{ relation_id_lower }}, {{ relation_id_upper }} } );
{{ indent }}            function( {{ instance }}, target );
{{ indent }}        }
{{ indent }}        break;
{{ indent }}        case WriteOperation::RESET:
{{ indent }}        {
{{ indent }}            static thread_local mega::runtime::relation::LinkReset function( g_pszModuleName,
mega::RelationID{ {{ relation_id_lower }}, {{ relation_id_upper }} } );
{{ indent }}            function( {{ instance }} );
{{ indent }}        }
{{ indent }}        break;
{{ indent }}        case WriteOperation::TOTAL_WRITE_OPERATIONS:
{{ indent }}        default:
{{ indent }}            throw mega::runtime::JITException{ "Unrecognised write link overload" };
{{ indent }}        break;
{{ indent }}    }
{{ indent }}}
{{ indent }}return {{ instance }};
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        THROW_TODO;
        Concrete::Link*      pLink          = pWriteLink->get_concrete_link();
        auto                 pLinkReference = pLink->get_link_reference();
        Variables::Variable* pVariable       = pWriteLink->get_variable();

        RelationID           relationID     = pLink->get_link_interface()->get_relation()->get_id();

        std::ostringstream osIndent;
        osIndent << args.indent;

        nlohmann::json templateData( { { "indent", osIndent.str() },
                                       { "relation_id_lower", printTypeID( relationID.getLower() ) },
                                       { "relation_id_upper", printTypeID( relationID.getUpper() ) },
                                       { "instance", args.get( pVariable ) }

        } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Range
void gen( Args args, FinalStage::Invocations::Operations::Range* pRange )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    THROW_TODO;
}
*/
} // namespace

void CodeGenerator::generateInstructions( const JITDatabase&                                  database,
                                          FinalStage::Invocations::Instructions::Instruction* pInstruction,
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
        /*else if( auto pEnumDerivation = db_cast< Instructions::EnumDerivation >( pInstructionGroup ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pEnumDerivation );
        }
        else if( auto pEnumeration = db_cast< Instructions::Enumeration >( pInstructionGroup ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pEnumeration );
        }
        else if( auto pDimensionReferenceRead = db_cast< Instructions::DimensionReferenceRead >( pInstructionGroup ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pDimensionReferenceRead );
        }*/
        else if( auto pPolyReference = db_cast< Instructions::PolyBranch >( pInstructionGroup ) )
        {
            bTailRecursion = false;

            {
                std::ostringstream os;
                os << indent << "// PolyBranch\n";
                os << indent << "switch( " << Args::get( variables, pPolyReference->get_from_reference() ) << ".getType() )\n";
                os << indent << "{";
                data[ "assignments" ].push_back( os.str() );
            }

            {
                ++indent;
                for( auto pChildInstruction : pInstructionGroup->get_children() )
                {
                    generateInstructions( database, pChildInstruction, variables, functions, data, indent );
                }
                --indent;
            }

            {
                std::ostringstream os;
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

                // const Variables::Reference* pReference = pPolyCase->get_reference();
                Concrete::Context* pType = pPolyCase->get_type();

                os << indent << "case " << printTypeID( pType->get_concrete_id() ) << " :\n";
                os << indent << "{\n";
                ++indent;
                // os << indent << Args::get( variables, pInstance ) << " = mega::reference::make( "
                //    << Args::get( variables, pReference ) << ", mega::TypeInstance{"
                //    << printTypeID( pInstance->get_concrete()->get_concrete_id() ) << ", "
                //    << Args::get( variables, pInstance ) << ".getInstance()} );\n";
                data[ "assignments" ].push_back( os.str() );
            }

            {
                for( auto pChildInstruction : pInstructionGroup->get_children() )
                {
                    generateInstructions( database, pChildInstruction, variables, functions, data, indent );
                }
            }

            {
                --indent;
                std::ostringstream os;
                os << indent << "}";
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
                generateInstructions( database, pChildInstruction, variables, functions, data, indent );
            }
            --indent;
        }
    }
    else if( auto pOperation = db_cast< FinalStage::Invocations::Operations::Operation >( pInstruction ) )
    {
        using namespace FinalStage::Invocations::Operations;

        THROW_TODO;
        /*if( auto pAllocate = db_cast< Allocate >( pOperation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pAllocate );
        }
        else*/
        /*if( auto pCall = db_cast< Call >( pOperation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pCall );
        }
        else if( auto pStart = db_cast< Start >( pOperation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pStart );
        }
        else if( auto pStop = db_cast< Stop >( pOperation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pStop );
        }
        else if( auto pMove = db_cast< Move >( pOperation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pMove );
        }
        else if( auto pGet = db_cast< GetContext >( pOperation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pGet );
        }
        else if( auto pGetDimension = db_cast< GetDimension >( pOperation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pGetDimension );
        }
        else if( auto pRead = db_cast< Read >( pOperation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pRead );
        }
        else if( auto pWrite = db_cast< Write >( pOperation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pWrite );
        }
        else if( auto pReadLink = db_cast< ReadLink >( pOperation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pReadLink );
        }
        else if( auto pWriteLink = db_cast< WriteLink >( pOperation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pWriteLink );
        }
        else if( auto pRange = db_cast< Range >( pOperation ) )
        {
            gen( Args{ database, variables, functions, data, indent, *m_pInja }, pRange );
        }
        else
        {
            THROW_RTE( "Unknown operation type" );
        }*/
    }
    else
    {
        THROW_RTE( "Unknown instruction type" );
    }
}

CodeGenerator::VariableMap CodeGenerator::generateVariables(
    const std::vector< ::FinalStage::Invocations::Variables::Variable* >& invocationVariables,
    ::FinalStage::Invocations::Variables::Parameter*                      pRootContext,
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
                printConcreteFullType( pInstanceVar->get_concrete(), osName );
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
                osName << "var_";
                printConcreteFullType( pDimensionVar->get_types().front(), osName );
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
        else
        {
            THROW_RTE( "Unknown variable type" );
        }
    }
    return variables;
}
} // namespace mega::runtime
