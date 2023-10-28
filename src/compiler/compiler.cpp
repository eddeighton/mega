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
    std::optional< compiler::Configuration >    m_configuration;
    std::optional< mega::utilities::ToolChain > m_toolChain;

public:
    CompilerPipeline() = default;

    // pipeline::Pipeline
    virtual pipeline::Schedule getSchedule( pipeline::Progress& progress, pipeline::Stash& stash ) override;
    virtual void               execute( const pipeline::TaskDescriptor& pipelineTask, pipeline::Progress& progress,
                                        pipeline::Stash& stash, pipeline::DependencyProvider& dependencies ) override;

    virtual void initialise( const mega::utilities::ToolChain& toolChain, const pipeline::Configuration& pipelineConfig,
                             std::ostream& osLog ) override
    {
        m_toolChain     = toolChain;
        m_configuration = fromPipelineConfiguration( pipelineConfig );

        // check the version is latest
        // const auto actualVersion = Version::getVersion();
        // using ::   operator<<;
        // VERIFY_RTE_MSG( actualVersion == m_configuration->header.version,
        //                 "Pipeine version mismatch: Configuration version: " << m_configuration->header.version
        //                                                                    << " Actual Version: " << actualVersion );
        using ::operator<<;
        osLog << "SUCCESS: Initialised pipeline: " << m_configuration->header.pipelineID
              << " with version: " << m_configuration->header.version;
    }
};

