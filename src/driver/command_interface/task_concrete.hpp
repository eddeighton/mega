#ifndef TASK_CONCRETE_12_MAY_2022
#define TASK_CONCRETE_12_MAY_2022

#include "base_task.hpp"

#include "database/model/ConcreteStage.hxx"
#include <common/stash.hpp>

namespace driver
{
namespace interface
{

class Task_ConcreteTree : public BaseTask
{
public:
    Task_ConcreteTree( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( task::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath interfaceAnalysisFile = m_environment.InterfaceAnalysisStage_Clang( m_sourceFilePath );
        const mega::io::CompilationFilePath concreteFile = m_environment.ConcreteStage_Concrete( m_sourceFilePath );
        start( taskProgress, "Task_ConcreteTree", interfaceAnalysisFile.path(), concreteFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( interfaceAnalysisFile ) } );

        if ( m_environment.restore( concreteFile, determinant ) )
        {
            m_environment.setBuildHashCode( concreteFile );
            cached( taskProgress );
            return;
        }

        using namespace ConcreteStage;
        using namespace ConcreteStage::Concrete;

        Database database( m_environment, m_sourceFilePath );

        Interface::Root* pRoot = database.one< Interface::Root >( m_sourceFilePath );





        const task::FileHash buildHash = database.save_Concrete_to_temp();
        m_environment.temp_to_real( concreteFile );
        m_environment.setBuildHashCode( concreteFile, buildHash );
        m_environment.stash( concreteFile, determinant );

        succeeded( taskProgress );
    }

    const mega::io::megaFilePath& m_sourceFilePath;
};


}}

#endif //TASK_CONCRETE_12_MAY_2022
