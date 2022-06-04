
#include "compiler/configuration.hpp"

#include "base_task.hpp"

#include "pipeline/task.hpp"
#include "pipeline/stash.hpp"
#include "pipeline/pipeline.hpp"

#include "database/common/serialisation.hpp"
#include "database/types/sources.hpp"
#include "database/common/environments.hpp"

#include "common/assert_verify.hpp"

#include "boost/config.hpp"
#include "boost/archive/binary_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"

#include <common/string.hpp>

#include <sstream>
#include <thread>
#include <chrono>
#include <variant>

namespace driver
{
namespace interface
{

extern BaseTask::Ptr create_Task_GenerateManifest( const TaskArguments&                          taskArguments,
                                                   const std::vector< boost::filesystem::path >& componentInfoPaths );
extern BaseTask::Ptr create_Task_GenerateComponents( const TaskArguments&                          taskArguments,
                                                     const std::vector< boost::filesystem::path >& componentInfoPaths );

extern BaseTask::Ptr create_Task_ParseAST( const TaskArguments&          taskArguments,
                                           const mega::io::megaFilePath& sourceFilePath );

extern BaseTask::Ptr create_Task_Include( const TaskArguments&          taskArguments,
                                          const mega::io::megaFilePath& sourceFilePath );
extern BaseTask::Ptr create_Task_IncludePCH( const TaskArguments&          taskArguments,
                                             const mega::io::megaFilePath& sourceFilePath );
extern BaseTask::Ptr create_Task_ObjectInterfaceGen( const TaskArguments&          taskArguments,
                                                     const mega::io::megaFilePath& sourceFilePath );

extern BaseTask::Ptr create_Task_SymbolAnalysis( const TaskArguments&              taskArguments,
                                                 const mega::io::manifestFilePath& manifestFilePath );
extern BaseTask::Ptr create_Task_DependencyAnalysis( const TaskArguments&              taskArguments,
                                                     const mega::io::manifestFilePath& manifestFilePath );

extern BaseTask::Ptr create_Task_SymbolRollout( const TaskArguments&          taskArguments,
                                                const mega::io::megaFilePath& sourceFilePath );
extern BaseTask::Ptr create_Task_ObjectInterfaceGeneration( const TaskArguments&          taskArguments,
                                                            const mega::io::megaFilePath& sourceFilePath );
extern BaseTask::Ptr create_Task_ObjectInterfaceAnalysis( const TaskArguments&          taskArguments,
                                                          const mega::io::megaFilePath& sourceFilePath );
extern BaseTask::Ptr create_Task_ConcreteTree( const TaskArguments&          taskArguments,
                                               const mega::io::megaFilePath& sourceFilePath );

extern BaseTask::Ptr create_Task_Operations( const TaskArguments&          taskArguments,
                                             const mega::io::megaFilePath& sourceFilePath );
extern BaseTask::Ptr create_Task_OperationsPCH( const TaskArguments&          taskArguments,
                                                const mega::io::megaFilePath& sourceFilePath );

extern BaseTask::Ptr create_Task_Implementation( const TaskArguments&          taskArguments,
                                                 const mega::io::megaFilePath& sourceFilePath );
extern BaseTask::Ptr create_Task_ImplementationObj( const TaskArguments&          taskArguments,
                                                    const mega::io::megaFilePath& sourceFilePath );

class Task_Complete : public BaseTask
{
    const mega::io::manifestFilePath& m_manifest;

public:
    Task_Complete( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifest )
        : BaseTask( taskArguments )
        , m_manifest( manifest )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        start( taskProgress, "Task_Complete", m_manifest.path(), m_manifest.path() );
        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Complete( const TaskArguments&              taskArguments,
                                    const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_Complete >( taskArguments, manifestFilePath );
}

} // namespace interface
} // namespace driver

namespace mega
{
namespace compiler
{
namespace
{

struct Task
{
    std::string strTaskName; // not serialised

    using FilePathVar = std::variant< mega::io::megaFilePath, mega::io::manifestFilePath >;
    FilePathVar sourceFilePath;

    Task( const std::string& strName )
        : strTaskName( strName )
    {
    }
    Task( const std::string& strName, const FilePathVar& filePathVar )
        : strTaskName( strName )
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
            void operator()( const mega::io::manifestFilePath& filePath ) const { ar& filePath; }
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
                mega::io::manifestFilePath filePath;
                ar&                        filePath;
                sourceFilePath = filePath;
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
    Task task( taskDescriptor.getName() );
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
                                        pipeline::Stash& stash ) override;

