
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

void CodeGenerator::generate_decision( const LLVMCompiler& compiler, const JITDatabase& database, TypeID concreteTypeID,
                                       std::ostream& os )
{
    // clang-format off
static const char* szTemplate =
R"TEMPLATE(

#include "mega/values/native_types.hpp"
#include "mega/values/runtime/reference.hpp"

void decision_{{ type_id }}( const mega::reference& source )
{
}

)TEMPLATE";
    // clang-format on

    std::ostringstream osCPPCode;
    try
    {
        nlohmann::json templateData(
            { { "type_id", printTypeID( concreteTypeID ) }, { "concrete_types", nlohmann::json::array() }

            } );

        osCPPCode << m_pInja->render( szTemplate, templateData );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::gen_cast: {}", ex.what() );
        THROW_RTE( "inja::InjaError in CodeGenerator::gen_cast: " << ex.what() );
    }
    std::ostringstream osModule;
    osModule << "decision_" << printTypeID( concreteTypeID );
    compiler.compileToLLVMIR( osModule.str(), osCPPCode.str(), os, std::nullopt );
}

} // namespace mega::runtime
