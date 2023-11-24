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

#include <gtest/gtest.h>

#include "il/elements/types.hxx"
#include "il/elements/types.hpp"
#include "il/elements/functions.hxx"
#include "il/elements/elements.hpp"
#include "il/elements/mangle.hpp"

#include <limits>
#include <sstream>
#include <iostream>

#include "il/generator/generator.hpp"

TEST( IL, Basic )
{
    using namespace mega::il;
    using namespace std::string_literals;

    Type< Value > interfaceType = makeValueType( Mutable{ eInterfaceType } );
    Type< Const > stringType    = makeType< Const >( eCharStar );

    Variable param1{ interfaceType, "p1"s };
    Variable param2{ makeValueType( Const{ eCharStar } ), "p2"s };
    Variable result{ interfaceType, "result"s };
    Literal  lit{ stringType, "\"This is a string constant\"" };

    FunctionDefinition func{

        "Basic"s,
        interfaceType,
        { param1, param2 },
        {
            Assignment{ result, Read{ param1 } },                                     //
            ExpressionStatement{ Call{ makeType< Log >(), { ReadLiteral{ lit } } } }, //
            Return{ result }                                                          //
        } };
    std::cout << generate( func ) << std::endl;
}

TEST( IL, Switch )
{
    using namespace mega::il;
    using namespace std::string_literals;

    Type< Const > stringType = makeType< Const >( eCharStar );
    auto          voidType   = makeValueType( Mutable{ eVoid } );
    auto          intType    = makeValueType( Mutable{ eInt32 } );
    auto          log        = makeType< Log >();

    Variable param1{ intType, "type"s };
    Variable result{ voidType, "result"s };

    FunctionDefinition func{

        "Switch"s,
        voidType,
        { param1 },
        { Switch{ Read{ param1 },
                  {
                      Switch::Case{ Literal{ makeType< Const >( eInt32 ), "1" },
                                    { ExpressionStatement{
                                        Call{ log, { ReadLiteral{ Literal{ stringType, "\"Testing 1\"" } } } } } } }, //
                      Switch::Case{ Literal{ makeType< Const >( eInt32 ), "2" },
                                    { ExpressionStatement{
                                        Call{ log, { ReadLiteral{ Literal{ stringType, "\"Testing 2\"" } } } } } } } //
                  } } } };

    std::cout << generate( func ) << std::endl;
}

TEST( IL, If )
{
    using namespace mega::il;
    using namespace std::string_literals;

    Type< Const > stringType = makeType< Const >( eCharStar );
    auto          voidType   = makeValueType( Mutable{ eVoid } );
    auto          boolType   = makeValueType( Mutable{ eBool } );
    auto          log        = makeType< Log >();

    Variable param1{ boolType, "test1"s };
    Variable param2{ boolType, "test2"s };
    Variable result{ voidType, "result"s };

    FunctionDefinition func{

        "If"s,
        voidType,
        { param1, param2 },
        { If{ { If::Case{ Read{ param1 },
                          {
                              ExpressionStatement{
                                  Call{ log, { ReadLiteral{ Literal{ stringType, "\"test1 is true\"" } } } } } //
                          } },
                If::Case{ Read{ param2 },
                          {
                              ExpressionStatement{
                                  Call{ log, { ReadLiteral{ Literal{ stringType, "\"test2 is true\"" } } } } } //
                          } } },
              If::ElseCase{ {
                  ExpressionStatement{ Call{ log, { ReadLiteral{ Literal{ stringType, "\"all is false\"" } } } } } //
              } } } }
        //
    };

    std::cout << generate( func ) << std::endl;
}

TEST( IL, ForLoop )
{
    using namespace mega::il;
    using namespace std::string_literals;

    auto voidType = makeValueType( Mutable{ eVoid } );
    auto intType  = makeValueType( Mutable{ eInt32 } );
    auto log      = makeType< Log >();

    Variable param1{ intType, "start"s };
    Variable param2{ intType, "end"s };
    Variable iterator{ intType, "iter"s };

    FunctionDefinition func{

        "ForLoop"s,
        voidType,
        { param1, param2 },
        { ForLoop{ iterator,
                   Read{ param1 },
                   Operator{ { Read{ iterator }, Read{ param2 } }, Operator::eNotEqual },
                   Operator::makeIncrement( iterator ),
                   { ExpressionStatement{ Call{ log, { Read{ iterator } } } } } } }
        //
    };

    std::cout << generate( func ) << std::endl;
}

TEST( IL, Materialiser )
{
    using namespace mega::il;
    using namespace std::string_literals;

    auto voidType = makeValueType( Mutable{ eVoid } );
    auto typeID   = makeValueType( Mutable{ eConcreteType } );
    auto log      = makeType< Log >();

    auto voidStar          = makeValueType( Ptr{ eVoid } );
    auto boolType          = makeValueType( Mutable{ eBool } );
    auto constRefReference = makeValueType( ConstRef{ eReference } );

    Variable param1{ typeID, "typeID"s };
    Variable param2{ constRefReference, "ref"s };
    Variable param3{ voidStar, "pBuffer"s };
    Variable param4{ boolType, "bLinkReset"s };

    auto                     mat = makeType< ObjectMaterialiser >();
    Variable< Materialiser > objectMat{ mat, "objectMat"s };

    const FunctionTemplate& jitFunctionType = mat->getFunctionTemplate( ObjectMaterialiser::ObjectDtor );

    FunctionDefinition func{

        "Materialiser"s,
        voidType,
        { param1, param2, param3, param4 },
        { MaterialiserStatement //
          { objectMat, { Read{ param1 } }, jitFunctionType.function },
          ExpressionStatement //
          { CallFunctor{ objectMat,
                         jitFunctionType.function,
                         {
                             Read{ param2 }, //
                             Read{ param3 }, //
                             Read{ param4 }  //
                         } } } }

        //
    };

    std::cout << generate( func ) << std::endl;
}

TEST( IL, Mangle )
{
    using namespace mega::il;
    using namespace std::string_literals;

    auto voidType = makeValueType( Mutable{ eVoid } );
    auto voidStar = makeValueType( Ptr{ eVoid } );
    auto typeID   = makeValueType( Mutable{ eConcreteType } );
    auto log      = makeType< Log >();

    Variable param1{ voidStar, "pBuffer"s };

    FunctionDefinition func{

        "Mangle"s,
        voidType,
        { param1 },
        { ExpressionStatement //
          { Call              //
            {
                makeType< MangleCTor >( "int"s ), { Read{ param1 } } //
            } } } };

    std::cout << generate( func ) << std::endl;
}

TEST( IL, Assignment )
{
    using namespace mega::il;
    using namespace std::string_literals;

    auto voidType = makeValueType( Mutable{ eVoid } );
    auto voidStar = makeValueType( Ptr{ eVoid } );
    auto intType  = makeValueType( Mutable{ eU64 } );

    Variable param1{ intType, "param"s };
    Variable localVar1{ intType, "localVar"s };

    FunctionDefinition func{

        "Mangle"s,
        intType,
        { param1 },
        {
            VariableDeclaration{ localVar1, Read{ param1 } },           //
            Assignment{ localVar1, Operator::makeIncrement( param1 ) }, //
            Return{ localVar1 }                                         //
        }

    };

    std::cout << generate( func ) << std::endl;
}