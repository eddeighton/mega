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
