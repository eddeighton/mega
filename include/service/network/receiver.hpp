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

#ifndef RECEIVER_24_MAY_2022
#define RECEIVER_24_MAY_2022

#include "service/network/logical_thread.hpp"
#include "service/network/logical_thread_manager.hpp"
#include "service/network/end_point.hpp"
#include "service/network/network.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/spawn.hpp>

#include <functional>
#include <future>

namespace mega::network
{

class Sender;

class SocketReceiver
{
public:
    SocketReceiver( LogicalThreadManager& logicalthreadManager, Traits::Socket& socket,
                    std::function< void() > disconnectHandler );
    ~SocketReceiver();

    template < typename TExecutor >
    void run( TExecutor& strandOrIOContext, Sender::Ptr pSender )
    {
        boost::asio::spawn(
            strandOrIOContext, [ this, pSender ]( boost::asio::yield_context yield ) { receive( pSender, yield ); }
        // segmented stacks do NOT work on windows
#ifndef BOOST_USE_SEGMENTED_STACKS
            ,
            boost::coroutines::attributes( NON_SEGMENTED_STACK_SIZE )
#endif
        );
    }
    void stop() { m_bContinue = false; }

private:
    void receive( Sender::Ptr pSender, boost::asio::yield_context& yield_ctx );
    void onError( const boost::system::error_code& ec );

private:
    bool                    m_bContinue = true;
    LogicalThreadManager&   m_logicalThreadManager;
    Traits::Socket&         m_socket;
    std::function< void() > m_disconnectHandler;
};

class ConcurrentChannelReceiver
{
public:
    ConcurrentChannelReceiver( LogicalThreadManager& logicalthreadManager, ConcurrentChannel& channel );
    ~ConcurrentChannelReceiver();

    template < typename TExecutor >
    void run( TExecutor& strandOrIOContext, Sender::Ptr pSender )
    {
        ConcurrentChannelReceiver& receiver = *this;
        boost::asio::spawn(
            strandOrIOContext,
            [ &receiver, pSender ]( boost::asio::yield_context yield )
            {
                //
                receiver.receive( pSender, yield );
            }
        // segmented stacks do NOT work on windows
#ifndef BOOST_USE_SEGMENTED_STACKS
            ,
            boost::coroutines::attributes( NON_SEGMENTED_STACK_SIZE )
#endif
        );
    }

    void stop() { m_bContinue = false; }

private:
    void receive( Sender::Ptr pSender, boost::asio::yield_context& yield_ctx );
    void onError( const boost::system::error_code& ec );

private:
    bool                  m_bContinue = true;
    LogicalThreadManager& m_logicalThreadManager;
    ConcurrentChannel&    m_channel;
};

} // namespace mega::network

#endif // RECEIVER_24_MAY_2022
