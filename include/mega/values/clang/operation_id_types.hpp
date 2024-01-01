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



#ifndef EG_OPERATION_TYPES
#define EG_OPERATION_TYPES

#include "mega/values/compilation/interface/symbol_id.hpp"
#include "mega/values/compilation/operation_id.hpp"

struct [[clang::eg_type( mega::id_Imp_NoParams )]] __eg_ImpNoParams
{
    enum : mega::interface::SymbolID::ValueType
    {
        ID = mega::id_Imp_NoParams
    };
};

struct [[clang::eg_type( mega::id_Imp_Params )]] __eg_ImpParams
{
    enum : mega::interface::SymbolID::ValueType
    {
        ID = mega::id_Imp_Params
    };
};

struct [[clang::eg_type( mega::id_Move )]] MOVE
{
    enum : mega::interface::SymbolID::ValueType
    {
        ID = mega::id_Move
    };
};

struct [[clang::eg_type( mega::id_Get )]] GET
{
    enum : mega::interface::SymbolID::ValueType
    {
        ID = mega::id_Get
    };
};

struct [[clang::eg_type( mega::id_Range )]] RANGE
{
    enum : mega::interface::SymbolID::ValueType
    {
        ID = mega::id_Range
    };
};

struct [[clang::eg_type( mega::id_Remove )]] REMOVE
{
    enum : mega::interface::SymbolID::ValueType
    {
        ID = mega::id_Remove
    };
};

struct [[clang::eg_type( mega::id_Clear )]] CLEAR
{
    enum : mega::interface::SymbolID::ValueType
    {
        ID = mega::id_Clear
    };
};

#endif // EG_OPERATION_TYPES