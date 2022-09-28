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
namespace
{
inline void addStandardIncludes( std::ostream& os )
{
    os << "#include \"mega/include.hpp\"\n";
    os << "#include \"mega/default_traits.hpp\"\n";
    os << "#include \"runtime/runtime_functions.hpp\"\n";
    os << "#include \"boost/dll/alias.hpp\"\n";
}
} // namespace

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
        start( taskProgress, "Task_Include", m_sourceFilePath.path(), includeFilePath.path() );

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
                addStandardIncludes( os );

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
        start( taskProgress, "Task_IncludePCH", m_sourceFilePath.path(), pchPath.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.clangCompilerHash, m_environment.getBuildHashCode( includeFilePath ) } );

        using namespace ParserStage;
        Database               database( m_environment, m_sourceFilePath );
        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        const mega::Compilation compilationCMD = mega::Compilation::make_includePCH_compilation(
            m_environment, m_toolChain, pComponent, m_sourceFilePath );

        if ( m_environment.restore( pchPath, determinant ) )
        {
            if ( !run_cmd( taskProgress, compilationCMD.generatePCHVerificationCMD(), false ) )
            {
                m_environment.setBuildHashCode( pchPath );
                cached( taskProgress );
                return;
            }
        }

        if ( run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            std::ostringstream os;
            os << "Error compiling include files to pch for source file: " << m_sourceFilePath.path();
            msg( taskProgress, os.str() );
            failed( taskProgress );
            return;
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
        start( taskProgress, "Task_CPPInclude", m_strComponentName, includeFilePath.path() );

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
                addStandardIncludes( os );

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
        start( taskProgress, "Task_CPPIncludePCH", m_strComponentName, pchPath.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.clangCompilerHash, m_environment.getBuildHashCode( includeFilePath ) } );

        const mega::Compilation compilationCMD
            = mega::Compilation::make_cpp_includePCH_compilation( m_environment, m_toolChain, pComponent );

        if ( m_environment.restore( pchPath, determinant ) )
        {
            if ( !run_cmd( taskProgress, compilationCMD.generatePCHVerificationCMD(), false ) )
            {
                m_environment.setBuildHashCode( pchPath );
                cached( taskProgress );
                return;
            }
        }

        if ( run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            std::ostringstream os;
            os << "Error compiling include files to pch for component: " << pComponent->get_name();
            msg( taskProgress, os.str() );
            failed( taskProgress );
            return;
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
