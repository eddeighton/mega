
#ifndef BUILDSYSTEM_TOOLS_15_SEPT_2020
#define BUILDSYSTEM_TOOLS_15_SEPT_2020

#include "database/common/object_info.hpp"

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

    Compilation()                     = delete;
    Compilation( const Compilation& ) = delete;
    Compilation( Compilation&& )      = delete;
    Compilation& operator=( const Compilation& ) = delete;

    Compilation( const boost::filesystem::path&                compiler,
                 const std::vector< std::string >&             flags,
                 const std::vector< std::string >&             defines,
                 const std::vector< boost::filesystem::path >& includeDirs,
                 const boost::filesystem::path&                inputFile,
                 const boost::filesystem::path&                outputPCH )
        : compiler( compiler )
        , flags( flags )
        , defines( defines )
        , includeDirs( includeDirs )
        , inputFile( inputFile )
        , outputPCH( outputPCH )
    {
    }

    Compilation( const boost::filesystem::path& compiler, const boost::filesystem::path& compiler_plugin,
                 const std::vector< std::string >& flags, const std::vector< std::string >& defines,
                 const std::vector< boost::filesystem::path >& includeDirs,

                 mega::CompilationMode compilationMode, const boost::filesystem::path& srcDir,
                 const boost::filesystem::path& buildDir, const boost::filesystem::path& sourceFile,

                 const boost::filesystem::path& inputPCH, const boost::filesystem::path& inputFile,
                 const boost::filesystem::path& outputPCH )

        : compiler( compiler )
        , compiler_plugin( compiler_plugin )
        , flags( flags )
        , defines( defines )
        , includeDirs( includeDirs )

        , inputPCH( { inputPCH } )

        , compilationMode( compilationMode )
        , srcDir( srcDir )
        , buildDir( buildDir )
        , sourceFile( sourceFile )

        , inputFile( inputFile )
        , outputPCH( outputPCH )
    {
    }

    std::string operator()() const;
};

} // namespace mega

#endif // BUILDSYSTEM_TOOLS_15_SEPT_2020