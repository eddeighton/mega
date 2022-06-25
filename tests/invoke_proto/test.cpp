
#include <iostream>

extern void* get_function_magic();

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
};

void testFunction()
{
    Test t;
    t.foobar();
}