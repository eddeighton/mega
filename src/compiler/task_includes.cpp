
#include "base_task.hpp"

#include "database/model/ParserStage.hxx"
#include "database/model/InterfaceStage.hxx"

#include "database/types/clang_compilation.hpp"
#include <common/file.hpp>

namespace mega
{
namespace compiler
{

class Task_Include : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_Include( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath        astFile         = m_environment.ParserStage_AST( m_sourceFilePath );
        const mega::io::GeneratedHPPSourceFilePath includeFilePath = m_environment.Include( m_sourceFilePath );
        start( taskProgress, "Task_Include", astFile.path(), includeFilePath.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( astFile ) } );

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
                os << "#include \"mega/default_traits.hpp\"\n";

                for ( Parser::SystemInclude* pSystemInclude :
                      database.many< Parser::SystemInclude >( m_sourceFilePath ) )
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

BaseTask::Ptr create_Task_Include( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_Include >( taskArguments, sourceFilePath );
}

class Task_IncludePCH : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_IncludePCH( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::GeneratedHPPSourceFilePath includeFilePath = m_environment.Include( m_sourceFilePath );
        const mega::io::PrecompiledHeaderFile      pchPath         = m_environment.IncludePCH( m_sourceFilePath );
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
            = mega::Compilation::make_includePCH_compilation( m_environment, m_toolChain, pComponent, m_sourceFilePath )();

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

BaseTask::Ptr create_Task_IncludePCH( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_IncludePCH >( taskArguments, sourceFilePath );
}

} // namespace interface
} // namespace driver
