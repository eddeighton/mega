
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

namespace mega::service::python
{

PythonRequestLogicalThread::PythonRequestLogicalThread( Python& python, const network::LogicalThreadID& logicalthreadID )
    : InThreadLogicalThread( python, logicalthreadID )
    , m_python( python )
{
}
PythonRequestLogicalThread::~PythonRequestLogicalThread()
{
}

network::Message PythonRequestLogicalThread::dispatchRequest( const network::Message&     msg,
                                                             boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::leaf_python::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::python_leaf::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::mpo::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::project::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::status::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "PythonRequestLogicalThread::dispatchRequest failed: " << msg );
}

network::python_leaf::Request_Sender
PythonRequestLogicalThread::getPythonRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_python.getLeafSender(), yield_ctx };
}

network::Message PythonRequestLogicalThread::RootAllBroadcast( const network::Message&     request,
                                                              boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

network::Message PythonRequestLogicalThread::PythonRoot( const network::Message&     request,
                                                        boost::asio::yield_context& yield_ctx )
{
    return getPythonRequest( yield_ctx ).PythonRoot( request );
}
network::Message PythonRequestLogicalThread::PythonDaemon( const network::Message&     request,
                                                          boost::asio::yield_context& yield_ctx )
{
    return getPythonRequest( yield_ctx ).PythonDaemon( request );
}

network::Message PythonRequestLogicalThread::MPRoot( const network::Message& request, const mega::MP& mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    network::mpo::Request_Sender rq{ *this, m_python.getLeafSender(), yield_ctx };
    return rq.MPRoot( request, mp );
}

network::Message PythonRequestLogicalThread::MPUp( const network::Message& request, const mega::MP& mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    network::mpo::Request_Sender rq{ *this, m_python.getLeafSender(), yield_ctx };
    return rq.MPUp( request, mp );
}

network::Message PythonRequestLogicalThread::MPDown( const network::Message& request, const mega::MP& mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE( mega::MP( m_python.getMPO() ) == mp );
    return dispatchRequest( request, yield_ctx );
}

network::Message PythonRequestLogicalThread::MPODown( const network::Message& request, const mega::MPO& mpo,
                                                     boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE( mega::MPO( m_python.getMPO() ) == mpo );
    return dispatchRequest( request, yield_ctx );
}

} // namespace mega::service::python
