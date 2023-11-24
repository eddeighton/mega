
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

#include "mega/common_strings.hpp"

#include "symbol_utils.hpp"

namespace FinalStage
{
#include "compiler/concrete_printer.hpp"
}

namespace mega::runtime
{
using namespace FinalStage;

std::string recurseStep( Inja& inja, Indent& indent, const Derivation::Step* pStep )
{
    std::ostringstream os;

    if( auto pOr = db_cast< const Derivation::Or >( pStep ) )
    {
        static const char* szTemplate =
            R"TEMPLATE(
{{ indent }}
)TEMPLATE";

        auto edges = pStep->get_edges();
        VERIFY_RTE( edges.size() <= 1 );

        if( edges.empty() )
        {
            nlohmann::json templateData( {

                { "indent", indent.str() }

            } );
            os << inja.render( szTemplate, templateData );
        }
        else
        {
            auto pEdge = edges.front();

            ++indent;
            nlohmann::json templateData( {

                { "indent", indent.str() }, { "nested", recurseStep( inja, indent, pEdge->get_next() ) }

            } );
            --indent;
            os << inja.render( szTemplate, templateData );
        }
    }
    else
    {
        THROW_RTE( "Unknown step type" );
    }

    return os.str();
}

void CodeGenerator::generate_dispatcher( const LLVMCompiler& compiler, const JITDatabase& database,
                                         const FinalStage::Concrete::Interupt*   pInterupt,
                                         const FinalStage::Operations::EventDispatch* pDispatch, std::ostream& os )
{
    // clang-format off
static const char* szTemplate =
R"TEMPLATE(
    {{ code }}
)TEMPLATE";
    // clang-format on

    std::ostringstream osCPPCode;
    try
    {
        Indent indent;
        ++indent;

        nlohmann::json templateData( { { "code", "" } } ); // recurseStep( *m_pInja, indent, pDispatch )
        osCPPCode << m_pInja->render( szTemplate, templateData );
    }
    catch( inja::InjaError& ex )
    {
        SPDLOG_ERROR( "inja::InjaError in CodeGenerator::generate_dispatcher: {} for {}", ex.what(),
                      Concrete::printContextFullType( pInterupt ) );
        THROW_RTE( "inja::InjaError in CodeGenerator::generate_dispatcher: " << ex.what() );
    }
    std::ostringstream osModule;
    osModule << "decision_" << printTypeID( pInterupt->get_concrete_id() );
    compiler.compileToLLVMIR( osModule.str(), osCPPCode.str(), os, std::nullopt );
}

} // namespace mega::runtime
