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



#include <boost/dll/import.hpp>
#include <boost/dll/alias.hpp>
#include <boost/dll/library_info.hpp>

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
