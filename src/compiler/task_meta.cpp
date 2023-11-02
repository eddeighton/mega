
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

#include "base_task.hpp"

#include "database/MetaStage.hxx"
#include "database/component_type.hpp"
#include "database/sources.hpp"

#include <common/stash.hpp>

namespace mega::compiler
{

class Task_Meta : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_Meta( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath megaAnalysisCompilationFile
            = m_environment.MetaStage_MetaAnalysis( m_sourceFilePath );

        start( taskProgress, "Task_Meta", m_sourceFilePath.path(), megaAnalysisCompilationFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash,
              m_environment.getBuildHashCode( m_environment.InterfaceStage_Tree( m_sourceFilePath ) ) } );

        if( m_environment.restore( megaAnalysisCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( megaAnalysisCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace MetaStage;
        Database database( m_environment, m_sourceFilePath );

        using namespace std::string_literals;
        static const U64                                       TotalMetaTypes = 2;
        static const std::array< std::string, TotalMetaTypes > metaTypes      = { "OR"s, "Historical"s };

        for( Interface::State* pState : database.many< Interface::State >( m_sourceFilePath ) )
        {
            std::array< bool, TotalMetaTypes > metaValues{ false, false };

            if( pState->get_inheritance_trait().has_value() )
            {
                auto inheritance = pState->get_inheritance_trait().value();
                for( const std::string& strIdentifier : inheritance->get_strings() )
                {
                    auto iFind = std::find( metaTypes.begin(), metaTypes.end(), strIdentifier );
                    if( iFind != metaTypes.end() )
                    {
                        auto index          = std::distance( metaTypes.begin(), iFind );
                        metaValues[ index ] = true;
                    }
                }
            }

            database.construct< Interface::State >(
                Interface::State::Args{ pState, metaValues[ 0 ], metaValues[ 1 ] } );
        }

        const task::FileHash fileHashCode = database.save_MetaAnalysis_to_temp();
        m_environment.setBuildHashCode( megaAnalysisCompilationFile, fileHashCode );
        m_environment.temp_to_real( megaAnalysisCompilationFile );
        m_environment.stash( megaAnalysisCompilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Meta( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_Meta >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
