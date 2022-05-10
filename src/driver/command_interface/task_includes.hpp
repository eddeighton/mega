#ifndef TASK_INCLUDE_10_MAY_2022
#define TASK_INCLUDE_10_MAY_2022

#include "base_task.hpp"

#include "database/model/ParserStage.hxx"
#include "database/model/InterfaceStage.hxx"

#include "database/types/clang_compilation.hpp"
#include <common/file.hpp>

namespace driver
{
namespace interface
{

class Task_Include : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_Include( task::Task::RawPtr pDependency, const mega::io::BuildEnvironment& environment,
                  const ToolChain& toolChain, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( { pDependency }, environment, toolChain )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( task::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath        astFile         = m_environment.ParserStage_AST( m_sourceFilePath );
        const mega::io::GeneratedHPPSourceFilePath includeFilePath = m_environment.Include( m_sourceFilePath );
        start( taskProgress, "Task_Include", astFile.path(), includeFilePath.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.clangCompilerHash, m_environment.getBuildHashCode( astFile ) } );

        if ( m_environment.restore( includeFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( includeFilePath );
            cached( taskProgress );
            return;
        }

        {
            using namespace InterfaceStage;
            using namespace InterfaceStage::Interface;

            Database database( m_environment, m_sourceFilePath );

            std::ostringstream os;
            {
                for ( Parser::CPPInclude* pCPPInclude : database.many< Parser::CPPInclude >( m_sourceFilePath ) )
                {
                    os << "#include \"" << pCPPInclude->get_cppSourceFilePath().string() << "\"\n";
                }

                // mega library includes
                os << "#include \"mega/include.hpp\"\n";

                for ( Parser::SystemInclude* pSystemInclude : database.many< Parser::SystemInclude >( m_sourceFilePath ) )
                {
                    os << "#include <" << pSystemInclude->get_str() << ">\n";
                }
                os << "\n";
            }

            boost::filesystem::updateFileIfChanged( m_environment.FilePath( includeFilePath ), os.str() );
        }

        m_environment.setBuildHashCode( includeFilePath );
        m_environment.stash( includeFilePath, determinant );

        succeeded( taskProgress );
    }
};

class Task_IncludePCH : public BaseTask
{
    const boost::filesystem::path& m_compilerPath;
    const mega::io::megaFilePath&  m_sourceFilePath;

public:
    Task_IncludePCH( task::Task::RawPtr pDependency, const mega::io::BuildEnvironment& environment,
                     const ToolChain& toolChain, const boost::filesystem::path& compilerPath,
                     const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( { pDependency }, environment, toolChain )
        , m_compilerPath( compilerPath )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( task::Progress& taskProgress )
    {
        const mega::io::GeneratedHPPSourceFilePath includeFilePath = m_environment.Include( m_sourceFilePath );
        const mega::io::PrecompiledHeaderFile      pchPath         = m_environment.PCH( m_sourceFilePath );
        start( taskProgress, "Task_IncludePCH", includeFilePath.path(), pchPath.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.clangCompilerHash, m_environment.getBuildHashCode( includeFilePath ) } );

        if ( m_environment.restore( pchPath, determinant ) )
        {
            m_environment.setBuildHashCode( pchPath );
            cached( taskProgress );
            return;
        }

        using namespace ParserStage;

        Database database( m_environment, m_sourceFilePath );

        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        const std::string strCmd
            = mega::Compilation( m_compilerPath, pComponent->get_cpp_flags(), pComponent->get_cpp_defines(),
                                 pComponent->get_includeDirectories(), m_environment.FilePath( includeFilePath ),
                                 m_environment.FilePath( pchPath ) )();

        msg( taskProgress, strCmd );

        const int iResult = boost::process::system( strCmd );
        if ( iResult )
        {
            std::ostringstream os;
            os << "Error compiling include files to pch for source file: " << m_sourceFilePath.path();
            throw std::runtime_error( os.str() );
        }

        m_environment.setBuildHashCode( pchPath );
        m_environment.stash( pchPath, determinant );

        succeeded( taskProgress );
    }
};

} // namespace interface
} // namespace driver

#endif // TASK_INCLUDE_10_MAY_2022
