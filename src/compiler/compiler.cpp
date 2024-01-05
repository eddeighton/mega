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

#include "common/serialisation.hpp"

#include "compiler/configuration.hpp"

#include "base_task.hpp"
#include "database/component_type.hpp"
#include "task_utils.hpp"

#include "pipeline/task.hpp"
#include "pipeline/stash.hpp"
#include "pipeline/pipeline.hpp"
#include "pipeline/version.hpp"

#include "database/manifest.hxx"
#include "database/ComponentListingView.hxx"

#include "database/sources.hpp"

#include <common/string.hpp>
#include "common/assert_verify.hpp"

#include <boost/config.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <sstream>
#include <thread>
#include <chrono>
#include <utility>
#include <variant>

namespace mega::compiler
{

namespace
{

struct Task
{
    std::string strTaskName; // not serialised

    using FilePathVar = std::variant< mega::io::megaFilePath, mega::io::cppFilePath, mega::io::schFilePath,
                                      mega::io::manifestFilePath, std::string >;
    FilePathVar sourceFilePath;

    static std::string toString( const FilePathVar& filePathVar )
    {
        struct Visitor
        {
            std::string operator()( const mega::io::megaFilePath& filePath ) const { return filePath.path().string(); }
            std::string operator()( const mega::io::cppFilePath& filePath ) const { return filePath.path().string(); }
            std::string operator()( const mega::io::schFilePath& filePath ) const { return filePath.path().string(); }
            std::string operator()( const mega::io::manifestFilePath& filePath ) const
            {
                return filePath.path().string();
            }
            std::string operator()( const std::string& componentName ) const { return componentName; }
        } visitor;
        return std::visit( visitor, filePathVar );
    }

    Task( mega::compiler::TaskType taskType )
        : strTaskName( taskTypeToName( taskType ) )
    {
    }
    Task( mega::compiler::TaskType taskType, FilePathVar filePathVar )
        : strTaskName( taskTypeToName( taskType ) )
        , sourceFilePath( std::move( filePathVar ) )
    {
    }

    template < class Archive >
    void save( Archive& ar, const unsigned int version ) const
    {
        int index = sourceFilePath.index();
        ar& index;
        struct Visitor
        {
            Archive& ar;
            Visitor( Archive& ar )
                : ar( ar )
            {
            }
            void operator()( const mega::io::megaFilePath& filePath ) const { ar& filePath; }
            void operator()( const mega::io::cppFilePath& filePath ) const { ar& filePath; }
            void operator()( const mega::io::schFilePath& filePath ) const { ar& filePath; }
            void operator()( const mega::io::manifestFilePath& filePath ) const { ar& filePath; }
            void operator()( const std::string& componentName ) const { ar& componentName; }
        } visitor( ar );
        std::visit( visitor, sourceFilePath );
    }

