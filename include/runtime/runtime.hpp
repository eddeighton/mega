
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

#ifndef GUARD_2023_December_06_runtime
#define GUARD_2023_December_06_runtime

#include "runtime/clang.hpp"
#include "runtime/function_provider.hpp"
#include "runtime/functor_id.hxx"
#include "runtime/orc.hpp"

#include "il/frontend/factory.hpp"

#include "mega/values/service/program.hpp"
#include "mega/values/compilation/megastructure_installation.hpp"

#include <unordered_map>

namespace mega::runtime
{

class Runtime : public FunctionProvider
{
    struct FunctionInfo
    {
        void**           pFunction;
        Orc::Module::Ptr pModule;
    };
    using FunctionMap = std::unordered_map< FunctorID, FunctionInfo, FunctorID::Hash >;

public:
    Runtime( const boost::filesystem::path& tempDir, const MegastructureInstallation& megaInstall );

    void             loadProgram( const service::Program& program );
    void             unloadProgram();
    service::Program getProgram() const;

    // FunctionProvider
    virtual void getFunction( service::StashProvider& stashProvider, const FunctorID& functionID,
                              void** ppFunction ) override;

private:
    boost::filesystem::path m_tempDir;
    Clang                   m_clang;
    service::Program        m_program;
    FunctionMap             m_materialisedFunctions;
    il::Factory             m_materialisedFunctionFactory;
    Orc                     m_orc;
};

} // namespace mega::runtime

#endif // GUARD_2023_December_06_runtime
