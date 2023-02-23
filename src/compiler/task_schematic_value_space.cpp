
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

#include "database/model/SchematicValueSpaceStage.hxx"
#include "database/types/component_type.hpp"
#include "database/types/sources.hpp"

#include <common/stash.hpp>

namespace mega::compiler
{

class Task_SchematicValueSpace : public BaseTask
{
    const mega::io::schFilePath& m_schematicFilePath;

public:
    Task_SchematicValueSpace( const TaskArguments& taskArguments, const mega::io::schFilePath& schematicFilePath )
        : BaseTask( taskArguments )
        , m_schematicFilePath( schematicFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath schematicValueSpaceCompilationFile
            = m_environment.SchematicValueSpaceStage_SchematicValueSpace( m_schematicFilePath );

        start( taskProgress,
               "Task_SchematicValueSpace",
               m_schematicFilePath.path(),
               schematicValueSpaceCompilationFile.path() );

        const task::FileHash previousStageHash = m_environment.getBuildHashCode(
            m_environment.SchematicVisibilityStage_SchematicVisibility( m_schematicFilePath ) );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash, previousStageHash } );

        if( m_environment.restore( schematicValueSpaceCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( schematicValueSpaceCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace SchematicValueSpaceStage;
        Database database( m_environment, m_schematicFilePath );

        // Components::Component* pComponent = getComponent< Components::Component >( database, m_schematicFilePath );

        const task::FileHash fileHashCode = database.save_SchematicValueSpace_to_temp();
        m_environment.setBuildHashCode( schematicValueSpaceCompilationFile, fileHashCode );
        m_environment.temp_to_real( schematicValueSpaceCompilationFile );
        m_environment.stash( schematicValueSpaceCompilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_SchematicValueSpace( const TaskArguments&         taskArguments,
                                                const mega::io::schFilePath& schematicFilePath )
{
    return std::make_unique< Task_SchematicValueSpace >( taskArguments, schematicFilePath );
}

} // namespace mega::compiler
