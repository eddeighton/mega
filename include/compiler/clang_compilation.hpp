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

#ifndef BUILDSYSTEM_TOOLS_15_SEPT_2020
#define BUILDSYSTEM_TOOLS_15_SEPT_2020

#include "database/sources.hpp"

#include "mega/values/compilation/tool_chain_hash.hpp"
#include "mega/values/compilation/modes.hpp"

#include "environment/environment_build.hpp"

#include "common/process.hpp"

#include <boost/filesystem/path.hpp>

#include <optional>
#include <string>
#include <vector>

namespace mega
{

class Compilation
{
    std::string                              compiler_command;
    std::optional< boost::filesystem::path > compiler_plugin;
    std::vector< std::string >               flags;
    std::vector< std::string >               defines;
    std::vector< boost::filesystem::path >   includeDirs;

    std::vector< boost::filesystem::path > inputPCH;
    boost::filesystem::path                inputFile;

    std::optional< boost::filesystem::path > outputPPH;
    std::optional< boost::filesystem::path > outputPCH;
    std::optional< boost::filesystem::path > outputObject;

    std::optional< mega::CompilationMode >   compilationMode;
    std::optional< boost::filesystem::path > srcDir, buildDir, sourceFile;

    Compilation() = default;

public:
    common::Command generatePCHVerificationCMD() const;

    enum CompilerCacheOptions
    {
        eCache_none,
        eCache_cache,
        eCache_recache
    };

    common::Command generateCompilationCMD( CompilerCacheOptions cacheOptions = eCache_none ) const;

    template < typename TComponentType >
    static inline Compilation make_includePCH_compilation( const io::BuildEnvironment& environment,
                                                           const utilities::ToolChain& toolChain,
                                                           TComponentType*             pComponent )
    {
        Compilation compilation;
        compilation.compilationMode  = CompilationMode{ CompilationMode::eNormal };
        compilation.compiler_command = toolChain.clangCompilerPath.string();

        compilation.flags       = pComponent->get_cpp_flags();
        compilation.defines     = pComponent->get_cpp_defines();
        compilation.includeDirs = pComponent->get_include_directories();

        compilation.inputFile = environment.FilePath( environment.Include() );
        compilation.outputPCH = environment.FilePath( environment.IncludePCH() );

        return compilation;
    }

    template < typename TComponentType >
    static inline Compilation make_clang_traits_analysis_compilation( const io::BuildEnvironment& environment,
                                                                      const utilities::ToolChain& toolChain,
                                                                      TComponentType*             pComponent

    )
    {
        Compilation compilation;

        compilation.compilationMode = CompilationMode{ CompilationMode::eTraits };

        compilation.compiler_command = toolChain.clangCompilerPath.string();
        compilation.compiler_plugin  = toolChain.clangPluginPath;

        compilation.srcDir     = environment.srcDir();
        compilation.buildDir   = environment.buildDir();
        compilation.sourceFile = environment.FilePath( environment.project_manifest() );

        compilation.flags       = pComponent->get_cpp_flags();
        compilation.defines     = pComponent->get_cpp_defines();
        compilation.includeDirs = pComponent->get_include_directories();

        compilation.inputPCH = { environment.FilePath( environment.IncludePCH() ) };
        compilation.outputPCH = environment.FilePath( environment.TraitsPCH() );

        compilation.inputFile = environment.FilePath( environment.ClangTraits() );

        return compilation;
    }
    
    template < typename TComponentType >
    static inline Compilation make_cpp_pch_compilation( const io::BuildEnvironment& environment,
                                                        const utilities::ToolChain& toolChain,
                                                        TComponentType*             pComponent,
                                                        const io::cppFilePath&      sourceFile

    )
    {
        Compilation compilation;

        compilation.compilationMode = CompilationMode{ CompilationMode::eInvocations };

        compilation.compiler_command = toolChain.clangCompilerPath.string();
        compilation.compiler_plugin  = toolChain.clangPluginPath;

        compilation.srcDir     = environment.srcDir();
        compilation.buildDir   = environment.buildDir();
        compilation.sourceFile = environment.FilePath( sourceFile );

        compilation.flags       = pComponent->get_cpp_flags();
        compilation.defines     = pComponent->get_cpp_defines();
        compilation.includeDirs = pComponent->get_include_directories();

        // since compiling copied cpp -> hpp in build folder ensure cpp directory in include dirs
        compilation.includeDirs.push_back( environment.FilePath( sourceFile ).parent_path() );

        compilation.inputPCH
            = { environment.FilePath( environment.IncludePCH() ), environment.FilePath( environment.TraitsPCH() ) };

        compilation.inputFile = environment.FilePath( environment.CPPSource( sourceFile ) );
        compilation.outputPCH = environment.FilePath( environment.CPPPCH( sourceFile ) );

        return compilation;
    }

    template < typename TComponentType >
    static inline Compilation make_cpp_obj_compilation( const io::BuildEnvironment& environment,
                                                        const utilities::ToolChain& toolChain,
                                                        TComponentType*             pComponent,
                                                        const io::cppFilePath&      sourceFile

    )
    {
        Compilation compilation;

        compilation.compilationMode  = CompilationMode{ CompilationMode::eNormal };
        compilation.compiler_command = toolChain.clangCompilerPath.string();

        compilation.flags       = pComponent->get_cpp_flags();
        compilation.defines     = pComponent->get_cpp_defines();
        compilation.includeDirs = pComponent->get_include_directories();

        compilation.inputPCH
            = { environment.FilePath( environment.IncludePCH() ), environment.FilePath( environment.TraitsPCH() ),
                environment.FilePath( environment.CPPPCH( sourceFile ) ) };

        compilation.inputFile    = environment.FilePath( environment.CPPImpl( sourceFile ) );
        compilation.outputObject = environment.FilePath( environment.CPPObj( sourceFile ) );

        return compilation;
    }
};

} // namespace mega

#endif // BUILDSYSTEM_TOOLS_15_SEPT_2020