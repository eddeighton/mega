
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

#ifndef GUARD_2023_May_23_invocations
#define GUARD_2023_May_23_invocations

#include "driver/retail/templates.hpp"
#include "driver/retail/invocation_info.hpp"

namespace driver::retail
{

static inline std::string printTypeID( const mega::TypeID& typeID )
{
    std::ostringstream os;
    os << "0x" << std::hex << std::setfill( '0' ) << std::setw( 4 ) << typeID.getSymbolID();
    return os.str();
}
static inline std::string megaMangle( const std::string& str )
{
    return str;
}

struct RetailDatabase
{
    int getLocalDomainSize( const mega::TypeID targetType ) const { return 1; }
};

using VariableMap = std::map< const FinalStage::Invocations::Variables::Variable*, std::string >;

struct Args
{
    const RetailDatabase& database;
    const VariableMap&    variables;
    nlohmann::json&       data;
    TemplateEngine&       inja;

    static std::string get( const VariableMap& variables, const FinalStage::Invocations::Variables::Variable* pVar )
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
    os << "// ParentDerivation\n";

    const Variables::Instance* pFrom = pParentDerivation->get_from();
    const Variables::Instance* pTo   = pParentDerivation->get_to();

    const std::string  s           = args.get( pFrom );
    const mega::TypeID targetType  = pFrom->get_concrete()->get_concrete_id();
    const mega::U64    szLocalSize = args.database.getLocalDomainSize( targetType );

    os << args.get( pTo ) << " = mega::reference::make( " << args.get( pFrom ) << ", mega::TypeInstance{ "
       << printTypeID( targetType ) << ", "
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
    os << "// ChildDerivation\n";

    const Variables::Instance* pFrom = pChildDerivation->get_from();
    const Variables::Instance* pTo   = pChildDerivation->get_to();

    const std::string  s           = args.get( pFrom );
    const mega::TypeID targetType  = pTo->get_concrete()->get_concrete_id();
    const mega::U64    szLocalSize = args.database.getLocalDomainSize( targetType );

    os << args.get( pTo ) << " = mega::reference::make( " << args.get( pFrom ) << ", " << printTypeID( targetType )
       << " );\n";

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// EnumDerivation
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

///////////////////////////////////////////////////////////////////////////////////////////////////////
// MonoReference
void gen( Args args, FinalStage::Invocations::Instructions::MonoReference* pMonoReference )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    const Variables::Instance*  pInstance  = pMonoReference->get_instance();
    const Variables::Reference* pReference = pMonoReference->get_reference();

    std::ostringstream os;
    os << "// MonoReference\n";
    os << args.get( pInstance ) << " = mega::reference::make( " << args.get( pReference ) << ", "
       << printTypeID( pInstance->get_concrete()->get_concrete_id() ) << " );";

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Allocate
void gen( Args args, FinalStage::Invocations::Operations::Allocate* pAllocate )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{
    mega::reference allocatedRef = Thread::thisThread()->m_memoryManager.allocate( {{ object_type_id }} );
{% if has_owning_link %}
    //static thread_local mega::runtime::relation::LinkMake function( g_pszModuleName, mega::RelationID{ {{ owning_relation_id }} } );
    //function( {{ link_source }}, {{ link_target }} );
{% endif %}
    return allocatedRef;
}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        Variables::Instance* pInstance       = pAllocate->get_instance();
        Concrete::Context*   pConcreteTarget = pAllocate->get_concrete_target();

