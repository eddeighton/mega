
#include "base_task.hpp"

#include "database/model/ParserStage.hxx"
#include "database/model/InterfaceAnalysisStage.hxx"
#include "database/model/FinalStage.hxx"

#include "database/types/clang_compilation.hpp"
#include "database/types/sources.hpp"
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

        // const task::DeterminantHash determinant(
        //     { m_toolChain.toolChainHash, m_environment.getBuildHashCode( astFile ) } );

        /*if ( m_environment.restore( includeFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( includeFilePath );
            cached( taskProgress );
            return;
        }*/

        {
            using namespace InterfaceAnalysisStage;
            using namespace InterfaceAnalysisStage::Interface;

            Database database( m_environment, m_sourceFilePath );

            std::ostringstream os;
            {
                Dependencies::Analysis* pDependencyAnalysis
                    = database.one< Dependencies::Analysis >( m_environment.project_manifest() );

                auto megaDependencies = pDependencyAnalysis->get_mega_dependencies();

                auto iter = megaDependencies.find( m_sourceFilePath );
                VERIFY_RTE( iter != megaDependencies.end() );
                auto                                         megaDep      = iter->second;
                const std::vector< mega::io::megaFilePath >& dependencies = megaDep->get_mega_source_files();

                {
                    std::set< mega::io::megaFilePath >  uniqueFiles;
                    std::set< boost::filesystem::path > uniqueIncludes;
                    for ( const mega::io::megaFilePath& megaFile : dependencies )
                    {
                        if ( uniqueFiles.count( megaFile ) > 0 )
                            continue;
                        uniqueFiles.insert( megaFile );

                        for ( Parser::CPPInclude* pCPPInclude : database.many< Parser::CPPInclude >( megaFile ) )
                        {
                            const auto& includeFile = pCPPInclude->get_cppSourceFilePath();
                            if ( uniqueIncludes.count( includeFile ) > 0 )
                                continue;
                            uniqueIncludes.insert( includeFile );
                            os << "#include \"" << includeFile.string() << "\"\n";
                        }
                    }
                }

                // mega library includes
                os << "#include \"mega/include.hpp\"\n";
                os << "#include \"mega/default_traits.hpp\"\n";
                os << "#include \"runtime/runtime_api.hpp\"\n";

                {
                    std::set< mega::io::megaFilePath > uniqueFiles;
                    std::set< std::string >            uniqueIncludes;
                    for ( const mega::io::megaFilePath& megaFile : dependencies )
                    {
                        if ( uniqueFiles.count( megaFile ) > 0 )
                            continue;
                        uniqueFiles.insert( megaFile );

                        for ( Parser::SystemInclude* pSystemInclude :
                              database.many< Parser::SystemInclude >( megaFile ) )
                        {
                            const auto& includeFile = pSystemInclude->get_str();
                            if ( uniqueIncludes.count( includeFile ) > 0 )
                                continue;
                            uniqueIncludes.insert( includeFile );
                            os << "#include <" << includeFile << ">\n";
                        }
                    }
                }

                os << "\n";
            }

            boost::filesystem::updateFileIfChanged( m_environment.FilePath( includeFilePath ), os.str() );
        }

        m_environment.setBuildHashCode( includeFilePath );
        // m_environment.stash( includeFilePath, determinant );

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

        using namespace ParserStage;
        Database               database( m_environment, m_sourceFilePath );
        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        if ( m_environment.restore( pchPath, determinant ) )
        {
            // test if PCH is still valid
            const std::string strCmd = mega::PCHVerification::make_includePCH_verification(
                m_environment, m_toolChain, pComponent, pchPath )();

            if ( run_cmd( taskProgress, strCmd ) )
            {
                std::ostringstream os;
                os << "PCH file: " << pchPath.path() << " out of date";
                msg( taskProgress, os.str() );
            }
            else
            {
                m_environment.setBuildHashCode( pchPath );
                cached( taskProgress );
                return;
            }
        }

        const std::string strCmd = mega::Compilation::make_includePCH_compilation(
            m_environment, m_toolChain, pComponent, m_sourceFilePath )();

        if ( run_cmd( taskProgress, strCmd ) )
        {
            std::ostringstream os;
            os << "Error compiling include files to pch for source file: " << m_sourceFilePath.path();
            msg( taskProgress, os.str() );
            failed( taskProgress );
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

class Task_CPPInclude : public BaseTask
{
    const std::string& m_strComponentName;

public:
    Task_CPPInclude( const TaskArguments& taskArguments, const std::string& strComponentName )
        : BaseTask( taskArguments )
        , m_strComponentName( strComponentName )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;

        Database database( m_environment, m_environment.project_manifest() );

        Components::Component* pComponent = nullptr;
        for ( Components::Component* pIter :
              database.template many< Components::Component >( m_environment.project_manifest() ) )
        {
            if ( pIter->get_name() == m_strComponentName )
            {
                pComponent = pIter;
                break;
            }
        }
        VERIFY_RTE( pComponent );

        const mega::io::GeneratedHPPSourceFilePath includeFilePath
            = m_environment.Include( pComponent->get_build_dir(), m_strComponentName );
        start( taskProgress, "Task_Include", m_strComponentName, includeFilePath.path() );

        /*task::DeterminantHash determinant( { m_toolChain.toolChainHash } );

        if ( m_environment.restore( includeFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( includeFilePath );
            cached( taskProgress );
            return;
        }*/

        {
            std::ostringstream os;
            {
                Dependencies::Analysis* pDependencyAnalysis
                    = database.one< Dependencies::Analysis >( m_environment.project_manifest() );

                auto cppDependencies = pDependencyAnalysis->get_cpp_dependencies();

                std::vector< mega::io::megaFilePath > dependencies;
                for ( const io::cppFilePath& cppFile : pComponent->get_cpp_source_files() )
                {
                    auto iter = cppDependencies.find( cppFile );
                    VERIFY_RTE( iter != cppDependencies.end() );
                    auto        cppDep    = iter->second;
                    const auto& megaFiles = cppDep->get_mega_source_files();
                    std::copy( megaFiles.begin(), megaFiles.end(), std::back_inserter( dependencies ) );
                }

                {
                    std::set< mega::io::megaFilePath >  uniqueFiles;
                    std::set< boost::filesystem::path > uniqueIncludes;
                    for ( const mega::io::megaFilePath& megaFile : dependencies )
                    {
                        if ( uniqueFiles.count( megaFile ) > 0 )
                            continue;
                        uniqueFiles.insert( megaFile );

                        for ( Parser::CPPInclude* pCPPInclude : database.many< Parser::CPPInclude >( megaFile ) )
                        {
                            const auto& includeFile = pCPPInclude->get_cppSourceFilePath();
                            if ( uniqueIncludes.count( includeFile ) > 0 )
                                continue;
                            uniqueIncludes.insert( includeFile );
                            os << "#include \"" << includeFile.string() << "\"\n";
                        }
                    }
                }

                // mega library includes
                os << "#include \"mega/include.hpp\"\n";
                os << "#include \"mega/default_traits.hpp\"\n";
                os << "#include \"runtime/runtime_api.hpp\"\n";

                {
                    std::set< mega::io::megaFilePath > uniqueFiles;
                    std::set< std::string >            uniqueIncludes;
                    for ( const mega::io::megaFilePath& megaFile : dependencies )
                    {
                        if ( uniqueFiles.count( megaFile ) > 0 )
                            continue;
                        uniqueFiles.insert( megaFile );

                        for ( Parser::SystemInclude* pSystemInclude :
                              database.many< Parser::SystemInclude >( megaFile ) )
                        {
                            const auto& includeFile = pSystemInclude->get_str();
                            if ( uniqueIncludes.count( includeFile ) > 0 )
                                continue;
                            uniqueIncludes.insert( includeFile );
                            os << "#include <" << includeFile << ">\n";
                        }
                    }
                }

                os << "\n";
            }

            boost::filesystem::updateFileIfChanged( m_environment.FilePath( includeFilePath ), os.str() );
        }

        m_environment.setBuildHashCode( includeFilePath );
        // m_environment.stash( includeFilePath, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_CPPInclude( const TaskArguments& taskArguments, const std::string& strComponentName )
{
    return std::make_unique< Task_CPPInclude >( taskArguments, strComponentName );
}

class Task_CPPIncludePCH : public BaseTask
{
    const std::string& m_strComponentName;

public:
    Task_CPPIncludePCH( const TaskArguments& taskArguments, const std::string& strComponentName )
        : BaseTask( taskArguments )
        , m_strComponentName( strComponentName )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;

        Database database( m_environment, m_environment.project_manifest() );

        Components::Component* pComponent = nullptr;
        for ( Components::Component* pIter :
              database.template many< Components::Component >( m_environment.project_manifest() ) )
        {
            if ( pIter->get_name() == m_strComponentName )
            {
                pComponent = pIter;
                break;
            }
        }
        VERIFY_RTE( pComponent );

        const mega::io::GeneratedHPPSourceFilePath includeFilePath
            = m_environment.Include( pComponent->get_build_dir(), pComponent->get_name() );
        const mega::io::PrecompiledHeaderFile pchPath
            = m_environment.IncludePCH( pComponent->get_build_dir(), pComponent->get_name() );
        start( taskProgress, "Task_CPPIncludePCH", includeFilePath.path(), pchPath.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.clangCompilerHash, m_environment.getBuildHashCode( includeFilePath ) } );

        if ( m_environment.restore( pchPath, determinant ) )
        {
            m_environment.setBuildHashCode( pchPath );
            cached( taskProgress );
            return;
        }

        const std::string strCmd
            = mega::Compilation::make_includePCH_compilation( m_environment, m_toolChain, pComponent )();

        if ( run_cmd( taskProgress, strCmd ) )
        {
            std::ostringstream os;
            os << "Error compiling include files to pch for component: " << pComponent->get_name();
            msg( taskProgress, os.str() );
            failed( taskProgress );
        }

        m_environment.setBuildHashCode( pchPath );
        m_environment.stash( pchPath, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_CPPIncludePCH( const TaskArguments& taskArguments, const std::string& strComponentName )
{
    return std::make_unique< Task_CPPIncludePCH >( taskArguments, strComponentName );
}

} // namespace compiler
} // namespace mega
