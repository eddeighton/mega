
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

#include "python.hpp"

#include "mpo_logical_thread.hpp"

#include "service/mpo_context.hpp"

#include "service/network/logical_thread.hpp"
#include "service/network/logical_thread_manager.hpp"
#include "log/log.hpp"

#include "mega/values/service/logical_thread_id.hpp"

#include "python.hpp"
#include "request.hpp"

#include "event/file_log.hpp"

#include <spdlog/spdlog.h>

#include <boost/filesystem.hpp>

#include <thread>

namespace mega::service::python
{

Python::Python( boost::asio::io_context& io_context, network::Log log, short daemonPortNumber )
    : network::LogicalThreadManager( network::Node::makeProcessName( network::Node::Python ), io_context )
    , m_log( log )
    , m_io_context( io_context )
    , m_receiverChannel( m_io_context, *this )
    , m_host( m_receiverChannel.getSender(), network::Node::Python, daemonPortNumber )
{
    m_receiverChannel.run( m_host.getLeafSender() );
    m_host.startup();
}

Python::~Python()
{
    m_receiverChannel.stop();
}

void Python::shutdown()
{
    // TODO ?
}

network::LogicalThreadBase::Ptr Python::joinLogicalThread( const network::Message& msg )
{
    return network::LogicalThreadBase::Ptr( new PythonRequestLogicalThread( *this, msg.getLogicalThreadID() ) );
}

} // namespace mega::service::python
