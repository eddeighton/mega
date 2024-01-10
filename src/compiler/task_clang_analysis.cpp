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

#include "compiler/clang_compilation.hpp"

#include "mega/common_strings.hpp"

#include "mega/values/compilation/hyper_graph.hpp"
#include "mega/values/runtime/pointer.hpp"
#include "mega/make_unique_without_reorder.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <memory>

namespace ClangAnalysisStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/interface_visitor.hpp"
#include "compiler/common_ancestor.hpp"

} // namespace ClangAnalysisStage

namespace mega::compiler
{

using namespace ClangAnalysisStage;

class Task_Clang_Traits_Analysis : public BaseTask
{
    const mega::io::manifestFilePath m_manifestFilePath;

public:
    Task_Clang_Traits_Analysis( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
        , m_manifestFilePath( m_environment.project_manifest() )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::GeneratedHPPSourceFilePath traitsHeader = m_environment.ClangTraits();
        const mega::io::CompilationFilePath        clangTraitsAnalysisFile
            = m_environment.ClangTraitsStage_Traits( m_manifestFilePath );

        task::DeterminantHash determinant(
            m_toolChain.toolChainHash, m_environment.FilePath( traitsHeader ),
            m_environment.getBuildHashCode( m_environment.InterfaceStage_Tree( m_manifestFilePath ) ),
            m_environment.getBuildHashCode( m_environment.ConcreteStage_Concrete( m_manifestFilePath ) ) );

        for( const mega::io::megaFilePath& sourceFilePath : getSortedSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) );
        }

        start( taskProgress, "Task_Clang_Traits_Analysis", traitsHeader.path(), clangTraitsAnalysisFile.path() );

        if( m_environment.restore( clangTraitsAnalysisFile, determinant ) )
        {
            m_environment.setBuildHashCode( clangTraitsAnalysisFile );
            cached( taskProgress );
        }
        else
        {
            Database database( m_environment, m_manifestFilePath );

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

            const mega::Compilation compilationCMD = mega::Compilation::make_clang_traits_analysis_compilation(
                m_environment, m_toolChain, pInterfaceComponent );

            if( EXIT_SUCCESS == run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
            {
                m_environment.setBuildHashCode( clangTraitsAnalysisFile );
                m_environment.stash( clangTraitsAnalysisFile, determinant );

                succeeded( taskProgress );
                return;
            }
            else
            {
                failed( taskProgress );
            }
        }
    }
};

BaseTask::Ptr create_Task_Clang_Traits_Analysis( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_Clang_Traits_Analysis >( taskArguments );
}

} // namespace mega::compiler
