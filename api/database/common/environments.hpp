#ifndef ENVIRONMENTS_22_APRIL_2022
#define ENVIRONMENTS_22_APRIL_2022

#include "pipeline/stash.hpp"

#include "database/common/archive.hpp"
#include "database/common/serialisation.hpp"
#include "database/common/file_header.hpp"

#include "database/types/sources.hpp"

#include "database/model/environment.hxx"

#include "common/file.hpp"
#include "common/string.hpp"
#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <optional>

namespace mega
{
namespace io
{
class BuildEnvironment : public Environment
{
protected:
    using Path = boost::filesystem::path;

    const Path&                 m_rootSourceDir;
    const Path&                 m_rootBuildDir;
    const std::optional< Path > m_templatesDir;
    const Path                  m_tempDir;

    boost::filesystem::path toPath( const SourceFilePath& key ) const { return m_rootSourceDir / key.path(); }
    boost::filesystem::path toPath( const BuildFilePath& key ) const { return m_rootBuildDir / key.path(); }

    void copyToTargetPath( const boost::filesystem::path& from, const boost::filesystem::path& to ) const
    {
        VERIFY_RTE( boost::filesystem::exists( from ) );
        if ( boost::filesystem::exists( to ) )
        {
            boost::filesystem::remove( to );
        }
        boost::filesystem::ensureFoldersExist( to );
        boost::filesystem::copy_file( from, to );
    }

    Path tempDir() const
    {
        Path tempDir = boost::filesystem::temp_directory_path() / common::uuid();
        boost::filesystem::create_directories( tempDir );
        return tempDir;
    }

public:
    BuildEnvironment( const Path& rootSourceDir, const Path& rootBuildDir )
        : m_rootSourceDir( rootSourceDir )
        , m_rootBuildDir( rootBuildDir )
        , m_tempDir( tempDir() )
    {
    }

    BuildEnvironment( const Path& rootSourceDir, const Path& rootBuildDir, const Path& templatesDir )
        : m_rootSourceDir( rootSourceDir )
        , m_rootBuildDir( rootBuildDir )
        , m_templatesDir( templatesDir )
        , m_tempDir( tempDir() )
    {
    }

    ~BuildEnvironment()
    {
    }

    const Path& rootSourceDir() const { return m_rootSourceDir; }
    const Path& rootBuildDir() const { return m_rootBuildDir; }

    Path FilePath( const GeneratedHPPSourceFilePath& filePath ) const { return toPath( filePath ); }
    Path FilePath( const PrecompiledHeaderFile& filePath ) const { return toPath( filePath ); }
    Path FilePath( const megaFilePath& filePath ) const { return toPath( filePath ); }

    template < typename TFrom, typename TTo >
    void matchFileTime( const TFrom& from, const TTo& to ) const
    {
        boost::filesystem::last_write_time( toPath( to ), boost::filesystem::last_write_time( toPath( from ) ) );
    }

    Path ContextTemplate() const
    {
        VERIFY_RTE( m_templatesDir.has_value() );
        Path result = m_templatesDir.value() / "context.jinja";
        VERIFY_RTE_MSG( boost::filesystem::exists( result ), "Cannot locate inja template: " << result.string() );
        return result;
    }

    Path NamespaceTemplate() const
    {
        VERIFY_RTE( m_templatesDir.has_value() );
        Path result = m_templatesDir.value() / "namespace.jinja";
        VERIFY_RTE_MSG( boost::filesystem::exists( result ), "Cannot locate inja template: " << result.string() );
        return result;
    }

    Path InterfaceTemplate() const
    {
        VERIFY_RTE( m_templatesDir.has_value() );
        Path result = m_templatesDir.value() / "interface.jinja";
        VERIFY_RTE_MSG( boost::filesystem::exists( result ), "Cannot locate inja template: " << result.string() );
        return result;
    }

    GeneratedHPPSourceFilePath Include( const megaFilePath& source ) const
    {
        std::ostringstream os;
        os << source.path().filename().string() << ".include" << GeneratedHPPSourceFilePath::extension().string();
        auto dirPath = source.path();
        dirPath.remove_filename();
        return GeneratedHPPSourceFilePath( dirPath / os.str() );
    }

