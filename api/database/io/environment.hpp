#ifndef DRIVER_ENVIRONMENT_24_MARCH_2022
#define DRIVER_ENVIRONMENT_24_MARCH_2022

#include "boost/filesystem/path.hpp"

#include <string>

namespace mega
{
namespace io
{
    class Environment
    {
    public:
        using Path = boost::filesystem::path;

        static const std::string MEGA_EXTENSION;
        static const std::string DB_EXTENSION;
        static const std::string CPP_HEADER_EXTENSION;
        static const std::string CPP_SOURCE_EXTENSION;

        Environment( const Path& rootSourceDir,
                     const Path& rootBuildDir );

        Path stashDir() const;

        Path rootSourceDir() const;
        Path rootBuildDir() const;

        Path project_manifest() const;
        Path source_list( const Path& srcDir ) const;

        Path objectAST( const Path& megaSource ) const;
        Path objectBody( const Path& megaSource ) const;

    private:
        Path buildDirFromSrcDir( const Path& srcDir ) const;

        const Path& m_rootSourceDir;
        const Path& m_rootBuildDir;
    };
} // namespace io
} // namespace mega

#endif // DRIVER_ENVIRONMENT_24_MARCH_2022
