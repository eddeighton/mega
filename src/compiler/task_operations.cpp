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

#include "database/types/clang_compilation.hpp"

#include "database/types/sources.hpp"
#include "utilities/clang_format.hpp"

#include "common/file.hpp"
#include <common/stash.hpp>

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <vector>
#include <string>

namespace OperationsStage
{
using namespace OperationsStage;
using namespace OperationsStage::Interface;
#include "compiler/generator_operations.hpp"
} // namespace OperationsStage

namespace mega::compiler
{

class Task_Operations : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_Operations( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath concreteFile = m_environment.ConcreteStage_Concrete( m_sourceFilePath );
        ;
        const mega::io::GeneratedHPPSourceFilePath operationsFile = m_environment.Operations( m_sourceFilePath );
        start( taskProgress, "Task_Operations", m_sourceFilePath.path(), operationsFile.path() );

        task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.OperationsTemplate(), m_environment.OperationsExternsTemplate(),
              m_environment.getBuildHashCode( m_environment.ParserStage_Body( m_sourceFilePath ) ),
              m_environment.getBuildHashCode(
                  m_environment.ConcreteTypeRollout_PerSourceConcreteTable( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( concreteFile ) } );

        if( m_environment.restore( operationsFile, determinant ) )
        {
            m_environment.setBuildHashCode( operationsFile );
            cached( taskProgress );
            return;
        }

        {
            using namespace OperationsStage;
            using namespace OperationsStage::Interface;

            Database database( m_environment, m_sourceFilePath );
            // NOTE: MUST ensure automata analysis loaded
            database.many< Automata::Start >( m_sourceFilePath );

            std::string strOperations;
            {
                ::inja::Environment injaEnvironment;
                {
                    injaEnvironment.set_trim_blocks( true );
                }
                OperationsGen::TemplateEngine templateEngine( m_environment, injaEnvironment );
                Interface::Root*              pRoot = database.one< Interface::Root >( m_sourceFilePath );
                strOperations                       = OperationsGen::generate( templateEngine, pRoot, true );
            }
            mega::utilities::clang_format( strOperations, std::optional< boost::filesystem::path >() );
            boost::filesystem::updateFileIfChanged( m_environment.FilePath( operationsFile ), strOperations );
        }

        m_environment.setBuildHashCode( operationsFile );
        m_environment.stash( operationsFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Operations( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_Operations >( taskArguments, sourceFilePath );
}

class Task_OperationsPCH : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_OperationsPCH( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath compilationFile
            = m_environment.OperationsStage_Operations( m_sourceFilePath );
        const mega::io::GeneratedHPPSourceFilePath operationsHPPFile = m_environment.Operations( m_sourceFilePath );
        const mega::io::PrecompiledHeaderFile      operationsPCH     = m_environment.OperationsPCH( m_sourceFilePath );
        start( taskProgress, "Task_OperationsPCH", m_sourceFilePath.path(), operationsPCH.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( operationsHPPFile ),
              m_environment.getBuildHashCode( m_environment.IncludePCH( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.InterfacePCH( m_sourceFilePath ) ) } );

        using namespace OperationsStage;

        Database               database( m_environment, m_sourceFilePath );
        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        const mega::Compilation compilationCMD = mega::Compilation::make_operationsPCH_compilation(
            m_environment, m_toolChain, pComponent, m_sourceFilePath );

        if( m_environment.restore( operationsPCH, determinant )
            && m_environment.restore( compilationFile, determinant ) )
        {
            // if( !run_cmd( taskProgress, compilationCMD.generatePCHVerificationCMD() ) )
            {
                m_environment.setBuildHashCode( operationsPCH );
                m_environment.setBuildHashCode( compilationFile );
                cached( taskProgress );
                return;
            }
        }

        if( run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            std::ostringstream os;
            os << "Error compiling operations pch file for source file: " << m_sourceFilePath.path();
            msg( taskProgress, os.str() );
            failed( taskProgress );
            return;
        }
        else
        {
            if( m_environment.exists( compilationFile ) && m_environment.exists( operationsPCH ) )
            {
                m_environment.setBuildHashCode( compilationFile );
                m_environment.stash( compilationFile, determinant );

                m_environment.setBuildHashCode( operationsPCH );
                m_environment.stash( operationsPCH, determinant );

                succeeded( taskProgress );
            }
            else
            {
                failed( taskProgress );
            }
        }
    }
};

BaseTask::Ptr create_Task_OperationsPCH( const TaskArguments&          taskArguments,
                                         const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_OperationsPCH >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
