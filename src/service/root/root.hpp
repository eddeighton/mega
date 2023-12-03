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

#include "service/network/server.hpp"
#include "service/network/logical_thread_manager.hpp"
#include "service/network/log.hpp"

#include "reports/report.hpp"

#include "mega/values/compilation/megastructure_installation.hpp"
#include "mega/values/service/root_config.hpp"

#include "common/stash.hpp"

#include <boost/asio/io_context.hpp>

namespace mega::service
{

class Root : public network::LogicalThreadManager
{
    friend class RootPipelineLogicalThread;
    friend class RootRequestLogicalThread;
    friend class RootSimulation;

    using StartupUUIDMap = std::map< std::string, MP >;

public:
    Root( boost::asio::io_context& ioContext, network::Log log, const boost::filesystem::path& stashFolder,
          short portNumber );
    void shutdown();

    // network::LogicalThreadManager
    virtual network::LogicalThreadBase::Ptr joinLogicalThread( const network::Message& msg );

    MegastructureInstallation getMegastructureInstallation();

    void                setStartupUUIDMP( const std::string& strUUID, MP mp );
    std::optional< MP > getAndResetStartupUUID( const std::string& strUUID );

    void getGeneralStatusReport( const mega::reports::URL& url, mega::reports::Branch& report );

private:
    void loadConfig();
    void saveConfig();

    void onDaemonDisconnect( mega::MachineID machineID );

private:
    network::Log                               m_log;
    const boost::filesystem::path              m_stashFolder;
    network::Server                            m_server;
    task::BuildHashCodes                       m_buildHashCodes;
    task::Stash                                m_stash;
    mega::SymbolTable                          m_symbolTable;
    mega::network::RootConfig                  m_config;
    std::optional< MegastructureInstallation > m_megastructureInstallationOpt;
    MPOManager                                 m_mpoManager;
    StartupUUIDMap                             m_startupUUIDs;
};

} // namespace mega::service

#endif // ROOT_26_MAY_2022
