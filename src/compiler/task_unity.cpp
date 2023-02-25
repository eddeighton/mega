
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

#include "database/model/UnityStage.hxx"
#include "database/types/component_type.hpp"
#include "database/types/sources.hpp"

#include <common/stash.hpp>

namespace mega::compiler
{

class Task_Unity : public BaseTask
{
    const mega::io::manifestFilePath& m_manifest;

public:
    Task_Unity( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifest )
        : BaseTask( taskArguments )
        , m_manifest( manifest )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath unityAnalysisCompilationFile
            = m_environment.UnityStage_UnityAnalysis( m_manifest );

        start( taskProgress, "Task_Unity", m_manifest.path(), unityAnalysisCompilationFile.path() );

        // const task::FileHash previousStageHash = m_environment.getBuildHashCode(
        //    m_environment.OperationsStage_Operations() ( m_schematicFilePath ) );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash } );

        if( m_environment.restore( unityAnalysisCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( unityAnalysisCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace UnityStage;
        Database database( m_environment, m_manifest );

        // ./Unity 
        // -projectPath /workspace/root/src/mcgs/MCGS/ 
        // -logFile /workspace/root/src/mcgs/log.txt 
        // -noUpm -batchmode  -quit -nographics -disable-gpu-skinning -disable-assembly-updater -disableManagedDebugger 
        // -executeMethod UnityProtocol.Run


        const task::FileHash fileHashCode = database.save_UnityAnalysis_to_temp();
        m_environment.setBuildHashCode( unityAnalysisCompilationFile, fileHashCode );
        m_environment.temp_to_real( unityAnalysisCompilationFile );
        m_environment.stash( unityAnalysisCompilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Unity( const TaskArguments&              taskArguments,
                                 const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_Unity >( taskArguments, manifestFilePath );
}

} // namespace mega::compiler