    PrecompiledHeaderFile PCH( const megaFilePath& source ) const
    {
        std::ostringstream os;
        os << source.path().filename().string() << ".include" << PrecompiledHeaderFile::extension().string();
        auto dirPath = source.path();
        dirPath.remove_filename();
        return PrecompiledHeaderFile( dirPath / os.str() );
    }

    GeneratedHPPSourceFilePath Interface( const megaFilePath& source ) const
    {
        std::ostringstream os;
        os << source.path().filename().string() << ".interface" << GeneratedHPPSourceFilePath::extension().string();
        auto dirPath = source.path();
        dirPath.remove_filename();
        return GeneratedHPPSourceFilePath( dirPath / os.str() );
    }

    PrecompiledHeaderFile InterfacePCH( const megaFilePath& source ) const
    {
        std::ostringstream os;
        os << source.path().filename().string() << ".interface" << PrecompiledHeaderFile::extension().string();
        auto dirPath = source.path();
        dirPath.remove_filename();
        return PrecompiledHeaderFile( dirPath / os.str() );
    }

    GeneratedHPPSourceFilePath Operations( const megaFilePath& source ) const
    {
        std::ostringstream os;
        os << source.path().filename().string() << ".operations" << GeneratedHPPSourceFilePath::extension().string();
        auto dirPath = source.path();
        dirPath.remove_filename();
        return GeneratedHPPSourceFilePath( dirPath / os.str() );
    }

    GeneratedCPPSourceFilePath Implementation( const megaFilePath& source ) const
    {
        std::ostringstream os;
        os << source.path().filename().string() << ".impl" << GeneratedCPPSourceFilePath::extension().string();
        auto dirPath = source.path();
        dirPath.remove_filename();
        return GeneratedCPPSourceFilePath( dirPath / os.str() );
    }

