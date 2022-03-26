#ifndef DRIVER_ENVIRONMENT_24_MARCH_2022
#define DRIVER_ENVIRONMENT_24_MARCH_2022

#include "common/task.hpp"
#include "common/stash.hpp"

namespace driver
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
                 const Path& rootBuildDir,
                 const Path& sourceDir,
                 const Path& buildDir );
    ~Environment();

    Path sourceDir() const;
    Path buildDir() const;

    Path project_manifest() const;
    Path source_list() const;

    Path parserAST( const Path& megaSource ) const;
    Path parserBody( const Path& megaSource ) const;

private:
    const Path& m_rootSourceDir;
    const Path& m_rootBuildDir;
    const Path& m_sourceDir;
    const Path& m_buildDir;
    task::Stash m_stash;
};

} // namespace driver

#endif // DRIVER_ENVIRONMENT_24_MARCH_2022
