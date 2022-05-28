
#include "compiler/compiler.hpp"

#include "pipeline/pipeline.hpp"

#include "common/assert_verify.hpp"

#include "boost/config.hpp"

#include <thread>
#include <chrono>

namespace mega
{
namespace compiler
{
namespace
{

class CompilerPipeline : public pipeline::Pipeline
{
public:
    CompilerPipeline();

    // pipeline::Pipeline
    virtual pipeline::Schedule getSchedule();
    virtual void               execute( const pipeline::Task& task, pipeline::Progress& progress );
};
void task_a( pipeline::Progress& progress )
{
    progress.onStarted();

    using namespace std::chrono_literals;
    std::this_thread::sleep_for( 250ms );

    progress.onCompleted();
}
void task_b( pipeline::Progress& progress )
{
    progress.onStarted();

    using namespace std::chrono_literals;
    std::this_thread::sleep_for( 250ms );

    progress.onCompleted();
}
void task_c( pipeline::Progress& progress )
{
    progress.onStarted();

    using namespace std::chrono_literals;
    std::this_thread::sleep_for( 250ms );

    progress.onCompleted();
}

CompilerPipeline::CompilerPipeline() {}

pipeline::Schedule CompilerPipeline::getSchedule()
{
    using namespace pipeline;

    Dependencies dependencies;
    dependencies.add( Task( "a" ), Task::Vector{ Task( "b" ) } );
    dependencies.add( Task( "b" ), Task::Vector{ Task( "c" ) } );
    dependencies.add( Task( "c" ), Task::Vector{} );

    Schedule schedule( dependencies );

    return schedule;
}

void CompilerPipeline::execute( const pipeline::Task& task, pipeline::Progress& progress )
{
    //
    if ( task.str() == "a" )
    {
        task_a( progress );
    }
    else if ( task.str() == "b" )
    {
        task_b( progress );
    }
    else if ( task.str() == "c" )
    {
        task_c( progress );
    }
    else
    {
        THROW_RTE( "Unknown task" );
    }
}

} // namespace

extern "C" BOOST_SYMBOL_EXPORT CompilerPipeline mega_pipeline;
CompilerPipeline                                mega_pipeline;

} // namespace compiler
} // namespace mega
