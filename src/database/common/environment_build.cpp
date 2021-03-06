
#include "database/common/environment_build.hpp"

#include "common/string.hpp"

namespace mega
{
namespace io
{
BuildEnvironment::BuildEnvironment( const compiler::Directories& directories )
    : m_directories( directories )
    , m_tempDir( tempDir() )
{
}

BuildEnvironment::~BuildEnvironment() {}

BuildEnvironment::Path BuildEnvironment::DatabaseArchive() const
{
    VERIFY_RTE( !m_directories.installDir.empty() );
    return m_directories.installDir / "bin/archive.adb";
}

void BuildEnvironment::copyToTargetPath( const boost::filesystem::path& from, const boost::filesystem::path& to ) const
{
    VERIFY_RTE( boost::filesystem::exists( from ) );
    if ( boost::filesystem::exists( to ) )
    {
        boost::filesystem::remove( to );
    }
    boost::filesystem::ensureFoldersExist( to );
    boost::filesystem::copy_file( from, to );
}

BuildEnvironment::Path BuildEnvironment::tempDir() const
{
    Path tempDir = boost::filesystem::temp_directory_path() / "megaenv" / common::uuid();
    boost::filesystem::create_directories( tempDir );
    return tempDir;
}

BuildEnvironment::Path BuildEnvironment::ContextTemplate() const
{
    VERIFY_RTE( !m_directories.templatesDir.empty() );
    Path result = m_directories.templatesDir / "context.jinja";
    VERIFY_RTE_MSG( boost::filesystem::exists( result ), "Cannot locate inja template: " << result.string() );
    return result;
}

BuildEnvironment::Path BuildEnvironment::InterfaceTemplate() const
{
    VERIFY_RTE( !m_directories.templatesDir.empty() );
    Path result = m_directories.templatesDir / "interface.jinja";
    VERIFY_RTE_MSG( boost::filesystem::exists( result ), "Cannot locate inja template: " << result.string() );
    return result;
}

BuildEnvironment::Path BuildEnvironment::OperationsTemplate() const
{
    VERIFY_RTE( !m_directories.templatesDir.empty() );
    Path result = m_directories.templatesDir / "operations.jinja";
    VERIFY_RTE_MSG( boost::filesystem::exists( result ), "Cannot locate inja template: " << result.string() );
    return result;
}

GeneratedHPPSourceFilePath BuildEnvironment::Include( const megaFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << ".include" << GeneratedHPPSourceFilePath::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return GeneratedHPPSourceFilePath( dirPath / os.str() );
}

PrecompiledHeaderFile BuildEnvironment::IncludePCH( const megaFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << ".include" << PrecompiledHeaderFile::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return PrecompiledHeaderFile( dirPath / os.str() );
}

GeneratedHPPSourceFilePath BuildEnvironment::Interface( const megaFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << ".interface" << GeneratedHPPSourceFilePath::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return GeneratedHPPSourceFilePath( dirPath / os.str() );
}

PrecompiledHeaderFile BuildEnvironment::InterfacePCH( const megaFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << ".interface" << PrecompiledHeaderFile::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return PrecompiledHeaderFile( dirPath / os.str() );
}

GeneratedHPPSourceFilePath BuildEnvironment::Include( const boost::filesystem::path& componentBuildDir,
                                                      const std::string&             strComponentName ) const
{
    std::ostringstream os;
    os << strComponentName << ".include" << GeneratedHPPSourceFilePath::extension().string();
    return GeneratedHPPSourceFilePath( boost::filesystem::relative( componentBuildDir, m_directories.buildDir )
                                       / os.str() );
}
PrecompiledHeaderFile BuildEnvironment::IncludePCH( const boost::filesystem::path& componentBuildDir,
                                                    const std::string&             strComponentName ) const
{
    std::ostringstream os;
    os << strComponentName << ".include" << PrecompiledHeaderFile::extension().string();
    return PrecompiledHeaderFile( boost::filesystem::relative( componentBuildDir, m_directories.buildDir ) / os.str() );
}
GeneratedHPPSourceFilePath BuildEnvironment::Interface( const boost::filesystem::path& componentBuildDir,
                                                        const std::string&             strComponentName ) const
{
    std::ostringstream os;
    os << strComponentName << ".interface" << GeneratedHPPSourceFilePath::extension().string();
    return GeneratedHPPSourceFilePath( boost::filesystem::relative( componentBuildDir, m_directories.buildDir )
                                       / os.str() );
}
PrecompiledHeaderFile BuildEnvironment::InterfacePCH( const boost::filesystem::path& componentBuildDir,
                                                      const std::string&             strComponentName ) const
{
    std::ostringstream os;
    os << strComponentName << ".interface" << PrecompiledHeaderFile::extension().string();
    return PrecompiledHeaderFile( boost::filesystem::relative( componentBuildDir, m_directories.buildDir ) / os.str() );
}

GeneratedCPPSourceFilePath BuildEnvironment::Operations( const megaFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << ".operations" << GeneratedCPPSourceFilePath::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return GeneratedCPPSourceFilePath( dirPath / os.str() );
}

ObjectFilePath BuildEnvironment::OperationsObj( const megaFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << ".operations" << ObjectFilePath::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return ObjectFilePath( dirPath / os.str() );
}

ObjectFilePath BuildEnvironment::Obj( const cppFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << ObjectFilePath::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return ObjectFilePath( dirPath / os.str() );
}

ComponentListingFilePath BuildEnvironment::ComponentListingFilePath_fromPath( const Path& buildDirectory ) const
{
    VERIFY_RTE_MSG( boost::filesystem::is_directory( buildDirectory ),
                    "Source List path is not a directory: " << buildDirectory.string() );
    return ComponentListingFilePath(
        boost::filesystem::relative( buildDirectory / "component.listing", m_directories.buildDir ) );
}
manifestFilePath BuildEnvironment::manifestFilePath_fromPath( const boost::filesystem::path& filePath ) const
{
    return manifestFilePath( boost::filesystem::relative( filePath, m_directories.srcDir ) );
}
megaFilePath BuildEnvironment::megaFilePath_fromPath( const boost::filesystem::path& filePath ) const
{
    return megaFilePath( boost::filesystem::relative( filePath, m_directories.srcDir ) );
}
cppFilePath BuildEnvironment::cppFilePath_fromPath( const boost::filesystem::path& filePath ) const
{
    return cppFilePath( boost::filesystem::relative( filePath, m_directories.srcDir ) );
}

std::unique_ptr< std::istream > BuildEnvironment::read( const BuildFilePath& filePath ) const
{
    return boost::filesystem::createBinaryInputFileStream( toPath( filePath ) );
}
std::unique_ptr< std::ostream > BuildEnvironment::write_temp( const BuildFilePath&     filePath,
                                                              boost::filesystem::path& tempFilePath ) const
{
    tempFilePath = m_tempDir / filePath.path();
    return boost::filesystem::createBinaryOutputFileStream( tempFilePath );
}
void BuildEnvironment::temp_to_real( const BuildFilePath& filePath ) const
{
    copyToTargetPath( m_tempDir / filePath.path(), toPath( filePath ) );
}

std::unique_ptr< std::istream > BuildEnvironment::read( const SourceFilePath& filePath ) const
{
    return boost::filesystem::createBinaryInputFileStream( toPath( filePath ) );
}
std::unique_ptr< std::ostream > BuildEnvironment::write_temp( const SourceFilePath&    filePath,
                                                              boost::filesystem::path& tempFilePath ) const
{
    tempFilePath = m_tempDir / filePath.path();
    return boost::filesystem::createBinaryOutputFileStream( tempFilePath );
}
void BuildEnvironment::temp_to_real( const SourceFilePath& filePath ) const
{
    copyToTargetPath( m_tempDir / filePath.path(), toPath( filePath ) );
}
} // namespace io
} // namespace mega
