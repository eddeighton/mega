
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

#include "mega/values/native_types.hpp"

namespace mega
{

    struct MTimeStamp
    {
        U32 m_value;
    };

    inline MTimeStamp increment( MTimeStamp t )
    {
        return MTimeStamp{ t.m_value + 1 };
    }

    inline bool less_than( MTimeStamp left, MTimeStamp right )
    {
        return left.m_value < right.m_value;
    }
    inline bool equal_to( MTimeStamp left, MTimeStamp right )
    {
        return left.m_value == right.m_value;
    }
}

#endif //GUARD_2023_November_27_time_stamp_h
