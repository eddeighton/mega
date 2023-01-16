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

#include "database/common/environment_stash.hpp"

#include "database/types/sources.hpp"

// #include "database/model/file_info.hxx"

#include "utilities/tool_chain_hash.hpp"

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
    DO_STUFF_AND_REQUIRE_SEMI_COLON( if ( !( expression ) ) {                                                   \
        std::ostringstream _os2;                                                                                \
        _os2 << common::COLOUR_RED_BEGIN << msg << ". Source Location: " << scoped_id->get_source_file() << ":" \
             << scoped_id->get_line_number() << common::COLOUR_END;                                             \
        throw std::runtime_error( _os2.str() );                                                                 \
    } )

namespace mega::compiler
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
    bool               isCompleted() const { return m_bCompleted; }

    template < typename TComponentType, typename TDatabase >
    TComponentType* getComponent( TDatabase& database, const mega::io::SourceFilePath& sourceFilePath ) const
    {
        TComponentType* pComponent = nullptr;
        for ( TComponentType* pIter : database.template many< TComponentType >( m_environment.project_manifest() ) )
        {
            for ( const mega::io::megaFilePath& megaSourceFile : pIter->get_mega_source_files() )
            {
                if ( sourceFilePath == megaSourceFile )
                {
                    pComponent = pIter;
                    break;
                }
            }
            for ( const mega::io::cppFilePath& cppSourceFile : pIter->get_cpp_source_files() )
            {
                if ( sourceFilePath == cppSourceFile )
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

    int run_cmd( mega::pipeline::Progress& taskProgress, const std::string& strCmd, bool bTreatFailureAsError = true )
    {

        std::string strOutput, strError;
        const int iExitCode = common::runProcess( strCmd, strOutput, strError );
        
        {
            std::ostringstream os;
            os << common::COLOUR_BLUE_BEGIN << "MSG    : " << m_strTaskName << "\nCMD    : " << strCmd;
            {
                std::istringstream isOut( strOutput );
                std::string str;
                while ( isOut && std::getline( isOut, str ) )
                {
                    if ( !str.empty() )
                    {
                        os << "\nOUT    : " << str;
                    }
                }
            }
            
            os << common::COLOUR_END;
            taskProgress.onProgress( os.str() );
        }
        
        if ( iExitCode && bTreatFailureAsError )
        {
            std::istringstream isErr( strError );
        
            std::ostringstream osError;
            osError << common::COLOUR_RED_BEGIN << "FAILED : " << m_strTaskName;
            std::string str;
            while ( isErr && std::getline( isErr, str ) )
            {
                if ( !str.empty() )
                {
                    osError << "\nERROR  : " << str;
                }
            }
            osError << common::COLOUR_END;
            taskProgress.onProgress( osError.str() );
        }
        
        return iExitCode;
    }

    void start( mega::pipeline::Progress& taskProgress, const char* pszName, const boost::filesystem::path& fromPath,
                const boost::filesystem::path& toPath )
    {
        {
            std::ostringstream os;
            os << std::setw( 30 ) << pszName << " " << std::setw( 35 ) << fromPath.string() << " -> " << std::setw( 85 )
               << toPath.string();
            m_strTaskName = os.str();
        }

        {
            std::ostringstream os;
            os << "STARTED: " << common::COLOUR_YELLOW_BEGIN << m_strTaskName << common::COLOUR_END;
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
        os << common::COLOUR_BLUE_BEGIN << "MSG    : " << m_strTaskName << "\nMSG    : " << strMsg
           << common::COLOUR_END;
        taskProgress.onProgress( os.str() );
    }
};

} // namespace mega::compiler

#endif // BASE_TASK_10_MAY_2022
