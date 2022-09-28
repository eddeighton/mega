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

#include "service/network/receiver.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/end_point.hpp"

#include "service/protocol/common/header.hpp"

#include "utilities/serialization_helpers.hpp"

#include "common/assert_verify.hpp"

#include <boost/asio/associated_allocator.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/read.hpp>

#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/streams/vectorstream.hpp>

#include <memory>
#include <iostream>
#include <spdlog/spdlog.h>

namespace mega
{
namespace network
{

SocketReceiver::SocketReceiver( ConversationManager& conversationManager, boost::asio::ip::tcp::socket& socket,
                                std::function< void() > disconnectHandler )
    : m_conversationManager( conversationManager )
    , m_socket( socket )
    , m_disconnectHandler( disconnectHandler )
{
}

SocketReceiver::~SocketReceiver() { m_bContinue = false; }

void SocketReceiver::onError( const boost::system::error_code& ec )
{
    if ( ec == boost::asio::error::eof )
    {
        //  This is what happens when close socket normally
    }
    else if ( ec == boost::asio::error::operation_aborted )
    {
        //  This is what happens when close socket normally
    }
    else if ( ec == boost::asio::error::connection_reset )
    {
    }
    else
    {
        SPDLOG_ERROR( "SocketReceiver: Connection: {} closed. Error: {}", m_connectionID, ec.what() );
    }
}

void SocketReceiver::receive( boost::asio::yield_context& yield_ctx )
{
    static const mega::U64 MessageSizeSize = sizeof( network::MessageSize );
    using ReceiveBuffer                    = std::vector< char >;
    ReceiveBuffer                       buffer( 1024 );
    boost::system::error_code           ec;
    mega::U64                           szBytesTransferred = 0U;
    std::array< char, MessageSizeSize > buf;
    MessageID                           messageID;

    if ( m_bContinue && m_socket.is_open() )
    {
        while ( m_bContinue && m_socket.is_open() )
        {
            // read message size
            network::MessageSize size = 0U;
            {
                while ( m_bContinue && m_socket.is_open() )
                {
                    szBytesTransferred
                        = boost::asio::async_read( m_socket, boost::asio::buffer( buf ), yield_ctx[ ec ] );
                    if ( !ec )
                    {
                        if ( szBytesTransferred == MessageSizeSize )
                        {
                            size = *reinterpret_cast< const network::MessageSize* >( buf.data() );
                            break;
                        }
                        else
                        {
                            SPDLOG_ERROR( "Socket: {} error reading message size", m_connectionID );
                            m_bContinue = false;
                        }
                    }
                    else // if( ec.failed() )
                    {
                        m_bContinue = false;
                        onError( ec );
                    }
                }
            }

            // read message
            if ( m_bContinue && m_socket.is_open() )
            {
                buffer.resize( size );
                szBytesTransferred
                    = boost::asio::async_read( m_socket, boost::asio::buffer( buffer ), yield_ctx[ ec ] );
                if ( !ec )
                {
                    VERIFY_RTE( size == szBytesTransferred );

                    boost::interprocess::basic_vectorbuf< ReceiveBuffer > is( buffer );
                    boost::archive::binary_iarchive                       ia( is );

                    const auto        msg = decode( ia );
                    const ReceivedMsg receivedMsg{ m_connectionID, std::move( msg ) };
                    m_conversationManager.dispatch( receivedMsg );
                }
                else // if( ec.failed() )
                {
                    m_bContinue = false;
                    onError( ec );
                }
            }
        }
    }
    m_disconnectHandler();
}

ConcurrentChannelReceiver::ConcurrentChannelReceiver( ConversationManager& conversationManager,
                                                      ConcurrentChannel&   channel )
    : m_conversationManager( conversationManager )
    , m_channel( channel )
{
}

ConcurrentChannelReceiver::~ConcurrentChannelReceiver() { m_bContinue = false; }

void ConcurrentChannelReceiver::onError( const boost::system::error_code& ec )
{
    if ( ec == boost::asio::error::eof )
    {
        //  This is what happens when close socket normally
    }
    else if ( ec == boost::asio::error::operation_aborted )
    {
        //  This is what happens when close socket normally
    }
    else if ( ec == boost::asio::experimental::error::channel_closed )
    {
    }
    else if ( ec == boost::asio::experimental::error::channel_cancelled )
    {
    }
    else
    {
        SPDLOG_ERROR( "ConcurrentChannelReceiver: Connection: {} closed. Error: {}", m_connectionID, ec.what() );
    }
}

void ConcurrentChannelReceiver::receive( boost::asio::yield_context& yield_ctx )
{
    ChannelMsg                msg;
    ReceivedMsg               receivedMsg;
    boost::system::error_code ec;
    if ( m_bContinue && m_channel.is_open() )
    {
        while ( m_bContinue && m_channel.is_open() )
        {
            msg = m_channel.async_receive( yield_ctx[ ec ] );
            if ( !ec )
            {
                receivedMsg = ReceivedMsg{ m_connectionID, msg };
                m_conversationManager.dispatch( receivedMsg );
            }
            else
            {
                m_bContinue = false;
                onError( ec );
            }
        }
    }
}

} // namespace network
} // namespace mega
