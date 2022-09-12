

#include "common/assert_verify.hpp"

#include "mega/common.hpp"

#include "boost/function.hpp"
#include "boost/dll/runtime_symbol_info.hpp"
#include "boost/dll/import.hpp"
#include "boost/dll/alias.hpp"
#include "boost/dll/library_info.hpp"
#include <boost/dll/shared_library.hpp>

#include <iostream>

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

void testone()
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
        boost::shared_ptr< void > pVoid
            = boost::dll::import_alias< void* >( boost::dll::program_location(), "eds_function_alias" );
        using ActualFunctionType        = void* ( * )();
        ActualFunctionType pFunctionPtr = reinterpret_cast< ActualFunctionType >( pVoid.get() );
        void*              pVoidStar    = pFunctionPtr();

        funcPtr pFunc7 = reinterpret_cast< funcPtr >( pVoidStar );
        pFunc7( "alias works via void* alias" );
    }

    {
        boost::dll::library_info   libInfo( boost::dll::program_location() );
        std::vector< std::string > exports = libInfo.symbols( "mega_sec" );

        std::cout << "Found: " << exports.size() << " mega_sec symbols" << std::endl;
        for ( const std::string& str : exports )
        {
            auto f = boost::dll::import_alias< void*() >( boost::dll::program_location(), str );

            void* pVoidStar = f();

            funcPtr pFunc = reinterpret_cast< funcPtr >( pVoidStar );
            pFunc( "alias via enumeration works" );
        }
    }
}
/*
    struct InterfaceComponent
    {
        using ComponentPath = boost::dll::fs::path;

        using FunctionPtr    = boost::shared_ptr< const void >;
        using FunctionPtrMap = std::unordered_map< mega::TypeID, FunctionPtr >;
        using Ptr = std::shared_ptr< InterfaceComponent >;

        InterfaceComponent( const ComponentPath& path, FunctionPtrMap& functions )
            : m_path( path )
            , m_tempPath( makeTempComponent( m_path ) )
            , m_libraryInfo( m_tempPath )
            , m_library( m_tempPath )
        {
            SPDLOG_TRACE( "InterfaceComponent loaded : {}", m_path.string() );

            // load all functions from mega alias section
            for ( const std::string& strSymbol : m_libraryInfo.symbols( "mega" ) )
            {
                SPDLOG_TRACE( "InterfaceComponent symbol : {}", strSymbol );
                mega::TypeID interfaceTypeID = 0U;
                {
                    // symbold is always '_ma123'
                    VERIFY_RTE_MSG( strSymbol.size() > 3, "Invalid symbol in mega alias section: " << strSymbol );
                    const std::string  strID( strSymbol.begin() + 3, strSymbol.end() );
                    std::istringstream is( strID );
                    is >> interfaceTypeID;
                    VERIFY_RTE_MSG( interfaceTypeID != 0U, "Invalid type ID decoded from symbol: " << strSymbol );
                }

                // ComponentManager::FunctionPtr pFunctionPtr = m_library.get_alias< void* >( strSymbol );

                ComponentManager::FunctionPtr pFunctionPtr = boost::dll::import_alias< void* >( m_library, strSymbol );
                VERIFY_RTE_MSG( functions.insert( { interfaceTypeID, pFunctionPtr } ).second,
                                "Duplicate interface symbol found for: " << strSymbol );
            }
        }

        const ComponentPath& getPath() const { return m_path; }

    private:
        ComponentPath              m_path;
        ComponentPath              m_tempPath;
        boost::dll::library_info   m_libraryInfo;
        boost::dll::shared_library m_library;
    };*/

void testtwo()
{
    using FunctionType = class std::basic_string< char > ( * )( mega::reference, int, int );
    mega::reference ref;

    boost::dll::fs::path       path( "/build/linux_gcc_shared_debug/megatest/install/bin/libTestInterface.so" );
    boost::dll::library_info   libraryInfo( path );
    boost::dll::shared_library library( path );

    {
        boost::function< std::string( mega::reference, int, int ) > pSymbolDirect
            = boost::dll::import_symbol< std::string( mega::reference, int, int ) >( library, "_mw32" );
        VERIFY_RTE_MSG( pSymbolDirect, "Failed to load symbol direct" );
        std::cout << "Symbol Direct: " << std::hex << pSymbolDirect.vtable << std::endl;

        std::string strResult = pSymbolDirect( ref, 1, 2 );
        std::cout << "Function returned: " << strResult << std::endl;
    }

    {
        boost::shared_ptr< void > pSymbolDirect = boost::dll::import_symbol< void* >( library, "_mw32" );
        VERIFY_RTE_MSG( pSymbolDirect, "Failed to load symbol void" );
        std::cout << "Symbol void: " << std::hex << pSymbolDirect << std::endl;

        FunctionType pFunctionPtr = reinterpret_cast< FunctionType >( pSymbolDirect.get() );
        std::string  strResult    = pFunctionPtr( ref, 1, 2 );
        std::cout << "Function returned: " << strResult << std::endl;
    }

    {
        // load all functions from mega alias section
        boost::shared_ptr< void** > pSymbol;
        for ( const std::string& strSymbol : libraryInfo.symbols( "mega" ) )
        {
            std::cout << strSymbol << std::endl;
            if ( strSymbol == "_ma32" )
                pSymbol = boost::dll::import_alias< void** >( library, strSymbol );
        }
        VERIFY_RTE_MSG( pSymbol, "Failed to load symbol" );
        std::cout << "Symbol Alias: " << std::hex << pSymbol.get() << std::endl;

        FunctionType pFunctionPtr = reinterpret_cast< FunctionType >( pSymbol.get() );
        std::string  strResult    = pFunctionPtr( ref, 1, 2 );
        std::cout << "Function returned: " << strResult << std::endl;
    }
}

int main( int argc, const char* argv[] )
{
    // objcopy --add-section hello2=./helloworld.txt editor

    // ld -r -b binary ./helloworld.txt -o helloworld.o
    // objcopy --add-section hello=helloworld.o symbol_test
    // objdump -s -j hello symbol_test | tail -3 | xxd -r

    auto pBegin
        = boost::dll::import_symbol< const char* >( boost::dll::program_location(), "_binary___helloworld_txt_start" );
    auto pEnd
        = boost::dll::import_symbol< const char* >( boost::dll::program_location(), "_binary___helloworld_txt_end" );
    auto pSize
        = boost::dll::import_symbol< const char* >( boost::dll::program_location(), "_binary___helloworld_txt_size" );

    VERIFY_RTE_MSG( ( pEnd.get() - pBegin.get() ) < 10000, "Invalid data section pointers found" );

    std::cout << "Found symbols" << std::endl;

    return 0;
}
