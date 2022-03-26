
#include "database/io/environment.hpp"

#include "database/io/source_listing.hpp"
#include "database/io/manifest.hpp"

#include "common/file.hpp"

namespace mega
{
namespace io
{
    const std::string Environment::MEGA_EXTENSION( ".mega" );
    const std::string Environment::DB_EXTENSION( ".db" );
    const std::string Environment::CPP_HEADER_EXTENSION( ".hpp" );
    const std::string Environment::CPP_SOURCE_EXTENSION( ".cpp" );

    Environment::Environment( const Path& rootSourceDir,
                              const Path& rootBuildDir,
                              const Path& sourceDir,
                              const Path& buildDir )
        : m_rootSourceDir( rootSourceDir )
        , m_rootBuildDir( rootBuildDir )
        , m_sourceDir( sourceDir )
        , m_buildDir( buildDir )
    {
    }

    using Path = Environment::Path;

    Path Environment::stashDir() const
    {
        return m_buildDir / "stash";
    }

    Path Environment::sourceDir() const
    {
        return m_sourceDir;
    }

    Path Environment::buildDir() const
    {
        return m_buildDir;
    }

    Path Environment::project_manifest() const
    {
        return m_buildDir / "project_manifest.txt";
    }

    Path Environment::source_list() const
    {
        return m_buildDir / "source_list.txt";
    }

    Path Environment::objectAST( const Path& megaSourcePath ) const
    {
        std::ostringstream os;
        os << megaSourcePath.filename().string() << ".ast" << DB_EXTENSION;
        return boost::filesystem::edsCannonicalise( m_buildDir / os.str() );
    }

    Path Environment::objectBody( const Path& megaSourcePath ) const
    {
        std::ostringstream os;
        os << megaSourcePath.filename().string() << ".body" << DB_EXTENSION;
        return boost::filesystem::edsCannonicalise( m_buildDir / os.str() );
    }

} // namespace io
} // namespace mega