    ComponentListingFilePath ComponentListingFilePath_fromPath( const Path& buildDirectory ) const
    {
        VERIFY_RTE_MSG( boost::filesystem::is_directory( buildDirectory ),
                        "Source List path is not a directory: " << buildDirectory.string() );
        return ComponentListingFilePath(
            boost::filesystem::relative( buildDirectory / "component.listing", m_rootBuildDir ) );
    }
    manifestFilePath manifestFilePath_fromPath( const boost::filesystem::path& filePath ) const
    {
        return manifestFilePath( boost::filesystem::relative( filePath, m_rootSourceDir ) );
    }
    megaFilePath megaFilePath_fromPath( const boost::filesystem::path& filePath ) const
    {
        return megaFilePath( boost::filesystem::relative( filePath, m_rootSourceDir ) );
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    // Generic FilePath handling
    template < typename TFilePathType >
    bool exists( const TFilePathType& filePath ) const
    {
        return boost::filesystem::exists( toPath( filePath ) );
    }

    template < typename TFilePathType >
    std::unique_ptr< boost::filesystem::ofstream > write( const TFilePathType& filePath ) const
    {
        return boost::filesystem::createNewFileStream( toPath( filePath ) );
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    // FileSystem
    virtual std::unique_ptr< std::istream > read( const BuildFilePath& filePath ) const
    {
        return boost::filesystem::createBinaryInputFileStream( toPath( filePath ) );
    }
    virtual std::unique_ptr< std::ostream > write_temp( const BuildFilePath&     filePath,
                                                        boost::filesystem::path& tempFilePath ) const
    {
        tempFilePath = m_tempDir / filePath.path();
        return boost::filesystem::createBinaryOutputFileStream( tempFilePath );
    }
    virtual void temp_to_real( const BuildFilePath& filePath ) const
    {
        copyToTargetPath( m_tempDir / filePath.path(), toPath( filePath ) );
    }

    virtual std::unique_ptr< std::istream > read( const SourceFilePath& filePath ) const
    {
        return boost::filesystem::createBinaryInputFileStream( toPath( filePath ) );
    }
    virtual std::unique_ptr< std::ostream > write_temp( const SourceFilePath&    filePath,
                                                        boost::filesystem::path& tempFilePath ) const
    {
        tempFilePath = m_tempDir / filePath.path();
        return boost::filesystem::createBinaryOutputFileStream( tempFilePath );
    }
    virtual void temp_to_real( const SourceFilePath& filePath ) const
    {
        copyToTargetPath( m_tempDir / filePath.path(), toPath( filePath ) );
    }
};

class StashEnvironment : public BuildEnvironment
{
    mega::pipeline::Stash& m_stash;

public:
    StashEnvironment( mega::pipeline::Stash& stash,
                      const Path&            rootSourceDir,
                      const Path&            rootBuildDir )
        : BuildEnvironment( rootSourceDir, rootBuildDir )
        , m_stash( stash )
    {
    }
    StashEnvironment( mega::pipeline::Stash& stash,
                      const Path&            rootSourceDir,
                      const Path&            rootBuildDir,
                      const Path&            templatesDir )
        : BuildEnvironment( rootSourceDir, rootBuildDir, templatesDir )
        , m_stash( stash )
    {
    }

    template < typename TFilePathType >
    task::FileHash getBuildHashCode( const TFilePathType& filePath ) const
    {
        return m_stash.getBuildHashCode( toPath( filePath ) );
    }

    template < typename TFilePathType >
    void setBuildHashCode( const TFilePathType& filePath, task::FileHash hashCode ) const
    {
        m_stash.setBuildHashCode( toPath( filePath ), hashCode );
    }

    template < typename TFilePathType >
    void setBuildHashCode( const TFilePathType& filePath ) const
    {
        m_stash.setBuildHashCode( toPath( filePath ), task::FileHash( toPath( filePath ) ) );
    }

    template < typename TFilePathType >
    void stash( const TFilePathType& filePath, task::DeterminantHash hashCode ) const
    {
        m_stash.stash( toPath( filePath ), hashCode );
    }

    bool restore( const CompilationFilePath& filePath, task::DeterminantHash hashCode ) const
    {
        const Path actualPath = toPath( filePath );
        if ( m_stash.restore( actualPath, hashCode ) )
        {
            // check the file header for the correct version
            data::NullObjectPartLoader nullObjectPartLoader;

            std::unique_ptr< std::istream > pFileStream = read( filePath );
            boost::archive::MegaIArchive    archive( *pFileStream, nullObjectPartLoader );

            mega::io::FileHeader fileHeader;
            archive >> fileHeader;
            if ( fileHeader.getVersion() == VERSION )
            {
                return true;
            }
        }
        return false;
    }

    template < typename TFilePathType >
    bool restore( const TFilePathType& filePath, task::DeterminantHash hashCode ) const
    {
        return m_stash.restore( toPath( filePath ), hashCode );
    }
};

class RetailEnvironment : public Environment
{
    ReadArchive m_fileArchive;

public:
    RetailEnvironment( const boost::filesystem::path& archiveFilePath )
        : m_fileArchive( archiveFilePath )
    {
    }

    // FileSystem
    virtual std::unique_ptr< std::istream > read( const BuildFilePath& filePath ) const
    {
        return m_fileArchive.read( filePath );
    }
    virtual std::unique_ptr< std::ostream > write_temp( const BuildFilePath&     filePath,
                                                        boost::filesystem::path& tempFilePath ) const
    {
        THROW_RTE( "Invalid use of retail environment" );
    }
    virtual void temp_to_real( const BuildFilePath& filePath ) const
    {
        THROW_RTE( "Invalid use of retail environment" );
    }

    virtual std::unique_ptr< std::istream > read( const SourceFilePath& filePath ) const
    {
        return m_fileArchive.read( filePath );
    }
    virtual std::unique_ptr< std::ostream > write_temp( const SourceFilePath&    filePath,
                                                        boost::filesystem::path& tempFilePath ) const
    {
        THROW_RTE( "Invalid use of retail environment" );
    }
    virtual void temp_to_real( const SourceFilePath& filePath ) const
    {
        THROW_RTE( "Invalid use of retail environment" );
    }
};

} // namespace io
} // namespace mega

#endif // ENVIRONMENTS_22_APRIL_2022