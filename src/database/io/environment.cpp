
#include "database/io/environment.hpp"

#include "database/io/component_info.hpp"
#include "database/io/manifest.hpp"

#include "common/file.hpp"
#include <boost/filesystem/directory.hpp>

namespace mega
{
namespace io
{
    const std::string Environment::MEGA_EXTENSION( ".mega" );
    const std::string Environment::DB_EXTENSION( ".db" );
    const std::string Environment::CPP_HEADER_EXTENSION( ".hpp" );
    const std::string Environment::CPP_SOURCE_EXTENSION( ".cpp" );

    using Path = Environment::Path;

    Environment::Environment( const Path& rootSourceDir, const Path& rootBuildDir )
        : m_rootSourceDir( rootSourceDir )
        , m_rootBuildDir( rootBuildDir )
    {
    }

    Path Environment::buildDirFromSrcDir( const Path& srcDir ) const
    {
        VERIFY_RTE_MSG( boost::filesystem::is_directory( srcDir ),
                        "Source path is not a directory: " << srcDir.string() );
        return m_rootBuildDir / boost::filesystem::relative( srcDir, m_rootSourceDir );
    }

    Path Environment::stashDir() const { return m_rootBuildDir / "stash"; }

    Path Environment::rootSourceDir() const { return m_rootSourceDir; }

    Path Environment::rootBuildDir() const { return m_rootBuildDir; }

    Path Environment::project_manifest() const { return m_rootBuildDir / "project_manifest.txt"; }

    Path Environment::source_list( const Path& buildDir ) const
    {
        VERIFY_RTE_MSG( boost::filesystem::is_directory( buildDir ),
                        "Source List path is not a directory: " << buildDir.string() );
        return buildDir / "source_list.txt";
    }

    Path Environment::dependency( const std::string& strOpaque ) const
    {
        std::ostringstream os;
        os << strOpaque << MEGA_EXTENSION;
        return Path( os.str() );
    }

// clang-format off
// clang-format on
#define FILE_TYPE( filetype, stagetype )                                                       \
    Path Environment::filetype( const Path& megaSourcePath ) const                             \
    {                                                                                          \
        VERIFY_RTE_MSG( boost::filesystem::is_regular_file( megaSourcePath ),                  \
                        "Mega Source File is not regular file: " << megaSourcePath.string() ); \
        std::ostringstream os;                                                                 \
        os << megaSourcePath.filename().string() << "." << #filetype << DB_EXTENSION;          \
        return boost::filesystem::edsCannonicalise(                                            \
            buildDirFromSrcDir( Path( megaSourcePath ).remove_filename() ) / os.str() );       \
    }
#include "database/model/file_types_object.hxx"
#undef FILE_TYPE

#define FILE_TYPE( filetype, stagetype )  \
    Path Environment::filetype() const    \
    {                                     \
        std::ostringstream os;            \
        os << #filetype << DB_EXTENSION;  \
        return m_rootBuildDir / os.str(); \
    }
#include "database/model/file_types_global.hxx"
#undef FILE_TYPE

} // namespace io
} // namespace mega