    virtual void initialise( const pipeline::Configuration& pipelineConfig ) override
    {
        m_configuration = fromPipelineConfiguration( pipelineConfig );

        // check the version is latest
        const auto actualVersion = Version::getVersion();
        VERIFY_RTE_MSG( actualVersion == m_configuration->header.version,
                        "Pipeine version mismatch: Configuration version: " << m_configuration->header.version
                                                                            << " Actual Version: " << actualVersion );
    }
};

pipeline::Schedule CompilerPipeline::getSchedule( pipeline::Progress& progress, pipeline::Stash& stash )
{
    VERIFY_RTE( m_configuration.has_value() );

    using TskDesc    = pipeline::TaskDescriptor;
    using TskDescVec = pipeline::TaskDescriptor::Vector;

    Configuration& config = m_configuration.value();

    mega::io::BuildEnvironment environment(
        config.directories.rootSourceDir, config.directories.rootBuildDir, config.directories.templatesDir );
    mega::io::manifestFilePath manifestFilePath = environment.project_manifest();

    // paradox - need a manifest to determine the schedule - the schedule generates the manifest!
    {
        const TskDesc generateManifest = encode( Task{ "Task_GenerateManifest", manifestFilePath } );
        execute( generateManifest, progress, stash );
    }
    mega::io::Manifest manifest( environment, manifestFilePath );

    TskDescVec                                  interfaceGenTasks;
    std::map< mega::io::megaFilePath, TskDesc > includePCHTasks;

    pipeline::Dependencies dependencies;

    const TskDesc generateComponents = encode( Task{ "Task_GenerateComponents", manifestFilePath } );
    dependencies.add( generateComponents, TskDescVec{} );

    {
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc parseAst           = encode( Task{ "Task_ParseAST", sourceFilePath } );
            const TskDesc includes           = encode( Task{ "Task_Include", sourceFilePath } );
            const TskDesc includePCH         = encode( Task{ "Task_IncludePCH", sourceFilePath } );
            const TskDesc objectInterfaceGen = encode( Task{ "Task_ObjectInterfaceGen", sourceFilePath } );

            dependencies.add( parseAst, TskDescVec{ generateComponents } );
            dependencies.add( includes, TskDescVec{ parseAst } );
            dependencies.add( includePCH, TskDescVec{ includes } );
            dependencies.add( objectInterfaceGen, TskDescVec{ parseAst } );

            includePCHTasks.insert( { sourceFilePath, includePCH } );
            interfaceGenTasks.push_back( objectInterfaceGen );
        }
    }

    const TskDesc dependencyAnalysis = encode( Task{ "Task_DependencyAnalysis", manifestFilePath } );
    const TskDesc symbolAnalysis     = encode( Task{ "Task_SymbolAnalysis", manifestFilePath } );

    dependencies.add( dependencyAnalysis, interfaceGenTasks );
    dependencies.add( symbolAnalysis, TskDescVec{ dependencyAnalysis } );

    TskDescVec symbolRolloutTasks;
    {
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc symbolRollout = encode( Task{ "Task_SymbolRollout", sourceFilePath } );
            symbolRolloutTasks.push_back( symbolRollout );

            dependencies.add( symbolRollout, TskDescVec{ symbolAnalysis } );
        }
    }

    TskDescVec implementationObjTasks;
    {
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            const TskDesc objectInterfaceGeneration
                = encode( Task{ "Task_ObjectInterfaceGeneration", sourceFilePath } );
            const TskDesc objectInterfaceAnalysis = encode( Task{ "Task_ObjectInterfaceAnalysis", sourceFilePath } );
            const TskDesc concreteTree            = encode( Task{ "Task_ConcreteTree", sourceFilePath } );
            const TskDesc operations              = encode( Task{ "Task_Operations", sourceFilePath } );
            const TskDesc operationsPCH           = encode( Task{ "Task_OperationsPCH", sourceFilePath } );
            const TskDesc implementation          = encode( Task{ "Task_Implementation", sourceFilePath } );
            const TskDesc implementationObj       = encode( Task{ "Task_ImplementationObj", sourceFilePath } );

            dependencies.add( objectInterfaceGeneration, symbolRolloutTasks );
            dependencies.add(
                objectInterfaceAnalysis, TskDescVec{ objectInterfaceGeneration, includePCHTasks[ sourceFilePath ] } );
            dependencies.add( concreteTree, TskDescVec{ objectInterfaceAnalysis } );
            dependencies.add( operations, TskDescVec{ concreteTree } );
            dependencies.add( operationsPCH, TskDescVec{ operations } );
            dependencies.add( implementation, TskDescVec{ operationsPCH } );
            dependencies.add( implementationObj, TskDescVec{ implementation } );

            implementationObjTasks.push_back( implementationObj );
        }
    }

    TskDesc complete = encode( Task{ "Task_Complete", manifestFilePath } );
    dependencies.add( complete, TskDescVec{ implementationObjTasks } );

    return pipeline::Schedule( dependencies );
}

