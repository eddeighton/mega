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

#include "database/common/component_info.hpp"
#include "database/common/serialisation.hpp"
#include "database/common/environments.hpp"

#include "utilities/cmake.hpp"

#include "common/scheduler.hpp"
#include "common/assert_verify.hpp"
#include <common/hash.hpp>

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <common/stash.hpp>
#include <string>
#include <vector>
#include <iostream>

namespace driver
{
namespace list
{
class BaseTask : public task::Task
{
public:
    BaseTask( const mega::io::BuildEnvironment& environment, const RawPtrSet& dependencies )
        : task::Task( dependencies )
        , m_environment( environment )
    {
    }
    virtual void failed( task::Progress& taskProgress )
    {
        taskProgress.failed();
    }

protected:
    const mega::io::BuildEnvironment& m_environment;
};

class Task_ComponentInfoToManifest : public BaseTask
{
public:
    Task_ComponentInfoToManifest( const mega::io::BuildEnvironment&             environment,
                                  const std::string&                            strComponentName,
                                  const std::vector< std::string >&             cppFlags,
                                  const std::vector< std::string >&             cppDefines,
                                  const boost::filesystem::path&                srcDir,
                                  const boost::filesystem::path&                buildDir,
                                  const std::vector< boost::filesystem::path >& inputMegaSourceFiles,
                                  const std::vector< boost::filesystem::path >& includeDirectories )
        : BaseTask( environment, {} )
        , m_srcDir( srcDir )
        , m_buildDir( buildDir )
        , m_componentInfo( strComponentName, cppFlags, cppDefines, srcDir, inputMegaSourceFiles, includeDirectories )
    {
    }

    virtual void run( task::Progress& taskProgress )
    {
        // inputMegaSourceFiles
        const mega::io::ComponentListingFilePath componentListingFilePath
            = m_environment.ComponentListingFilePath_fromPath( m_buildDir );

        taskProgress.start( "Task_ComponentInfoToManifest", m_srcDir, componentListingFilePath.path() );

        boost::filesystem::path tempFile;
        {
            std::unique_ptr< std::ostream > pOfstream = m_environment.write_temp( componentListingFilePath, tempFile );
            mega::OutputArchiveType         oa( *pOfstream );
            oa << boost::serialization::make_nvp( "componentInfo", m_componentInfo );
        }

        const task::DeterminantHash determinant( tempFile );

        if ( m_environment.restore( componentListingFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( componentListingFilePath );
            taskProgress.cached();
            return;
        }
        else
        {
            m_environment.temp_to_real( componentListingFilePath );
            m_environment.setBuildHashCode( componentListingFilePath );
            m_environment.stash( componentListingFilePath, determinant );
            taskProgress.succeeded();
        }
    }

private:
    const boost::filesystem::path m_srcDir;
    const boost::filesystem::path m_buildDir;
    mega::io::ComponentInfo       m_componentInfo;
};

void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path    rootSourceDir, rootBuildDir, sourceDir, buildDir;
    std::string                strComponentName, strCPPFlags, strCPPDefines, strIncludeDirectories;
    std::vector< std::string > objectSourceFileNames;

    namespace po = boost::program_options;
    po::options_description commandOptions( " List input mega source files" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "root_src_dir",   po::value< boost::filesystem::path >( &rootSourceDir ),             "Root source directory" )
            ( "root_build_dir", po::value< boost::filesystem::path >( &rootBuildDir ),              "Root build directory" )
            ( "src_dir",        po::value< boost::filesystem::path >( &sourceDir ),                 "Source directory" )
            ( "build_dir",      po::value< boost::filesystem::path >( &buildDir ),                  "Build Directory" )
            ( "name",           po::value< std::string >( &strComponentName ),                      "Component name" )
            ( "cpp_flags",      po::value< std::string >( &strCPPFlags ),                           "C++ Compiler Flags" )
            ( "cpp_defines",    po::value< std::string >( &strCPPDefines ),                         "C++ Compiler Defines" )
            ( "include_dirs",   po::value< std::string >( &strIncludeDirectories ),                 "Include directories ( semicolon delimited )" )
            ( "src",            po::value< std::vector< std::string > >( &objectSourceFileNames ),  "Mega source file names" )
            ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "src", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    if ( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        // tokenize semi colon delimited names into absolute mega source file paths
        const std::vector< boost::filesystem::path > inputSourceFiles
            = mega::utilities::pathListToFiles( sourceDir, objectSourceFileNames );
        const std::vector< std::string > cppFlags   = mega::utilities::parseCMakeStringList( strCPPFlags, " " );
        const std::vector< std::string > cppDefines = mega::utilities::parseCMakeStringList( strCPPDefines, " " );
        const std::vector< boost::filesystem::path > includeDirectories
            = mega::utilities::pathListToFolders( mega::utilities::parseCMakeStringList( strIncludeDirectories, ";" ) );

        mega::io::BuildEnvironment environment( rootSourceDir, rootBuildDir );

        task::Task::PtrVector tasks;

        Task_ComponentInfoToManifest* pTask = new Task_ComponentInfoToManifest( environment,
                                                                                strComponentName,
                                                                                cppFlags,
                                                                                cppDefines,
                                                                                sourceDir,
                                                                                buildDir,
                                                                                inputSourceFiles,
                                                                                includeDirectories );
        tasks.push_back( task::Task::Ptr( pTask ) );

        task::Schedule::Ptr pSchedule( new task::Schedule( tasks ) );
        task::run( pSchedule, std::cout );
    }
}
} // namespace list
} // namespace driver
