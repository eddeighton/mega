
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

#ifndef GUARD_2023_August_07_return_reason
#define GUARD_2023_August_07_return_reason

#include "mega/values/runtime/pointer.hpp"

#include <chrono>
#include <optional>
#include <vector>

namespace mega
{

enum Reason
{
    eReason_Wait,
    eReason_Wait_All,
    eReason_Wait_Any,
    eReason_Sleep,
    eReason_Sleep_All,
    eReason_Sleep_Any,
    eReason_Timeout,
    eReason_Complete
};

struct ReturnReason
{
    Reason                                                 reason;
    std::vector< runtime::PointerHeap >                    events;
    std::optional< std::chrono::steady_clock::time_point > timeout;

    ReturnReason()
        : reason( eReason_Complete )
    {
    }

    ReturnReason( Reason _reason )
        : reason( _reason )
    {
    }

    ReturnReason( Reason _reason, const runtime::PointerHeap& event )
        : reason( _reason )
        , events( 1, event )
    {
    }

    ReturnReason( Reason _reason, std::initializer_list< runtime::PointerHeap > _events )
        : reason( _reason )
        , events( _events )
    {
    }

    ReturnReason( const std::chrono::steady_clock::time_point& _timeout )
        : reason( eReason_Timeout )
        , timeout( _timeout )
    {
    }
};

} // namespace mega

#endif // GUARD_2023_August_07_return_reason
