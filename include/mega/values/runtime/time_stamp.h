
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

#ifndef GUARD_2023_November_27_time_stamp_h
#define GUARD_2023_November_27_time_stamp_h

#include "mega/values/runtime.h"
#include "mega/values/native_types.h"

C_RUNTIME_START

typedef struct _c_time_stamp
{
    c_u32 m_value;
} c_time_stamp;

inline c_time_stamp c_time_stamp_increment( c_time_stamp t )
{
    return c_time_stamp{ t.m_value + 1 };
}

inline c_bool c_time_stamp_less_than( c_time_stamp left, c_time_stamp right )
{
    return left.m_value < right.m_value;
}
inline c_bool c_time_stamp_equal_to( c_time_stamp left, c_time_stamp right )
{
    return left.m_value == right.m_value;
}

C_RUNTIME_END

#endif //GUARD_2023_November_27_time_stamp_h
