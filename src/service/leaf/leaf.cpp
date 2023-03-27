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

#include "service/leaf.hpp"

#include "request.hpp"

#include "service/network/log.hpp"

#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/project.hxx"
#include "service/protocol/model/memory.hxx"
#include "service/protocol/model/stash.hxx"

#include "service/protocol/common/conversation_id.hpp"

#include "common/requireSemicolon.hpp"

#include <boost/system/detail/error_code.hpp>

namespace mega::service
{

class LeafEnrole : public LeafRequestConversation
{
    std::promise< void >& m_promise;

public:
    LeafEnrole( Leaf& leaf, const network::ConnectionID& originatingConnectionID, std::promise< void >& promise )
        : LeafRequestConversation( leaf, leaf.createConversationID( originatingConnectionID ), originatingConnectionID )
        , m_promise( promise )
    {
    }
    void run( boost::asio::yield_context& yield_ctx )
    {
        SPDLOG_TRACE( "LeafEnrole run" );

        auto daemonSender = getDaemonSender( yield_ctx );

        // enrole - getting MP
        {
            network::enrole::Request_Encoder encoder( [ &daemonSender ]( const network::Message& msg ) mutable
                                                      { return daemonSender.LeafDaemon( msg ); },
                                                      getID() );
            m_leaf.m_mp = encoder.EnroleLeafWithDaemon( m_leaf.getType() );

            SPDLOG_TRACE( "Leaf enrole mp: {}", m_leaf.m_mp );
        }

        // allocate remote object memory manager
        {
            m_leaf.m_pRemoteMemoryManager = std::make_unique< runtime::RemoteMemoryManager >(
                m_leaf.m_mp,
                [ leaf = &m_leaf ]( TypeID typeID, runtime::CodeGenerator::LLVMCompiler& llvmCompiler )
                { return leaf->getJIT().getAllocator( llvmCompiler, typeID ); } );
        }

        // determine the current project and stuff and initialise the runtime
        {
            network::project::Request_Encoder projectRequest(
                [ &daemonSender ]( const network::Message& msg ) { return daemonSender.LeafRoot( msg ); }, getID() );

            const Project currentProject          = projectRequest.GetProject();
            m_leaf.m_megastructureInstallationOpt = projectRequest.GetMegastructureInstallation();
            m_leaf.setActiveProject( currentProject );
        }

        // set process description
        {
            using ::           operator<<;
            std::ostringstream os;
            os << network::Node::toStr( m_leaf.m_nodeType ) << " " << m_leaf.m_mp;
            common::ProcessID::setDescription( os.str().c_str() );
        }

        boost::asio::post( [ &promise = m_promise ]() { promise.set_value(); } );
    }
};

Leaf::Leaf( network::Sender::Ptr pSender, network::Node::Type nodeType, short daemonPortNumber )
    : network::ConversationManager( network::makeProcessName( network::Node::Leaf ), m_io_context )
    , m_pSender( std::move( pSender ) )
    , m_nodeType( nodeType )
    , m_io_context( 1 ) // single threaded concurrency hint
    , m_receiverChannel( m_io_context, *this )
    , m_client( m_io_context, *this, "localhost", daemonPortNumber )
    , m_work_guard( m_io_context.get_executor() )
    , m_io_thread( [ &io_context = m_io_context ]() { io_context.run(); } )
{
    m_receiverChannel.run( network::makeProcessName( network::Node::Leaf ) );

    m_pSelfSender = m_receiverChannel.getSender();

    {
        std::promise< void > promise;
        std::future< void >  future = promise.get_future();
        conversationInitiated(
            std::make_shared< LeafEnrole >( *this, getDaemonSender().getConnectionID(), promise ), getDaemonSender() );
        future.get();
    }
}

Leaf::~Leaf()
{
    m_client.stop();
    m_receiverChannel.stop();
    m_work_guard.reset();
    m_io_thread.join();
}

void Leaf::setActiveProject( const Project& currentProject )
{
    switch( m_nodeType )
    {
        case network::Node::Leaf:
        case network::Node::Terminal:
            break;
        case network::Node::Tool:
        case network::Node::Python:
        case network::Node::Executor:
        case network::Node::Plugin:
            if( !currentProject.isEmpty() && m_megastructureInstallationOpt.has_value() )
            {
                if( boost::filesystem::exists( currentProject.getProjectDatabase() ) )
                {
                    try
                    {
                        if( m_pJIT && m_activeProject.has_value() )
                        {
                            m_pJIT = std::make_unique< runtime::JIT >(
                                m_megastructureInstallationOpt.value(), currentProject, *m_pJIT );

                            if( m_activeProject.value().getProjectInstallPath()
                                == currentProject.getProjectInstallPath() )
                            {
                                SPDLOG_INFO( "Leaf: {} setActiveProject reloading project {}", m_mp,
                                              currentProject.getProjectInstallPath().string() );
                            }
                            else
                            {
                                SPDLOG_INFO( "Leaf: {} setActiveProject changing project from {} to: {}", m_mp,
                                              m_activeProject.value().getProjectInstallPath().string(),
                                              currentProject.getProjectInstallPath().string() );
                            }
                        }
                        else
                        {
                            SPDLOG_INFO( "Leaf: {} setActiveProject creating runtime for project: {}", m_mp,
                                          currentProject.getProjectInstallPath().string() );
                            m_pJIT = std::make_unique< runtime::JIT >(
                                m_megastructureInstallationOpt.value(), currentProject );
                        }
                        m_activeProject = currentProject;
                    }
                    catch( mega::io::DatabaseVersionException& ex )
                    {
                        SPDLOG_ERROR( "Database version exception: {}", currentProject.getProjectInstallPath().string(),
                                      ex.what() );
                    }
                    catch( std::exception& ex )
                    {
                        SPDLOG_ERROR( "ComponentManager failed to initialise project: {} error: {}",
                                      currentProject.getProjectInstallPath().string(), ex.what() );
                        throw;
                    }
                }
                else
                {
                    m_pJIT.reset();
                    SPDLOG_WARN( "JIT uninitialised.  Active project: {} has no database",
                                 currentProject.getProjectInstallPath().string() );
                }
            }
            else
            {
                m_pJIT.reset();
                SPDLOG_WARN( "JIT uninitialised.  No active project" );
            }
            break;
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::TOTAL_NODE_TYPES:
        default:
            THROW_RTE( "Leaf: Unknown leaf type" );
            break;
    }
}

// network::ConversationManager
network::ConversationBase::Ptr Leaf::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                       const network::Message&      msg )
{
    switch( m_nodeType )
    {
        case network::Node::Leaf:
        case network::Node::Terminal:
        case network::Node::Tool:
        case network::Node::Python:
        case network::Node::Executor:
        case network::Node::Plugin:
            return network::ConversationBase::Ptr(
                new LeafRequestConversation( *this, msg.getReceiverID(), originatingConnectionID ) );
            break;
        case network::Node::Daemon:
        case network::Node::Root:
        case network::Node::TOTAL_NODE_TYPES:
        default:
            THROW_RTE( "Leaf: Unknown leaf type" );
            break;
    }
}

} // namespace mega::service
