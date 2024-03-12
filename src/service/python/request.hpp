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

#include "python.hpp"

#include "service/network/logical_thread.hpp"

#include "service/protocol/model/leaf_python.hxx"
#include "service/protocol/model/python_leaf.hxx"
#include "service/protocol/model/python_leaf.hxx"
#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/status.hxx"
#include "service/protocol/model/report.hxx"
#include "service/protocol/model/project.hxx"
#include "service/protocol/model/python.hxx"

namespace mega::service::python
{

class PythonRequestLogicalThread : public network::InThreadLogicalThread,
                                   public network::leaf_python::Impl,
                                   public network::python_leaf::Impl,
                                   public network::mpo::Impl,
                                   public network::status::Impl,
                                   public network::report::Impl,
                                   public network::project::Impl
{
protected:
    Python& m_python;

public:
    PythonRequestLogicalThread( Python& python, const network::LogicalThreadID& logicalthreadID );
    virtual ~PythonRequestLogicalThread();

    virtual network::Message dispatchInBoundRequest( const network::Message&     msg,
                                                     boost::asio::yield_context& yield_ctx ) override;

    network::python_leaf::Request_Sender getPythonRequest( boost::asio::yield_context& yield_ctx );

    template < typename RequestEncoderType >
    RequestEncoderType getRootRequest( boost::asio::yield_context& yield_ctx )
    {
        return RequestEncoderType( [ rootRequest = getPythonRequest( yield_ctx ) ](
                                       const network::Message& msg ) mutable { return rootRequest.PythonRoot( msg ); },
                                   getID() );
    }

    template < typename RequestEncoderType >
    RequestEncoderType getDaemonRequest( boost::asio::yield_context& yield_ctx )
    {
        return RequestEncoderType(
            [ rootRequest = getPythonRequest( yield_ctx ) ]( const network::Message& msg ) mutable
            { return rootRequest.PythonDaemon( msg ); },
            getID() );
    }

    // network::leaf_python::Impl
    virtual network::Message RootAllBroadcast( const network::Message&     request,
                                               boost::asio::yield_context& yield_ctx ) override;

    // network::python_leaf::Impl
    virtual network::Message PythonRoot( const network::Message&     request,
                                         boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message PythonDaemon( const network::Message&     request,
                                           boost::asio::yield_context& yield_ctx ) override;

    // network::mpo::Impl
    virtual network::Message MPRoot( const network::Message& request, const mega::runtime::MP& mp,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPUp( const network::Message& request, const mega::runtime::MP& mp,
                                   boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPOUp( const network::Message& request, const mega::runtime::MPO& mpo,
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
    virtual Report GetReport( const URL&    url,
                              const std::vector< Report >& report,
                              boost::asio::yield_context&  yield_ctx ) override;
};

} // namespace mega::service::python

#endif // REQUEST_PYTHON_9_MAR_2023
