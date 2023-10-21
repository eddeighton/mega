
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

#ifndef GUARD_2023_March_09_mpo_logicalthread
#define GUARD_2023_March_09_mpo_logicalthread

#include "request.hpp"

#include "service/mpo_context.hpp"

#include "service/protocol/model/report.hxx"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <boost/asio/ip/tcp.hpp>

#include <memory>

namespace mega::service::report
{

class MPOLogicalThread : public ReportRequestLogicalThread, public network::report::Impl, public mega::MPOContext
{
public:
    using Ptr = std::shared_ptr< MPOLogicalThread >;

    MPOLogicalThread( Report&                         report,
                      const network::LogicalThreadID& logicalthreadID,
                      boost::asio::ip::tcp::socket&   socket );

    virtual network::Message dispatchInBoundRequest( const network::Message&     msg,
                                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message dispatchInBoundRequestsUntilResponse( boost::asio::yield_context& yield_ctx ) override;

    network::report_leaf::Request_Sender     getReportRequest( boost::asio::yield_context& yield_ctx );
    network::mpo::Request_Sender             getMPRequest( boost::asio::yield_context& yield_ctx );
    virtual network::enrole::Request_Encoder getRootEnroleRequest() override;
    virtual network::stash::Request_Encoder  getRootStashRequest() override;
    virtual network::memory::Request_Encoder getDaemonMemoryRequest() override;
    virtual network::sim::Request_Encoder    getMPOSimRequest( mega::MPO mpo ) override;
    virtual network::memory::Request_Sender  getLeafMemoryRequest() override;
    virtual network::jit::Request_Sender     getLeafJITRequest() override;
    virtual network::mpo::Request_Sender     getMPRequest() override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& childNodeStatus,
                                       boost::asio::yield_context&           yield_ctx ) override;

    // network::report::Impl
    mega::network::HTTPRequestData HTTPRequest( boost::asio::yield_context& ) override;

    void run( boost::asio::yield_context& yield_ctx ) override;

    virtual void
    RootSimRun( const Project& project, const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override;

    bool isRunning() const { return m_bRunning; }
    bool isRunComplete() const { return m_bRunComplete; }
    void stopRunning() { m_bRunning = false; }

private:
    enum HTTPVerbType
    {
        eClose,
        eError,
        eGet,
        eHead,
        ePost,
        TOTAL_HTTP_VERBS
    };
    using HTTPRequestMsg = network::report::MSG_HTTPRequest_Response;
    void startTCPStream();
    boost::beast::http::message_generator handleHTTPRequest( const network::HTTPRequestData& msg,
                                                             boost::asio::yield_context&     yield_ctx );

private:
    bool                                    m_bRunning = false;
    Report&                                 m_report;
    boost::beast::tcp_stream                m_tcpStream;
    std::vector< network::ReceivedMessage > m_messageQueue;
    bool                                    m_bRunComplete = false;
};
} // namespace mega::service::report

#endif // GUARD_2023_March_09_mpo_logicalthread
