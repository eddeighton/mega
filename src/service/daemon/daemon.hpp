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

#ifndef DAEMON_25_MAY_2022
#define DAEMON_25_MAY_2022

#include "shared_memory_manager.hpp"

#include "database/database.hpp"

#include "service/network/client.hpp"
#include "service/network/server.hpp"
#include "service/network/conversation_manager.hpp"

#include "service/network/network.hpp"

#include "mega/common.hpp"

#include <boost/asio/io_context.hpp>

namespace mega::service
{

class Daemon : public network::ConversationManager
{
    friend class DaemonRequestConversation;
    friend class DaemonEnrole;

public:
    Daemon( boost::asio::io_context& ioContext,
            const std::string&       strRootIP,
            short                    rootPortNumber   = mega::network::MegaRootPort(),
            short                    daemonPortNumber = mega::network::MegaDaemonPort() );
    ~Daemon();

    void shutdown();

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID& originatingConnectionID,
                                                             const network::Message&      msg );

    void                                    setActiveProject( const network::Project& project );
    const std::optional< network::Project > getActiveProject() const { return m_activeProject; }

    SharedMemoryManager& getMemoryManager()
    {
        VERIFY_RTE_MSG( m_pMemoryManager.get(), "Memory Manager not initialised" );
        return *m_pMemoryManager.get();
    }

private:
    void onLeafDisconnect( const network::ConnectionID& connectionID, mega::MP mp );

    network::Client                              m_rootClient;
    network::Server                              m_server;
    std::unique_ptr< SharedMemoryManager >       m_pMemoryManager;
    mega::MP                                     m_mp;
    std::optional< network::Project >            m_activeProject;
    std::unique_ptr< runtime::DatabaseInstance > m_pDatabase;
};

} // namespace mega::service

#endif // DAEMON_25_MAY_2022
