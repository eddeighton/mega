


#include <iostream>


class Test
{
public:

    template< typename T, typename... Args >
    void invoke( T param, Args... args );

};



int main( int argc, const char* argv[] )
{
    std::cout << "Hello World" << std::endl;

    Test t;
    //t.invoke( 1, true );
    //t.invoke( 2.0f );

    //t.invoke( 1, true, true );
    return 0;
}
