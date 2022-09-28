

#ifndef EG_OPERATION_TYPES
#define EG_OPERATION_TYPES

#include "common.hpp"

struct [[clang::eg_type( mega::id_Imp_NoParams )]] __eg_ImpNoParams
{
    enum : mega::TypeID
    {
        ID = mega::id_Imp_NoParams
    };
};

struct [[clang::eg_type( mega::id_Imp_Params )]] __eg_ImpParams
{
    enum : mega::TypeID
    {
        ID = mega::id_Imp_Params
    };
};

struct [[clang::eg_type( mega::id_Start )]] Start
{
    enum : mega::TypeID
    {
        ID = mega::id_Start
    };
};

struct [[clang::eg_type( mega::id_Stop )]] Stop
{
    enum : mega::TypeID
    {
        ID = mega::id_Stop
    };
};

struct [[clang::eg_type( mega::id_Pause )]] Pause
{
    enum : mega::TypeID
    {
        ID = mega::id_Pause
    };
};

struct [[clang::eg_type( mega::id_Resume )]] Resume
{
    enum : mega::TypeID
    {
        ID = mega::id_Resume
    };
};

struct [[clang::eg_type( mega::id_Wait )]] Wait
{
    enum : mega::TypeID
    {
        ID = mega::id_Wait
    };
};

struct [[clang::eg_type( mega::id_Get )]] Get
{
    enum : mega::TypeID
    {
        ID = mega::id_Get
    };
};

struct [[clang::eg_type( mega::id_Done )]] Done
{
    enum : mega::TypeID
    {
        ID = mega::id_Done
    };
};

struct [[clang::eg_type( mega::id_Range )]] Range
{
    enum : mega::TypeID
    {
        ID = mega::id_Range
    };
};

struct [[clang::eg_type( mega::id_Raw )]] Raw
{
    enum : mega::TypeID
    {
        ID = mega::id_Raw
    };
};

#endif // EG_OPERATION_TYPES