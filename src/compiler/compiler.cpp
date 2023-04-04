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

#include "compiler/configuration.hpp"

#include "base_task.hpp"
#include "database/types/component_type.hpp"
#include "task_utils.hpp"

#include "pipeline/task.hpp"
#include "pipeline/stash.hpp"
#include "pipeline/pipeline.hpp"

#include "database/model/manifest.hxx"
#include "database/model/ComponentListingView.hxx"

#include "database/types/sources.hpp"

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
        const auto actualVersion = Version::getVersion();
        VERIFY_RTE_MSG( actualVersion == m_configuration->header.version,
                        "Pipeine version mismatch: Configuration version: " << m_configuration->header.version
                                                                            << " Actual Version: " << actualVersion );
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

    TskDescVec interfaceTreeTasks;

    pipeline::Dependencies dependencies;

    using namespace ComponentListingView;
    Database                                                    database( environment, manifestFilePath );
    std::vector< ComponentListingView::Components::Component* > components
        = database.template many< Components::Component >( environment.project_manifest() );

    {
        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc parseAst      = encode( Task{ eTask_ParseAST, sourceFilePath } );
            const TskDesc interfaceTree = encode( Task{ eTask_InterfaceTree, sourceFilePath } );

            dependencies.add( parseAst, TskDescVec{} );
            dependencies.add( interfaceTree, TskDescVec{ parseAst } );

            interfaceTreeTasks.push_back( interfaceTree );
        }
    }

    const TskDesc pythonOperations   = encode( Task{ eTask_PythonOperations, manifestFilePath } );
    const TskDesc dependencyAnalysis = encode( Task{ eTask_DependencyAnalysis, manifestFilePath } );
    const TskDesc symbolAnalysis     = encode( Task{ eTask_SymbolAnalysis, manifestFilePath } );

    dependencies.add( pythonOperations, interfaceTreeTasks );
    dependencies.add( dependencyAnalysis, interfaceTreeTasks );
    dependencies.add( symbolAnalysis, TskDescVec{ pythonOperations, dependencyAnalysis } );

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
            const TskDesc includes                  = encode( Task{ eTask_Include, sourceFilePath } );
            const TskDesc includePCH                = encode( Task{ eTask_IncludePCH, sourceFilePath } );
            const TskDesc objectInterfaceGeneration = encode( Task{ eTask_InterfaceGeneration, sourceFilePath } );
            const TskDesc objectInterfaceAnalysis   = encode( Task{ eTask_InterfaceAnalysis, sourceFilePath } );

            dependencies.add( includes, symbolRolloutTasks );
            dependencies.add( includePCH, TskDescVec{ includes } );
            dependencies.add( objectInterfaceGeneration, TskDescVec{ includePCH } );
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

    const TskDesc derivation = encode( Task{ eTask_Derivation, manifestFilePath } );
    dependencies.add( derivation, concreteTreeTasks );

    TskDescVec derivationRolloutTasks;
    {
        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc derivationRollout = encode( Task{ eTask_DerivationRollout, sourceFilePath } );
            dependencies.add( derivationRollout, TskDescVec{ derivation } );
            derivationRolloutTasks.push_back( derivationRollout );
        }
    }

    const TskDesc hyperGraph = encode( Task{ eTask_HyperGraph, manifestFilePath } );
    dependencies.add( hyperGraph, derivationRolloutTasks );

    TskDescVec memoryTasks;
    {
        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc hyperGraphRollout = encode( Task{ eTask_HyperGraphRollout, sourceFilePath } );
            const TskDesc allocators        = encode( Task{ eTask_Allocators, sourceFilePath } );

            dependencies.add( hyperGraphRollout, TskDescVec{ hyperGraph } );
            dependencies.add( allocators, TskDescVec{ hyperGraphRollout } );
            memoryTasks.push_back( allocators );
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

    const TskDesc concreteTypeAnalysis = encode( Task{ eTask_ConcreteTypeAnalysis, manifestFilePath } );
    dependencies.add( concreteTypeAnalysis, globalMemoryRolloutTasks );

    TskDescVec concreteTypeIDRolloutTasks;
    {
        for( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc concreteTypeRollout = encode( Task{ eTask_ConcreteTypeRollout, sourceFilePath } );
            dependencies.add( concreteTypeRollout, TskDescVec{ concreteTypeAnalysis } );
            concreteTypeIDRolloutTasks.push_back( concreteTypeRollout );
        }
    }

    TskDescVec operationsTasks;
    TskDescVec componentTasks;
    {
        for( ComponentListingView::Components::Component* pComponent : components )
        {
            switch( pComponent->get_type().get() )
            {
                case mega::ComponentType::eInterface:
                {
                    TskDescVec binaryTasks;
                    {
                        for( const mega::io::megaFilePath& sourceFilePath : pComponent->get_mega_source_files() )
                        {
                            const TskDesc operations        = encode( Task{ eTask_Operations, sourceFilePath } );
                            const TskDesc pythonWrapper     = encode( Task{ eTask_PythonWrapper, sourceFilePath } );
                            const TskDesc initialiser       = encode( Task{ eTask_Initialiser, sourceFilePath } );
                            const TskDesc operationsPCH     = encode( Task{ eTask_OperationsPCH, sourceFilePath } );
                            const TskDesc implementation    = encode( Task{ eTask_Implementation, sourceFilePath } );
                            const TskDesc implementationObj = encode( Task{ eTask_ImplementationObj, sourceFilePath } );
                            const TskDesc initialiserObj    = encode( Task{ eTask_InitialiserObject, sourceFilePath } );
                            const TskDesc pythonObj         = encode( Task{ eTask_PythonObject, sourceFilePath } );

                            dependencies.add( operations, concreteTypeIDRolloutTasks );
                            dependencies.add( pythonWrapper, concreteTypeIDRolloutTasks );
                            dependencies.add( initialiser, concreteTypeIDRolloutTasks );
                            dependencies.add( operationsPCH, TskDescVec{ operations } );
                            dependencies.add( implementation, TskDescVec{ operationsPCH } );
                            dependencies.add( implementationObj, TskDescVec{ implementation } );
                            dependencies.add( pythonObj, TskDescVec{ pythonWrapper } );
                            dependencies.add( initialiserObj, TskDescVec{ initialiser } );

                            operationsTasks.push_back( operationsPCH );
                            binaryTasks.push_back( implementationObj );
                            binaryTasks.push_back( pythonObj );
                            binaryTasks.push_back( initialiserObj );
                        }
                    }
                    const TskDesc interfaceComponent
                        = encode( Task{ eTask_InterfaceComponent, pComponent->get_name() } );
                    dependencies.add( interfaceComponent, binaryTasks );
                    componentTasks.push_back( interfaceComponent );
                }
                break;
                case mega::ComponentType::eLibrary:
                {
                    TskDescVec binaryTasks;
                    {
                        const TskDesc includes   = encode( Task{ eTask_CPPInclude, pComponent->get_name() } );
                        const TskDesc includePCH = encode( Task{ eTask_CPPIncludePCH, pComponent->get_name() } );
                        const TskDesc objectInterfaceGeneration
                            = encode( Task{ eTask_CPPInterfaceGeneration, pComponent->get_name() } );
                        const TskDesc objectInterfaceAnalysis
                            = encode( Task{ eTask_CPPInterfaceAnalysis, pComponent->get_name() } );

                        dependencies.add( includes, symbolRolloutTasks );
                        dependencies.add( includePCH, TskDescVec{ includes } );

                        TskDescVec deps = derivationRolloutTasks;
                        deps.push_back( includePCH );

                        dependencies.add( objectInterfaceGeneration, deps );
                        dependencies.add( objectInterfaceAnalysis, TskDescVec{ objectInterfaceGeneration } );

                        TskDescVec tasks = concreteTypeIDRolloutTasks;
                        tasks.push_back( objectInterfaceAnalysis );

                        for( const mega::io::cppFilePath& sourceFile : pComponent->get_cpp_source_files() )
                        {
                            const TskDesc cppPCH               = encode( Task{ eTask_CPPPCH, sourceFile } );
                            const TskDesc cppCPPImplementation = encode( Task{ eTask_CPPImplementation, sourceFile } );
                            const TskDesc cppObj               = encode( Task{ eTask_CPPObj, sourceFile } );

                            dependencies.add( cppPCH, tasks );
                            dependencies.add( cppCPPImplementation, TskDescVec{ cppPCH } );
                            dependencies.add( cppObj, TskDescVec{ cppCPPImplementation } );

                            operationsTasks.push_back( cppPCH );
                            binaryTasks.push_back( cppObj );
                        }
                    }

                    const TskDesc libraryComponent = encode( Task{ eTask_LibraryComponent, pComponent->get_name() } );
                    dependencies.add( libraryComponent, binaryTasks );
                    componentTasks.push_back( libraryComponent );
                }
                break;
                default:
                    THROW_RTE( "Unknown component type" );
                    break;
            }
        }
    }

    TskDesc meta = encode( Task{ eTask_Meta, manifestFilePath } );
    dependencies.add( meta, operationsTasks );

    TskDesc unityReflection = encode( Task{ eTask_UnityReflection, manifestFilePath } );
    TskDesc unityAnalysis   = encode( Task{ eTask_UnityAnalysis, manifestFilePath } );
    TskDesc unity           = encode( Task{ eTask_Unity, manifestFilePath } );
    TskDesc unityDatabase   = encode( Task{ eTask_UnityDatabase, manifestFilePath } );
    dependencies.add( unityReflection, TskDescVec{ meta } );
    dependencies.add( unityAnalysis, TskDescVec{ unityReflection } );
    dependencies.add( unity, TskDescVec{ unityAnalysis } );
    dependencies.add( unityDatabase, TskDescVec{ unity } );

    TskDescVec schematicMapTasks;
    for( const mega::io::schFilePath& schematicFilePath : manifest.getSchematicSourceFiles() )
    {
        TskDesc schematicParse        = encode( Task{ eTask_SchematicParse, schematicFilePath } );
        TskDesc schematicContours     = encode( Task{ eTask_SchematicContours, schematicFilePath } );
        TskDesc schematicExtrusions   = encode( Task{ eTask_SchematicExtrusions, schematicFilePath } );
        TskDesc schematicConnections  = encode( Task{ eTask_SchematicConnections, schematicFilePath } );
        TskDesc schematicWallSections = encode( Task{ eTask_SchematicWallSections, schematicFilePath } );
        TskDesc schematicFloorPlan    = encode( Task{ eTask_SchematicFloorPlan, schematicFilePath } );
        TskDesc schematicVisibility   = encode( Task{ eTask_SchematicVisibility, schematicFilePath } );
        TskDesc schematicValueSpace   = encode( Task{ eTask_SchematicValueSpace, schematicFilePath } );
        TskDesc schematicSpawnPoints  = encode( Task{ eTask_SchematicSpawnPoints, schematicFilePath } );
        TskDesc schematicMapFile      = encode( Task{ eTask_SchematicMapFile, schematicFilePath } );

        dependencies.add( schematicParse, TskDescVec{ unityDatabase } );

        dependencies.add( schematicContours, TskDescVec{ schematicParse } );
        dependencies.add( schematicExtrusions, TskDescVec{ schematicContours } );
        dependencies.add( schematicConnections, TskDescVec{ schematicExtrusions } );
        dependencies.add( schematicWallSections, TskDescVec{ schematicConnections } );
        dependencies.add( schematicFloorPlan, TskDescVec{ schematicWallSections } );
        dependencies.add( schematicVisibility, TskDescVec{ schematicFloorPlan } );
        dependencies.add( schematicValueSpace, TskDescVec{ schematicVisibility } );
        dependencies.add( schematicSpawnPoints, TskDescVec{ schematicValueSpace } );
        dependencies.add( schematicMapFile, TskDescVec{ schematicSpawnPoints } );

        schematicMapTasks.push_back( schematicMapFile );
    }

    {
        TskDescVec completionTasks = componentTasks;
        completionTasks.push_back( unityDatabase );
        std::copy( schematicMapTasks.begin(), schematicMapTasks.end(), std::back_inserter( completionTasks ) );

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
            std::ostringstream os;
            os << "FAILED : " << pTask->getTaskName() << " Task did NOT complete";
            progress.onProgress( os.str() );
            pTask->failed( progress );
        }
    }
    catch( std::exception& ex )
    {
        std::ostringstream os;
        os << "FAILED : " << pTask->getTaskName();
        os << "\n" << ex.what();
        progress.onProgress( os.str() );

        pTask->failed( progress );
    }
}

} // namespace

extern "C" BOOST_SYMBOL_EXPORT CompilerPipeline mega_pipeline;
CompilerPipeline                                mega_pipeline;

} // namespace mega::compiler
