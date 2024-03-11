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

#include "service/tool.hpp"

#include "service/mpo_context.hpp"

#include "service/network/logical_thread.hpp"
#include "service/network/logical_thread_manager.hpp"
#include "log/log.hpp"

#include "mega/values/service/logical_thread_id.hpp"

#include "event/file_log.hpp"

namespace mega::service
{

class ToolMPOLogicalThread : public ToolRequestLogicalThread, public runtime::MPOContext
{
    Tool&         m_tool;
    Tool::Functor m_functor;

public:
    ToolMPOLogicalThread( Tool& tool, const network::LogicalThreadID& logicalthreadID, Tool::Functor&& functor );

    virtual network::Message dispatchInBoundRequest( const network::Message&     msg,
                                                     boost::asio::yield_context& yield_ctx ) override;

    network::tool_leaf::Request_Sender getToolRequest( boost::asio::yield_context& yield_ctx );
    network::mpo::Request_Sender       getMPRequest( boost::asio::yield_context& yield_ctx );

    virtual network::enrole::Request_Encoder getRootEnroleRequest() override;
    virtual network::stash::Request_Encoder  getRootStashRequest() override;
    virtual network::memory::Request_Encoder getDaemonMemoryRequest() override;
    virtual network::sim::Request_Encoder    getMPOSimRequest( runtime::MPO mpo ) override;
    virtual network::memory::Request_Sender  getLeafMemoryRequest() override;
    virtual network::jit::Request_Sender     getLeafJITRequest() override;
    virtual network::mpo::Request_Sender     getMPRequest() override;

    void         run( boost::asio::yield_context& yield_ctx ) override;
    virtual void RootSimRun( const runtime::MPO& mpo, boost::asio::yield_context& yield_ctx ) override;

    virtual network::Status GetStatus( const std::vector< network::Status >& childNodeStatus,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual Report          GetReport( const report::URL&           url,
                                       const std::vector< Report >& report,
                                       boost::asio::yield_context&  yield_ctx ) override;
};

} // namespace mega::service