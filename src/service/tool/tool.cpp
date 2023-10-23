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

#include "service/tool.hpp"

#include "request.hpp"
#include "tool_logical_thread.hpp"

#include "service/mpo_context.hpp"

#include "service/network/logical_thread.hpp"
#include "service/network/logical_thread_manager.hpp"
#include "service/network/log.hpp"

#include "mega/values/service/logical_thread_id.hpp"

#include "service/protocol/model/project.hxx"

#include "log/file_log.hpp"

#include "common/requireSemicolon.hpp"

#include <spdlog/spdlog.h>
#include <boost/filesystem.hpp>

#include <thread>

namespace mega::service
{

Tool::Tool( short daemonPortNumber, network::Log log )
    : network::LogicalThreadManager( network::Node::makeProcessName( network::Node::Tool ), m_io_context )
    , m_log( log )
    , m_receiverChannel( m_io_context, *this )
    , m_leaf( m_receiverChannel.getSender(), network::Node::Tool, daemonPortNumber )
{
    m_receiverChannel.run( m_leaf.getLeafSender() );
    m_leaf.startup();
}

Tool::~Tool()
{
    m_receiverChannel.stop();
    m_io_context.run();
}

void Tool::shutdown()
{
    // TODO ?
}
void Tool::runComplete()
{
    m_receiverChannel.stop();
}

network::LogicalThreadBase::Ptr Tool::joinLogicalThread( const network::Message& msg )
{
    return network::LogicalThreadBase::Ptr( new ToolRequestLogicalThread( *this, msg.getLogicalThreadID() ) );
}

void Tool::run( Tool::Functor& function )
{
    std::optional< std::variant< int, std::exception_ptr > > exceptionResult;
    {
        auto func = [ &exceptionResult, &function ]( boost::asio::yield_context& yield_ctx )
        {
            try
            {
                function( yield_ctx );
                exceptionResult = 0;
            }
            catch( std::exception& ex )
            {
                exceptionResult = std::current_exception();
            }
        };
        logicalthreadInitiated(
            std::make_shared< ToolMPOLogicalThread >( *this, createLogicalThreadID(), std::move( func ) ) );
    }

    // run until m_tool.runComplete();
    m_io_context.run();

    if( exceptionResult->index() == 1 )
        std::rethrow_exception( std::get< std::exception_ptr >( exceptionResult.value() ) );
}

} // namespace mega::service
