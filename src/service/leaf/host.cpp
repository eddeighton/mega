
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

#include "service/host.hpp"

#include "service/leaf/request.hpp"

namespace mega::service
{

Host::Host( network::Sender::Ptr pSender, network::Node nodeType, short daemonPortNumber )
    : Leaf( pSender, nodeType, daemonPortNumber )
{
}

void LeafRequestLogicalThread::SaveSnapshot( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestLogicalThread::SaveSnapshot" );
    THROW_TODO;
}
void LeafRequestLogicalThread::LoadSnapshot( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestLogicalThread::LoadSnapshot" );
    THROW_TODO;
}
void LeafRequestLogicalThread::LoadProgram( const mega::service::Program& program,
                                            boost::asio::yield_context&   yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestLogicalThread::LoadProgram {}", program );
    THROW_TODO;
}
void LeafRequestLogicalThread::UnloadProgram( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestLogicalThread::UnloadProgram" );
    THROW_TODO;
}
mega::service::Program LeafRequestLogicalThread::GetProgram( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "LeafRequestLogicalThread::GetProgram" );
    THROW_TODO;
}

} // namespace mega::service
