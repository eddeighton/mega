
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

namespace mega::runtime
{

namespace
{

void gen_new( const CodeGenerator::LLVMCompiler& compiler, Inja& inja, const JITDatabase& database,
              TypeID interfaceTypeID, std::ostream& os )
{
    // clang-format off
static const char* szTemplate =
R"TEMPLATE(

#include "mega/native_types.hpp"
#include "mega/reference.hpp"
#include "service/protocol/common/mpo_context_interface.hpp" 

mega::reference mega_new_{{ interface_type }}()
{
    return ::mega::runtime::allocate( mega::TypeID{ {{ concrete_type }} } );
}

)TEMPLATE";
    // clang-format on

    const auto concreteID = database.getSingularConcreteTypeID( interfaceTypeID );

    std::ostringstream osCPPCode;
    try
    {
        nlohmann::json templateData( {

            { "interface_type", printTypeID( interfaceTypeID ) }, 
            { "concrete_type", printTypeID( concreteID ) }

        } );

        osCPPCode << inja.render( szTemplate, templateData );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::gen_new: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::gen_new: " << ex.what() );
    }
    std::ostringstream osModule;
    osModule << "mega_new_" << printTypeID( interfaceTypeID );
    compiler.compileToLLVMIR( osModule.str(), osCPPCode.str(), os, std::nullopt );
}

void gen_delete( const CodeGenerator::LLVMCompiler& compiler, Inja& inja, const JITDatabase& database,
                 TypeID interfaceTypeID, std::ostream& os )
{
    // clang-format off
static const char* szTemplate =
R"TEMPLATE(

#include "mega/native_types.hpp"
#include "mega/reference.hpp"
#include "jit/object_functions.hxx"

void mega_delete_{{ interface_type }}( mega::reference ref )
{
    static thread_local mega::runtime::object::ObjectUnparent unparent( "operators", {{ concrete_type }} );
    unparent( ref.getObjectAddress() );
}

)TEMPLATE";
    // clang-format on

    const auto concreteID = database.getSingularConcreteTypeID( interfaceTypeID );

    std::ostringstream osCPPCode;
    try
    {
        nlohmann::json templateData( {

            { "interface_type", printTypeID( interfaceTypeID ) }, 
            { "concrete_type", printTypeID( concreteID ) }

        } );

        osCPPCode << inja.render( szTemplate, templateData );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::gen_new: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::gen_new: " << ex.what() );
    }
    std::ostringstream osModule;
    osModule << "mega_new_" << printTypeID( interfaceTypeID );
    compiler.compileToLLVMIR( osModule.str(), osCPPCode.str(), os, std::nullopt );
}

void gen_cast( const CodeGenerator::LLVMCompiler& compiler, Inja& inja, const JITDatabase& database,
               TypeID interfaceTypeID, std::ostream& os )
{
    // clang-format off
static const char* szTemplate =
R"TEMPLATE(

#include "mega/native_types.hpp"
#include "mega/reference.hpp"

mega::reference mega_cast_{{ interface_type }}( const mega::reference& source )
{
    switch( source.getType() )
    {
{% for type in concrete_types %}
        case {{ type }}: return mega::reference::make( source, mega::TypeInstance( {{ type }}, source.getInstance() ) );
{% endfor %}
        default:  return mega::reference{};
    }
}

)TEMPLATE";
    // clang-format on

    std::ostringstream osCPPCode;
    try
    {
        nlohmann::json templateData(
            { { "interface_type", printTypeID( interfaceTypeID ) }, { "concrete_types", nlohmann::json::array() } } );

        for( TypeID type : database.getCompatibleConcreteTypes( interfaceTypeID ) )
        {
            templateData[ "concrete_types" ].push_back( printTypeID( type ) );
        }

        osCPPCode << inja.render( szTemplate, templateData );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::gen_cast: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::gen_cast: " << ex.what() );
    }
    std::ostringstream osModule;
    osModule << "mega_cast_" << printTypeID( interfaceTypeID );
    compiler.compileToLLVMIR( osModule.str(), osCPPCode.str(), os, std::nullopt );
}

} // namespace

void CodeGenerator::generate_operator( const LLVMCompiler& compiler, const JITDatabase& database,
                                       TypeID interfaceTypeID, mega::runtime::operators::FunctionType operatorType,
                                       std::ostream& os )
{
    switch( operatorType )
    {
        case operators::eNew:
        {
            gen_new( compiler, *m_pInja, database, interfaceTypeID, os );
        }
        break;
        case operators::eDelete:
        {
            gen_delete( compiler, *m_pInja, database, interfaceTypeID, os );
        }
        break;
        case operators::eCast:
        {
            gen_cast( compiler, *m_pInja, database, interfaceTypeID, os );
        }
        break;
        case operators::eActive:
        {
            THROW_TODO;
        }
        break;
        case operators::eEnabled:
        {
            THROW_TODO;
        }
        break;
        default:
        case operators::TOTAL_FUNCTION_TYPES:
        {
            THROW_RTE( "Unknown operator function type" );
        }
        break;
    }
}

} // namespace mega::runtime
