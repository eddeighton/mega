
#include "service/executor.hpp"

#include "request.hpp"
#include "job.hpp"
#include "simulation.hpp"

#include "service/network/network.hpp"
#include "service/network/log.hpp"

#include "parser/parser.hpp"

#include "boost/dll.hpp"
#include "boost/dll/shared_library_load_mode.hpp"

#include <optional>
#include <future>
#include <thread>

namespace mega
{
namespace service
{

template < typename TConversationFunctor >
class GenericConversation : public ExecutorRequestConversation
{
    TConversationFunctor m_functor;

public:
    GenericConversation( Executor& executor, const network::ConversationID& conversationID,
                         const network::ConnectionID& originatingConnectionID, TConversationFunctor&& functor )
        : ExecutorRequestConversation( executor, conversationID, originatingConnectionID )
        , m_functor( functor )
    {
    }

    void run( boost::asio::yield_context& yield_ctx )
    {
        ConversationBase::RequestStack stack(
            "GenericConversation", *this, m_executor.getLeafSender().getConnectionID() );
        m_functor( *this, m_executor.getLeafSender(), yield_ctx );
    }
};

Executor::Executor( boost::asio::io_context& io_context, int numThreads )
    : network::ConversationManager( network::makeProcessName( network::Node::Executor ), io_context )
    , m_io_context( io_context )
    , m_numThreads( numThreads )
    , m_receiverChannel( m_io_context, *this )
    , m_leaf(
          [ &m_receiverChannel = m_receiverChannel ]()
          {
              m_receiverChannel.run( network::makeProcessName( network::Node::Executor ) );
              return m_receiverChannel.getSender();
          }(),
          network::Node::Executor )
{
    m_pParser = boost::dll::import_symbol< EG_PARSER_INTERFACE >(
        "parser", "g_parserSymbol", boost::dll::load_mode::append_decorations );

    auto func = []( network::ConversationBase& con, network::Sender& sender, boost::asio::yield_context& yield_ctx )
    {
        network::exe_leaf::Request_Encode leaf( con, sender, yield_ctx );
        mega::runtime::initialiseRuntime( leaf.ExeGetProject() );
    };
    conversationInitiated( network::ConversationBase::Ptr(
        new GenericConversation( *this, createConversationID( getLeafSender().getConnectionID() ),
                                 getLeafSender().getConnectionID(), std::move( func ) ) ), m_leaf );
}

Executor::~Executor()
{
    m_receiverChannel.stop();
    SPDLOG_INFO( "Executor shutdown" );
}

void Executor::shutdown() {}

network::ConversationBase::Ptr Executor::joinConversation( const network::ConnectionID& originatingConnectionID,
                                                           const network::Header&       header,
                                                           const network::Message&      msg )
{
    return network::ConversationBase::Ptr(
        new ExecutorRequestConversation( *this, header.getConversationID(), originatingConnectionID ) );
}

} // namespace service
} // namespace mega
