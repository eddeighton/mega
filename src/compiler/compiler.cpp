
#include "compiler/compiler.hpp"

#include "pipeline/pipeline.hpp"

#include "common/assert_verify.hpp"

#include "boost/config.hpp"

#include <common/string.hpp>
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
void taskFunction( pipeline::Progress& progress )
{
    progress.onStarted();

    using namespace std::chrono_literals;
    std::this_thread::sleep_for( 10ms );

    progress.onCompleted();
}

CompilerPipeline::CompilerPipeline() {}

pipeline::Schedule CompilerPipeline::getSchedule()
{
    using namespace pipeline;

    Dependencies dependencies;

    std::vector< Task > tasks;
    for ( int i = 0; i < 1000; ++i )
    {
        std::ostringstream os;
        os << "Task" << i;
        Task t( os.str() );

        Task::Vector dp;
        for ( int j = 0; j < 20; ++j )
        {
            if ( !tasks.empty() )
            {
                int t = ( rand() + 1 ) % tasks.size();
                if ( std::find( dp.begin(), dp.end(), tasks[ t ] ) == dp.end() )
                    dp.push_back( tasks[ t ] );
            }
        }
        dependencies.add( t, dp );
        tasks.push_back( t );
    }

    Schedule schedule( dependencies );

    return schedule;
}

void CompilerPipeline::execute( const pipeline::Task& task, pipeline::Progress& progress )
{
    //
    taskFunction( progress );
}

} // namespace

extern "C" BOOST_SYMBOL_EXPORT CompilerPipeline mega_pipeline;
CompilerPipeline                                mega_pipeline;

} // namespace compiler
} // namespace mega
