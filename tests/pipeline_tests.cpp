
#include "pipeline/pipeline.hpp"

#include <gtest/gtest.h>

#include <utility>
#include <sstream>
#include <list>

TEST( Pipeline, Basic )
{
    using namespace mega::pipeline;

    Dependencies d;
    d.add( Task( "a" ), {} );

    Schedule s( d );
    ASSERT_TRUE( s.getReady() == Task::Vector{ Task( "a" ) } );

    s.complete( Task( "a" ) );
    ASSERT_TRUE( s.isComplete() );
    ASSERT_TRUE( s.getReady() == Task::Vector{} );
}

TEST( Pipeline, Schedule )
{
    using namespace mega::pipeline;

    Task a( "a" );
    Task b( "b" );
    Task c( "c" );

    Dependencies d;
    d.add( a, { b } );
    d.add( b, { c } );

    Schedule s( d );
    ASSERT_TRUE( s.getReady() == Task::Vector{ c } );
    s.complete( c );
    ASSERT_TRUE( s.getReady() == Task::Vector{ b } );
    s.complete( b );
    ASSERT_TRUE( s.getReady() == Task::Vector{ a } );
    s.complete( a );
    ASSERT_TRUE( s.getReady() == Task::Vector{} );
    ASSERT_TRUE( s.isComplete() );
}

std::ostream& operator<<( std::ostream& os, const mega::pipeline::Task& task ) { return os << task.str(); }
std::ostream& operator<<( std::ostream& os, const mega::pipeline::Task::Vector& tasks )
{
    bool bFirst = true;
    for ( const mega::pipeline::Task& task : tasks )
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

    Task a1( "a1" );
    Task b1( "b1" );
    Task c1( "c1" );
    Task d1( "d1" );
    Task a2( "a2" );
    Task b2( "b2" );
    Task c2( "c2" );
    Task d2( "d2" );

    Dependencies dp;

    dp.add( a1, { b1, c1, d1, a2 } );
    dp.add( a2, { b2, c2, d2 } );
    dp.add( c1, { d2 } );

    Schedule s( dp );

    std::list< Task::Vector > expected = { { b1, b2, c2, d1, d2 }, { a2, c1 }, { a1 } };

    while ( !s.isComplete() )
    {
        Task::Vector ready = s.getReady();
        std::sort( ready.begin(), ready.end() );
        ASSERT_EQ( expected.front(), ready );
        for ( const Task& t : ready )
            s.complete( t );
        expected.pop_front();
    }
    ASSERT_TRUE( s.getReady() == Task::Vector{} );
    ASSERT_TRUE( s.isComplete() );
}
