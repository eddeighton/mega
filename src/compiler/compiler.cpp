
#include "compiler/compiler.hpp"

#include "pipeline/pipeline.hpp"

#include "common/assert_verify.hpp"

#include "pipeline/task.hpp"

#include "boost/config.hpp"
#include "boost/archive/binary_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"

#include <common/string.hpp>

#include <sstream>
#include <thread>
#include <chrono>

namespace mega
{
namespace compiler
{
namespace
{

struct Task
{
    std::string strTaskName;
    std::string sourceFile;

    template < class Archive >
    inline void serialize( Archive& ar, const unsigned int version )
    {
        ar& strTaskName;
        ar& sourceFile;
    }
};

pipeline::TaskDescriptor encode( const Task& task )
{
    std::ostringstream os;
    {
        boost::archive::binary_oarchive oa( os );
        oa&                             task;
    }
    return pipeline::TaskDescriptor( os.str() );
}

Task decode( const pipeline::TaskDescriptor& taskDescriptor )
{
    Task task;
    {
        std::istringstream              is( taskDescriptor.get() );
        boost::archive::binary_iarchive ia( is );
        ia&                             task;
    }
    return task;
}

class CompilerPipeline : public pipeline::Pipeline
{
public:
    CompilerPipeline();

    // pipeline::Pipeline
    virtual pipeline::Schedule getSchedule();
    virtual void               execute( const pipeline::TaskDescriptor& task, pipeline::Progress& progress );
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

    TaskDescriptor::Vector tasks;
    for ( int i = 0; i < 100; ++i )
    {
        std::ostringstream os;
        os << "TaskDescriptor" << i;
        TaskDescriptor t( os.str() );

        TaskDescriptor::Vector dp;
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

void CompilerPipeline::execute( const pipeline::TaskDescriptor& task, pipeline::Progress& progress )
{

    if( task.get() == "TaskDescriptor20" )
    {
        THROW_RTE( "Test task exception ed was here!" );
    }

    //
    taskFunction( progress );
}

} // namespace

extern "C" BOOST_SYMBOL_EXPORT CompilerPipeline mega_pipeline;
CompilerPipeline                                mega_pipeline;

} // namespace compiler
} // namespace mega
