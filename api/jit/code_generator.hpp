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
#include "database/database.hpp"

#include "service/protocol/common/megastructure_installation.hpp"
#include "service/protocol/common/project.hpp"

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

    CodeGenerator( const mega::network::MegastructureInstallation& megastructureInstallation,
                   const mega::network::Project&                   project );

    void generate_allocation( const LLVMCompiler& compiler, const DatabaseInstance& database, mega::TypeID objectTypeID,
                              std::ostream& os );
    void generate_allocate( const LLVMCompiler& compiler, const DatabaseInstance& database,
                            const mega::InvocationID& invocationID, std::ostream& os );
    void generate_read( const LLVMCompiler& compiler, const DatabaseInstance& database,
                        const mega::InvocationID& invocationID, std::ostream& os );
    void generate_write( const LLVMCompiler& compiler, const DatabaseInstance& database,
                         const mega::InvocationID& invocationID, std::ostream& os );
    void generate_call( const LLVMCompiler& compiler, const DatabaseInstance& database,
                        const mega::InvocationID& invocationID, std::ostream& os );
    void generate_start( const LLVMCompiler& compiler, const DatabaseInstance& database,
                         const mega::InvocationID& invocationID, std::ostream& os );
    void generate_stop( const LLVMCompiler& compiler, const DatabaseInstance& database,
                        const mega::InvocationID& invocationID, std::ostream& os );
    void generate_save( const LLVMCompiler& compiler, const DatabaseInstance& database,
                        const mega::InvocationID& invocationID, std::ostream& os );
    void generate_load( const LLVMCompiler& compiler, const DatabaseInstance& database,
                        const mega::InvocationID& invocationID, std::ostream& os );
    void generate_program( const LLVMCompiler& compiler, const DatabaseInstance& database, std::ostream& os );

private:
    nlohmann::json generate( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                             std::string& strName ) const;

private:
    std::shared_ptr< Pimpl > m_pPimpl;
};
} // namespace mega::runtime

#endif // CODE_GENERATOR_16_AUG_2022
