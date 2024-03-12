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

#ifndef TERMINAL_REQUEST_23_SEPT_2022
#define TERMINAL_REQUEST_23_SEPT_2022

#include "service/terminal.hpp"

#include "service/protocol/model/leaf_term.hxx"
#include "service/protocol/model/status.hxx"
#include "service/protocol/model/report.hxx"
#include "service/protocol/model/project.hxx"

namespace mega::service
{

class TerminalRequestLogicalThread : public network::InThreadLogicalThread,
                                     public network::leaf_term::Impl,
                                     public network::status::Impl,
                                     public network::report::Impl,
                                     public network::project::Impl
{
protected:
    Terminal& m_terminal;

public:
    TerminalRequestLogicalThread( Terminal& terminal, const network::LogicalThreadID& logicalthreadID );
    virtual ~TerminalRequestLogicalThread();

    virtual network::Message dispatchInBoundRequest( const network::Message&     msg,
                                                     boost::asio::yield_context& yield_ctx ) override;

    // network::leaf_term::Impl
    virtual network::Message RootAllBroadcast( const network::Message&     request,
                                               boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual std::string     Ping( const std::string& strMsg, boost::asio::yield_context& yield_ctx ) override;

    // network::report::Impl
    virtual Report GetReport( const URL&    url,
                              const std::vector< Report >& report,
                              boost::asio::yield_context&  yield_ctx ) override;
};

} // namespace mega::service

#endif // TERMINAL_REQUEST_23_SEPT_2022
