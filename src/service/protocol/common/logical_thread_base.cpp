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
Sender::~Sender() = default;

LogicalThreadBase::LogicalThreadBase( LogicalThreadManager& logicalThreadManager, const LogicalThreadID& logicalthreadID )
    : m_logicalThreadManager( logicalThreadManager )
    , m_logicalThreadID( logicalthreadID )
{
}
LogicalThreadBase::~LogicalThreadBase() = default;

boost::system::error_code LogicalThreadBase::send( const Message& responseMessage )
{
    VERIFY_RTE_MSG(
        !network::isRequest( responseMessage ), "Logical thread sent request: " << responseMessage.getName() );
    ReceivedMessage    receivedMessage{ Sender::Ptr{}, responseMessage };
    LogicalThreadBase* pThis = this;
    pThis->receive( receivedMessage );
    return {};
}

boost::system::error_code LogicalThreadBase::send( const Message& responseMessage, boost::asio::yield_context& )
{
    VERIFY_RTE_MSG(
        !network::isRequest( responseMessage ), "Logical thread sent request: " << responseMessage.getName() );
    ReceivedMessage    receivedMessage{ Sender::Ptr{}, responseMessage };
    LogicalThreadBase* pThis = this;
    pThis->receive( receivedMessage );
    return {};
}

LogicalThreadBase::InitiatedRequestStack::InitiatedRequestStack( LogicalThreadBase::Ptr pLogicalThread )
    : pLogicalThread( pLogicalThread )
{
    pLogicalThread->requestStarted( Sender::Ptr{} );
}
LogicalThreadBase::InitiatedRequestStack::~InitiatedRequestStack()
{
    pLogicalThread->requestCompleted();
}

LogicalThreadBase::OutBoundRequestStack::OutBoundRequestStack( LogicalThreadBase::Ptr pLogicalThread )
    : pLogicalThread( pLogicalThread )
{
    pLogicalThread->requestStarted( Sender::Ptr{} );
}
LogicalThreadBase::OutBoundRequestStack::~OutBoundRequestStack()
{
    pLogicalThread->requestCompleted();
}

LogicalThreadBase::InBoundRequestStack::InBoundRequestStack( LogicalThreadBase::Ptr pLogicalThread,
                                                             Sender::Ptr            pRequestResponseSender )
    : pLogicalThread( pLogicalThread )
{
    ASSERT( pRequestResponseSender );
    pLogicalThread->requestStarted( pRequestResponseSender );
}
LogicalThreadBase::InBoundRequestStack::~InBoundRequestStack()
{
    pLogicalThread->requestCompleted();
}
} // namespace mega::network
