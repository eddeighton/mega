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

#ifndef LOGICALTHREAD_BASE_5_JAN_2023
#define LOGICALTHREAD_BASE_5_JAN_2023

#include "service/protocol/common/received_message.hpp"
#include "service/protocol/common/sender.hpp"
#include "mega/values/service/logical_thread_id.hpp"
#include "service/protocol/model/messages.hxx"

#include <cstddef>
#include <memory>

namespace mega::network
{

class LogicalThreadManager;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class LogicalThreadBase : public Sender
{
    LogicalThreadManager& m_logicalThreadManager;
    const LogicalThreadID m_logicalThreadID;

public:
    using Ptr = std::shared_ptr< LogicalThreadBase >;

    inline Ptr shared_from_this()
    {
        return std::dynamic_pointer_cast< LogicalThreadBase >( Sender::shared_from_this() );
    }

    LogicalThreadBase( LogicalThreadManager& logicalThreadManager, const LogicalThreadID& logicalthreadID );
    virtual ~LogicalThreadBase();

    inline LogicalThreadManager&  getThreadManager() const { return m_logicalThreadManager; }
    inline const LogicalThreadID& getID() const { return m_logicalThreadID; }

    // Sender
    // NOTE: sender implemented to enable logical thread to receive responses in inter-thread communication
    virtual boost::system::error_code send( const Message& responseMessage ) override;
    virtual boost::system::error_code send( const Message& responseMessage, boost::asio::yield_context& ) override;

    virtual void receiveMessage( const ReceivedMessage& msg ) = 0;

protected:
    virtual void requestStarted( Sender::Ptr ){};
    virtual void requestCompleted(){};

public:
    class InitiatedRequestStack
    {
        LogicalThreadBase::Ptr pLogicalThread;

    public:
        InitiatedRequestStack( LogicalThreadBase::Ptr pLogicalThread );
        ~InitiatedRequestStack();
    };
    friend class LogicalThreadBase::InitiatedRequestStack;
    class OutBoundRequestStack
    {
        LogicalThreadBase::Ptr pLogicalThread;

    public:
        OutBoundRequestStack( LogicalThreadBase::Ptr pLogicalThread );
        ~OutBoundRequestStack();
    };
    friend class LogicalThreadBase::OutBoundRequestStack;
    class InBoundRequestStack
    {
        LogicalThreadBase::Ptr pLogicalThread;

    public:
        InBoundRequestStack( LogicalThreadBase::Ptr pLogicalThread, Sender::Ptr pRequestResponseSender );
        ~InBoundRequestStack();
    };
    friend class LogicalThreadBase::InBoundRequestStack;
};

} // namespace mega::network

#endif // LOGICALTHREAD_BASE_5_JAN_2023
