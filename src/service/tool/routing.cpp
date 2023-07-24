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

#include "request.hpp"

#include "service/network/log.hpp"

namespace mega::service
{

ToolRequestLogicalThread::ToolRequestLogicalThread( Tool& tool, const network::LogicalThreadID& logicalthreadID )
    : InThreadLogicalThread( tool, logicalthreadID )
    , m_tool( tool )
{
}
ToolRequestLogicalThread::~ToolRequestLogicalThread()
{
}

network::Message ToolRequestLogicalThread::dispatchRequest( const network::Message&     msg,
                                                            boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::leaf_tool::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::mpo::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::status::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::project::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "ToolRequestLogicalThread::dispatchRequest failed: " << msg );
}

network::tool_leaf::Request_Sender ToolRequestLogicalThread::getToolRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_tool.getLeafSender(), yield_ctx };
}

network::Message ToolRequestLogicalThread::RootAllBroadcast( const network::Message&     request,
                                                             boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

network::Message ToolRequestLogicalThread::MPDown( const network::Message& request, const mega::MP& mp,
                                                   boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE( MP( m_tool.getMPO() ) == mp );
    return dispatchRequest( request, yield_ctx );
}

network::Message ToolRequestLogicalThread::MPODown( const network::Message& request, const mega::MPO& mpo,
                                                    boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE( MPO( m_tool.getMPO() ) == mpo );
    return dispatchRequest( request, yield_ctx );
}

} // namespace mega::service
