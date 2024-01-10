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

#include "database/ObjectStage.hxx"

#include "mega/common_strings.hpp"

#include "common/file.hpp"

#include <memory>
#include <iostream>

using namespace ObjectStage;

namespace mega::compiler
{

class Task_CPP_Compile : public BaseTask
{
    const mega::io::cppFilePath& m_sourceFilePath;

public:
    Task_CPP_Compile( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::GeneratedHPPSourceFilePath cppHeaderFile   = m_environment.CPPSource( m_sourceFilePath );
        const mega::io::PrecompiledHeaderFile      cppPCH          = m_environment.CPPPCH( m_sourceFilePath );
        const mega::io::CompilationFilePath        compilationFile = m_environment.ObjectStage_Obj( m_sourceFilePath );

        start( taskProgress, "Task_CPP_Compile", cppHeaderFile.path(), cppPCH.path() );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash } );

        if( m_environment.restore( compilationFile, determinant )
            && m_environment.restore( cppHeaderFile, determinant ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            m_environment.setBuildHashCode( cppHeaderFile );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_sourceFilePath );

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

        const mega::Compilation compilationCMD = mega::Compilation::make_cpp_pch_compilation(
            m_environment, m_toolChain, pInterfaceComponent, m_sourceFilePath );

        if( EXIT_SUCCESS == run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            m_environment.stash( compilationFile, determinant );

            m_environment.setBuildHashCode( cppPCH );
            m_environment.stash( cppPCH, determinant );

            succeeded( taskProgress );
            return;
        }
        failed( taskProgress );
    }
};

BaseTask::Ptr create_Task_CPP_Compile( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
{
    return std::make_unique< Task_CPP_Compile >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
