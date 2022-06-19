
#include <boost/dll/runtime_symbol_info.hpp>
#include <iostream>

#include "boost/dll/import.hpp"
#include "boost/dll/alias.hpp"
#include "boost/dll/library_info.hpp"

extern "C"
{
    void testFunction( const char* pszName ) { std::cout << "testFunction: " << pszName << std::endl; }

    typedef void ( *funcPtr )( const char* );

    funcPtr getFunction() { return &testFunction; }

    void* getVoidStarFunction()
    {
        funcPtr pFunction = &testFunction;
        return ( void* )pFunction;
    }
}

BOOST_DLL_ALIAS_SECTIONED( getVoidStarFunction, eds_function_alias, mega_sec )

int main( int argc, const char* argv[] )
{
    {
        auto func = boost::dll::import_symbol< void( const char* ) >( boost::dll::program_location(), "testFunction" );

        func( "it works" );
    }

    {
        auto func2 = boost::dll::import_symbol< funcPtr() >( boost::dll::program_location(), "getFunction" );

        auto func3 = func2();

        func3( "also works" );
    }

    {
        auto func4 = boost::dll::import_symbol< void*() >( boost::dll::program_location(), "getVoidStarFunction" );

        void* pVoidStar = func4();

        funcPtr pFunc5 = reinterpret_cast< funcPtr >( pVoidStar );
        pFunc5( "it all works" );
    }

    {
        auto func6 = boost::dll::import_alias< void*() >( boost::dll::program_location(), "eds_function_alias" );

        void* pVoidStar = func6();

        funcPtr pFunc7 = reinterpret_cast< funcPtr >( pVoidStar );
        pFunc7( "alias works" );
    }

    {
        boost::dll::library_info libInfo( boost::dll::program_location() );
        std::vector< std::string > exports = libInfo.symbols( "mega_sec" );

        std::cout << "Found: " << exports.size() << " mega_sec symbols" << std::endl;
        for( const std::string& str : exports )
        {
            auto f = boost::dll::import_alias< void*() >( boost::dll::program_location(), str );

            void* pVoidStar = f();

            funcPtr pFunc = reinterpret_cast< funcPtr >( pVoidStar );
            pFunc( "alias via enumeration works" );
        }

    }

    return 0;
}
