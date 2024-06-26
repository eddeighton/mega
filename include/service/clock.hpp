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

#ifndef EG_CLOCK_12_06_2019
#define EG_CLOCK_12_06_2019

#include "mega/values/service/project.hpp"

#include "service/protocol/common/sender_ref.hpp"

#include "event/range.hpp"

#include "mega/values/runtime/pointer.hpp"

#include <chrono>
#include <memory>

namespace mega::service
{
class ProcessClock
{
public:
    virtual void runtimeLock( network::LogicalThreadBase* pSender )   = 0;
    virtual void runtimeUnLock( network::LogicalThreadBase* pSender ) = 0;

    virtual void registerMPO( network::SenderRef sender )                                                  = 0;
    virtual void unregisterMPO( network::SenderRef sender )                                                = 0;
    virtual void requestClock( network::LogicalThreadBase* pSender, runtime::MPO mpo, event::Range range ) = 0;
    virtual bool unrequestClock( network::LogicalThreadBase* pSender, runtime::MPO mpo )                   = 0;
    virtual void requestMove( network::LogicalThreadBase* pSender, runtime::MPO mpo )                      = 0;
};
} // namespace mega::service

#endif // EG_CLOCK_12_06_2019