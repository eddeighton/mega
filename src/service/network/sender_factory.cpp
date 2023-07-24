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

#include "service/network/end_point.hpp"
#include "service/network/log.hpp"
#include "service/network/network.hpp"

#include "service/protocol/common/logical_thread_id.hpp"
#include "service/protocol/common/sender.hpp"

#include "service/protocol/model/messages.hxx"

#include "common/assert_verify.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/experimental/channel_error.hpp>
#include <boost/asio/write.hpp>

#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/streams/vectorstream.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <algorithm>
#include <sstream>
#include <utility>

namespace mega::network
{

Sender::~Sender() = default;

class SocketSender : public Sender
{
    using SendBuffer = std::vector< char >;

    Traits::Socket& m_socket;

public:
    SocketSender( Traits::Socket& socket )
        : m_socket( socket )
    {
    }

    virtual ~SocketSender() = default;

    virtual boost::system::error_code send( const Message& msg )
    {
        THROW_RTE( "non async send called on SocketSender" );
        UNREACHABLE;
    }

    virtual boost::system::error_code send( const Message& msg, boost::asio::yield_context& yield_ctx )
    {
        boost::interprocess::basic_vectorbuf< SendBuffer > m_os;
        SendBuffer                                         m_buffer;
        {
            encode( m_os, msg );
            const MessageSize size = m_os.vector().size();
            std::string_view  sizeView( reinterpret_cast< const char* >( &size ), sizeof( MessageSize ) );
            m_buffer.reserve( size + sizeof( MessageSize ) );
            std::copy( sizeView.begin(), sizeView.end(), std::back_inserter( m_buffer ) );
            std::copy( m_os.vector().begin(), m_os.vector().end(), std::back_inserter( m_buffer ) );
        }

        boost::system::error_code ec;
        {
            const mega::U64 szBytesWritten
                = boost::asio::async_write( m_socket, boost::asio::buffer( m_buffer ), yield_ctx[ ec ] );
            if( !ec )
            {
                VERIFY_RTE( szBytesWritten == m_buffer.size() );
            }
            return ec;
        }
    }
    
};

Sender::Ptr make_socket_sender( Traits::Socket& socket )
{
    return std::make_shared< SocketSender >( socket );
}

class ConcurrentChannelSender : public Sender
{
    ConcurrentChannel& m_channel;

public:
    ConcurrentChannelSender( ConcurrentChannel& channel )
        : m_channel( channel )
    {
    }

    virtual ~ConcurrentChannelSender() = default;

    virtual boost::system::error_code send( const Message& msg )
    {
        THROW_RTE( "non async send called on ConcurrentChannelSender" );
        UNREACHABLE;
    }

    virtual boost::system::error_code send( const Message& msg, boost::asio::yield_context& yield_ctx )
    {
        boost::system::error_code ec;

        if( msg.getID() == network::MSG_Error_Response::ID && !m_channel.is_open() )
        {
            SPDLOG_ERROR( "Failed to send error response: {} due to channel closed", msg );
            return ec;
        }

        VERIFY_RTE_MSG( m_channel.is_open(), "Channel NOT open sending:" << msg );
        m_channel.async_send( ec, msg, yield_ctx );

        if( ec != boost::system::error_code() )
        {
            if( ( ec.value() != boost::asio::experimental::error::channel_cancelled )
                && ( ec.value() != boost::asio::experimental::error::channel_closed ) )
            {
                SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg, ec.what() );
                THROW_RTE( "Failed to send request on channel: " << msg << " : " << ec.what() );
            }
            else
            {
                SPDLOG_ERROR( "Failed to send request due to channel closed: {}", ec.what() );
                THROW_RTE( "Failed to send request due to channel closed: " << ec.what() );
            }
        }
        return ec;
    }
    
};

Sender::Ptr make_concurrent_channel_sender( ConcurrentChannel& channel )
{
    return std::make_shared< ConcurrentChannelSender >( channel );
}

class ChannelSender : public Sender
{
    Channel&     m_channel;

public:
    ChannelSender( Channel& channel )
        : m_channel( channel )
    {
    }

    virtual ~ChannelSender() = default;

    virtual boost::system::error_code send( const Message& msg )
    {
        THROW_RTE( "non async send called on ChannelSender" );
        UNREACHABLE;
    }
    
    virtual boost::system::error_code send( const Message& msg, boost::asio::yield_context& yield_ctx )
    {
        boost::system::error_code ec;
        VERIFY_RTE_MSG( m_channel.is_open(), "Channel NOT open" );
        m_channel.async_send( ec, msg, yield_ctx );

        if( ec != boost::system::error_code() )
        {
            if( ( ec.value() != boost::asio::experimental::error::channel_cancelled )
                && ( ec.value() != boost::asio::experimental::error::channel_closed ) )
            {
                SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg, ec.what() );
                THROW_RTE( "Failed to send request on channel: " << msg << " : " << ec.what() );
            }
            else
            {
                SPDLOG_ERROR( "Failed to send request due to channel closed: {}", ec.what() );
                THROW_RTE( "Failed to send request due to channel closed: " << ec.what() );
            }
        }

        return ec;
    }
    
};

Sender::Ptr make_channel_sender( Channel& channel )
{
    return std::make_shared< ChannelSender >( channel );
}

} // namespace mega::network
