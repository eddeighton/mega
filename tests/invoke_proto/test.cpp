
#include <iostream>

class Test
{
public:

    template< typename T, typename... Args >
    void invoke( T param, Args... args );

    //template< typename TypePath, typename Operation, typename... Args >
    //void invoke( Args... args ) const;
};



template< typename T, typename... Args >
void Test::invoke( T param, Args... args )
{
    std::cout << param << std::endl;
}

/*
template void Test::invoke< int >( int param );
template void Test::invoke< float >( float param );

template void Test::invoke< int, bool >( int param, bool );
template void Test::invoke< int, bool >( int, bool, bool );
*/