    template < class Archive >
    void load( Archive& ar, const unsigned int version )
    {
        int index = 0;
        ar& index;
        switch( index )
        {
            case 0:
            {
                mega::io::megaFilePath filePath;
                ar&                    filePath;
                sourceFilePath = filePath;
            }
            break;
            case 1:
            {
                mega::io::cppFilePath filePath;
                ar&                   filePath;
                sourceFilePath = filePath;
            }
            break;
            case 2:
            {
                mega::io::schFilePath filePath;
                ar&                   filePath;
                sourceFilePath = filePath;
            }
            break;
            case 3:
            {
                mega::io::manifestFilePath filePath;
                ar&                        filePath;
                sourceFilePath = filePath;
            }
            break;
            case 4:
            {
                std::string strComponentName;
                ar&         strComponentName;
                sourceFilePath = strComponentName;
            }
            break;
            default:
                THROW_RTE( "Unknown file path type" );
        }
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

pipeline::TaskDescriptor encode( const Task& task )
{
    std::ostringstream os;
    {
        boost::archive::binary_oarchive oa( os );
        oa&                             task;
    }
    return { task.strTaskName, Task::toString( task.sourceFilePath ), os.str() };
}

Task decode( const pipeline::TaskDescriptor& taskDescriptor )
{
    Task task( nameToTaskType( taskDescriptor.getName().c_str() ) );
    {
        std::istringstream              is( taskDescriptor.getBuffer() );
        boost::archive::binary_iarchive ia( is );
        ia&                             task;
    }
    return task;
}

class CompilerPipeline : public pipeline::Pipeline
{
    struct Config
    {
        mega::utilities::ToolChain m_toolChain;
        compiler::Configuration    m_configuration;
        mega::io::Manifest         m_manifest;
        mega::io::BuildEnvironment m_environment;

        Config( const mega::utilities::ToolChain& toolChain, const pipeline::Configuration& pipelineConfig )
            : m_toolChain( toolChain )
            , m_configuration( fromPipelineConfiguration( pipelineConfig ) )
            , m_manifest( m_configuration.manifestData )
            , m_environment( m_configuration.directories )
        {
        }
    };
    std::unique_ptr< Config > m_pConfig;

public:
    CompilerPipeline() = default;

    // pipeline::Pipeline
    virtual pipeline::Schedule getSchedule( pipeline::Progress& progress, pipeline::Stash& stash ) override;
    virtual void               execute( const pipeline::TaskDescriptor& pipelineTask, pipeline::Progress& progress,
                                        pipeline::Stash& stash, pipeline::DependencyProvider& dependencies ) override;

    virtual void initialise( const mega::utilities::ToolChain& toolChain, const pipeline::Configuration& pipelineConfig,
                             std::ostream& osLog ) override
    {
        m_pConfig = std::make_unique< Config >( toolChain, pipelineConfig );

        // check the version is latest
        // const auto actualVersion = Version::getVersion();
        // VERIFY_RTE_MSG( actualVersion == m_pConfig->m_configuration->header.version,
        //                 "Pipeine version mismatch: Configuration version: " <<
        //                 m_pConfig->m_configuration->header.version
        //
        osLog << "SUCCESS: Initialised Compiler Pipeline: " << m_pConfig->m_configuration.header.pipelineID
              << " with version: " << m_pConfig->m_configuration.header.version << "\n";
    }
};

pipeline::Schedule CompilerPipeline::getSchedule( pipeline::Progress& progress, pipeline::Stash& stash )
{
    VERIFY_RTE( m_pConfig );

    using TskDesc    = pipeline::TaskDescriptor;
    using TskDescVec = pipeline::TaskDescriptor::Vector;

    Configuration& config = m_pConfig->m_configuration;

    pipeline::Dependencies dependencies;

    const TskDesc generateComponents = encode( Task{ eTask_GenerateComponents } );
    dependencies.add( generateComponents, {} );

    TskDescVec parserTasks;
    {
        for( const mega::io::megaFilePath& sourceFilePath : m_pConfig->m_manifest.getMegaSourceFiles() )
        {
            const TskDesc parseAst = encode( Task{ eTask_ParseAST, sourceFilePath } );
            dependencies.add( parseAst, TskDescVec{ generateComponents } );
            parserTasks.push_back( parseAst );
        }
    }

    const TskDesc interfaceTree = encode( Task{ eTask_AST } );
    dependencies.add( interfaceTree, parserTasks );

    const TskDesc symbolAnalysis = encode( Task{ eTask_SymbolAnalysis } );
    dependencies.add( symbolAnalysis, { interfaceTree } );

    TskDescVec symbolRolloutTasks;
    {
        for( const mega::io::megaFilePath& sourceFilePath : m_pConfig->m_manifest.getMegaSourceFiles() )
        {
            const TskDesc symbolRollout = encode( Task{ eTask_SymbolRollout, sourceFilePath } );
            symbolRolloutTasks.push_back( symbolRollout );

            dependencies.add( symbolRollout, TskDescVec{ symbolAnalysis } );
        }
    }

    const TskDesc concreteTree = encode( Task{ eTask_ConcreteTree } );
    dependencies.add( concreteTree, symbolRolloutTasks );

    const TskDesc concreteTypeID = encode( Task{ eTask_ConcreteTypeAnalysis } );
    dependencies.add( concreteTypeID, { concreteTree } );

    const TskDesc hyperGraph = encode( Task{ eTask_HyperGraph } );
    dependencies.add( hyperGraph, { concreteTypeID } );

    const TskDesc automata = encode( Task{ eTask_Automata } );
    dependencies.add( automata, { hyperGraph } );

    const TskDesc includes = encode( Task{ eTask_Include } );
    dependencies.add( includes, parserTasks );

    const TskDesc includePCH = encode( Task{ eTask_IncludePCH } );
    dependencies.add( includePCH, { includes } );

    const TskDesc clang_Traits_Gen = encode( Task{ eTask_Clang_Traits_Gen } );
    dependencies.add( clang_Traits_Gen, { automata } );

    // NOTE: nothing requires decision
    const TskDesc decisions = encode( Task{ eTask_Decisions } );
    dependencies.add( decisions, { clang_Traits_Gen } );

    const TskDesc clang_Traits_Analysis = encode( Task{ eTask_Clang_Traits_Analysis } );
    dependencies.add( clang_Traits_Analysis, { includePCH, clang_Traits_Gen } );

    const TskDesc CPP_Decl = encode( Task{ eTask_CPP_Decl } );
    dependencies.add( CPP_Decl, { clang_Traits_Analysis } );

    std::vector< TskDesc > cppObjects;
    for( const auto& componentInfo : config.componentInfos )
    {
        if( componentInfo.getComponentType().get() == mega::ComponentType::eLibrary )
        {
            for( const auto& filePath : componentInfo.getSourceFiles() )
            {
                auto cppSourceFile = m_pConfig->m_environment.cppFilePath_fromPath( filePath );

                const TskDesc cppSource  = encode( Task{ eTask_CPP_Source, cppSourceFile } );
                const TskDesc cppCompile = encode( Task{ eTask_CPP_Compile, cppSourceFile } );
                const TskDesc cppImpl    = encode( Task{ eTask_CPP_Impl, cppSourceFile } );
                const TskDesc cppObj     = encode( Task{ eTask_CPP_Obj, cppSourceFile } );

                dependencies.add( cppSource, { CPP_Decl } );
                dependencies.add( cppCompile, { cppSource } );
                dependencies.add( cppImpl, { cppCompile } );
                dependencies.add( cppObj, { cppImpl } );

                cppObjects.push_back( cppObj );
            }
        }
    }

    // TskDesc unityReflection = encode( Task{ eTask_UnityReflection} );
    // TskDesc unityAnalysis   = encode( Task{ eTask_UnityAnalysis } );
    // TskDesc unity           = encode( Task{ eTask_Unity } );
    // TskDesc unityDatabase   = encode( Task{ eTask_UnityDatabase } );

    // dependencies.add( unityReflection, unityDependencyTasks );
    // dependencies.add( unityAnalysis, TskDescVec{ unityReflection } );
    // dependencies.add( unity, TskDescVec{ unityAnalysis } );
    // dependencies.add( unityDatabase, TskDescVec{ unity } );

    // {
    //     TskDescVec completionTasks = componentTasks;
    //     completionTasks.push_back( unityDatabase );
    //     TskDesc complete = encode( Task{ eTask_Complete } );
    //     dependencies.add( complete, completionTasks );
    // }

    return { dependencies };
}

void CompilerPipeline::execute( const pipeline::TaskDescriptor& pipelineTask, pipeline::Progress& progress,
                                pipeline::Stash& stash, pipeline::DependencyProvider& dependencies )
{
    VERIFY_RTE( m_pConfig );

    Configuration& config = m_pConfig->m_configuration;

    const Task task = decode( pipelineTask );

    mega::io::StashEnvironment environment( stash, config.directories );

    mega::compiler::TaskArguments taskArguments( environment, m_pConfig->m_manifest, m_pConfig->m_toolChain,
                                                 config.unityProjectDir, config.unityEditor, dependencies.getParser() );

    mega::compiler::BaseTask::Ptr pTask;

#define TASK_PROJECT( taskName )                                         \
    if( task.strTaskName == "Task_" #taskName )                          \
    {                                                                    \
        VERIFY_RTE( !pTask );                                            \
        pTask = mega::compiler::create_Task_##taskName( taskArguments ); \
    }
#define TASK_SRC( taskName, sourceFileType, argumentType )                               \
    if( task.strTaskName == "Task_" #taskName )                                          \
    {                                                                                    \
        VERIFY_RTE( !pTask );                                                            \
        pTask = mega::compiler::create_Task_##taskName( taskArguments, sourceFileType ); \
    }
#include "tasks.xmc"
#undef TASK_PROJECT
#undef TASK_SRC

    VERIFY_RTE_MSG( pTask, "Failed to create task: " << task.strTaskName );

    try
    {
        pTask->run( progress );
        if( !pTask->isCompleted() )
        {
            pTask->msg( progress, "Task did NOT complete" );
            pTask->failed( progress );
        }
    }
    catch( std::exception& ex )
    {
        pTask->msg( progress, ex.what() );
        pTask->failed( progress );
    }
}

} // namespace

extern "C" BOOST_SYMBOL_EXPORT CompilerPipeline mega_pipeline;
CompilerPipeline                                mega_pipeline;

} // namespace mega::compiler
