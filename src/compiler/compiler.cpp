
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
    return pipeline::TaskDescriptor( task.strTaskName, os.str() );
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

    TskDescVec interfaceGenTasks;

    pipeline::Dependencies dependencies;

    using namespace ComponentListingView;
    Database                                                    database( environment, manifestFilePath );
    std::vector< ComponentListingView::Components::Component* > components
        = database.template many< Components::Component >( environment.project_manifest() );

    {
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc parseAst           = encode( Task{ eTask_ParseAST, sourceFilePath } );
            const TskDesc objectInterfaceGen = encode( Task{ eTask_ObjectInterfaceGen, sourceFilePath } );

            dependencies.add( parseAst, TskDescVec{} );
            dependencies.add( objectInterfaceGen, TskDescVec{ parseAst } );

            interfaceGenTasks.push_back( objectInterfaceGen );
        }
    }

    const TskDesc dependencyAnalysis = encode( Task{ eTask_DependencyAnalysis, manifestFilePath } );
    const TskDesc symbolAnalysis     = encode( Task{ eTask_SymbolAnalysis, manifestFilePath } );

    dependencies.add( dependencyAnalysis, interfaceGenTasks );
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

    TskDescVec concreteTreeTasks;
    {
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc includes                  = encode( Task{ eTask_Include, sourceFilePath } );
            const TskDesc includePCH                = encode( Task{ eTask_IncludePCH, sourceFilePath } );
            const TskDesc objectInterfaceGeneration = encode( Task{ eTask_InterfaceGeneration, sourceFilePath } );
            const TskDesc objectInterfaceAnalysis   = encode( Task{ eTask_InterfaceAnalysis, sourceFilePath } );
            const TskDesc concreteTree              = encode( Task{ eTask_ConcreteTree, sourceFilePath } );

            dependencies.add( includes, symbolRolloutTasks );
            dependencies.add( includePCH, TskDescVec{ includes } );
            dependencies.add( objectInterfaceGeneration, TskDescVec{ includePCH } );
            dependencies.add( objectInterfaceAnalysis, TskDescVec{ objectInterfaceGeneration } );
            dependencies.add( concreteTree, TskDescVec{ objectInterfaceAnalysis } );

            concreteTreeTasks.push_back( concreteTree );
        }
    }

    const TskDesc hyperGraph = encode( Task{ eTask_HyperGraph, manifestFilePath } );
    const TskDesc derivation = encode( Task{ eTask_Derivation, manifestFilePath } );

    dependencies.add( hyperGraph, concreteTreeTasks );
    dependencies.add( derivation, TskDescVec{ hyperGraph } );

    TskDescVec binaryTasks;
    {
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc operations    = encode( Task{ eTask_Operations, sourceFilePath } );
            const TskDesc operationsObj = encode( Task{ eTask_OperationsObj, sourceFilePath } );

            dependencies.add( operations, TskDescVec{ derivation } );
            dependencies.add( operationsObj, TskDescVec{ operations } );

            binaryTasks.push_back( operationsObj );
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

                    dependencies.add( includes, TskDescVec{ derivation } );
                    dependencies.add( includePCH, TskDescVec{ includes } );
                    dependencies.add( objectInterfaceGeneration, TskDescVec{ includePCH } );
                    dependencies.add( objectInterfaceAnalysis, TskDescVec{ objectInterfaceGeneration } );

                    for ( const mega::io::cppFilePath& sourceFile : pComponent->get_cpp_source_files() )
                    {
                        const TskDesc cppObj = encode( Task{ eTask_CPP, sourceFile } );

                        dependencies.add( cppObj, TskDescVec{ objectInterfaceAnalysis } );
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
