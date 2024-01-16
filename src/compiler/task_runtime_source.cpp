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

class Task_Runtime_Source : public BaseTask
{
    const mega::io::manifestFilePath m_manifestFilePath;

public:
    Task_Runtime_Source( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
        , m_manifestFilePath( m_environment.project_manifest() )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath compilationFilePath
            = m_environment.ClangAnalysisStage_Analysis( m_manifestFilePath );
        const mega::io::GeneratedCPPSourceFilePath runtimeSource = m_environment.RuntimeSource();

        start( taskProgress, "Task_Runtime_Source", compilationFilePath.path(), runtimeSource.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( compilationFilePath ) } );

        if( m_environment.restore( runtimeSource, determinant ) )
        {
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
        VERIFY_RTE( pInterfaceComponent );

        std::ostringstream osSourceFile;

        if( boost::filesystem::updateFileIfChanged( m_environment.FilePath( runtimeSource ), osSourceFile.str() ) )
        {
            m_environment.stash( runtimeSource, determinant );
            succeeded( taskProgress );
        }
        else
        {
            cached( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_Runtime_Source( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_Runtime_Source >( taskArguments );
}

} // namespace mega::compiler
