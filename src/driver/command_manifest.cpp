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

#include "database/model/manifest.hxx"
#include "database/model/environment.hxx"
#include "database/model/ComponentListing.hxx"

#include "utilities/cmake.hpp"

#include "common/scheduler.hpp"
#include "common/file.hpp"
#include "common/assert_verify.hpp"
#include "common/terminal.hpp"
#include "common/stash.hpp"
#include "common/hash.hpp"

#include "spdlog/spdlog.h"
#include <spdlog/fmt/bundled/color.h>

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace driver
{
namespace manifest
{

static const char psz_start[]   = "{} START   {:>55} -> {:<55}";
static const char psz_cached[]  = "{} CACHED  {:>55} -> {:<55} : {}";
static const char psz_success[] = "{} SUCCESS {:>55} -> {:<55} : {}";
static const char psz_error[]   = "{} ERROR   {:>55} -> {:<55} : {}";
static const char psz_msg[]     = "{} MSG     {:>55} -> {:<55} : {}\n{}";

class BaseTask : public task::Task
{
public:
    BaseTask( const mega::io::BuildEnvironment& environment, const RawPtrSet& dependencies )
        : task::Task( dependencies )
        , m_environment( environment )
    {
    }
    inline const std::string& name( const task::Progress& taskProgress ) const
    {
        return taskProgress.getStatus().m_strTaskName;
    }
    inline std::string source( const task::Progress& taskProgress ) const
    {
        return std::get< boost::filesystem::path >( taskProgress.getStatus().m_source.value() ).string();
    }
    inline std::string target( const task::Progress& taskProgress ) const
    {
        return std::get< boost::filesystem::path >( taskProgress.getStatus().m_target.value() ).string();
    }
    inline std::string elapsed( const task::Progress& taskProgress ) const
    {
        return taskProgress.getStatus().m_elapsed.value();
    }

    void drawIndex( std::ostream& os )
    {
        static const int max_bars = 16;
        for ( int i = 0; i < max_bars; ++i )
        {
            if ( ( 0 % max_bars ) == i )
            {
                os << 0;
                int j = 0 / 10;
                while ( j )
                {
                    j = j / 10;
                    ++i;
                }
            }
            else
                os << ' ';
        }
    }
    void start( task::Progress& taskProgress, const char* pszName, const boost::filesystem::path& fromPath,
                const boost::filesystem::path& toPath )
    {
        // generate the name string to use for all logging
        {
            std::ostringstream os;
            drawIndex( os );
            os << std::setw( 32 ) << pszName;
            taskProgress.start( os.str(), fromPath, toPath );
        }

        spdlog::info( psz_start,
                      fmt::format( fmt::bg( fmt::terminal_color::yellow ) | fmt::fg( fmt::terminal_color::black )
                                       | fmt::emphasis::bold,
                                   name( taskProgress ) ),
                      fromPath.string(), toPath.string() );
    }
    void cached( task::Progress& taskProgress )
    {
        taskProgress.cached();
        spdlog::info( psz_cached,
                      fmt::format( fmt::bg( fmt::terminal_color::cyan ) | fmt::fg( fmt::terminal_color::black )
                                       | fmt::emphasis::bold,
                                   name( taskProgress ) ),
                      source( taskProgress ), target( taskProgress ), elapsed( taskProgress ) );
    }
    void succeeded( task::Progress& taskProgress )
    {
        taskProgress.succeeded();
        spdlog::info( psz_success,
                      fmt::format( fmt::bg( fmt::terminal_color::green ) | fmt::fg( fmt::terminal_color::black )
                                       | fmt::emphasis::bold,
                                   name( taskProgress ) ),
                      source( taskProgress ), target( taskProgress ), elapsed( taskProgress ) );
    }
    virtual void failed( task::Progress& taskProgress )
    {
        taskProgress.failed();
        spdlog::critical( psz_error,
                          fmt::format( fmt::bg( fmt::terminal_color::red ) | fmt::fg( fmt::terminal_color::black )
                                           | fmt::emphasis::bold,
                                       name( taskProgress ) ),
                          source( taskProgress ), target( taskProgress ), elapsed( taskProgress ) );
    }
    void msg( task::Progress& taskProgress, const std::string& strMsg )
    {
        spdlog::info( psz_msg,
                      fmt::format( fmt::bg( fmt::terminal_color::black ) | fmt::fg( fmt::terminal_color::white ),
                                   name( taskProgress ) ),
                      source( taskProgress ), target( taskProgress ), elapsed( taskProgress ), strMsg );
    }

protected:
    const mega::io::BuildEnvironment& m_environment;
};

class Task_GenerateManifest : public BaseTask
{
    const std::vector< boost::filesystem::path >& m_componentInfoPaths;

public:
    Task_GenerateManifest( const mega::io::BuildEnvironment&             environment,
                           const std::vector< boost::filesystem::path >& componentInfoPaths )
        : BaseTask( environment, {} )
        , m_componentInfoPaths( componentInfoPaths )
    {
    }
    virtual void run( task::Progress& taskProgress )
    {
        const mega::io::manifestFilePath projectManifestPath = m_environment.project_manifest();

        start( taskProgress, "Task_GenerateManifest", boost::filesystem::path{}, projectManifestPath.path() );
        // make a note of the schema version
        {
            std::ostringstream os;
            os << "Schema version: " << mega::io::Environment::VERSION;
            msg( taskProgress, os.str() );
        }

        const mega::io::Manifest    manifest( m_environment, m_componentInfoPaths );
        const task::FileHash        hashCode = manifest.save_temp( m_environment, projectManifestPath );
        const task::DeterminantHash determinant( hashCode );

        if ( m_environment.restore( projectManifestPath, determinant ) )
        {
            m_environment.setBuildHashCode( projectManifestPath, hashCode );
            cached( taskProgress );
            return;
        }
        else
        {
            m_environment.temp_to_real( projectManifestPath );
            m_environment.setBuildHashCode( projectManifestPath, hashCode );
            m_environment.stash( projectManifestPath, determinant );
            succeeded( taskProgress );
        }
    }
};

class Task_GenerateComponents : public BaseTask
{
    const std::vector< boost::filesystem::path >& m_componentInfoPaths;

public:
    Task_GenerateComponents( Task_GenerateManifest*                        pTask,
                             const mega::io::BuildEnvironment&             environment,
                             const std::vector< boost::filesystem::path >& componentInfoPaths )
        : BaseTask( environment, { pTask } )
        , m_componentInfoPaths( componentInfoPaths )
    {
    }

    virtual void run( task::Progress& taskProgress )
    {
        using namespace mega;

        const io::manifestFilePath    projectManifestPath = m_environment.project_manifest();
        const io::CompilationFilePath componentsListing
            = m_environment.ComponentListing_Components( projectManifestPath );

        start( taskProgress, "Task_GenerateComponents", projectManifestPath.path(), componentsListing.path() );

        task::DeterminantHash determinant = m_environment.getBuildHashCode( projectManifestPath );
        for ( const boost::filesystem::path& componentInfoPath : m_componentInfoPaths )
        {
            determinant ^= componentInfoPath;
        }

        if ( m_environment.restore( componentsListing, determinant ) )
        {
            m_environment.setBuildHashCode( componentsListing );
            cached( taskProgress );
            return;
        }

        using namespace ComponentListing;
        Database database( m_environment, projectManifestPath );

        for ( const boost::filesystem::path& componentInfoPath : m_componentInfoPaths )
        {
            io::ComponentInfo componentInfo;
            {
                VERIFY_RTE_MSG( boost::filesystem::exists( componentInfoPath ),
                                "Failed to locate file: " << componentInfoPath.string() );
                std::ifstream inputFileStream(
                    componentInfoPath.native().c_str(), std::ios::in | std::ios_base::binary );
                if ( !inputFileStream.good() )
                {
                    THROW_RTE( "Failed to open file: " << componentInfoPath.string() );
                }
                InputArchiveType ia( inputFileStream );
                ia >> boost::serialization::make_nvp( "componentInfo", componentInfo );
            };

            Components::Component* pComponent = database.construct< Components::Component >(
                Components::Component::Args( componentInfo.getName(),
                                             componentInfo.getDirectory(),
                                             componentInfo.getCPPFlags(),
                                             componentInfo.getCPPDefines(),
                                             componentInfo.getIncludeDirectories(),
                                             componentInfo.getSourceFiles() ) );

            VERIFY_RTE( pComponent->get_name() == componentInfo.getName() );
        }

        const task::FileHash fileHashCode = database.save_Components_to_temp();
        m_environment.setBuildHashCode( componentsListing, fileHashCode );
        m_environment.temp_to_real( componentsListing );
        m_environment.stash( componentsListing, determinant );

        succeeded( taskProgress );
    }
};

void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path rootSourceDir, rootBuildDir;
    std::string             projectName, strComponentInfoPaths;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Generate Mega Project Manifest" );
    {
        // clang-format off
                commandOptions.add_options()
                ( "root_src_dir",   po::value< boost::filesystem::path >( &rootSourceDir ), "Root source directory" )
                ( "root_build_dir", po::value< boost::filesystem::path >( &rootBuildDir ),  "Root build directory" )
                ( "project",        po::value< std::string >( &projectName ),               "Mega Project Name" )
                ( "components",     po::value< std::string >( &strComponentInfoPaths ),     "Component info files" )
                ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "components", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    if ( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        mega::io::BuildEnvironment environment( rootSourceDir, rootBuildDir );

        const std::vector< boost::filesystem::path > componentInfoPaths
            = mega::utilities::pathListToFiles( mega::utilities::parseCMakeStringList( strComponentInfoPaths, ";" ) );

        task::Task::PtrVector tasks;

        Task_GenerateManifest* pTask = new Task_GenerateManifest( environment, componentInfoPaths );
        tasks.push_back( task::Task::Ptr( pTask ) );

        Task_GenerateComponents* pTaskComponents
            = new Task_GenerateComponents( pTask, environment, componentInfoPaths );
        tasks.push_back( task::Task::Ptr( pTaskComponents ) );

        task::Schedule::Ptr pSchedule( new task::Schedule( tasks ) );
        task::run( pSchedule, std::cout );
    }
}
} // namespace manifest
} // namespace driver
