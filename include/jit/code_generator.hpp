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

#ifndef CODE_GENERATOR_16_AUG_2022
#define CODE_GENERATOR_16_AUG_2022

#include "api.hpp"
#include "inja.hpp"
#include "function_declarations.hpp"
#include "indent.hpp"

#include "environment/jit_database.hpp"

#include "invocation_functions.hxx"
#include "operator_functions.hxx"

#include "mega/values/compilation/megastructure_installation.hpp"
#include "mega/values/service/project.hpp"

#include "mega/values/compilation/relation_id.hpp"

#include <boost/asio/spawn.hpp>

#include <memory>

namespace mega::runtime
{

class JIT_EXPORT CodeGenerator
{
    class Pimpl;

public:
    class LLVMCompiler
    {
    public:
        virtual void compileToLLVMIR( const std::string& strName, const std::string& strCPPCode, std::ostream& osIR,
                                      std::optional< const FinalStage::Components::Component* > pComponent ) const
            = 0;
    };

    CodeGenerator( const mega::MegastructureInstallation& megastructureInstallation, const mega::Project& project );

    void generate_alllocator( const LLVMCompiler& compiler, const JITDatabase& database, mega::TypeID objectTypeID,
                              std::ostream& os );

    void generate_invocation( const LLVMCompiler& compiler, const JITDatabase& database,
                              const mega::InvocationID&               invocationID,
                              mega::runtime::invocation::FunctionType invocationType, std::ostream& os );

    void generate_relation( const LLVMCompiler& compiler, const JITDatabase& database, const RelationID& relationID,
                            std::ostream& os );

    void generate_program( const LLVMCompiler& compiler, const JITDatabase& database, std::ostream& os );

    void generate_operator( const LLVMCompiler& compiler, const JITDatabase& database, TypeID target,
                            mega::runtime::operators::FunctionType invocationType, std::ostream& os );

    void generate_decision( const LLVMCompiler& compiler, const JITDatabase& database, TypeID target,
                            std::ostream& os );

    void generate_dispatcher( const LLVMCompiler& compiler, const JITDatabase& database,
                              const FinalStage::Concrete::Interupt*        pInterupt,
                              const FinalStage::Operations::EventDispatch* pDispatch, std::ostream& os );

private:
    /*VariableMap
    generateVariables( const std::vector< ::FinalStage::Instructions::Variables::Variable* >& invocationVariables,
                       nlohmann::json&                                                        data,
                       Indent&                                                                indent ) const;

    void generateInstructions( const JITDatabase& database, const FinalStage::Operations::Invocation* pInvocation,
                               const FinalStage::Instructions::Calculation::Instruction* pInstruction,
                               const VariableMap& variables, FunctionDeclarations& functions, nlohmann::json& data,
                               Indent& indent ) const;*/

    nlohmann::json generate( const JITDatabase& database, const mega::InvocationID& invocationID,
                             std::string& strName ) const;

private:
    Inja::Ptr m_pInja;
};

} // namespace mega::runtime

#endif // CODE_GENERATOR_16_AUG_2022
