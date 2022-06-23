
#include "base_task.hpp"

#include "database/model/ConcreteStage.hxx"
#include "database/model/FinalStage.hxx"

#include "database/types/clang_compilation.hpp"

#include "database/types/sources.hpp"
#include "utilities/clang_format.hpp"

#include "common/file.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"
#include <common/stash.hpp>

#include <vector>
#include <string>

namespace mega
{
namespace compiler
{

class Task_CPP : public BaseTask
{
    const mega::io::cppFilePath& m_sourceFilePath;

public:
    Task_CPP( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath compilationFile
            = m_environment.OperationsStage_Operations( m_sourceFilePath );
        const mega::io::ObjectFilePath cppObjectFile = m_environment.Obj( m_sourceFilePath );
        start( taskProgress, "Task_CPP", m_sourceFilePath.path(), compilationFile.path() );

        using namespace FinalStage;
        Database database( m_environment, m_sourceFilePath );
        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        task::DeterminantHash inputPCHFilesDeterminant;
        {
            for ( const mega::io::megaFilePath& megaSourceFile : pComponent->get_dependencies() )
            {
                inputPCHFilesDeterminant ^= m_environment.getBuildHashCode( m_environment.IncludePCH( megaSourceFile ) );
                inputPCHFilesDeterminant ^= m_environment.getBuildHashCode( m_environment.InterfacePCH( megaSourceFile ) );
                //inputPCHFilesDeterminant ^= m_environment.getBuildHashCode( m_environment.GenericsPCH( megaSourceFile ) );
            }
        }

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, inputPCHFilesDeterminant, m_environment.FilePath( m_sourceFilePath ) } );

        if ( m_environment.restore( cppObjectFile, determinant )
             && m_environment.restore( compilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( cppObjectFile );
            m_environment.setBuildHashCode( compilationFile );
            cached( taskProgress );
            return;
        }

        const std::string strCmd
            = mega::Compilation::make_cpp_compilation( m_environment, m_toolChain, pComponent, m_sourceFilePath )();

        msg( taskProgress, strCmd );

        const int iResult = boost::process::system( strCmd );
        if ( iResult )
        {
            std::ostringstream os;
            os << "Error compiling C++ source file: " << m_sourceFilePath.path();
            throw std::runtime_error( os.str() );
        }

        if ( m_environment.exists( compilationFile ) && m_environment.exists( cppObjectFile ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            m_environment.stash( compilationFile, determinant );

            m_environment.setBuildHashCode( cppObjectFile );
            m_environment.stash( cppObjectFile, determinant );

            succeeded( taskProgress );
        }
        else
        {
            failed( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_CPP( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
{
    return std::make_unique< Task_CPP >( taskArguments, sourceFilePath );
}

} // namespace compiler
} // namespace mega
