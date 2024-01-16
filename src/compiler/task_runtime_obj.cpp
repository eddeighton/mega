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

#include "database/ClangAnalysisStage.hxx"

#include "mega/common_strings.hpp"

#include "common/file.hpp"

#include <memory>
#include <iostream>

namespace ClangAnalysisStage
{
#include "compiler/interface.hpp"
#include "compiler/concrete.hpp"

} // namespace ClangAnalysisStage

using namespace ClangAnalysisStage;

namespace mega::compiler
{

class Task_Runtime_Obj : public BaseTask
{
    const mega::io::manifestFilePath m_manifestFilePath;

public:
    Task_Runtime_Obj( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
        , m_manifestFilePath( m_environment.project_manifest() )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::GeneratedCPPSourceFilePath runtimeSourceFile = m_environment.RuntimeSource();
        const mega::io::ObjectFilePath             objFile           = m_environment.RuntimeObj();
        start( taskProgress, "Task_Runtime_Obj", runtimeSourceFile.path(), objFile.path() );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash } );

        if( m_environment.restore( objFile, determinant ) )
        {
            m_environment.setBuildHashCode( objFile );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_manifestFilePath, true );

        Components::Component* pInterfaceComponent = nullptr;
        {
            for( auto pComponent : database.many< Components::Component >( m_environment.project_manifest() ) )
            {
                if( pComponent->get_type() == mega::ComponentType::eInterface )
                {
                    VERIFY_RTE_MSG( !pInterfaceComponent, "Multiple interface components found" );
                    pInterfaceComponent = pComponent;
                }
            }
        }

        const mega::Compilation compilationCMD
            = mega::Compilation::make_runtime_obj_compilation( m_environment, m_toolChain, pInterfaceComponent );

        if( run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            std::ostringstream os;
            os << "Error compiling C++ source file: " << runtimeSourceFile.path();
            msg( taskProgress, os.str() );
            failed( taskProgress );
            return;
        }

        if( m_environment.exists( objFile ) )
        {
            m_environment.setBuildHashCode( objFile );
            m_environment.stash( objFile, determinant );

            succeeded( taskProgress );
        }
        else
        {
            failed( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_Runtime_Obj( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_Runtime_Obj >( taskArguments );
}

} // namespace mega::compiler
