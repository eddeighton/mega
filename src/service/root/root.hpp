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

#ifndef ROOT_26_MAY_2022
#define ROOT_26_MAY_2022

#include "mpo_manager.hpp"
#include "logical_address_space.hpp"

#include "service/network/server.hpp"
#include "service/network/conversation_manager.hpp"

#include "service/protocol/common/megastructure_installation.hpp"
#include "service/protocol/common/root_config.hpp"

#include "common/stash.hpp"

#include <boost/asio/io_context.hpp>

namespace mega::service
{

class Root : public network::ConversationManager
{
    friend class RootPipelineConversation;
    friend class RootRequestConversation;
    friend class RootSimulation;

public:
    Root( boost::asio::io_context& ioContext, const boost::filesystem::path& stashFolder, short portNumber );
    void shutdown();

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID& originatingConnectionID,
                                                             const network::Message&      msg );

    network::MegastructureInstallation getMegastructureInstallation();
    const network::Project&            getProject() const { return m_config.getProject(); }
    void                               setProject( const network::Project& project ) { m_config.setProject( project ); }

private:
    void loadConfig();
    void saveConfig();

    void onDaemonDisconnect( const network::ConnectionID& connectionID, mega::MP mp );

private:
    network::Server                                     m_server;
    task::BuildHashCodes                                m_buildHashCodes;
    task::Stash                                         m_stash;
    mega::network::RootConfig                           m_config;
    std::optional< network::MegastructureInstallation > m_megastructureInstallationOpt;
    MPOManager                                          m_mpoManager;
    LogicalAddressSpace                                 m_logicalAddressSpace;
};

} // namespace mega::service

#endif // ROOT_26_MAY_2022
