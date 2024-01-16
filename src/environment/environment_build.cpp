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

#include "environment/environment_build.hpp"

#include "common/string.hpp"

namespace mega::io
{

BuildEnvironment::BuildEnvironment( const Directories& directories )
    : m_directories( directories )
    , m_tempDir( tempDir() )
{
}

BuildEnvironment::~BuildEnvironment() = default;

BuildEnvironment::Path BuildEnvironment::DatabaseArchive() const
{
    VERIFY_RTE( !m_directories.installDir.empty() );
    return m_directories.installDir / "bin/archive.adb";
}

BuildEnvironment::Path BuildEnvironment::UnityReflection() const
{
    VERIFY_RTE( !m_directories.installDir.empty() );
    return m_directories.installDir / "bin/reflection.json";
}

BuildEnvironment::Path BuildEnvironment::UnityAnalysis() const
{
    VERIFY_RTE( !m_directories.installDir.empty() );
    return m_directories.installDir / "bin/unity.json";
}

BuildEnvironment::Path BuildEnvironment::UnityDatabase() const
{
    VERIFY_RTE( !m_directories.installDir.empty() );
    return m_directories.installDir / "bin/unityDatabase.json";
}

void BuildEnvironment::copyToTargetPath( const boost::filesystem::path& from, const boost::filesystem::path& to ) const
{
    VERIFY_RTE_MSG( boost::filesystem::exists( from ), "Failed to locate file: " << from.string() );
    if( boost::filesystem::exists( to ) )
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

BuildEnvironment::Path BuildEnvironment::OperationsExternsTemplate() const
{
    VERIFY_RTE( !m_directories.templatesDir.empty() );
    Path result = m_directories.templatesDir / "operations_externs.jinja";
    VERIFY_RTE_MSG( boost::filesystem::exists( result ), "Cannot locate inja template: " << result.string() );
    return result;
}

BuildEnvironment::Path BuildEnvironment::PythonWrapperTemplate() const
{
    VERIFY_RTE( !m_directories.templatesDir.empty() );
    Path result = m_directories.templatesDir / "python_wrapper.jinja";
    VERIFY_RTE_MSG( boost::filesystem::exists( result ), "Cannot locate inja template: " << result.string() );
    return result;
}

BuildEnvironment::Path BuildEnvironment::InitialiserTemplate() const
{
    VERIFY_RTE( !m_directories.templatesDir.empty() );
    Path result = m_directories.templatesDir / "initialiser.jinja";
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

GeneratedHPPSourceFilePath BuildEnvironment::Include() const
{
    std::ostringstream os;
    os << "include" << GeneratedHPPSourceFilePath::extension().string();
    return { os.str() };
}

PrecompiledHeaderFile BuildEnvironment::IncludePCH() const
{
    std::ostringstream os;
    os << "include" << PrecompiledHeaderFile::extension().string();
    return { os.str() };
}

GeneratedHPPSourceFilePath BuildEnvironment::ClangTraits() const
{
    std::ostringstream os;
    os << "Traits" << GeneratedHPPSourceFilePath::extension().string();
    return { os.str() };
}

GeneratedHPPSourceFilePath BuildEnvironment::CPPDecls() const
{
    std::ostringstream os;
    os << "Decls" << GeneratedHPPSourceFilePath::extension().string();
    return { os.str() };
}
PrecompiledHeaderFile BuildEnvironment::CPPDeclsPCH() const
{
    std::ostringstream os;
    os << "Decls" << PrecompiledHeaderFile::extension().string();
    return { os.str() };
}
GeneratedHPPSourceFilePath BuildEnvironment::CPPSource( const cppFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << GeneratedHPPSourceFilePath::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return { dirPath / os.str() };
}
PrecompiledHeaderFile BuildEnvironment::CPPPCH( const cppFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << PrecompiledHeaderFile::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return { dirPath / os.str() };
}
GeneratedCPPSourceFilePath BuildEnvironment::CPPImpl( const cppFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << GeneratedCPPSourceFilePath::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return { dirPath / os.str() };
}
ObjectFilePath BuildEnvironment::CPPObj( const cppFilePath& source ) const
{
    std::ostringstream os;
    os << source.path().filename().string() << ObjectFilePath::extension().string();
    auto dirPath = source.path();
    dirPath.remove_filename();
    return { dirPath / os.str() };
}

GeneratedCPPSourceFilePath BuildEnvironment::RuntimeSource() const
{
    std::ostringstream os;
    os << "Runtime" << GeneratedCPPSourceFilePath::extension().string();
    return { os.str() };
}

ObjectFilePath BuildEnvironment::RuntimeObj() const
{
    std::ostringstream os;
    os << "Runtime" << ObjectFilePath::extension().string();
    return { os.str() };
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
pyFilePath BuildEnvironment::pyFilePath_fromPath( const boost::filesystem::path& filePath ) const
{
    return pyFilePath( boost::filesystem::relative( filePath, m_directories.srcDir ) );
}
schFilePath BuildEnvironment::schFilePath_fromPath( const boost::filesystem::path& filePath ) const
{
    return schFilePath( boost::filesystem::relative( filePath, m_directories.srcDir ) );
}
ComponentFilePath BuildEnvironment::ComponentPath_fromPath( const boost::filesystem::path& filePath ) const
{
    return ComponentFilePath( boost::filesystem::relative( filePath, m_directories.buildDir ) );
}
ComponentFilePath BuildEnvironment::PythonComponentPath_fromPath( const boost::filesystem::path& filePath ) const
{
    boost::filesystem::path pythonFileName;
    {
        auto filename = filePath.filename();
        filename.replace_extension( "" );
        VERIFY_RTE( !filename.empty() );

        std::ostringstream osFileName;
        osFileName << filename.string() << "_python" << filePath.extension().string();

        pythonFileName = filePath.parent_path() / osFileName.str();
    }
    return ComponentFilePath( boost::filesystem::relative( pythonFileName, m_directories.buildDir ) );
}
ComponentFilePath BuildEnvironment::InitComponentPath_fromPath( const boost::filesystem::path& filePath ) const
{
    boost::filesystem::path pythonFileName;
    {
        auto filename = filePath.filename();
        filename.replace_extension( "" );
        VERIFY_RTE( !filename.empty() );

        std::ostringstream osFileName;
        osFileName << filename.string() << "_init" << filePath.extension().string();

        pythonFileName = filePath.parent_path() / osFileName.str();
    }
    return ComponentFilePath( boost::filesystem::relative( pythonFileName, m_directories.buildDir ) );
}

bool BuildEnvironment::exists( const BuildFilePath& filePath ) const
{
    return boost::filesystem::exists( toPath( filePath ) );
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
