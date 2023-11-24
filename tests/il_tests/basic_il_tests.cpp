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