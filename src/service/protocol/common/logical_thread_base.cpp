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


#include "service/protocol/common/logical_thread_base.hpp"

#include "service/network/log.hpp"

namespace mega::network
{

LogicalThreadBase::RequestStack::RequestStack( const char* pszMsg, LogicalThreadBase::Ptr pLogicalThread,
                                              const ConnectionID& connectionID )
    : m_pszMsg( pszMsg )
    //, m_startTime( std::chrono::steady_clock::now() )
    , pLogicalThread( pLogicalThread )
{
    // SPDLOG_DEBUG( "RequestStack::ctor prc:{} conv:{} conid:{} msg:{} stack:{}", pLogicalThread->getProcessName(), pLogicalThread->getID(), connectionID, m_pszMsg, pLogicalThread->getStackSize() + 1 );
    pLogicalThread->requestStarted( connectionID );
}
LogicalThreadBase::RequestStack::~RequestStack()
{
    // SPDLOG_DEBUG( "RequestStack::dtor prc:{} conv:{} msg:{} stack:{}", pLogicalThread->getProcessName(), pLogicalThread->getID(), m_pszMsg, pLogicalThread->getStackSize() );
    // const auto timeDelta = std::chrono::steady_clock::now() - m_startTime;
    // SPDLOG_DEBUG( "{} {} {} {}", logicalthread.getProcessName(), logicalthread.getID(), m_pszMsg, timeDelta );
    pLogicalThread->requestCompleted();
}

}
