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

#ifndef CHANNEL_16_JUNE_2022
#define CHANNEL_16_JUNE_2022

#include "end_point.hpp"
#include "receiver.hpp"
#include "sender_factory.hpp"

#include <optional>

namespace mega::network
{

class ReceiverChannel
{
public:
    ReceiverChannel( boost::asio::io_context& ioContext, LogicalThreadManager& logicalthreadManager )
        : m_ioContext( ioContext )
        , m_channel( ioContext )
        , m_receiver( logicalthreadManager, m_channel )
    {
    }

    void run( const ConnectionID& connectionID )
    {
        m_connectionIDOpt = connectionID;
        m_receiver.run( m_ioContext, m_connectionIDOpt.value() );
    }

    void stop()
    {
        m_receiver.stop();
        m_channel.cancel();
        m_channel.close();
    }

    Sender::Ptr getSender()
    {
        VERIFY_RTE( m_connectionIDOpt.has_value() );
        return make_concurrent_channel_sender( m_channel, m_connectionIDOpt.value() );
    }

private:
    boost::asio::io_context&      m_ioContext;
    ConcurrentChannel             m_channel;
    ConcurrentChannelReceiver     m_receiver;
    std::optional< ConnectionID > m_connectionIDOpt;
};

} // namespace mega::network

#endif // CHANNEL_16_JUNE_2022