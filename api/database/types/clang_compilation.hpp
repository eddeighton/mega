
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
        compilation.includeDirs = pComponent->get_includeDirectories();

        compilation.inputFile = environment.FilePath( environment.Include( sourceFile ) );
        compilation.outputPCH = environment.FilePath( environment.IncludePCH( sourceFile ) );

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
        compilation.includeDirs = pComponent->get_includeDirectories();

        compilation.inputPCH  = { environment.FilePath( environment.IncludePCH( sourceFile ) ) };
        compilation.inputFile = environment.FilePath( environment.Interface( sourceFile ) );
        compilation.outputPCH = environment.FilePath( environment.InterfacePCH( sourceFile ) );

        return compilation;
    }

    template < typename TComponentType >
    static inline Compilation make_genericsPCH_compilation( const io::BuildEnvironment& environment,
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
        compilation.includeDirs = pComponent->get_includeDirectories();

        compilation.inputPCH = { environment.FilePath( environment.IncludePCH( sourceFile ) ),
                                 environment.FilePath( environment.InterfacePCH( sourceFile ) ) };

        compilation.inputFile = environment.FilePath( environment.Generics( sourceFile ) );
        compilation.outputPCH = environment.FilePath( environment.GenericsPCH( sourceFile ) );

        return compilation;
    }

    template < typename TComponentType >
    static inline Compilation make_operationsPCH_compilation( const io::BuildEnvironment& environment,
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
        compilation.includeDirs = pComponent->get_includeDirectories();

        compilation.inputPCH = { environment.FilePath( environment.IncludePCH( sourceFile ) ),
                                 environment.FilePath( environment.InterfacePCH( sourceFile ) ),
                                 environment.FilePath( environment.GenericsPCH( sourceFile ) ) };

        compilation.inputFile = environment.FilePath( environment.Operations( sourceFile ) );
        compilation.outputPCH = environment.FilePath( environment.OperationsPCH( sourceFile ) );

        return compilation;
    }

    template < typename TComponentType >
    static inline Compilation make_implementationObj_compilation( const io::BuildEnvironment& environment,
                                                                  const utilities::ToolChain& toolChain,
                                                                  TComponentType*             pComponent,
                                                                  const io::megaFilePath&     sourceFile )
    {
        Compilation compilation;

        compilation.compilationMode = CompilationMode::eImplementation;

        compilation.compiler        = toolChain.clangCompilerPath;
        compilation.compiler_plugin = toolChain.clangPluginPath;

        compilation.srcDir     = environment.srcDir();
        compilation.buildDir   = environment.buildDir();
        compilation.sourceFile = environment.FilePath( sourceFile );

        compilation.flags       = pComponent->get_cpp_flags();
        compilation.defines     = pComponent->get_cpp_defines();
        compilation.includeDirs = pComponent->get_includeDirectories();

        compilation.inputPCH = { environment.FilePath( environment.IncludePCH( sourceFile ) ),
                                 environment.FilePath( environment.InterfacePCH( sourceFile ) ),
                                 environment.FilePath( environment.GenericsPCH( sourceFile ) ),
                                 environment.FilePath( environment.OperationsPCH( sourceFile ) ) };

        compilation.inputFile    = environment.FilePath( environment.Implementation( sourceFile ) );
        compilation.outputObject = environment.FilePath( environment.ImplementationObj( sourceFile ) );

        return compilation;
    }

    std::string operator()() const;

private:
    Compilation() {}
};

} // namespace mega

#endif // BUILDSYSTEM_TOOLS_15_SEPT_2020