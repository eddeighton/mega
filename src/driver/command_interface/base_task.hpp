#ifndef BASE_TASK_10_MAY_2022
#define BASE_TASK_10_MAY_2022

#include "database/common/environments.hpp"
#include "database/types/sources.hpp"

#include "common/string.hpp"
#include "common/terminal.hpp"
#include "common/scheduler.hpp"
#include "common/file.hpp"
#include "common/assert_verify.hpp"
#include "common/hash.hpp"
#include "common/stl.hpp"

#include "spdlog/spdlog.h"
#include <spdlog/fmt/bundled/color.h>

#include <boost/filesystem/file_status.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/dll.hpp>
#pragma warning( push )
#pragma warning( disable : 4996 ) // iterator thing
#pragma warning( disable : 4244 ) // conversion to DWORD from system_clock::rep
#include <boost/process.hpp>
#pragma warning( pop )

#include <common/stash.hpp>
#include <iostream>
#include <ostream>
#include <vector>
#include <string>

std::ostream& operator<<( std::ostream& os, const std::vector< std::string >& name )
{
    common::delimit( name.begin(), name.end(), ".", os );
    return os;
}

#define VERIFY_PARSER( expression, msg, scoped_id )                                                             \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( if ( !( expression ) ) {                                                   \
        std::ostringstream _os2;                                                                                \
        _os2 << common::COLOUR_RED_BEGIN << msg << ". Source Location: " << scoped_id->get_source_file() << ":" \
             << scoped_id->get_line_number() << common::COLOUR_END;                                             \
        throw std::runtime_error( _os2.str() );                                                                 \
    } )

namespace driver
{
namespace interface
{

struct ToolChain
{
    const boost::filesystem::path &parserDllPath, &megaCompilerPath, &clangCompilerPath, &clangPluginPath,
        &databasePath;
    const task::FileHash        parserDllHash, megaCompilerHash, clangCompilerHash, clangPluginHash, databaseHash;
    const task::DeterminantHash toolChainHash;

    ToolChain( const boost::filesystem::path& parserDllPath, const boost::filesystem::path& megaCompilerPath,
               const boost::filesystem::path& clangCompilerPath, const boost::filesystem::path& clangPluginPath,
               const boost::filesystem::path& databasePath )
        : parserDllPath( parserDllPath )
        , megaCompilerPath( megaCompilerPath )
        , clangCompilerPath( clangCompilerPath )
        , clangPluginPath( clangPluginPath )
        , databasePath( databasePath )
        , parserDllHash( parserDllPath )
        , megaCompilerHash( megaCompilerPath )
        , clangCompilerHash( clangCompilerPath )
        , clangPluginHash( clangPluginPath )
        , databaseHash( databasePath )
        , toolChainHash( { parserDllHash, megaCompilerHash, clangCompilerHash, clangPluginHash, databaseHash } )
    {
    }
};

struct TaskArguments
{
    TaskArguments( const task::Task::RawPtrSet& dependencies, const mega::io::BuildEnvironment& environment,
                   const ToolChain& toolChain, int index )
        : dependencies( dependencies )
        , environment( environment )
        , toolChain( toolChain )
        , index( index )
    {
    }
    const task::Task::RawPtrSet&      dependencies;
    const mega::io::BuildEnvironment& environment;
    const ToolChain&                  toolChain;
    const int                         index;
};

static const char psz_start[]   = "{} START   {:>55} -> {:<55}";
static const char psz_cached[]  = "{} CACHED  {:>55} -> {:<55} : {}";
static const char psz_success[] = "{} SUCCESS {:>55} -> {:<55} : {}";
static const char psz_error[]   = "{} ERROR   {:>55} -> {:<55} : {}";
static const char psz_msg[]     = "{} MSG     {:>55} -> {:<55} : {}\n{}";

class BaseTask : public task::Task
{
public:
    BaseTask( const TaskArguments& taskArguments )
        : task::Task( taskArguments.dependencies )
        , m_environment( taskArguments.environment )
        , m_toolChain( taskArguments.toolChain )
        , m_index( taskArguments.index )
    {
    }

    template < typename TComponentType, typename TDatabase >
    TComponentType* getComponent( TDatabase& database, const mega::io::megaFilePath& sourceFilePath ) const
    {
        TComponentType* pComponent = nullptr;
        for ( TComponentType* pIter : database.template many< TComponentType >( m_environment.project_manifest() ) )
        {
            for ( const boost::filesystem::path& sourceFile : pIter->get_sourceFiles() )
            {
                if ( m_environment.megaFilePath_fromPath( sourceFile ) == sourceFilePath )
                {
                    pComponent = pIter;
                    break;
                }
            }
        }
        VERIFY_RTE_MSG( pComponent, "Failed to locate component for source file: " << sourceFilePath.path().string() );
        return pComponent;
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
            if ( ( m_index % max_bars ) == i )
            {
                os << m_index;
                int j = m_index / 10;
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
    void failed( task::Progress& taskProgress )
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
    const ToolChain&                  m_toolChain;
    int                               m_index;
};

inline const mega::io::FileInfo& findFileInfo( const boost::filesystem::path&           filePath,
                                               const std::vector< mega::io::FileInfo >& fileInfos )
{
    for ( const mega::io::FileInfo& fileInfo : fileInfos )
    {
        if ( fileInfo.getFilePath().path() == filePath )
        {
            return fileInfo;
        }
    }
    THROW_RTE( "Failed to locate file: " << filePath << " in manifest" );
}

} // namespace interface
} // namespace driver

#endif // BASE_TASK_10_MAY_2022
