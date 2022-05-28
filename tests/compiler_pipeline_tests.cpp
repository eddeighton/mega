

#include "pipeline/pipeline.hpp"

#include <gtest/gtest.h>

TEST( Compiler, Basic )
{
    mega::pipeline::Registry registry;

    mega::pipeline::Pipeline::Ptr pPipeline
        = registry.getPipeline( "/build/linux_gcc_shared_debug/mega/install/bin/compiler" );

    int counter = 0;
    struct TestProgress : public mega::pipeline::Progress
    {
        int& m_counter;
        TestProgress( int& counter )
            : m_counter( counter )
        {
        }
        void onStarted() {}
        void onProgress() {}
        void onCompleted() { ++m_counter; }
    } progress( counter );

    mega::pipeline::Schedule schedule = pPipeline->getSchedule();
    while ( !schedule.isComplete() )
    {
        bool bProgress = false;
        for ( const mega::pipeline::Task& task : schedule.getReady() )
        {
            pPipeline->execute( task, progress );
            schedule.complete( task );
            bProgress = true;
        }
        ASSERT_TRUE( bProgress );
    }
    ASSERT_EQ( counter, 3 );
}
