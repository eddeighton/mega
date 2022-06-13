#ifndef BASE_TASK_10_MAY_2022
#define BASE_TASK_10_MAY_2022

#include "pipeline/pipeline.hpp"

#include "database/common/environments.hpp"
#include "database/types/sources.hpp"

#include "utilities/tool_chain_hash.hpp"

#include "parser/parser.hpp"

#include "common/string.hpp"
#include "common/terminal.hpp"
#include "common/file.hpp"
#include "common/assert_verify.hpp"
#include "common/hash.hpp"
#include "common/stl.hpp"
#include "common/stash.hpp"

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
#include <memory>

#define VERIFY_PARSER( expression, msg, scoped_id )                                                             \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( if ( !( expression ) ) {                                                   \
        std::ostringstream _os2;                                                                                \
        _os2 << common::COLOUR_RED_BEGIN << msg << ". Source Location: " << scoped_id->get_source_file() << ":" \
             << scoped_id->get_line_number() << common::COLOUR_END;                                             \
        throw std::runtime_error( _os2.str() );                                                                 \
    } )

namespace mega
{
namespace compiler
{
struct TaskArguments
{
    TaskArguments( const mega::io::StashEnvironment& environment, const mega::utilities::ToolChain& toolChain,
                   EG_PARSER_INTERFACE* parser )
        : environment( environment )
        , toolChain( toolChain )
        , parser( parser )
    {
    }
    const mega::io::StashEnvironment& environment;
    const mega::utilities::ToolChain& toolChain;
    EG_PARSER_INTERFACE*              parser;
};

class BaseTask
{
protected:
    std::string                       m_strTaskName;
    const mega::io::StashEnvironment& m_environment;
    const mega::utilities::ToolChain& m_toolChain;
    EG_PARSER_INTERFACE*              m_parser;
    bool                              m_bCompleted = false;

public:
    using Ptr = std::unique_ptr< BaseTask >;

    BaseTask( const TaskArguments& taskArguments )
        : m_environment( taskArguments.environment )
        , m_toolChain( taskArguments.toolChain )
        , m_parser( taskArguments.parser )
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
             os << "STARTED: " <<  common::COLOUR_YELLOW_BEGIN <<  m_strTaskName << common::COLOUR_END;
            taskProgress.onStarted( os.str() );
        }
    }

    void cached( mega::pipeline::Progress& taskProgress )
    {
        VERIFY_RTE( !m_bCompleted );
        m_bCompleted = true;

        std::ostringstream os;
        os << common::COLOUR_CYAN_BEGIN << "CACHED : " << m_strTaskName << common::COLOUR_END;
        taskProgress.onCompleted( os.str() );
    }

    void succeeded( mega::pipeline::Progress& taskProgress )
    {
        VERIFY_RTE( !m_bCompleted );
        m_bCompleted = true;

        std::ostringstream os;
        os << common::COLOUR_GREEN_BEGIN << "SUCCESS: " << m_strTaskName << common::COLOUR_END;
        taskProgress.onCompleted( os.str() );
    }

    virtual void failed( mega::pipeline::Progress& taskProgress )
    {
        VERIFY_RTE( !m_bCompleted );
        m_bCompleted = true;

        std::ostringstream os;
        os << common::COLOUR_RED_BEGIN << "FAILED : " << m_strTaskName << common::COLOUR_END;
        taskProgress.onFailed( os.str() );
    }

    void msg( mega::pipeline::Progress& taskProgress, const std::string& strMsg )
    {
        std::ostringstream os;
        os << common::COLOUR_BLUE_BEGIN << "MSG    : " << m_strTaskName << strMsg << common::COLOUR_END;
        taskProgress.onProgress( os.str() );
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

} // namespace compiler
} // namespace mega

#endif // BASE_TASK_10_MAY_2022
