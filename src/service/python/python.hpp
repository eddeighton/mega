
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

#ifndef GUARD_2023_March_09_python
#define GUARD_2023_March_09_python

#include "service/leaf.hpp"

#include "service/network/client.hpp"
#include "service/network/conversation_manager.hpp"
#include "service/network/sender_factory.hpp"
#include "service/network/channel.hpp"

#include <boost/asio/io_service.hpp>

#include <memory>
#include <vector>
#include <thread>
#include <functional>

namespace mega::service
{

class Python : public network::ConversationManager
{
    friend class PythonRequestConversation;

public:
    Python( boost::asio::io_context& io_context, short daemonPortNumber );
    ~Python();

    void shutdown();

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID& originatingConnectionID,
                                                             const network::Message&      msg );

    network::Sender& getLeafSender() { return m_leaf; }

    MPO getMPO() const
    {
        VERIFY_RTE( m_mpo.has_value() );
        return m_mpo.value();
    }
    void setMPO( MPO mpo ) { m_mpo = mpo; }

private:
    boost::asio::io_context&   m_io_context;
    network::ReceiverChannel   m_receiverChannel;
    Leaf                       m_leaf;
    std::optional< mega::MPO > m_mpo;
};
} // namespace mega::service

#endif // GUARD_2023_March_09_python
