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


#include "service/protocol/common/conversation_base.hpp"

#include "service/network/log.hpp"

namespace mega::network
{

ConversationBase::RequestStack::RequestStack( const char* pszMsg, ConversationBase::Ptr pConversation,
                                              const ConnectionID& connectionID )
    : m_pszMsg( pszMsg )
    //, m_startTime( std::chrono::steady_clock::now() )
    , pConversation( pConversation )
{
    // SPDLOG_DEBUG( "RequestStack::ctor prc:{} conv:{} conid:{} msg:{} stack:{}", pConversation->getProcessName(), pConversation->getID(), connectionID, m_pszMsg, pConversation->getStackSize() + 1 );
    pConversation->requestStarted( connectionID );
}
ConversationBase::RequestStack::~RequestStack()
{
    // SPDLOG_DEBUG( "RequestStack::dtor prc:{} conv:{} msg:{} stack:{}", pConversation->getProcessName(), pConversation->getID(), m_pszMsg, pConversation->getStackSize() );
    // const auto timeDelta = std::chrono::steady_clock::now() - m_startTime;
    // SPDLOG_DEBUG( "{} {} {} {}", conversation.getProcessName(), conversation.getID(), m_pszMsg, timeDelta );
    pConversation->requestCompleted();
}

}
