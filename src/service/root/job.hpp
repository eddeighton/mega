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

#ifndef ROOT_JOB_23_SEPT_2022
#define ROOT_JOB_23_SEPT_2022

#include "request.hpp"

#include "log/log.hpp"

#include "service/protocol/model/job.hxx"

namespace mega::service
{
class Root;

class RootJobLogicalThread : public RootRequestLogicalThread
{
public:
    RootJobLogicalThread( Root& root, const network::LogicalThreadID& logicalthreadID );

    virtual network::Message dispatchInBoundRequest( const network::Message&     msg,
                                                     boost::asio::yield_context& yield_ctx ) override;

    // network::job::Impl
    virtual void JobReadyForWork( const network::LogicalThreadID& rootLogicalThreadID,
                                  boost::asio::yield_context&     yield_ctx ) override;
    virtual void JobProgress( const std::string& message, boost::asio::yield_context& yield_ctx ) override;
};

} // namespace mega::service

#endif // ROOT_JOB_23_SEPT_2022
