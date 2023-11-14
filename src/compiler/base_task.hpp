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

#ifndef BASE_TASK_10_MAY_2022
#define BASE_TASK_10_MAY_2022

#include "pipeline/pipeline.hpp"

#include "compiler/build_report.hpp"
#include "compiler/clang_compilation.hpp"

#include "environment/environment_stash.hpp"

#include "database/sources.hpp"

#include "mega/values/compilation/tool_chain_hash.hpp"

#include "parser/parser.hpp"

#include "common/string.hpp"
#include "common/terminal.hpp"
#include "common/file.hpp"
#include "common/assert_verify.hpp"
#include "common/hash.hpp"
#include "common/stl.hpp"
#include "common/stash.hpp"
#include "common/process.hpp"

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
    DO_STUFF_AND_REQUIRE_SEMI_COLON( if( !( expression ) ) {                                                    \
        std::ostringstream _os2;                                                                                \
        _os2 << common::COLOUR_RED_BEGIN << msg << ". Source Location: " << scoped_id->get_source_file() << ":" \
             << scoped_id->get_line_number() << common::COLOUR_END;                                             \
        throw std::runtime_error( _os2.str() );                                                                 \
    } )

namespace mega::compiler
{
// used by Task_InterfaceTree and others
inline std::string toString( const std::vector< std::string >& name )
{
    std::ostringstream os;
    bool               bFirst = true;
    for( const auto& s : name )
    {
        if( bFirst )
        {
            bFirst = false;
            os << s;
        }
        else
        {
            os << ' ' << s;
        }
    }
    return os.str();
}

struct TaskArguments
{
    TaskArguments( const mega::io::StashEnvironment& environment, const mega::utilities::ToolChain& toolChain,
                   const boost::filesystem::path& unityProjectDir, const boost::filesystem::path& unityEditor,
                   EG_PARSER_INTERFACE* parser )
        : environment( environment )
        , toolChain( toolChain )
        , unityProjectDir( unityProjectDir )
        , unityEditor( unityEditor )
        , parser( parser )
    {
    }
    const mega::io::StashEnvironment& environment;
    const mega::utilities::ToolChain& toolChain;
    const boost::filesystem::path&    unityProjectDir;
    const boost::filesystem::path&    unityEditor;
    EG_PARSER_INTERFACE*              parser;
};

class BaseTask
{
protected:
    TaskName                          m_taskName;
    const mega::io::StashEnvironment& m_environment;
    const mega::utilities::ToolChain& m_toolChain;
    const boost::filesystem::path&    m_unityProjectDir;
    const boost::filesystem::path&    m_unityEditor;
    EG_PARSER_INTERFACE*              m_parser;
    bool                              m_bCompleted = false;

public:
    using Ptr = std::unique_ptr< BaseTask >;

    BaseTask( const TaskArguments& taskArguments )
        : m_environment( taskArguments.environment )
        , m_toolChain( taskArguments.toolChain )
        , m_unityProjectDir( taskArguments.unityProjectDir )
        , m_unityEditor( taskArguments.unityEditor )
        , m_parser( taskArguments.parser )
    {
    }
    virtual ~BaseTask() = default;

    const TaskName& getTaskName() const { return m_taskName; }
    bool            isCompleted() const { return m_bCompleted; }

