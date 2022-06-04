#ifndef BASE_TASK_10_MAY_2022
#define BASE_TASK_10_MAY_2022

#include "pipeline/pipeline.hpp"

#include "database/common/environments.hpp"
#include "database/types/sources.hpp"

#include "utilities/tool_chain_hash.hpp"

#include "common/string.hpp"
#include "common/terminal.hpp"
#include "common/file.hpp"
#include "common/assert_verify.hpp"
#include "common/hash.hpp"
#include "common/stl.hpp"
#include "common/stash.hpp"

//#include "spdlog/spdlog.h"
//#include "spdlog/fmt/bundled/color.h"

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

#include <iostream>
#include <ostream>
#include <vector>
#include <string>

/*
inline std::ostream& operator<<( std::ostream& os, const std::vector< std::string >& name )
{
    common::delimit( name.begin(), name.end(), ".", os );
    return os;
}
*/

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

struct TaskArguments
{
    TaskArguments( const mega::io::StashEnvironment& environment, const mega::utilities::ToolChain& toolChain )
        : environment( environment )
        , toolChain( toolChain )
    {
    }
    const mega::io::StashEnvironment& environment;
    const mega::utilities::ToolChain& toolChain;
};
/*
static const char psz_start[]   = "{} START   {:>55} -> {:<55}";
static const char psz_cached[]  = "{} CACHED  {:>55} -> {:<55} : {}";
static const char psz_success[] = "{} SUCCESS {:>55} -> {:<55} : {}";
static const char psz_error[]   = "{} ERROR   {:>55} -> {:<55} : {}";
static const char psz_msg[]     = "{} MSG     {:>55} -> {:<55} : {}\n{}";
*/
class BaseTask
{
protected:
    std::string                       m_strTaskName;
    const mega::io::StashEnvironment& m_environment;
    const mega::utilities::ToolChain& m_toolChain;

public:
    using Ptr = std::unique_ptr< BaseTask >;

    BaseTask( const TaskArguments& taskArguments )
        : m_environment( taskArguments.environment )
        , m_toolChain( taskArguments.toolChain )
    {
    }
    virtual ~BaseTask() {}

    const std::string& getTaskName() const { return m_strTaskName; }

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

    virtual void run( mega::pipeline::Progress& taskProgress ) = 0;

    void start( mega::pipeline::Progress& taskProgress, const char* pszName, const boost::filesystem::path& fromPath,
                const boost::filesystem::path& toPath )
    {
        {
            std::ostringstream os;
            os << std::setw( 32 ) << pszName << " " << std::setw( 55 ) << fromPath.string() << " -> " << std::setw( 55 )
               << toPath.string();
            m_strTaskName = os.str();
        }

        {
            std::ostringstream os;
            os << "STARTED: " << m_strTaskName;
            taskProgress.onStarted( os.str() );
        }

        // taskProgress.onStarted(
        //    fmt::format( fmt::bg( fmt::terminal_color::bright_black ) | fmt::fg( fmt::terminal_color::white )
        //                               | fmt::emphasis::bold,
        //                           m_strTaskName ));
    }

    void cached( mega::pipeline::Progress& taskProgress )
    {
        std::ostringstream os;
        os << "CACHED : " << m_strTaskName;
        taskProgress.onCompleted( os.str() );

        // taskProgress.cached();
        /*spdlog::info( psz_cached,
                      fmt::format( fmt::bg( fmt::terminal_color::cyan ) | fmt::fg( fmt::terminal_color::black )
                                       | fmt::emphasis::bold,
                                   name( taskProgress ) ),
                      source( taskProgress ), target( taskProgress ), elapsed( taskProgress ) );*/
    }
    void succeeded( mega::pipeline::Progress& taskProgress )
    {
        std::ostringstream os;
        os << "SUCCESS: " << m_strTaskName;
        taskProgress.onCompleted( os.str() );
        // taskProgress.succeeded();
        /*spdlog::info( psz_success,
                      fmt::format( fmt::bg( fmt::terminal_color::green ) | fmt::fg( fmt::terminal_color::black )
                                       | fmt::emphasis::bold,
                                   name( taskProgress ) ),
                      source( taskProgress ), target( taskProgress ), elapsed( taskProgress ) );*/
    }
    virtual void failed( mega::pipeline::Progress& taskProgress )
    {
        std::ostringstream os;
        os << "FAILED : " << m_strTaskName;
        taskProgress.onFailed( os.str() );
        // taskProgress.failed();
        /*spdlog::critical( psz_error,
                          fmt::format( fmt::bg( fmt::terminal_color::red ) | fmt::fg( fmt::terminal_color::black )
                                           | fmt::emphasis::bold,
                                       name( taskProgress ) ),
                          source( taskProgress ), target( taskProgress ), elapsed( taskProgress ) );*/
    }
    void msg( mega::pipeline::Progress& taskProgress, const std::string& strMsg )
    {
        std::ostringstream os;
        os << "MSG    : " << m_strTaskName << strMsg;
        taskProgress.onProgress( os.str() );
        /*spdlog::info( psz_msg,
                      fmt::format( fmt::bg( fmt::terminal_color::black ) | fmt::fg( fmt::terminal_color::white ),
                                   name( taskProgress ) ),
                      source( taskProgress ), target( taskProgress ), elapsed( taskProgress ), strMsg );*/
    }
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