pipeline::Schedule CompilerPipeline::getSchedule( pipeline::Progress& progress, pipeline::Stash& stash )
{
    VERIFY_RTE( m_configuration.has_value() );

    using TskDesc    = pipeline::TaskDescriptor;
    using TskDescVec = pipeline::TaskDescriptor::Vector;

    Configuration& config = m_configuration.value();

    mega::io::BuildEnvironment environment( config.directories );
    mega::io::manifestFilePath manifestFilePath = environment.project_manifest();

    // paradox - need a manifest to determine the schedule - the schedule generates the manifest!
    {
        const TskDesc generateManifest   = encode( Task{ eTask_GenerateManifest, manifestFilePath } );
        const TskDesc generateComponents = encode( Task{ eTask_GenerateComponents, manifestFilePath } );
        struct Dummy : public pipeline::DependencyProvider
        {
            virtual EG_PARSER_INTERFACE* getParser() { return nullptr; }
        } dummy;
        execute( generateManifest, progress, stash, dummy );
        execute( generateComponents, progress, stash, dummy );
    }
    mega::io::Manifest manifest( environment, manifestFilePath );

    pipeline::Dependencies dependencies;

    using namespace ComponentListingView;
    Database                                                    database( environment, manifestFilePath );
    std::vector< ComponentListingView::Components::Component* > components
        = database.template many< Components::Component >( environment.project_manifest() );

    TskDescVec interfaceTreeTasks;
    {
        // special case for python so that later stages work
        {
            const TskDesc pythonDumbTasks = encode( Task{ eTask_PythonStages, manifestFilePath } );
            dependencies.add( pythonDumbTasks, TskDescVec{} );
            interfaceTreeTasks.push_back( pythonDumbTasks );
        }

        for( ComponentListingView::Components::Component* pComponent : components )
        {
            if( pComponent->get_type().get() == mega::ComponentType::eLibrary )
            {
                for( const mega::io::cppFilePath& sourceFile : pComponent->get_cpp_source_files() )
                {
                    const TskDesc cppDumpTasks = encode( Task{ eTask_CPPStages, sourceFile } );
                    dependencies.add( cppDumpTasks, TskDescVec{} );
                    interfaceTreeTasks.push_back( cppDumpTasks );
                }
            }
        }

        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc parseAst      = encode( Task{ eTask_ParseAST, sourceFilePath } );
            const TskDesc interfaceTree = encode( Task{ eTask_InterfaceTree, sourceFilePath } );

            dependencies.add( parseAst, TskDescVec{} );
            dependencies.add( interfaceTree, TskDescVec{ parseAst } );

            interfaceTreeTasks.push_back( interfaceTree );
        }
    }

    const TskDesc dependencyAnalysis = encode( Task{ eTask_DependencyAnalysis, manifestFilePath } );
    const TskDesc symbolAnalysis     = encode( Task{ eTask_SymbolAnalysis, manifestFilePath } );

    dependencies.add( dependencyAnalysis, interfaceTreeTasks );
    dependencies.add( symbolAnalysis, TskDescVec{ dependencyAnalysis } );

    TskDescVec symbolRolloutTasks;
    {
        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc symbolRollout = encode( Task{ eTask_SymbolRollout, sourceFilePath } );
            symbolRolloutTasks.push_back( symbolRollout );

            dependencies.add( symbolRollout, TskDescVec{ symbolAnalysis } );
        }
    }

    TskDescVec interfaceAnalysisTasks;
    {
        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc metaTask                  = encode( Task{ eTask_Meta, sourceFilePath } );
            const TskDesc includes                  = encode( Task{ eTask_Include, sourceFilePath } );
            const TskDesc includePCH                = encode( Task{ eTask_IncludePCH, sourceFilePath } );
            const TskDesc objectInterfaceGeneration = encode( Task{ eTask_InterfaceGeneration, sourceFilePath } );
            const TskDesc objectInterfaceAnalysis   = encode( Task{ eTask_InterfaceAnalysis, sourceFilePath } );

            dependencies.add( metaTask, symbolRolloutTasks );
            dependencies.add( includes, symbolRolloutTasks );
            dependencies.add( includePCH, TskDescVec{ includes } );
            dependencies.add( objectInterfaceGeneration, TskDescVec{ includePCH, metaTask } );
            dependencies.add( objectInterfaceAnalysis, TskDescVec{ objectInterfaceGeneration } );

            interfaceAnalysisTasks.push_back( objectInterfaceAnalysis );
        }
    }

    TskDescVec concreteTreeTasks;
    {
        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc concreteTree = encode( Task{ eTask_ConcreteTree, sourceFilePath } );
            dependencies.add( concreteTree, interfaceAnalysisTasks );

            concreteTreeTasks.push_back( concreteTree );
        }
    }

    const TskDesc inheritance = encode( Task{ eTask_Inheritance, manifestFilePath } );
    dependencies.add( inheritance, concreteTreeTasks );

    TskDescVec inheritanceRolloutTasks;
    {
        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc inheritanceRollout = encode( Task{ eTask_InheritanceRollout, sourceFilePath } );
            dependencies.add( inheritanceRollout, TskDescVec{ inheritance } );
            inheritanceRolloutTasks.push_back( inheritanceRollout );
        }
    }

    const TskDesc hyperGraph = encode( Task{ eTask_HyperGraph, manifestFilePath } );
    dependencies.add( hyperGraph, inheritanceRolloutTasks );

    TskDescVec hyperGraphRolloutTasks;
    {
        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc hyperGraphRollout = encode( Task{ eTask_HyperGraphRollout, sourceFilePath } );
            const TskDesc allocators        = encode( Task{ eTask_Allocators, sourceFilePath } );

            dependencies.add( hyperGraphRollout, TskDescVec{ hyperGraph } );
            dependencies.add( allocators, TskDescVec{ hyperGraphRollout } );
            hyperGraphRolloutTasks.push_back( hyperGraphRollout );
        }
    }

    const TskDesc alias = encode( Task{ eTask_Alias, manifestFilePath } );
    dependencies.add( alias, hyperGraphRolloutTasks );

    TskDescVec aliasTasks;
    {
        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc aliasRollout = encode( Task{ eTask_AliasRollout, sourceFilePath } );

            dependencies.add( aliasRollout, TskDescVec{ alias } );
            aliasTasks.push_back( aliasRollout );
        }
    }

    const TskDesc concreteTypeAnalysis = encode( Task{ eTask_ConcreteTypeAnalysis, manifestFilePath } );
    dependencies.add( concreteTypeAnalysis, aliasTasks );

    TskDescVec memoryTasks;
    {
        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc concreteTypeRollout = encode( Task{ eTask_ConcreteTypeRollout, sourceFilePath } );
            const TskDesc memoryAnalysis      = encode( Task{ eTask_Allocators, sourceFilePath } );

            dependencies.add( concreteTypeRollout, TskDescVec{ concreteTypeAnalysis } );
            dependencies.add( memoryAnalysis, TskDescVec{ concreteTypeRollout } );

            memoryTasks.push_back( memoryAnalysis );
        }
    }

    const TskDesc globalMemory = encode( Task{ eTask_GlobalMemoryStage, manifestFilePath } );
    dependencies.add( globalMemory, memoryTasks );

    TskDescVec globalMemoryRolloutTasks;
    {
        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc globalMemoryRollout = encode( Task{ eTask_GlobalMemoryStageRollout, sourceFilePath } );
            dependencies.add( globalMemoryRollout, TskDescVec{ globalMemory } );
            globalMemoryRolloutTasks.push_back( globalMemoryRollout );
        }
    }

    TskDescVec unityDependencyTasks;
    TskDescVec componentTasks;
    {
        TskDescVec valueSpaceTasks;
        for( ComponentListingView::Components::Component* pComponent : components )
        {
            if( pComponent->get_type().get() == mega::ComponentType::eInterface )
            {
                TskDescVec binaryTasks;
                {
                    for( const mega::io::megaFilePath& sourceFilePath : pComponent->get_mega_source_files() )
                    {
                        const TskDesc automata       = encode( Task{ eTask_Automata, sourceFilePath } );
                        const TskDesc operations     = encode( Task{ eTask_Operations, sourceFilePath } );
                        const TskDesc operationsPCH  = encode( Task{ eTask_OperationsPCH, sourceFilePath } );
                        const TskDesc operationsLocs = encode( Task{ eTask_OperationsLocs, sourceFilePath } );
                        const TskDesc valueSpace     = encode( Task{ eTask_ValueSpace, sourceFilePath } );

                        dependencies.add( automata, globalMemoryRolloutTasks );
                        dependencies.add( operations, TskDescVec{ automata } );
                        dependencies.add( operationsPCH, TskDescVec{ operations } );
                        dependencies.add( operationsLocs, TskDescVec{ operationsPCH } );
                        dependencies.add( valueSpace, TskDescVec{ operationsLocs } );

                        unityDependencyTasks.push_back( operationsLocs );
                        unityDependencyTasks.push_back( valueSpace );
                        valueSpaceTasks.push_back( valueSpace );
                        binaryTasks.push_back( operationsPCH );
                    }
                }
                const TskDesc interfaceComponent = encode( Task{ eTask_InterfaceComponent, pComponent->get_name() } );
                dependencies.add( interfaceComponent, binaryTasks );
                componentTasks.push_back( interfaceComponent );
            }
        }

        for( ComponentListingView::Components::Component* pComponent : components )
        {
            if( pComponent->get_type().get() == mega::ComponentType::eInterface )
            {
                TskDescVec binaryTasks;
                {
                    for( const mega::io::megaFilePath& sourceFilePath : pComponent->get_mega_source_files() )
                    {
                        const TskDesc implementation    = encode( Task{ eTask_Implementation, sourceFilePath } );
                        const TskDesc implementationObj = encode( Task{ eTask_ImplementationObj, sourceFilePath } );
                        const TskDesc initialiserObj    = encode( Task{ eTask_InitialiserObject, sourceFilePath } );
                        const TskDesc pythonWrapper     = encode( Task{ eTask_PythonWrapper, sourceFilePath } );
                        const TskDesc pythonObj         = encode( Task{ eTask_PythonObject, sourceFilePath } );
                        const TskDesc initialiser       = encode( Task{ eTask_Initialiser, sourceFilePath } );

                        dependencies.add( pythonWrapper, globalMemoryRolloutTasks );
                        dependencies.add( initialiser, globalMemoryRolloutTasks );
                        dependencies.add( implementation, valueSpaceTasks );
                        dependencies.add( implementationObj, TskDescVec{ implementation } );
                        dependencies.add( pythonObj, TskDescVec{ pythonWrapper } );
                        dependencies.add( initialiserObj, TskDescVec{ initialiser } );

                        binaryTasks.push_back( implementationObj );
                        binaryTasks.push_back( pythonObj );
                        binaryTasks.push_back( initialiserObj );
                    }
                }
                const TskDesc interfaceComponent = encode( Task{ eTask_InterfaceComponent, pComponent->get_name() } );
                dependencies.add( interfaceComponent, binaryTasks );
                componentTasks.push_back( interfaceComponent );
            }
        }

        for( ComponentListingView::Components::Component* pComponent : components )
        {
            if( pComponent->get_type().get() == mega::ComponentType::eLibrary )
            {
                TskDescVec binaryTasks;
                {
                    const TskDesc includes   = encode( Task{ eTask_CPPInclude, pComponent->get_name() } );
                    const TskDesc includePCH = encode( Task{ eTask_CPPIncludePCH, pComponent->get_name() } );
                    const TskDesc objectInterfaceGeneration
                        = encode( Task{ eTask_CPPInterfaceGeneration, pComponent->get_name() } );
                    const TskDesc objectInterfaceAnalysis
                        = encode( Task{ eTask_CPPInterfaceAnalysis, pComponent->get_name() } );

                    dependencies.add( includes, globalMemoryRolloutTasks );
                    dependencies.add( includePCH, TskDescVec{ includes } );

                    TskDescVec deps = inheritanceRolloutTasks;
                    deps.push_back( includePCH );

                    dependencies.add( objectInterfaceGeneration, deps );
                    dependencies.add( objectInterfaceAnalysis, TskDescVec{ objectInterfaceGeneration } );

                    TskDescVec tasks = valueSpaceTasks;
                    tasks.push_back( objectInterfaceAnalysis );

                    for( const mega::io::cppFilePath& sourceFile : pComponent->get_cpp_source_files() )
                    {
                        const TskDesc cppPCH               = encode( Task{ eTask_CPPPCH, sourceFile } );
                        const TskDesc cppCPPImplementation = encode( Task{ eTask_CPPImplementation, sourceFile } );
                        const TskDesc cppObj               = encode( Task{ eTask_CPPObj, sourceFile } );

                        dependencies.add( cppPCH, tasks );
                        dependencies.add( cppCPPImplementation, TskDescVec{ cppPCH } );
                        dependencies.add( cppObj, TskDescVec{ cppCPPImplementation } );

                        unityDependencyTasks.push_back( cppPCH );
                        binaryTasks.push_back( cppObj );
                    }
                }

                const TskDesc libraryComponent = encode( Task{ eTask_LibraryComponent, pComponent->get_name() } );
                dependencies.add( libraryComponent, binaryTasks );
                componentTasks.push_back( libraryComponent );
            }
        }
    }

    TskDesc unityReflection = encode( Task{ eTask_UnityReflection, manifestFilePath } );
    TskDesc unityAnalysis   = encode( Task{ eTask_UnityAnalysis, manifestFilePath } );
    TskDesc unity           = encode( Task{ eTask_Unity, manifestFilePath } );
    TskDesc unityDatabase   = encode( Task{ eTask_UnityDatabase, manifestFilePath } );
    dependencies.add( unityReflection, unityDependencyTasks );
    dependencies.add( unityAnalysis, TskDescVec{ unityReflection } );
    dependencies.add( unity, TskDescVec{ unityAnalysis } );
    dependencies.add( unityDatabase, TskDescVec{ unity } );

    {
        TskDescVec completionTasks = componentTasks;
        completionTasks.push_back( unityDatabase );
        TskDesc complete = encode( Task{ eTask_Complete, manifestFilePath } );
        dependencies.add( complete, completionTasks );
    }

    return { dependencies };
}

void CompilerPipeline::execute( const pipeline::TaskDescriptor& pipelineTask, pipeline::Progress& progress,
                                pipeline::Stash& stash, pipeline::DependencyProvider& dependencies )
{
    VERIFY_RTE( m_toolChain.has_value() );
    VERIFY_RTE( m_configuration.has_value() );

    Configuration& config = m_configuration.value();

    const Task task = decode( pipelineTask );

    mega::io::StashEnvironment environment( stash, config.directories );

    mega::compiler::TaskArguments taskArguments(
        environment, m_toolChain.value(), config.unityProjectDir, config.unityEditor, dependencies.getParser() );

    mega::compiler::BaseTask::Ptr pTask;

#define TASK( taskName, sourceFileType, argumentType )                                   \
    if( task.strTaskName == "Task_" #taskName )                                          \
    {                                                                                    \
        VERIFY_RTE( !pTask );                                                            \
        pTask = mega::compiler::create_Task_##taskName( taskArguments, sourceFileType ); \
    }
#include "tasks.xmc"
#undef TASK

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