    template < typename TComponentType, typename TDatabase >
    TComponentType* getComponent( TDatabase& database, const mega::io::SourceFilePath& sourceFilePath ) const
    {
        TComponentType* pComponent = nullptr;
        for( TComponentType* pIter : database.template many< TComponentType >( m_environment.project_manifest() ) )
        {
            for( const mega::io::megaFilePath& megaSourceFile : pIter->get_mega_source_files() )
            {
                if( sourceFilePath == megaSourceFile )
                {
                    pComponent = pIter;
                    break;
                }
            }
            for( const mega::io::cppFilePath& cppSourceFile : pIter->get_cpp_source_files() )
            {
                if( sourceFilePath == cppSourceFile )
                {
                    pComponent = pIter;
                    break;
                }
            }
            for( const mega::io::schFilePath& schSourceFile : pIter->get_sch_source_files() )
            {
                if( sourceFilePath == schSourceFile )
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

    int run_cmd( mega::pipeline::Progress& taskProgress, const common::Command& command, bool bTreatFailureAsError = true )
    {
        std::string strOutput, strError;

        // always print cmd before anything
        {
            taskProgress.onProgress( TaskReport{ TaskReport::eCMD, m_taskName, command.str() }.str() );
        }

        try
        {
            const int iExitCode = common::runCmd( command, strOutput, strError );

            {
                std::ostringstream os;
                {
                    std::istringstream isOut( strOutput );
                    std::string        str;
                    while( isOut && std::getline( isOut, str ) )
                    {
                        if( !str.empty() )
                        {
                            os << TaskReport{ TaskReport::eOUT, m_taskName, str }.str();
                        }
                    }
                }

                os << common::COLOUR_END;
                taskProgress.onProgress( os.str() );
            }

            if( ( iExitCode == EXIT_FAILURE ) && bTreatFailureAsError )
            {
                std::ostringstream osError;
                osError << TaskReport{ TaskReport::eFAILED, m_taskName }.str();

                std::string        str;
                std::istringstream isErr( strError );
                while( isErr && std::getline( isErr, str ) )
                {
                    if( !str.empty() )
                    {
                        osError << TaskReport{ TaskReport::eERROR, m_taskName, str }.str();
                    }
                }

                taskProgress.onProgress( osError.str() );
            }

            return iExitCode;
        }
        catch( std::exception& ex )
        {
            THROW_RTE( "Command : " << command.str() << " failed with exception: " << ex.what() );
        }
        return EXIT_FAILURE;
    }

    int run_pch_compilation( mega::pipeline::Progress& taskProgress, const mega::Compilation& compilationCMD )
    {
        // attempt running using ccache first
        if( EXIT_SUCCESS
            != run_cmd( taskProgress, compilationCMD.generateCompilationCMD( Compilation::eCache_cache ) ) )
        {
            return EXIT_FAILURE;
        }
        else
        {
            // then verify and if fail run without ccache
            if( EXIT_SUCCESS == run_cmd( taskProgress, compilationCMD.generatePCHVerificationCMD(), false ) )
            {
                return EXIT_SUCCESS;
            }
            else if( EXIT_SUCCESS
                     != run_cmd( taskProgress, compilationCMD.generateCompilationCMD( Compilation::eCache_recache ) ) )
            {
                return EXIT_FAILURE;
            }
            else
            {
                return EXIT_SUCCESS;
            }
        }
    }

    void start( mega::pipeline::Progress& taskProgress, const char* pszName, const boost::filesystem::path& fromPath,
                const boost::filesystem::path& toPath )
    {
        m_taskName = TaskName{ pszName, fromPath.string(), toPath.string() };
        taskProgress.onStarted( TaskReport{ TaskReport::eSTARTED, m_taskName }.str() );
    }

    void cached( mega::pipeline::Progress& taskProgress )
    {
        VERIFY_RTE( !m_bCompleted );
        m_bCompleted = true;
        taskProgress.onCompleted( TaskReport{ TaskReport::eCACHED, m_taskName }.str() );
    }

    void succeeded( mega::pipeline::Progress& taskProgress )
    {
        VERIFY_RTE( !m_bCompleted );
        m_bCompleted = true;
        taskProgress.onCompleted( TaskReport{ TaskReport::eSUCCESS, m_taskName }.str() );
    }

    virtual void failed( mega::pipeline::Progress& taskProgress )
    {
        VERIFY_RTE( !m_bCompleted );
        m_bCompleted = true;
        taskProgress.onFailed( TaskReport{ TaskReport::eFAILED, m_taskName }.str() );
    }

    void msg( mega::pipeline::Progress& taskProgress, const std::string& strMsg )
    {
        taskProgress.onProgress( TaskReport{ TaskReport::eMSG, m_taskName, strMsg }.str() );
    }
};

} // namespace mega::compiler

#endif // BASE_TASK_10_MAY_2022
