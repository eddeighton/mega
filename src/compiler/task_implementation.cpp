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

#include "database/FinalStage.hxx"

#include "compiler/clang_compilation.hpp"

#include "mega/mangle/traits.hpp"

#include <common/file.hpp>
#include <common/string.hpp>

#include "mega/values/compilation/operation_id.hpp"
#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

namespace FinalStage
{
    #include "compiler/generator_implementation.hpp"
}

namespace mega::compiler
{

class Task_Implementation : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_Implementation( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;
        using namespace FinalStage::Interface;

        const mega::io::GeneratedCPPSourceFilePath implementationFile
            = m_environment.Implementation( m_sourceFilePath );

        start( taskProgress, "Task_Implementation", m_sourceFilePath.path(), implementationFile.path() );

        task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.ImplementationTemplate(),
              m_environment.getBuildHashCode( m_environment.OperationsStage_Operations( m_sourceFilePath ) ) } );

        if( m_environment.restore( implementationFile, determinant ) )
        {
            m_environment.setBuildHashCode( implementationFile );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_environment.project_manifest() );

        std::ostringstream os;
        {
            ImplementationGen implGen( m_environment, m_sourceFilePath.path().string() );
            implGen.generate( database.one< Operations::Invocations >( m_sourceFilePath ), os );
        }
        if( boost::filesystem::updateFileIfChanged( m_environment.FilePath( implementationFile ), os.str() ) )
        {
            m_environment.setBuildHashCode( implementationFile );
            m_environment.stash( implementationFile, determinant );
            succeeded( taskProgress );
        }
        else
        {
            m_environment.setBuildHashCode( implementationFile );
            cached( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_Implementation( const TaskArguments&          taskArguments,
                                          const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_Implementation >( taskArguments, sourceFilePath );
}

class Task_ImplementationObj : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_ImplementationObj( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;
        using namespace FinalStage::Interface;

        const mega::io::GeneratedCPPSourceFilePath implementationFile
            = m_environment.Implementation( m_sourceFilePath );

        const mega::io::PrecompiledHeaderFile includePCH        = m_environment.IncludePCH( m_sourceFilePath );
        const mega::io::PrecompiledHeaderFile interfacePCH      = m_environment.InterfacePCH( m_sourceFilePath );
        const mega::io::PrecompiledHeaderFile operationsPCH     = m_environment.OperationsPCH( m_sourceFilePath );
        const mega::io::ObjectFilePath        implementationObj = m_environment.ImplementationObj( m_sourceFilePath );

        start( taskProgress, "Task_ImplementationObj", m_sourceFilePath.path(), implementationObj.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( includePCH ),
              m_environment.getBuildHashCode( interfacePCH ), m_environment.getBuildHashCode( operationsPCH ),
              m_environment.getBuildHashCode( implementationFile ) } );

        if( m_environment.restore( implementationObj, determinant ) )
        {
            m_environment.setBuildHashCode( implementationObj );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_environment.project_manifest() );

        const mega::Compilation compilationCMD = mega::Compilation::make_implementationObj_compilation(
            m_environment,
            m_toolChain,
            getComponent< Components::Component >( database, m_sourceFilePath ),
            m_sourceFilePath );

        if( run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            failed( taskProgress );
            return;
        }

        if( m_environment.exists( implementationObj ) )
        {
            m_environment.setBuildHashCode( implementationObj );
            m_environment.stash( implementationObj, determinant );
            succeeded( taskProgress );
        }
        else
        {
            failed( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_ImplementationObj( const TaskArguments&          taskArguments,
                                             const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_ImplementationObj >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
