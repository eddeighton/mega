
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

#include "service/plugin/plugin.hpp"

#include <memory>

#include "service/network/network.hpp"
#include "service/network/log.hpp"

#include "service/protocol/model/project.hxx"
#include "service/protocol/model/sim.hxx"

#include <chrono>

namespace mega::service
{

Plugin::Plugin( boost::asio::io_context& ioContext, network::Log log, U64 uiNumThreads )
    : m_channel( ioContext )
    , m_executor( ioContext, log, uiNumThreads, mega::network::MegaDaemonPort(), *this, network::Node::Plugin )
    , m_stateMachine( *this )
{
    SPDLOG_TRACE( "Plugin::Plugin()" );
}

Plugin::~Plugin()
{
    SPDLOG_TRACE( "Plugin::~Plugin()" );
}

// ProcessClock
void Plugin::registerMPO( network::SenderRef sender )
{
    using namespace network::sim;
    send( MSG_SimRegister_Request::make( m_logicalThreadID, MSG_SimRegister_Request{ std::move( sender ) } ) );
}

void Plugin::unregisterMPO( network::SenderRef sender )
{
    using namespace network::sim;
    send( MSG_SimUnregister_Request::make( m_logicalThreadID, MSG_SimUnregister_Request{ sender.m_mpo } ) );
}

void Plugin::requestClock( network::LogicalThreadBase* pSender, MPO mpo, log::Range range )
{
    using namespace network::sim;
    send( MSG_SimClock_Request::make( m_logicalThreadID, MSG_SimClock_Request{ mpo, std::move( range ) } ) );
}

bool Plugin::unrequestClock( network::LogicalThreadBase* pSender, MPO mpo )
{
    THROW_TODO;
    using namespace network::sim;
    // send( MSG_SimClock_Request::make( m_logicalThreadID, MSG_SimClock_Request{ mpo, std::move( range ) } ) );
}

void Plugin::requestMove( network::LogicalThreadBase* pSender, MPO mpo )
{
    using namespace network::sim;
    THROW_TODO;
}

void Plugin::send( const network::Message& msg )
{
    // SPDLOG_TRACE( "Plugin::send" );
    m_channel.async_send( boost::system::error_code(), msg,
                          [ &msg ]( boost::system::error_code ec )
                          {
                              if( ec )
                              {
                                  if( ec.value() == boost::asio::error::eof )
                                  {
                                  }
                                  else if( ec.value() == boost::asio::error::operation_aborted )
                                  {
                                  }
                                  else if( ec.value() == boost::asio::experimental::error::channel_closed )
                                  {
                                  }
                                  else if( ec.value() == boost::asio::experimental::error::channel_cancelled )
                                  {
                                  }
                                  else if( ec.failed() )
                                  {
                                      SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg, ec.what() );
                                      THROW_RTE( "Failed to send request on channel: " << msg << " : " << ec.what() );
                                  }
                              }
                          } );
}

void Plugin::runOne()
{
    if( !tryRun() )
    {
        std::promise< network::Message > pro;
        std::future< network::Message >  fut = pro.get_future();
        m_channel.async_receive(
            [ &pro ]( boost::system::error_code ec, const network::Message& msg )
            {
                if( ec )
                {
                    SPDLOG_ERROR( "Failed to receive msg with error: {}", ec.what() );
                    pro.set_exception( std::make_exception_ptr( std::runtime_error( ec.what() ) ) );
                }
                else
                {
                    pro.set_value( msg );
                }
            } );

        dispatch( fut.get() );
    }
}

bool Plugin::tryRun()
{
    bool bDispatchedMsg = false;
    while( true )
    {
        std::optional< network::Message > result;
        m_channel.try_receive(
            [ &optMsg = result ]( boost::system::error_code ec, const network::Message& msg )
            {
                if( !ec )
                {
                    optMsg = msg;
                }
                else
                {
                    THROW_TODO;
                }
            } );
        if( result.has_value() )
        {
            dispatch( result.value() );
            bDispatchedMsg = true;
        }
        else
        {
            break;
        }
    }
    return bDispatchedMsg;
}

void Plugin::dispatch( const network::Message& msg )
{
    using namespace network::project;
    using namespace network::sim;

    switch( msg.getID() )
    {
        // simulation clock
        case MSG_SimRegister_Request::ID:
        {
            const network::sim::MSG_SimRegister_Request& msg_ = MSG_SimRegister_Request::get( msg );
            m_stateMachine.simRegister( msg_ );
        }
        break;
        case MSG_SimUnregister_Request::ID:
        {
            const auto& msg_ = MSG_SimUnregister_Request::get( msg );
            m_stateMachine.simUnregister( msg_ );
        }
        break;
        case MSG_SimClock_Request::ID:
        {
            m_stateMachine.simClock( MSG_SimClock_Request::get( msg ) );
        }
        break;

        // project
        // case MSG_SetUnityProject_Request::ID:
        // {
        //     const MSG_SetUnityProject_Request& projectMsg = MSG_SetUnityProject_Request::get( msg );
        //     SPDLOG_INFO( "plugin::dispatch: Set project request received for project: {} {}",
        //                  projectMsg.project.getProjectInstallPath().string(),
        //                  projectMsg.dbHash );
        //     m_strDatabasePath  = projectMsg.project.getProjectUnityDatabase().string();
        //     m_databaseHashcode = projectMsg.dbHash;
        // }
        // break;

        // errors
        case network::MSG_Error_Response::ID:
        {
            SPDLOG_ERROR( "{} {}", msg, network::MSG_Error_Response::get( msg ).what );
            THROW_RTE( network::MSG_Error_Response::get( msg ).what );
        }
        break;
        default:
            SPDLOG_ERROR( "Unhandled message in Plugin::dispatch: {}", msg.getName() );
            // THROW_RTE( "plugin::dispatch Unsupported msg type: " << msg.getName() );
            break;
    }
}

} // namespace mega::service
