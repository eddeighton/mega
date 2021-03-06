
#ifndef BUILDSYSTEM_TOOLS_15_SEPT_2020
#define BUILDSYSTEM_TOOLS_15_SEPT_2020

#include "database/common/object_info.hpp"
#include "database/common/environment_build.hpp"

#include "database/types/sources.hpp"
#include "utilities/tool_chain_hash.hpp"

#include "database/types/modes.hpp"

#include "common/task.hpp"

#include <boost/filesystem/path.hpp>

#include <optional>
#include <string>
#include <vector>

namespace mega
{
struct Compilation
{
    boost::filesystem::path                  compiler;
    std::optional< boost::filesystem::path > compiler_plugin;
    std::vector< std::string >               flags;
    std::vector< std::string >               defines;
    std::vector< boost::filesystem::path >   includeDirs;

    std::vector< boost::filesystem::path > inputPCH;
    boost::filesystem::path                inputFile;

    std::optional< boost::filesystem::path > outputPCH;
    std::optional< boost::filesystem::path > outputObject;

    std::optional< mega::CompilationMode >   compilationMode;
    std::optional< boost::filesystem::path > srcDir, buildDir, sourceFile;

    template < typename TComponentType >
    static inline Compilation
    make_includePCH_compilation( const io::BuildEnvironment& environment, const utilities::ToolChain& toolChain,
                                 TComponentType* pComponent, const io::megaFilePath& sourceFile )
    {
        Compilation compilation;

        compilation.compiler = toolChain.clangCompilerPath;

        compilation.flags       = pComponent->get_cpp_flags();
        compilation.defines     = pComponent->get_cpp_defines();
        compilation.includeDirs = pComponent->get_include_directories();

        compilation.inputFile = environment.FilePath( environment.Include( sourceFile ) );
        compilation.outputPCH = environment.FilePath( environment.IncludePCH( sourceFile ) );

        return compilation;
    }

    template < typename TComponentType >
    static inline Compilation make_includePCH_compilation( const io::BuildEnvironment& environment,
                                                           const utilities::ToolChain& toolChain,
                                                           TComponentType*             pComponent )
    {
        Compilation compilation;

        compilation.compiler = toolChain.clangCompilerPath;

        compilation.flags       = pComponent->get_cpp_flags();
        compilation.defines     = pComponent->get_cpp_defines();
        compilation.includeDirs = pComponent->get_include_directories();

        compilation.inputFile
            = environment.FilePath( environment.Include( pComponent->get_build_dir(), pComponent->get_name() ) );
        compilation.outputPCH
            = environment.FilePath( environment.IncludePCH( pComponent->get_build_dir(), pComponent->get_name() ) );

        return compilation;
    }

    template < typename TComponentType >
    static inline Compilation make_interfacePCH_compilation( const io::BuildEnvironment& environment,
                                                             const utilities::ToolChain& toolChain,
                                                             TComponentType*             pComponent,
                                                             const io::megaFilePath&     sourceFile

    )
    {
        Compilation compilation;

        compilation.compilationMode = CompilationMode::eInterface;

        compilation.compiler        = toolChain.clangCompilerPath;
        compilation.compiler_plugin = toolChain.clangPluginPath;

        compilation.srcDir     = environment.srcDir();
        compilation.buildDir   = environment.buildDir();
        compilation.sourceFile = environment.FilePath( sourceFile );

        compilation.flags       = pComponent->get_cpp_flags();
        compilation.defines     = pComponent->get_cpp_defines();
        compilation.includeDirs = pComponent->get_include_directories();

        compilation.inputPCH = { environment.FilePath( environment.IncludePCH( sourceFile ) ) };

        compilation.inputFile = environment.FilePath( environment.Interface( sourceFile ) );
        compilation.outputPCH = environment.FilePath( environment.InterfacePCH( sourceFile ) );

        return compilation;
    }

    template < typename TComponentType >
    static inline Compilation make_interfacePCH_compilation( const io::BuildEnvironment& environment,
                                                             const utilities::ToolChain& toolChain,
                                                             TComponentType*             pComponent

    )
    {
        Compilation compilation;

        compilation.compilationMode = CompilationMode::eLibrary;

        compilation.compiler        = toolChain.clangCompilerPath;
        compilation.compiler_plugin = toolChain.clangPluginPath;

        compilation.srcDir     = environment.srcDir();
        compilation.buildDir   = environment.buildDir();

        compilation.flags       = pComponent->get_cpp_flags();
        compilation.defines     = pComponent->get_cpp_defines();
        compilation.includeDirs = pComponent->get_include_directories();

        compilation.inputPCH = { environment.FilePath( environment.IncludePCH( pComponent->get_build_dir(), pComponent->get_name() ) ) };

        compilation.inputFile = environment.FilePath( environment.Interface( pComponent->get_build_dir(), pComponent->get_name() ) );
        compilation.outputPCH = environment.FilePath( environment.InterfacePCH( pComponent->get_build_dir(), pComponent->get_name() ) );

        return compilation;
    }

    template < typename TComponentType >
    static inline Compilation make_operationsObj_compilation( const io::BuildEnvironment& environment,
                                                              const utilities::ToolChain& toolChain,
                                                              TComponentType*             pComponent,
                                                              const io::megaFilePath&     sourceFile

    )
    {
        Compilation compilation;

        compilation.compilationMode = CompilationMode::eOperations;

        compilation.compiler        = toolChain.clangCompilerPath;
        compilation.compiler_plugin = toolChain.clangPluginPath;

        compilation.srcDir     = environment.srcDir();
        compilation.buildDir   = environment.buildDir();
        compilation.sourceFile = environment.FilePath( sourceFile );

        compilation.flags       = pComponent->get_cpp_flags();
        compilation.defines     = pComponent->get_cpp_defines();
        compilation.includeDirs = pComponent->get_include_directories();

        compilation.inputPCH = { environment.FilePath( environment.IncludePCH( sourceFile ) ),
                                 environment.FilePath( environment.InterfacePCH( sourceFile ) ) };

        compilation.inputFile    = environment.FilePath( environment.Operations( sourceFile ) );
        compilation.outputObject = environment.FilePath( environment.OperationsObj( sourceFile ) );

        return compilation;
    }

    template < typename TComponentType >
    static inline Compilation make_cpp_compilation( const io::BuildEnvironment& environment,
                                                    const utilities::ToolChain& toolChain,
                                                    TComponentType*             pComponent,
                                                    const io::cppFilePath&      sourceFile

    )
    {
        Compilation compilation;

        compilation.compilationMode = CompilationMode::eCPP;

        compilation.compiler        = toolChain.clangCompilerPath;
        compilation.compiler_plugin = toolChain.clangPluginPath;

        compilation.srcDir     = environment.srcDir();
        compilation.buildDir   = environment.buildDir();
        compilation.sourceFile = environment.FilePath( sourceFile );

        compilation.flags       = pComponent->get_cpp_flags();
        compilation.defines     = pComponent->get_cpp_defines();
        compilation.includeDirs = pComponent->get_include_directories();

        compilation.inputPCH = {
            environment.FilePath( environment.IncludePCH( pComponent->get_build_dir(), pComponent->get_name() ) ),
            environment.FilePath( environment.InterfacePCH( pComponent->get_build_dir(), pComponent->get_name() ) ) };

        compilation.inputFile    = environment.FilePath( sourceFile );
        compilation.outputObject = environment.FilePath( environment.Obj( sourceFile ) );

        return compilation;
    }

    std::string operator()() const;

private:
    Compilation() {}
};

} // namespace mega

#endif // BUILDSYSTEM_TOOLS_15_SEPT_2020