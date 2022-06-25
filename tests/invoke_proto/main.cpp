

#include "boost/dll/import.hpp"
#include "boost/dll/alias.hpp"
#include "boost/dll/library_info.hpp"

#include <iostream>

void foo( int i );

void* get_function_magic() { return ( void* )&foo; }

class Test
{
public:
    template < typename T, typename... Args >
    void invoke( T param, Args... args )
    {
        using FPtr            = void ( * )( T, Args... );
        static FPtr pFunction = ( FPtr )get_function_magic();

        pFunction( param, args... );
    }

    void foobar();

    class TestInner
    {
    public:
        template < typename T, typename... Args >
        void invoke( T param, Args... args )
        {
            using FPtr            = void ( * )( T, Args... );
            static FPtr pFunction = ( FPtr )get_function_magic();

            pFunction( param, args... );
        }
    };
};

void Test::foobar()
{
    std::cout << "foobar" << std::endl;
}

//BOOST_DLL_ALIAS_SECTIONED( Test::foobar, eds_function_alias, mega_sec )

//using FPtr2 = void(Test::*)();
//static FPtr2 pFoobar = &Test::foobar;


void foo( int i ) { std::cout << "foo called with: " << i << std::endl; }

int main( int argc, const char* argv[] )
{
    std::cout << "Hello World" << std::endl;

    Test t;
    t.foobar();
    t.invoke( 1 );
    t.invoke( 2 );
    t.invoke( 3 );

    Test::TestInner t2;
    t2.invoke( 4 );

    // t.invoke( 1, true );
    // t.invoke( 2.0f );

    // t.invoke( 1, true, true );
    return 0;
}
