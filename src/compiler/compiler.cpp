
#include "compiler/configuration.hpp"

#include "base_task.hpp"
#include "database/types/component_type.hpp"
#include "task_utils.hpp"

#include "pipeline/task.hpp"
#include "pipeline/stash.hpp"
#include "pipeline/pipeline.hpp"

#include "database/model/manifest.hxx"
#include "database/model/ComponentListingView.hxx"

#include "database/common/serialisation.hpp"
#include "database/common/environment_build.hpp"
#include "database/types/sources.hpp"

#include <common/string.hpp>
#include "common/assert_verify.hpp"

#include "boost/config.hpp"
#include "boost/archive/binary_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"

#include <sstream>
#include <thread>
#include <chrono>
#include <variant>

namespace mega
{
namespace compiler
{
namespace
{

struct Task
{
    std::string strTaskName; // not serialised

    using FilePathVar
        = std::variant< mega::io::megaFilePath, mega::io::cppFilePath, mega::io::manifestFilePath, std::string >;
    FilePathVar sourceFilePath;

    static std::string toString( const FilePathVar& filePathVar )
    {
        struct Visitor
        {
            std::string operator()( const mega::io::megaFilePath& filePath ) const { return filePath.path().string(); }
            std::string operator()( const mega::io::cppFilePath& filePath ) const { return filePath.path().string(); }
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
    Task( mega::compiler::TaskType taskType, const FilePathVar& filePathVar )
        : strTaskName( taskTypeToName( taskType ) )
        , sourceFilePath( filePathVar )
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
        switch ( index )
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
                mega::io::manifestFilePath filePath;
                ar&                        filePath;
                sourceFilePath = filePath;
            }
            break;
            case 3:
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
    return pipeline::TaskDescriptor( task.strTaskName, Task::toString( task.sourceFilePath ), os.str() );
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
    std::optional< compiler::Configuration > m_configuration;

public:
    CompilerPipeline() {}

    // pipeline::Pipeline
    virtual pipeline::Schedule getSchedule( pipeline::Progress& progress, pipeline::Stash& stash ) override;
    virtual void               execute( const pipeline::TaskDescriptor& pipelineTask, pipeline::Progress& progress,
                                        pipeline::Stash& stash, pipeline::DependencyProvider& dependencies ) override;

    virtual void initialise( const pipeline::Configuration& pipelineConfig, std::ostream& osLog ) override
    {
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
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
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
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc symbolRollout = encode( Task{ eTask_SymbolRollout, sourceFilePath } );
            symbolRolloutTasks.push_back( symbolRollout );

            dependencies.add( symbolRollout, TskDescVec{ symbolAnalysis } );
        }
    }

    TskDescVec interfaceAnalysisTasks;
    {
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
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
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc concreteTree = encode( Task{ eTask_ConcreteTree, sourceFilePath } );
            dependencies.add( concreteTree, interfaceAnalysisTasks );

            concreteTreeTasks.push_back( concreteTree );
        }
    }

    const TskDesc concreteTypeAnalysis = encode( Task{ eTask_ConcreteTypeAnalysis, manifestFilePath } );
    dependencies.add( concreteTypeAnalysis, concreteTreeTasks );

    TskDescVec concreteTypeIDRollout;
    {
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc concreteTypeRollout = encode( Task{ eTask_ConcreteTypeRollout, sourceFilePath } );
            dependencies.add( concreteTypeRollout, TskDescVec{ concreteTypeAnalysis } );
            concreteTypeIDRollout.push_back( concreteTypeRollout );
        }
    }

    const TskDesc derivation = encode( Task{ eTask_Derivation, manifestFilePath } );
    dependencies.add( derivation, concreteTypeIDRollout );

    TskDescVec derivationRolloutTasks;
    {
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc derivationRollout = encode( Task{ eTask_DerivationRollout, sourceFilePath } );
            dependencies.add( derivationRollout, TskDescVec{ derivation } );
            derivationRolloutTasks.push_back( derivationRollout );
        }
    }

    const TskDesc hyperGraph = encode( Task{ eTask_HyperGraph, manifestFilePath } );
    dependencies.add( hyperGraph, derivationRolloutTasks );

