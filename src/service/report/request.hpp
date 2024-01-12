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

#ifndef REQUEST_PYTHON_9_MAR_2023
#define REQUEST_PYTHON_9_MAR_2023

#include "report.hpp"

#include "service/network/logical_thread.hpp"

#include "service/protocol/model/leaf_report.hxx"
#include "service/protocol/model/report_leaf.hxx"

#include "service/protocol/model/report.hxx"

#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/status.hxx"
#include "service/protocol/model/project.hxx"

namespace mega::service::report
{

class ReportRequestLogicalThread : public network::InThreadLogicalThread,
                                   public network::leaf_report::Impl,
                                   public network::report_leaf::Impl,
                                   public network::mpo::Impl,
                                   public network::status::Impl,
                                   public network::project::Impl,
                                   public network::report::Impl
{
protected:
    Report& m_report;

public:
    ReportRequestLogicalThread( Report& report, const network::LogicalThreadID& logicalthreadID );
    virtual ~ReportRequestLogicalThread();

    virtual network::Message dispatchInBoundRequest( const network::Message&     msg,
                                                     boost::asio::yield_context& yield_ctx ) override;

    network::report_leaf::Request_Sender getReportRequest( boost::asio::yield_context& yield_ctx );

    template < typename RequestEncoderType >
    RequestEncoderType getRootRequest( boost::asio::yield_context& yield_ctx )
    {
        return RequestEncoderType( [ rootRequest = getReportRequest( yield_ctx ) ](
                                       const network::Message& msg ) mutable { return rootRequest.ReportRoot( msg ); },
                                   getID() );
    }

    template < typename RequestEncoderType >
    RequestEncoderType getDaemonRequest( boost::asio::yield_context& yield_ctx )
    {
        return RequestEncoderType(
            [ rootRequest = getReportRequest( yield_ctx ) ]( const network::Message& msg ) mutable
            { return rootRequest.ReportDaemon( msg ); },
            getID() );
    }

    // network::leaf_report::Impl
    virtual network::Message RootAllBroadcast( const network::Message&     request,
                                               boost::asio::yield_context& yield_ctx ) override;

    // network::report_leaf::Impl
    virtual network::Message ReportRoot( const network::Message&     request,
                                         boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message ReportDaemon( const network::Message&     request,
                                           boost::asio::yield_context& yield_ctx ) override;

    // network::mpo::Impl
    virtual network::Message MPRoot( const network::Message& request, const mega::runtime::MP& mp,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPUp( const network::Message& request, const mega::runtime::MP& mp,
                                   boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPDown( const network::Message& request, const mega::runtime::MP& mp,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPODown( const network::Message& request, const mega::runtime::MPO& mpo,
                                      boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& childNodeStatus,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual std::string     Ping( const std::string& strMsg, boost::asio::yield_context& yield_ctx ) override;

    // network::report::Impl
    virtual mega::reports::Container GetReport( const mega::reports::URL&                      url,
                                                const std::vector< mega::reports::Container >& report,
                                                boost::asio::yield_context&                    yield_ctx ) override;
};

} // namespace mega::service::report

#endif // REQUEST_PYTHON_9_MAR_2023
