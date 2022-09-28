

#ifndef EG_CLOCK_12_06_2019
#define EG_CLOCK_12_06_2019

#include "common.hpp"

struct clock
{
    static mega::TimeStamp cycle( mega::TypeID type );
    static mega::F32 ct();
    static mega::F32 dt();
    static void next(); //for use by host
};

#endif //EG_CLOCK_12_06_2019