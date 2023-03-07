
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

#ifndef GUARD_2023_March_07_steamWorks
#define GUARD_2023_March_07_steamWorks

#include "service/executor/request.hpp"

#include "service/protocol/common/conversation_id.hpp"

#include "service/protocol/model/steam.hxx"

namespace mega::service
{

class Executor;

class SteamWorks : public ExecutorRequestConversation, public network::steam::Impl
{
public:
    using Ptr = std::shared_ptr< SteamWorks >;

    SteamWorks( Executor& executor, const network::ConversationID& conversationID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;

    virtual void run( boost::asio::yield_context& yield_ctx ) override;

    //void sendSteamDestroyToSelf();

    // network::steam::Impl
    virtual void SteamDestroy( boost::asio::yield_context& yield_ctx ) override;

private:
    boost::asio::yield_context* m_pYieldContext = nullptr;
    network::Sender::Ptr        m_pRequestChannelSender;

    std::vector< network::ReceivedMsg > m_messageQueue;
    bool                                m_bRunning = true;
};

} // namespace mega::service

#endif // GUARD_2023_March_07_steamWorks
