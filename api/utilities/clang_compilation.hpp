
#ifndef BUILDSYSTEM_TOOLS_15_SEPT_2020
#define BUILDSYSTEM_TOOLS_15_SEPT_2020

#include "common/task.hpp"
#include "database/common/object_info.hpp"

#include <boost/filesystem/path.hpp>

#include <optional>
#include <string>
#include <vector>

namespace mega
{
namespace utilities
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

    std::optional< boost::filesystem::path >      egdb;
    std::optional< boost::filesystem::path >      egtu;
    std::optional< mega::io::ObjectInfo::FileID > egtuid;

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

    std::string operator()() const;
};

}; // namespace utilities

} // namespace mega

#endif // BUILDSYSTEM_TOOLS_15_SEPT_2020