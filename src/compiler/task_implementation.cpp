#include "base_task.hpp"

#include "database/model/ParserStage.hxx"
#include "database/model/InterfaceStage.hxx"

#include "database/types/clang_compilation.hpp"
#include <common/file.hpp>

namespace mega
{
namespace compiler
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
        const mega::io::CompilationFilePath        astFile = m_environment.ParserStage_AST( m_sourceFilePath );
        const mega::io::GeneratedCPPSourceFilePath implementationFile
            = m_environment.Implementation( m_sourceFilePath );
        start( taskProgress, "Task_Implementation", astFile.path(), implementationFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.clangCompilerHash, m_environment.getBuildHashCode( astFile ) } );

        if ( m_environment.restore( implementationFile, determinant ) )
        {
            m_environment.setBuildHashCode( implementationFile );
            cached( taskProgress );
            return;
        }

        {
            using namespace InterfaceStage;
            using namespace InterfaceStage::Interface;

            Database database( m_environment, m_sourceFilePath );

            std::ostringstream os;
            {
                /*for ( Parser::CPPInclude* pCPPInclude : database.many< Parser::CPPInclude >( m_sourceFilePath ) )
                {
                    os << "#include \"" << pCPPInclude->get_cppSourceFilePath().string() << "\"\n";
                }

                // mega library includes
                os << "#include \"mega/include.hpp\"\n";

                for ( Parser::SystemInclude* pSystemInclude :
                      database.many< Parser::SystemInclude >( m_sourceFilePath ) )
                {
                    os << "#include <" << pSystemInclude->get_str() << ">\n";
                }
                os << "\n";*/
            }

            boost::filesystem::updateFileIfChanged( m_environment.FilePath( implementationFile ), os.str() );
        }

        m_environment.setBuildHashCode( implementationFile );
        m_environment.stash( implementationFile, determinant );

        succeeded( taskProgress );
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
        const mega::io::GeneratedCPPSourceFilePath implementationFile
            = m_environment.Implementation( m_sourceFilePath );
        const mega::io::ObjectFilePath implementationObj = m_environment.ImplementationObj( m_sourceFilePath );
        start( taskProgress, "Task_ImplementationObj", implementationFile.path(), implementationObj.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.clangCompilerHash, m_environment.getBuildHashCode( implementationFile ) } );

        if ( m_environment.restore( implementationObj, determinant ) )
        {
            m_environment.setBuildHashCode( implementationObj );
            cached( taskProgress );
            return;
        }

        using namespace ParserStage;

        Database database( m_environment, m_sourceFilePath );

        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        const std::string strCmd = mega::Compilation::make_implementationObj_compilation(
            m_environment, m_toolChain, pComponent, m_sourceFilePath )();

        msg( taskProgress, strCmd );

        const int iResult = boost::process::system( strCmd );
        if ( iResult )
        {
            std::ostringstream os;
            os << "Error compiling operations file to pch for source file: " << m_sourceFilePath.path();
            throw std::runtime_error( os.str() );
        }

        m_environment.setBuildHashCode( implementationObj );
        m_environment.stash( implementationObj, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_ImplementationObj( const TaskArguments&          taskArguments,
                                             const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_ImplementationObj >( taskArguments, sourceFilePath );
}

} // namespace compiler
} // namespace mega
