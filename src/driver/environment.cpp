
#include "environment.hpp"

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
    , m_stash( buildDir )
{
}

Environment::Path Environment::parseFilePath( const Environment::Path& megaSourcePath ) const
{
    Path filename = megaSourcePath.filename();
    filename.replace_extension( DB_EXTENSION );
    return m_buildDir / filename;
}

Environment::~Environment()
{
    // stash.saveBuildHashCodes( projectTree.getBuildInfoFile() );
}

} // namespace driver
