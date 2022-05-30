

#include "pipeline/pipeline.hpp"
#include "pipeline/task.hpp"

#include <gtest/gtest.h>

TEST( Compiler, Basic )
{
    /*mega::pipeline::Registry registry;

    mega::pipeline::Pipeline::Ptr pPipeline
        = pipeline::Registry::getPipeline( "compiler" );

    int counter = 0;
    struct TestProgress : public mega::pipeline::Progress
    {
        int& m_counter;
        TestProgress( int& counter )
            : m_counter( counter )
        {
        }
        void onStarted( const std::string& strMsg ) {}
        void onProgress( const std::string& strMsg ) {}
        void onFailed( const std::string& strMsg ) {}
        void onCompleted( const std::string& strMsg ) { ++m_counter; }
    } progress( counter );

    mega::pipeline::Schedule schedule = pPipeline->getSchedule();
    while ( !schedule.isComplete() )
    {
        bool bProgress = false;
        for ( const mega::pipeline::TaskDescriptor& task : schedule.getReady() )
        {
            pPipeline->execute( task, progress );
            schedule.complete( task );
            bProgress = true;
        }
        ASSERT_TRUE( bProgress );
    }
    ASSERT_TRUE( counter > 0 );*/
}
