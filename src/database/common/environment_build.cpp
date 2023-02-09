//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#include "database/common/environment_build.hpp"

#include "common/string.hpp"

namespace mega::io
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

    // attempt rename - which may fail if temp file is not on same volume as target
    boost::system::error_code ec;
    boost::filesystem::rename( from, to, ec );
    if( ec.failed() )
    {
        boost::filesystem::copy( from, to, boost::filesystem::copy_options::synchronize );
    }
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

BuildEnvironment::Path BuildEnvironment::ImplementationTemplate() const
{
    VERIFY_RTE( !m_directories.templatesDir.empty() );
    Path result = m_directories.templatesDir / "impl.jinja";
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

GeneratedHPPSourceFilePath BuildEnvironment::Operations( const megaFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << ".operations" << GeneratedHPPSourceFilePath::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return GeneratedHPPSourceFilePath( dirPath / os.str() );
}

PrecompiledHeaderFile BuildEnvironment::OperationsPCH( const megaFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << ".operations" << PrecompiledHeaderFile::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return PrecompiledHeaderFile( dirPath / os.str() );
}

GeneratedCPPSourceFilePath BuildEnvironment::Implementation( const megaFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << ".impl" << GeneratedCPPSourceFilePath::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return GeneratedCPPSourceFilePath( dirPath / os.str() );
}

ObjectFilePath BuildEnvironment::ImplementationObj( const megaFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << ".impl" << ObjectFilePath::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return ObjectFilePath( dirPath / os.str() );
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

GeneratedHPPSourceFilePath BuildEnvironment::CPPTempHpp( const cppFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << GeneratedHPPSourceFilePath::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return GeneratedHPPSourceFilePath( dirPath / os.str() );
}

PrecompiledHeaderFile BuildEnvironment::CPPPCH( const cppFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << PrecompiledHeaderFile::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return PrecompiledHeaderFile( dirPath / os.str() );
}

GeneratedCPPSourceFilePath BuildEnvironment::CPPImplementation( const cppFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << GeneratedCPPSourceFilePath::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return GeneratedCPPSourceFilePath( dirPath / os.str() );
}

ObjectFilePath BuildEnvironment::CPPObj( const cppFilePath& source ) const
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
ComponentFilePath BuildEnvironment::ComponentPath_fromPath( const boost::filesystem::path& filePath ) const
{
    return ComponentFilePath( boost::filesystem::relative( filePath, m_directories.buildDir ) );
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

} // namespace mega::io