void CompilerPipeline::execute( const pipeline::TaskDescriptor& pipelineTask, pipeline::Progress& progress,
                                pipeline::Stash& stash )
{
    VERIFY_RTE( m_configuration.has_value() );
    Configuration& config = m_configuration.value();

    const Task task = decode( pipelineTask );

    mega::io::StashEnvironment environment(
        stash, config.directories.rootSourceDir, config.directories.rootBuildDir, config.directories.templatesDir );

    driver::interface::TaskArguments taskArguments( environment, config.toolChain );

    driver::interface::BaseTask::Ptr pTask;

    if ( task.strTaskName == "Task_GenerateManifest" )
    {
        pTask = driver::interface::create_Task_GenerateManifest( taskArguments, config.componentInfoPaths );
    }
    else if ( task.strTaskName == "Task_GenerateComponents" )
    {
        pTask = driver::interface::create_Task_GenerateComponents( taskArguments, config.componentInfoPaths );
    }
    else if ( task.strTaskName == "Task_ParseAST" )
    {
        pTask = driver::interface::create_Task_ParseAST(
            taskArguments, std::get< mega::io::megaFilePath >( task.sourceFilePath ) );
    }
    else if ( task.strTaskName == "Task_Include" )
    {
        pTask = driver::interface::create_Task_Include(
            taskArguments, std::get< mega::io::megaFilePath >( task.sourceFilePath ) );
    }
    else if ( task.strTaskName == "Task_IncludePCH" )
    {
        pTask = driver::interface::create_Task_IncludePCH(
            taskArguments, std::get< mega::io::megaFilePath >( task.sourceFilePath ) );
    }
    else if ( task.strTaskName == "Task_ObjectInterfaceGen" )
    {
        pTask = driver::interface::create_Task_ObjectInterfaceGen(
            taskArguments, std::get< mega::io::megaFilePath >( task.sourceFilePath ) );
    }
    else if ( task.strTaskName == "Task_DependencyAnalysis" )
    {
        pTask = driver::interface::create_Task_DependencyAnalysis(
            taskArguments, std::get< mega::io::manifestFilePath >( task.sourceFilePath ) );
    }
    else if ( task.strTaskName == "Task_SymbolAnalysis" )
    {
        pTask = driver::interface::create_Task_SymbolAnalysis(
            taskArguments, std::get< mega::io::manifestFilePath >( task.sourceFilePath ) );
    }
    else if ( task.strTaskName == "Task_SymbolRollout" )
    {
        pTask = driver::interface::create_Task_SymbolRollout(
            taskArguments, std::get< mega::io::megaFilePath >( task.sourceFilePath ) );
    }
    else if ( task.strTaskName == "Task_ObjectInterfaceGeneration" )
    {
        pTask = driver::interface::create_Task_ObjectInterfaceGeneration(
            taskArguments, std::get< mega::io::megaFilePath >( task.sourceFilePath ) );
    }
    else if ( task.strTaskName == "Task_ObjectInterfaceAnalysis" )
    {
        pTask = driver::interface::create_Task_ObjectInterfaceAnalysis(
            taskArguments, std::get< mega::io::megaFilePath >( task.sourceFilePath ) );
    }
    else if ( task.strTaskName == "Task_ConcreteTree" )
    {
        pTask = driver::interface::create_Task_ConcreteTree(
            taskArguments, std::get< mega::io::megaFilePath >( task.sourceFilePath ) );
    }
    else if ( task.strTaskName == "Task_Operations" )
    {
        pTask = driver::interface::create_Task_Operations(
            taskArguments, std::get< mega::io::megaFilePath >( task.sourceFilePath ) );
    }
    else if ( task.strTaskName == "Task_OperationsPCH" )
    {
        pTask = driver::interface::create_Task_OperationsPCH(
            taskArguments, std::get< mega::io::megaFilePath >( task.sourceFilePath ) );
    }
    else if ( task.strTaskName == "Task_Implementation" )
    {
        pTask = driver::interface::create_Task_Implementation(
            taskArguments, std::get< mega::io::megaFilePath >( task.sourceFilePath ) );
    }
    else if ( task.strTaskName == "Task_ImplementationObj" )
    {
        pTask = driver::interface::create_Task_ImplementationObj(
            taskArguments, std::get< mega::io::megaFilePath >( task.sourceFilePath ) );
    }
    else if ( task.strTaskName == "Task_Complete" )
    {
        pTask = create_Task_Complete( taskArguments, std::get< mega::io::manifestFilePath >( task.sourceFilePath ) );
    }
    else
    {
        THROW_RTE( "Unexpected task type: " << task.strTaskName );
    }

    VERIFY_RTE_MSG( pTask, "Failed to create task: " << task.strTaskName );

    try
    {
        pTask->run( progress );
    }
    catch ( std::exception& ex )
    {
        std::ostringstream os;
        os << "FAILED : " << pTask->getTaskName();
        os << "\n" << ex.what();

        progress.onFailed( os.str() );
    }
}

} // namespace

extern "C" BOOST_SYMBOL_EXPORT CompilerPipeline mega_pipeline;
CompilerPipeline                                mega_pipeline;

} // namespace compiler
} // namespace mega
