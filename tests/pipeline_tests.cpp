
#include "pipeline/pipeline.hpp"
#include "pipeline/task.hpp"

#include <boost/archive/text_oarchive.hpp>

#include <gtest/gtest.h>

#include <utility>
#include <sstream>
#include <list>

mega::pipeline::TaskDescriptor make_task( const std::string& str )
{
    return mega::pipeline::TaskDescriptor{ str, {} };
}

TEST( Pipeline, Basic )
{
    using namespace mega::pipeline;

    Dependencies d;
    d.add( make_task( "a" ), {} );

    Schedule s( d );
    ASSERT_TRUE( s.getReady() == TaskDescriptor::Vector{ make_task( "a" ) } );

    s.complete( make_task( "a" ) );
    ASSERT_TRUE( s.isComplete() );
    ASSERT_TRUE( s.getReady() == TaskDescriptor::Vector{} );
}

TEST( Pipeline, Schedule )
{
    using namespace mega::pipeline;

    TaskDescriptor a = make_task( "a" );
    TaskDescriptor b = make_task( "b" );
    TaskDescriptor c = make_task( "c" );

    Dependencies d;
    d.add( a, { b } );
    d.add( b, { c } );

    Schedule s( d );
    ASSERT_TRUE( s.getReady() == TaskDescriptor::Vector{ c } );
    s.complete( c );
    ASSERT_TRUE( s.getReady() == TaskDescriptor::Vector{ b } );
    s.complete( b );
    ASSERT_TRUE( s.getReady() == TaskDescriptor::Vector{ a } );
    s.complete( a );
    ASSERT_TRUE( s.getReady() == TaskDescriptor::Vector{} );
    ASSERT_TRUE( s.isComplete() );
}

std::ostream& operator<<( std::ostream& os, const mega::pipeline::TaskDescriptor& task ) 
{ 
    return os << task.getName();
}

std::ostream& operator<<( std::ostream& os, const mega::pipeline::TaskDescriptor::Vector& tasks )
{
    bool bFirst = true;
    for ( const mega::pipeline::TaskDescriptor& task : tasks )
    {
        if ( bFirst )
            bFirst = false;
        else
            os << ",";
        os << task;
    }
    return os;
}

TEST( Pipeline, Schedule2 )
{
    using namespace mega::pipeline;

    TaskDescriptor a1 = make_task( "a1" );
    TaskDescriptor b1 = make_task( "b1" );
    TaskDescriptor c1 = make_task( "c1" );
    TaskDescriptor d1 = make_task( "d1" );
    TaskDescriptor a2 = make_task( "a2" );
    TaskDescriptor b2 = make_task( "b2" );
    TaskDescriptor c2 = make_task( "c2" );
    TaskDescriptor d2 = make_task( "d2" );

    Dependencies dp;

    dp.add( a1, { b1, c1, d1, a2 } );
    dp.add( a2, { b2, c2, d2 } );
    dp.add( c1, { d2 } );

    Schedule s( dp );

    std::list< TaskDescriptor::Vector > expected = { { b1, b2, c2, d1, d2 }, { a2, c1 }, { a1 } };

    while ( !s.isComplete() )
    {
        TaskDescriptor::Vector ready = s.getReady();
        std::sort( ready.begin(), ready.end() );
        ASSERT_EQ( expected.front(), ready );
        for ( const TaskDescriptor& t : ready )
            s.complete( t );
        expected.pop_front();
    }
    ASSERT_TRUE( s.getReady() == TaskDescriptor::Vector{} );
    ASSERT_TRUE( s.isComplete() );
}
