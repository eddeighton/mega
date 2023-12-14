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
#include "il/elements/materialisers.hxx"
#include "il/elements/elements.hpp"
#include "il/elements/mangle.hpp"

#include <limits>
#include <sstream>
#include <iostream>

#include "il/backend/backend.hpp"

TEST( IR, Basic )
{
    using namespace mega::il;
    using namespace std::string_literals;
/*
    ValueType interfaceType = Mutable{ e_TypeID };
    ConstPtr  stringType{ e_char };

    Variable< ValueType > param1{ interfaceType, "p1"s };
    Variable< ValueType > param2{ ConstPtr{ e_char }, "p2"s };
    Variable              result{ interfaceType, "result"s };
    Literal               lit{ stringType, "\"This is a string constant\"" };

    FunctionDefinition func{

        "Basic"s,
        interfaceType,
        { param1, param2 },
        {
            Assignment{ result, Read{ param1 } },                         //
            ExpressionStatement{ Call{ log{}, { ReadLiteral{ lit } } } }, //
            Return{ result }                                              //
        } };
    std::cout << generateIR( func ) << std::endl;*/
}