        nlohmann::json templateData( { { "object_type_id", pConcreteTarget->get_concrete_id().getObjectID() },
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
                osLinkReference << "mega::reference::make( allocatedRef, "
                                << printTypeID( pOwningLink->get_concrete_id() ) << " )";

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
                THROW_RTE( "Invalid allocation parent" );
            }
        }

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
}

std::string printIContextFullType( FinalStage::Interface::IContext* pContext )
{
    std::ostringstream os;
    using namespace FinalStage;
    using IContextVector = std::vector< Interface::IContext* >;
    IContextVector path;
    while( pContext )
    {
        path.push_back( pContext );
        pContext = db_cast< Interface::IContext >( pContext->get_parent() );
    }
    std::reverse( path.begin(), path.end() );
    for( auto i = path.begin(), iNext = path.begin(), iEnd = path.end(); i != iEnd; ++i )
    {
        ++iNext;
        if( iNext == iEnd )
        {
            os << ( *i )->get_identifier();
        }
        else
        {
            os << ( *i )->get_identifier() << "::";
        }
    }
    return os.str();
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
{
    return {{ interface_type }}( {{instance}} )( args... );
}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        Concrete::Context*   pConcreteTarget = pCall->get_concrete_target();
        Variables::Instance* pInstance       = pCall->get_instance();

        nlohmann::json templateData( { { "interface_type", printIContextFullType( pConcreteTarget->get_interface() ) },
                                       { "instance", args.get( pInstance ) } } );

        os << args.inja.render( szTemplate, templateData );
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
{
    mega::reference action = mega::reference::make( {{ instance }}, {{ concrete_type_id }} );
    // mega::mangle::action_start( action );
    return action;
}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        Concrete::Context*   pConcreteTarget = pStart->get_concrete_target();
        Variables::Instance* pInstance       = pStart->get_instance();

        VERIFY_RTE_MSG( ( pConcreteTarget->get_parent() == pInstance->get_concrete() )
                            || ( pConcreteTarget == pInstance->get_concrete() ),
                        "Start operation target is not child or equal to instance variable type" );

        nlohmann::json templateData( { { "concrete_type_id", printTypeID( pConcreteTarget->get_concrete_id() ) },
                                       { "instance", args.get( pInstance ) } } );

        os << args.inja.render( szTemplate, templateData );
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
{
    mega::reference action = mega::reference::make( {{ instance }}, {{ concrete_type_id }} );
    // mega::mangle::action_stop( action );
    return action;
}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        Concrete::Context*   pConcreteTarget = pStop->get_concrete_target();
        Variables::Instance* pInstance       = pStop->get_instance();

        VERIFY_RTE_MSG( ( pConcreteTarget->get_parent() == pInstance->get_concrete() )
                            || ( pConcreteTarget == pInstance->get_concrete() ),
                        "Start operation target is not child or equal to instance variable type" );

        nlohmann::json templateData( { { "concrete_type_id", printTypeID( pConcreteTarget->get_concrete_id() ) },
                                       { "instance", args.get( pInstance ) } } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Save
void gen( Args args, FinalStage::Invocations::Operations::Save* pSave )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // THROW_TODO;
    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{
    // TODO
    // static thread_local mega::runtime::object::ObjectSaveXMLStructure functionStructure( g_pszModuleName, {{ concrete_type_id }} );
    // functionStructure( {{ instance }}, pArchive );
    // static thread_local mega::runtime::object::ObjectSaveXML function( g_pszModuleName, {{ concrete_type_id }} );
    // function( {{ instance }}, pArchive );
}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        Concrete::Context*   pConcreteTarget = pSave->get_concrete_target();
        Variables::Instance* pInstance       = pSave->get_instance();

        nlohmann::json templateData( { { "concrete_type_id", printTypeID( pConcreteTarget->get_concrete_id() ) },
                                       { "instance", args.get( pInstance ) } } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Load
void gen( Args args, FinalStage::Invocations::Operations::Load* pLoad )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{
    // TODO
    // mega::mangle::xml_load_allocation( {{ instance }}, pArchive );
    // static thread_local mega::runtime::object::ObjectLoadXMLStructure functionStructure( g_pszModuleName, {{ concrete_type_id }} );
    // functionStructure( {{ instance }}, pArchive );
    // static thread_local mega::runtime::object::ObjectLoadXML function( g_pszModuleName, {{ concrete_type_id }} );
    // function( {{ instance }}, pArchive );
}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        Concrete::Context*   pConcreteTarget = pLoad->get_concrete_target();
        Variables::Instance* pInstance       = pLoad->get_instance();

        nlohmann::json templateData( { { "concrete_type_id", printTypeID( pConcreteTarget->get_concrete_id() ) },
                                       { "instance", args.get( pInstance ) } } );

        os << args.inja.render( szTemplate, templateData );
    }

    args.data[ "assignments" ].push_back( os.str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// GetAction
void gen( Args args, FinalStage::Invocations::Operations::GetAction* pGet )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
{
    return mega::reference::make( {{ instance }}, {{ concrete_type_id }} );
}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        Variables::Instance* pInstance = pGet->get_instance();

        nlohmann::json templateData(
            { { "instance", args.get( pInstance ) },
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
{
    return mega::reference::make( {{ instance }}, {{ concrete_type_id }} );
}
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        Variables::Instance* pInstance = pGetDimension->get_instance();

        nlohmann::json templateData(
            { { "instance", args.get( pInstance ) },
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
{
    // Read
    return static_cast< o_{{ concrete_type_id }}* >( {{ instance }}.getHeap() )->m_{{ member_type_id }};
}
)TEMPLATE";
    // clang-format on
    std::ostringstream os;
    {
        Concrete::Dimensions::User* pDimension = pRead->get_concrete_dimension();
        Variables::Instance*        pInstance  = pRead->get_instance();

        nlohmann::json templateData(
            { { "concrete_type_id", pInstance->get_concrete()->get_concrete_id().getObjectID() },
              { "member_type_id", pDimension->get_concrete_id().getSubObjectID() },
              { "instance", args.get( pInstance ) } } );

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
{
    // Write
    auto& target = static_cast< o_{{ concrete_type_id }}* >( {{ instance }}.getHeap() )->m_{{ member_type_id }};
    target = decltype( target ){ args... };

    // mega::mangle::save_record
    return {{ instance }};
}
)TEMPLATE";
    // clang-format on
    std::ostringstream os;
    {
        Concrete::Dimensions::User* pDimension = pWrite->get_concrete_dimension();
        Variables::Instance*        pInstance  = pWrite->get_instance();
        MemoryLayout::Part*         pPart      = pDimension->get_part();
        const bool                  bSimple    = pDimension->get_interface_dimension()->get_simple();

        nlohmann::json templateData(
            { { "concrete_type_id", pInstance->get_concrete()->get_concrete_id().getObjectID() },
              { "member_type_id", pDimension->get_concrete_id().getSubObjectID() },
              { "instance", args.get( pInstance ) } } );

        os << args.inja.render( szTemplate, templateData );
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
{
    // ReadLink
    return static_cast< o_{{ concrete_type_id }}* >( {{ instance }}.getHeap() )->m_{{ member_type_id }};
}
)TEMPLATE";
    // clang-format on
    std::ostringstream os;
    {
        Concrete::Link*      pLink          = pReadLink->get_concrete_link();
        auto                 pLinkReference = pLink->get_link_reference();
        Variables::Instance* pInstance      = pReadLink->get_instance();
        MemoryLayout::Part*  pPart          = pLinkReference->get_part();

        nlohmann::json templateData(
            { { "concrete_type_id", pInstance->get_concrete()->get_concrete_id().getObjectID() },
              { "member_type_id", pLinkReference->get_concrete_id().getSubObjectID() },
              { "instance", args.get( pInstance ) } } );

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
{
    switch( overload )
    {
        case WriteOperation::DEFAULT:
        {
            //mega::RelationID{ {{ relation_id_lower }}, {{ relation_id_upper }} }
        }
        break;
        case WriteOperation::REMOVE:
        {
            //mega::RelationID{ {{ relation_id_lower }}, {{ relation_id_upper }} }
        }
        break;
        case WriteOperation::RESET:
        {
            //mega::RelationID{ {{ relation_id_lower }}, {{ relation_id_upper }} }
        }
        break;
        case WriteOperation::TOTAL_WRITE_OPERATIONS:
        default:
            throw mega::runtime::JITException{ "Unrecognised write link overload" };
        break;
    }
}
return {{ instance }};
)TEMPLATE";
    // clang-format on

    std::ostringstream os;
    {
        Concrete::Link*      pLink          = pWriteLink->get_concrete_link();
        auto                 pLinkReference = pLink->get_link_reference();
        Variables::Instance* pInstance      = pWriteLink->get_instance();
        mega::RelationID     relationID     = pLink->get_link_interface()->get_relation()->get_id();

        VERIFY_RTE_MSG( pInstance->get_concrete() == pLink, "Something is wrong!!" );

        nlohmann::json templateData( { { "relation_id_lower", printTypeID( relationID.getLower() ) },
                                       { "relation_id_upper", printTypeID( relationID.getUpper() ) },
                                       { "instance", args.get( pInstance ) }

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

} // namespace

void generateInstructions( const RetailDatabase&                               database,
                           FinalStage::Invocations::Instructions::Instruction* pInstruction,
                           const VariableMap&                                  variables,
                           nlohmann::json&                                     data,
                           TemplateEngine&                                     templateEngine )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    if( auto pInstructionGroup = db_cast< Instructions::InstructionGroup >( pInstruction ) )
    {
        bool bTailRecursion = true;
        if( auto pParentDerivation = db_cast< Instructions::ParentDerivation >( pInstructionGroup ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pParentDerivation );
        }
        else if( auto pChildDerivation = db_cast< Instructions::ChildDerivation >( pInstructionGroup ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pChildDerivation );
        }
        else if( auto pEnumDerivation = db_cast< Instructions::EnumDerivation >( pInstructionGroup ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pEnumDerivation );
        }
        else if( auto pEnumeration = db_cast< Instructions::Enumeration >( pInstructionGroup ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pEnumeration );
        }
        else if( auto pDimensionReferenceRead = db_cast< Instructions::DimensionReferenceRead >( pInstructionGroup ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pDimensionReferenceRead );
        }
        else if( auto pMonoReference = db_cast< Instructions::MonoReference >( pInstructionGroup ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pMonoReference );
        }
        else if( auto pPolyReference = db_cast< Instructions::PolyReference >( pInstructionGroup ) )
        {
            bTailRecursion = false;

            {
                std::ostringstream os;
                os << "// PolyReference\n";
                const Variables::Reference* pReference = pPolyReference->get_from_reference();

                os << "switch( " << Args::get( variables, pReference ) << ".getType() )\n";
                os << "{";
                data[ "assignments" ].push_back( os.str() );
            }

            {
                for( auto pChildInstruction : pInstructionGroup->get_children() )
                {
                    generateInstructions( database, pChildInstruction, variables, data, templateEngine );
                }
            }

            {
                std::ostringstream os;
                os << "}";
                data[ "assignments" ].push_back( os.str() );
            }
        }
        else if( auto pPolyCase = db_cast< Instructions::PolyCase >( pInstructionGroup ) )
        {
            bTailRecursion = false;

            {
                std::ostringstream os;
                os << "// PolyCase\n";

                const Variables::Reference* pReference = pPolyCase->get_reference();
                const Variables::Instance*  pInstance  = pPolyCase->get_to();

                os << "case " << printTypeID( pInstance->get_concrete()->get_concrete_id() ) << " :\n";
                os << "{\n";
                os << Args::get( variables, pInstance ) << " = mega::reference::make( "
                   << Args::get( variables, pReference ) << ", "
                   << printTypeID( pInstance->get_concrete()->get_concrete_id() ) << " );\n";
                data[ "assignments" ].push_back( os.str() );
            }

            {
                for( auto pChildInstruction : pInstructionGroup->get_children() )
                {
                    generateInstructions( database, pChildInstruction, variables, data, templateEngine );
                }
            }

            {
                std::ostringstream os;
                os << "}";
                data[ "assignments" ].push_back( os.str() );
            }
        }
        else if( db_cast< Instructions::Elimination >( pInstructionGroup ) )
        {
            // do nothing
        }
        else if( db_cast< Instructions::Failure >( pInstructionGroup ) )
        {
            THROW_RTE( "Invalid Failure instruction" );
        }
        else if( db_cast< Instructions::Prune >( pInstructionGroup ) )
        {
            THROW_RTE( "Invalid Prune instruction" );
        }
        else
        {
            THROW_RTE( "Unknown instruction type" );
        }

        if( bTailRecursion )
        {
            for( auto pChildInstruction : pInstructionGroup->get_children() )
            {
                generateInstructions( database, pChildInstruction, variables, data, templateEngine );
            }
        }
    }
    else if( auto pOperation = db_cast< FinalStage::Invocations::Operations::Operation >( pInstruction ) )
    {
        using namespace FinalStage::Invocations::Operations;

        if( auto pAllocate = db_cast< Allocate >( pOperation ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pAllocate );
        }
        else if( auto pCall = db_cast< Call >( pOperation ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pCall );
        }
        else if( auto pStart = db_cast< Start >( pOperation ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pStart );
        }
        else if( auto pStop = db_cast< Stop >( pOperation ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pStop );
        }
        else if( auto pSave = db_cast< Save >( pOperation ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pSave );
        }
        else if( auto pLoad = db_cast< Load >( pOperation ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pLoad );
        }
        else if( auto pFiles = db_cast< Move >( pOperation ) )
        {
            THROW_TODO;
        }
        else if( auto pGet = db_cast< GetAction >( pOperation ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pGet );
        }
        else if( auto pGetDimension = db_cast< GetDimension >( pOperation ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pGetDimension );
        }
        else if( auto pRead = db_cast< Read >( pOperation ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pRead );
        }
        else if( auto pWrite = db_cast< Write >( pOperation ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pWrite );
        }
        else if( auto pReadLink = db_cast< ReadLink >( pOperation ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pReadLink );
        }
        else if( auto pWriteLink = db_cast< WriteLink >( pOperation ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pWriteLink );
        }
        else if( auto pRange = db_cast< Range >( pOperation ) )
        {
            gen( Args{ database, variables, data, templateEngine }, pRange );
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

VariableMap
generateVariables( const std::vector< ::FinalStage::Invocations::Variables::Variable* >& invocationVariables,
                   ::FinalStage::Invocations::Variables::Context*                        pRootContext,
                   nlohmann::json&                                                       data )
{
    using namespace FinalStage;
    using namespace FinalStage::Invocations;

    VariableMap variables;

    int iVariableCounter = 0;
    for( auto pVariable : invocationVariables )
    {
        if( auto pInstanceVar = db_cast< Variables::Instance >( pVariable ) )
        {
            std::ostringstream osName;
            {
                osName << "var_" << pInstanceVar->get_concrete()->get_interface()->get_identifier() << "_"
                       << iVariableCounter++;
            }
            variables.insert( { pVariable, osName.str() } );

            std::ostringstream osVar;
            {
                osVar << "mega::reference " << osName.str() << ";";
            }
            data[ "variables" ].push_back( osVar.str() );
        }
        else if( auto pDimensionVar = db_cast< Variables::Dimension >( pVariable ) )
        {
            auto               types    = pDimensionVar->get_types();
            Concrete::Context* pContext = types.front();

            std::ostringstream osName;
            {
                osName << "var_" << iVariableCounter++;
            }
            variables.insert( { pVariable, osName.str() } );

            std::ostringstream osVar;
            {
                osVar << "mega::reference " << osName.str() << ";";
            }
            data[ "variables" ].push_back( osVar.str() );
        }
        else if( auto pContextVar = db_cast< Variables::Context >( pVariable ) )
        {
            auto               types    = pContextVar->get_types();
            Concrete::Context* pContext = types.front();

            std::ostringstream osName;
            {
                osName << "var_" << iVariableCounter++;
            }
            variables.insert( { pVariable, osName.str() } );
            std::ostringstream osVar;
            {
                osVar << "mega::reference " << osName.str() << ";";
            }
            data[ "variables" ].push_back( osVar.str() );

            if( pRootContext == pContextVar )
            {
                // generate initial assignment
                std::ostringstream os;
                os << osName.str() << " = *this;";
                data[ "assignments" ].push_back( os.str() );
            }
        }
        else
        {
            THROW_RTE( "Unknown variable type" );
        }
    }
    return variables;
}

void recurseInvocations( const InvocationInfo& invocationInfo, TemplateEngine& templateEngine,
                         CleverUtility::IDList& namespaces, CleverUtility::IDList& types,
                         FinalStage::Interface::IContext* pContext, nlohmann::json& invocations )
{
    using namespace FinalStage;

    CleverUtility c( types, pContext->get_identifier() );

    std::ostringstream osNested;
    for( Interface::IContext* pChildContext : pContext->get_children() )
    {
        recurseInvocations( invocationInfo, templateEngine, namespaces, types, pChildContext, invocations );
    }

    nlohmann::json contextInvocations = nlohmann::json::array();
    {
        for( const auto& [ id, pInvocation ] : invocationInfo.contextInvocations[ pContext ] )
        {
            {
                std::ostringstream osInvocation;

                nlohmann::json invocation( { { "name", invocationInfo.generateInvocationName( pInvocation ) },
                                             { "return_type", pInvocation->get_return_type_str() },
                                             { "result_type_id", invocationInfo.generateResultTypeID( pInvocation ) },
                                             { "explicit_operation", mega::getExplicitOperationString(
                                                                         pInvocation->get_explicit_operation() ) },

                                             { "has_namespaces", !namespaces.empty() },
                                             { "namespaces", namespaces },
                                             { "types", types },

                                             { "variables", nlohmann::json::array() },
                                             { "assignments", nlohmann::json::array() }

                } );

                const VariableMap variables = generateVariables(
                    pInvocation->get_variables(), pInvocation->get_root_instruction()->get_context(), invocation );

                RetailDatabase database;
                for( auto pInstruction : pInvocation->get_root_instruction()->get_children() )
                {
                    generateInstructions( database, pInstruction, variables, invocation, templateEngine );
                }

                templateEngine.renderInvocation( invocation, osInvocation );

                invocations.push_back( osInvocation.str() );
            }
        }
    }
}

} // namespace driver::retail

#endif // GUARD_2023_May_23_invocations
