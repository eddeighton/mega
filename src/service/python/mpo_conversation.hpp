
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

#ifndef GUARD_2023_March_09_mpo_conversation
#define GUARD_2023_March_09_mpo_conversation

#include "request.hpp"

#include "service/mpo_context.hpp"

namespace mega::service::python
{

class MPOConversation : public PythonRequestConversation, public network::python::Impl, public mega::MPOContext
{
public:
    MPOConversation( Python& python, const network::ConversationID& conversationID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx ) override;

    network::python_leaf::Request_Sender     getPythonRequest( boost::asio::yield_context& yield_ctx );
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

    // network::python::Impl
    virtual std::unordered_map< std::string, mega::TypeID >
                 PythonGetIdentities( boost::asio::yield_context& yield_ctx ) override;
    virtual void PythonExecuteJIT( const mega::runtime::JITFunctor& func,
                                   boost::asio::yield_context&      yield_ctx ) override;

    void         run( boost::asio::yield_context& yield_ctx ) override;
    virtual void RootSimRun( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override;

    // mega::MPOContext
    // clock
    virtual TimeStamp cycle() override { return TimeStamp{}; }
    virtual F32       ct() override { return F32{}; }
    virtual F32       dt() override { return F32{}; }

private:
    bool                                m_bRunning = true;
    Python&                             m_python;
    std::vector< network::ReceivedMsg > m_messageQueue;
};
} // namespace mega::service::python

#endif // GUARD_2023_March_09_mpo_conversation
