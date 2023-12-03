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

#include "mega/values/compilation/symbol_table.hpp"

#include <gtest/gtest.h>

using namespace std::string_literals;

TEST( SymbolTable, Basic )
{
    using namespace mega;
    SymbolTable st;

    ASSERT_FALSE( st.findInterfaceObject( {} ) );
    ASSERT_FALSE( st.findConcreteObject( {} ) );

    ASSERT_TRUE( st.findSymbol( "" ) );
    ASSERT_TRUE( st.findInterfaceObject( { NULL_TYPE_ID } ) );
    ASSERT_TRUE( st.findConcreteObject( { NULL_TYPE_ID } ) );

    ASSERT_TRUE( st.findSymbol( ROOT_TYPE_NAME ) );
    ASSERT_TRUE( st.findInterfaceObject( { ROOT_SYMBOL_ID } ) );
    ASSERT_TRUE( st.findConcreteObject( { ROOT_TYPE_ID } ) );

    const std::set< std::string > newSymbols = { "test1"s, "test2"s };
    {
        SymbolRequest request{ newSymbols };
        st.add( request );
    }

    {
        auto result = st.findSymbol( "test1"s );
        ASSERT_TRUE( result.has_value() );
        ASSERT_EQ( result.value(), -2 );
        ASSERT_EQ( st.getSymbol( TypeID{ -2 } ), "test1"s );
    }

    {
        auto result = st.findSymbol( "test2"s );
        ASSERT_TRUE( result.has_value() );
        ASSERT_EQ( result.value(), -3 );
        ASSERT_EQ( st.getSymbol( TypeID{ -3 } ), "test2"s );
    }

    {
        SymbolRequest request{ newSymbols };
        st.add( request );
    }

    {
        auto result = st.findSymbol( "test1"s );
        ASSERT_TRUE( result.has_value() );
        ASSERT_EQ( result.value(), -2 );
        ASSERT_EQ( st.getSymbol( TypeID{ -2 } ), "test1"s );
    }

    {
        auto result = st.findSymbol( "test2"s );
        ASSERT_TRUE( result.has_value() );
        ASSERT_EQ( result.value(), -3 );
        ASSERT_EQ( st.getSymbol( TypeID{ -3 } ), "test2"s );
    }

    ASSERT_THROW( st.getSymbol( TypeID{ -4 } ), std::runtime_error );
}

TEST( SymbolTable, Interface )
{
    using namespace mega;
    SymbolTable st;

    const std::set< std::string > newSymbols = {
        ""s,           // 0
        "Root"s,       // 1
        "Namespace1"s, // 2
        "Object1"s,    // 3
        "Object2"s,    // 4
        "Function1"s,  // 5
        "Function2"s   // 6
    };

    {
        SymbolRequest request{ newSymbols };

        request.newInterfaceObjects.insert( { TypeID{ -3 } } );                        // Object1
        request.newInterfaceObjects.insert( { TypeID{ -2 }, TypeID{ -4 } } );          // Namespace1.Object2
        request.newInterfaceElements.insert( { { TypeID{ -3 } }, { TypeID{ -5 } } } ); // Object1.Function1
        request.newInterfaceElements.insert(
            { { TypeID{ -2 }, TypeID{ -4 } }, { TypeID{ -6 } } } ); // Namespace1.Object2.Function2

        st.add( request );
    }

    {
        ASSERT_FALSE( st.findInterfaceObject( { TypeID{ -2 } } ) );
        ASSERT_FALSE( st.findInterfaceObject( { TypeID{ -4 } } ) );
    }
    {
        const SymbolTraits::SymbolIDVector symbolIDSequence{ TypeID{ -3 } };
        const auto*                        pInterfaceObject = st.findInterfaceObject( symbolIDSequence );
        ASSERT_TRUE( pInterfaceObject );
        ASSERT_EQ( pInterfaceObject->getObjectID(), 2U );
        ASSERT_EQ( pInterfaceObject->getSymbols(), symbolIDSequence );
        ASSERT_TRUE( pInterfaceObject->find( { TypeID{ -5 } } ) );
        ASSERT_FALSE( pInterfaceObject->find( { TypeID{ -3 } } ) );
        ASSERT_EQ( &st.getInterfaceObjects()[ pInterfaceObject->getObjectID() ], pInterfaceObject );
    }
    {
        const SymbolTraits::SymbolIDVector symbolIDSequence{ TypeID{ -2 }, TypeID{ -4 } };

        const auto* pInterfaceObject = st.findInterfaceObject( symbolIDSequence );
        ASSERT_TRUE( pInterfaceObject );
        ASSERT_EQ( pInterfaceObject->getObjectID(), 3U );
        ASSERT_EQ( pInterfaceObject->getSymbols(), symbolIDSequence );
        ASSERT_TRUE( pInterfaceObject->find( { TypeID{ -6 } } ) );
        ASSERT_EQ( &st.getInterfaceObjects()[ pInterfaceObject->getObjectID() ], pInterfaceObject );
    }
}
TEST( SymbolTable, Concrete )
{
    using namespace mega;
    SymbolTable st;

    const std::set< std::string > newSymbols = {
        ""s,           // 0
        "Root"s,       // 1
        "Namespace1"s, // 2
        "Object1"s,    // 3
        "Object2"s,    // 4
        "Function1"s,  // 5
        "Function2"s   // 6
    };

    {
        SymbolRequest request{ newSymbols };

        request.newInterfaceObjects.insert( { TypeID{ -3 } } );                        // Object1
        request.newInterfaceObjects.insert( { TypeID{ -2 }, TypeID{ -4 } } );          // Namespace1.Object2
        request.newInterfaceElements.insert( { { TypeID{ -3 } }, { TypeID{ -5 } } } ); // Object1.Function1
        request.newInterfaceElements.insert(
            { { TypeID{ -2 }, TypeID{ -4 } }, { TypeID{ -6 } } } ); // Namespace1.Object2.Function2

        request.newConcreteObjects.insert( { TypeID::make_context( 2U, 0U ) } );
        request.newConcreteObjects.insert( { TypeID::make_context( 3U, 0U ) } );

        request.newConcreteElements.insert(
            { { TypeID::make_context( 2U, 0U ) }, { TypeID::make_context( 2U, 1U ) } } );
        request.newConcreteElements.insert(
            { { TypeID::make_context( 3U, 0U ) }, { TypeID::make_context( 3U, 1U ) } } );

        st.add( request );
    }
}