    TskDescVec hyperGraphRolloutTasks;
    {
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc hyperGraphRollout = encode( Task{ eTask_HyperGraphRollout, sourceFilePath } );
            dependencies.add( hyperGraphRollout, TskDescVec{ hyperGraph } );
            hyperGraphRolloutTasks.push_back( hyperGraphRollout );
        }
    }

    TskDescVec binaryTasks;
    {
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc allocators        = encode( Task{ eTask_Allocators, sourceFilePath } );
            const TskDesc operations        = encode( Task{ eTask_Operations, sourceFilePath } );
            const TskDesc operationsPCH     = encode( Task{ eTask_OperationsPCH, sourceFilePath } );
            const TskDesc implementation    = encode( Task{ eTask_Implementation, sourceFilePath } );
            const TskDesc implementationObj = encode( Task{ eTask_ImplementationObj, sourceFilePath } );

            dependencies.add( allocators, hyperGraphRolloutTasks );
            dependencies.add( operations, TskDescVec{ allocators } );
            dependencies.add( operationsPCH, TskDescVec{ operations } );
            dependencies.add( implementation, TskDescVec{ operationsPCH } );
            dependencies.add( implementationObj, TskDescVec{ implementation } );

            binaryTasks.push_back( implementationObj );
        }

        for ( ComponentListingView::Components::Component* pComponent : components )
        {
            switch ( pComponent->get_type().get() )
            {
                case mega::ComponentType::eInterface:
                    break;
                case mega::ComponentType::eLibrary:
                {
                    const TskDesc includes   = encode( Task{ eTask_CPPInclude, pComponent->get_name() } );
                    const TskDesc includePCH = encode( Task{ eTask_CPPIncludePCH, pComponent->get_name() } );
                    const TskDesc objectInterfaceGeneration
                        = encode( Task{ eTask_CPPInterfaceGeneration, pComponent->get_name() } );
                    const TskDesc objectInterfaceAnalysis
                        = encode( Task{ eTask_CPPInterfaceAnalysis, pComponent->get_name() } );

                    dependencies.add( includes, TskDescVec{ hyperGraph } );
                    dependencies.add( includePCH, TskDescVec{ includes } );
                    dependencies.add( objectInterfaceGeneration, TskDescVec{ includePCH } );
                    dependencies.add( objectInterfaceAnalysis, TskDescVec{ objectInterfaceGeneration } );

                    for ( const mega::io::cppFilePath& sourceFile : pComponent->get_cpp_source_files() )
                    {
                        const TskDesc cppPCH               = encode( Task{ eTask_CPPPCH, sourceFile } );
                        const TskDesc cppCPPImplementation = encode( Task{ eTask_CPPImplementation, sourceFile } );
                        const TskDesc cppObj               = encode( Task{ eTask_CPPObj, sourceFile } );

                        dependencies.add( cppPCH, TskDescVec{ objectInterfaceAnalysis } );
                        dependencies.add( cppCPPImplementation, TskDescVec{ cppPCH } );
                        dependencies.add( cppObj, TskDescVec{ cppCPPImplementation } );
                        binaryTasks.push_back( cppObj );
                    }
                }
                break;
                default:
                    THROW_RTE( "Unknown component type" );
                    break;
            }
        }
    }

    TskDesc complete = encode( Task{ eTask_Complete, manifestFilePath } );
    dependencies.add( complete, binaryTasks );

    return pipeline::Schedule( dependencies );
}

void CompilerPipeline::execute( const pipeline::TaskDescriptor& pipelineTask, pipeline::Progress& progress,
                                pipeline::Stash& stash, pipeline::DependencyProvider& dependencies )
{
    VERIFY_RTE( m_configuration.has_value() );
    Configuration& config = m_configuration.value();

    const Task task = decode( pipelineTask );

    mega::io::StashEnvironment environment( stash, config.directories );

    mega::compiler::TaskArguments taskArguments( environment, config.toolChain, dependencies.getParser() );

    mega::compiler::BaseTask::Ptr pTask;

#define TASK( taskName, sourceFileType, argumentType )                                   \
    if ( task.strTaskName == "Task_" #taskName )                                         \
    {                                                                                    \
        VERIFY_RTE( !pTask );                                                            \
        pTask = mega::compiler::create_Task_##taskName( taskArguments, sourceFileType ); \
    }
#include "tasks.hxx"
#undef TASK

    VERIFY_RTE_MSG( pTask, "Failed to create task: " << task.strTaskName );

    try
    {
        pTask->run( progress );
        if ( !pTask->isCompleted() )
        {
            std::ostringstream os;
            os << "FAILED : " << pTask->getTaskName() << " Task did NOT complete";
            progress.onProgress( os.str() );
            pTask->failed( progress );
        }
    }
    catch ( std::exception& ex )
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

} // namespace compiler
} // namespace mega
