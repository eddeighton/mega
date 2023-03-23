
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

#include "database/model/MetaStage.hxx"
#include "database/types/component_type.hpp"
#include "database/types/sources.hpp"

#include <common/stash.hpp>

namespace mega::compiler
{

class Task_Meta : public BaseTask
{
    const mega::io::manifestFilePath& m_manifest;

public:
    Task_Meta( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifest )
        : BaseTask( taskArguments )
        , m_manifest( manifest )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath megaAnalysisCompilationFile
            = m_environment.MetaStage_MetaAnalysis( m_manifest );

        start( taskProgress, "Task_Meta", m_manifest.path(), megaAnalysisCompilationFile.path() );

        // const task::FileHash previousStageHash = m_environment.getBuildHashCode(
        //    m_environment.OperationsStage_Operations() ( m_schematicFilePath ) );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash } );

        if( m_environment.restore( megaAnalysisCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( megaAnalysisCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace MetaStage;
        Database database( m_environment, m_manifest );


        

        // Components::Component* pComponent = getComponent< Components::Component >( database, m_schematicFilePath );

        const task::FileHash fileHashCode = database.save_MetaAnalysis_to_temp();
        m_environment.setBuildHashCode( megaAnalysisCompilationFile, fileHashCode );
        m_environment.temp_to_real( megaAnalysisCompilationFile );
        m_environment.stash( megaAnalysisCompilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Meta( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_Meta >( taskArguments, manifestFilePath );
}

} // namespace mega::compiler
