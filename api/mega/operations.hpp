//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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


#ifndef EG_OPERATION_TYPES
#define EG_OPERATION_TYPES

#include "common.hpp"

struct [[clang::eg_type( mega::id_Imp_NoParams    )]] __eg_ImpNoParams
{
    enum : mega::TypeID
    {
        ID = mega::id_Imp_NoParams
    };
};   //id_Imp_NoParams (-2147483648)
struct [[clang::eg_type( mega::id_Imp_Params      )]] __eg_ImpParams
{
    enum : mega::TypeID
    {
        ID = mega::id_Imp_Params
    };
};     //id_Imp_Params   (-2147483647)
struct [[clang::eg_type( mega::id_Start           )]] Start
{
    enum : mega::TypeID
    {
        ID = mega::id_Start
    };
};              //id_Start        (-2147483646)

struct [[clang::eg_type( mega::id_Stop            )]] Stop
{
    enum : mega::TypeID
    {
        ID = mega::id_Stop
    };
};               //id_Stop         (-2147483645)

struct [[clang::eg_type( mega::id_Pause           )]] Pause
{
    enum : mega::TypeID
    {
        ID = mega::id_Pause
    };
};              //id_Pause        (-2147483644)

struct [[clang::eg_type( mega::id_Resume          )]] Resume
{
    enum : mega::TypeID
    {
        ID = mega::id_Resume
    };
};             //id_Resume       (-2147483643)

struct [[clang::eg_type( mega::id_Wait            )]] Wait
{
    enum : mega::TypeID
    {
        ID = mega::id_Wait
    };
};               //id_Wait         (-2147483642)

struct [[clang::eg_type( mega::id_Get             )]] Get
{
    enum : mega::TypeID
    {
        ID = mega::id_Get
    };
};                //id_Get          (-2147483641)

struct [[clang::eg_type( mega::id_Done            )]] Done
{
    enum : mega::TypeID
    {
        ID = mega::id_Done
    };
};               //id_Done         (-2147483640)

struct [[clang::eg_type( mega::id_Range           )]] Range
{
    enum : mega::TypeID
    {
        ID = mega::id_Range
    };
};              //id_Range        (-2147483639)

struct [[clang::eg_type( mega::id_Raw             )]] Raw
{
    enum : mega::TypeID
    {
        ID = mega::id_Raw
    };
};                //id_Raw          (-2147483638)

#endif //EG_OPERATION_TYPES