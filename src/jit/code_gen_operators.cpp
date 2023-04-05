
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

void gen_cast( const CodeGenerator::LLVMCompiler& compiler, Inja& inja, const JITDatabase& database, TypeID target,
               std::ostream& os )
{
    // clang-format off
static const char* szTemplate =
R"TEMPLATE(

#include "mega/native_types.hpp"
#include "mega/reference.hpp"

mega::reference mega_cast_{{ target }}( const mega::reference& source )
{
    switch( source.getType() )
    {
{% for type in types %}
        case {{ type }}: return source;
{% endfor %}
        default:  return mega::reference{};
    }
}

)TEMPLATE";
    // clang-format on

    std::ostringstream osCPPCode;
    try
    {
        nlohmann::json templateData( { { "target", printTypeID( target ) }, { "types", nlohmann::json::array() } } );

        for( TypeID type : database.getCompatibleConcreteTypes( target ) )
        {
            templateData[ "types" ].push_back( printTypeID( type ) );
        }

        osCPPCode << inja.render( szTemplate, templateData );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::gen_cast: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::gen_cast: " << ex.what() );
    }
    std::ostringstream osModule;
    osModule << "mega_cast_" << printTypeID( target );
    compiler.compileToLLVMIR( osModule.str(), osCPPCode.str(), os, std::nullopt );
}

} // namespace

void CodeGenerator::generate_operator( const LLVMCompiler& compiler, const JITDatabase& database, TypeID target,
                                       mega::runtime::operators::FunctionType invocationType, std::ostream& os )
{
    switch( invocationType )
    {
        case operators::eCast:
        {
            gen_cast( compiler, *m_pInja, database, target, os );
        }
        break;
        case operators::eActive:
        {
            //
        }
        break;
        case operators::eStopped:
        {
            //
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
