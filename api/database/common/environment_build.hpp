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

#ifndef ENVIRONMENT_BUILD_18_JUNE
#define ENVIRONMENT_BUILD_18_JUNE

#include "database/model/environment.hxx"

#include "compiler/configuration.hpp"

#include "common/file.hpp"
#include "database/types/sources.hpp"

#include <boost/filesystem/operations.hpp>

namespace mega::io
{
class BuildEnvironment : public Environment
{
protected:
    using Path = boost::filesystem::path;

    const compiler::Directories& m_directories;
    const Path                   m_tempDir;

    boost::filesystem::path toPath( const SourceFilePath& key ) const { return m_directories.srcDir / key.path(); }
    boost::filesystem::path toPath( const BuildFilePath& key ) const { return m_directories.buildDir / key.path(); }

    void copyToTargetPath( const boost::filesystem::path& from, const boost::filesystem::path& to ) const;
    Path tempDir() const;

public:
    BuildEnvironment( const compiler::Directories& directories );
    ~BuildEnvironment();

    const Path& srcDir() const { return m_directories.srcDir; }
    const Path& buildDir() const { return m_directories.buildDir; }

    Path DatabaseArchive() const;

    Path FilePath( const GeneratedHPPSourceFilePath& filePath ) const { return toPath( filePath ); }
    Path FilePath( const GeneratedCPPSourceFilePath& filePath ) const { return toPath( filePath ); }
    Path FilePath( const PrecompiledHeaderFile& filePath ) const { return toPath( filePath ); }
    Path FilePath( const ObjectFilePath& filePath ) const { return toPath( filePath ); }
    Path FilePath( const megaFilePath& filePath ) const { return toPath( filePath ); }
    Path FilePath( const cppFilePath& filePath ) const { return toPath( filePath ); }
    Path FilePath( const ComponentFilePath& filePath ) const { return toPath( filePath ); }

    template < typename TFrom, typename TTo >
    void matchFileTime( const TFrom& from, const TTo& to ) const
    {
        boost::filesystem::last_write_time( toPath( to ), boost::filesystem::last_write_time( toPath( from ) ) );
    }

    Path ContextTemplate() const;
    Path InterfaceTemplate() const;
    Path OperationsTemplate() const;
    Path ImplementationTemplate() const;

    // interface file paths
    GeneratedHPPSourceFilePath Include( const megaFilePath& source ) const;
    PrecompiledHeaderFile      IncludePCH( const megaFilePath& source ) const;
    GeneratedHPPSourceFilePath Interface( const megaFilePath& source ) const;
    PrecompiledHeaderFile      InterfacePCH( const megaFilePath& source ) const;
    GeneratedHPPSourceFilePath Operations( const megaFilePath& source ) const;
    PrecompiledHeaderFile      OperationsPCH( const megaFilePath& source ) const;
    GeneratedCPPSourceFilePath Implementation( const megaFilePath& source ) const;
    ObjectFilePath             ImplementationObj( const megaFilePath& source ) const;

    // library cpp file paths
    GeneratedHPPSourceFilePath Include( const boost::filesystem::path& componentBuildDir,
                                        const std::string&             strComponentName ) const;
    PrecompiledHeaderFile      IncludePCH( const boost::filesystem::path& componentBuildDir,
                                           const std::string&             strComponentName ) const;
    GeneratedHPPSourceFilePath Interface( const boost::filesystem::path& componentBuildDir,
                                          const std::string&             strComponentName ) const;
    PrecompiledHeaderFile      InterfacePCH( const boost::filesystem::path& componentBuildDir,
                                             const std::string&             strComponentName ) const;
    GeneratedHPPSourceFilePath CPPTempHpp( const cppFilePath& source ) const;
    PrecompiledHeaderFile      CPPPCH( const cppFilePath& source ) const;
    GeneratedCPPSourceFilePath CPPImplementation( const cppFilePath& source ) const;
    ObjectFilePath             CPPObj( const cppFilePath& source ) const;

    // note lowercase file type function names are used in generated code based on file type
    ComponentListingFilePath ComponentListingFilePath_fromPath( const Path& buildDirectory ) const;
    manifestFilePath         manifestFilePath_fromPath( const boost::filesystem::path& filePath ) const;
    megaFilePath             megaFilePath_fromPath( const boost::filesystem::path& filePath ) const;
    cppFilePath              cppFilePath_fromPath( const boost::filesystem::path& filePath ) const;
    ComponentFilePath        ComponentPath_fromPath( const boost::filesystem::path& filePath ) const;

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
    virtual std::unique_ptr< std::istream > read( const BuildFilePath& filePath ) const;
    virtual std::unique_ptr< std::ostream > write_temp( const BuildFilePath&     filePath,
                                                        boost::filesystem::path& tempFilePath ) const;
    virtual void                            temp_to_real( const BuildFilePath& filePath ) const;

    virtual std::unique_ptr< std::istream > read( const SourceFilePath& filePath ) const;
    virtual std::unique_ptr< std::ostream > write_temp( const SourceFilePath&    filePath,
                                                        boost::filesystem::path& tempFilePath ) const;
    virtual void                            temp_to_real( const SourceFilePath& filePath ) const;
};

} // namespace mega::io

#endif // ENVIRONMENT_BUILD_18_JUNE
