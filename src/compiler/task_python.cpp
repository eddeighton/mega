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

#include "database/model/OperationsStage.hxx"

#include "database/types/component_type.hpp"
#include "database/types/sources.hpp"
#include <boost/filesystem/operations.hpp>
#include <common/stash.hpp>

namespace mega::compiler
{

class Task_PythonOperations : public BaseTask
{
    const mega::io::manifestFilePath& m_manifest;

public:
    Task_PythonOperations( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifest )
        : BaseTask( taskArguments )
        , m_manifest( manifest )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath pythonOperationsCompilationFile
            = m_environment.OperationsStage_Operations( m_manifest );
        start( taskProgress, "Task_PythonOperations", m_manifest.path(), pythonOperationsCompilationFile.path() );

        const task::DeterminantHash determinant = { m_toolChain.toolChainHash };

        if( m_environment.restore( pythonOperationsCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( pythonOperationsCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace OperationsStage;

        Database database( m_environment, m_manifest );

        const task::FileHash fileHashCode = database.save_Operations_to_temp();
        m_environment.setBuildHashCode( pythonOperationsCompilationFile, fileHashCode );
        m_environment.temp_to_real( pythonOperationsCompilationFile );
        m_environment.stash( pythonOperationsCompilationFile, determinant );
        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_PythonOperations( const TaskArguments&              taskArguments,
                                  const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_PythonOperations >( taskArguments, manifestFilePath );
}

} // namespace mega::compiler
