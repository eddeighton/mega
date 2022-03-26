
#include "environment.hpp"

#include "database/io/source_listing.hpp"
#include "database/io/manifest.hpp"

namespace driver
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
    , m_stash( rootBuildDir / "stash" )
{
}

Environment::Path Environment::sourceDir() const
{
    return m_sourceDir;
}

Environment::Path Environment::buildDir() const
{
    return m_sourceDir;
}

Environment::Path Environment::project_manifest() const
{
    return mega::io::Manifest::filepath( m_buildDir );
}

Environment::Path Environment::source_list() const
{
    return mega::io::SourceListing::filepath( m_buildDir );
}

Environment::Path Environment::parserAST( const Path& megaSourcePath ) const
{
    Path filename = megaSourcePath.filename();
    filename.replace_extension( DB_EXTENSION );
    return m_buildDir / filename;
}

Environment::Path Environment::parserBody( const Path& megaSourcePath ) const
{
    Path filename = megaSourcePath.filename().string() + "_body";
    filename.replace_extension( DB_EXTENSION );
    return m_buildDir / filename;
}

Environment::~Environment()
{
    // stash.saveBuildHashCodes( projectTree.getBuildInfoFile() );
}

} // namespace driver
