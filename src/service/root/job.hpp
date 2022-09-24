#ifndef ROOT_JOB_23_SEPT_2022
#define ROOT_JOB_23_SEPT_2022

#include "request.hpp"

#include "service/network/log.hpp"

#include "service/protocol/model/job.hxx"

namespace mega
{
namespace service
{
class Root;

class RootJobConversation : public RootRequestConversation
{
public:
    RootJobConversation( Root&                          root,
                         const network::ConversationID& conversationID,
                         const network::ConnectionID&   originatingConnectionID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;

    // network::job::Impl
    virtual void JobReadyForWork( const network::ConversationID& rootConversationID,
                                  boost::asio::yield_context&    yield_ctx ) override;
    virtual void JobProgress( const std::string& message, boost::asio::yield_context& yield_ctx ) override;
};

} // namespace service
} // namespace mega

#endif // ROOT_JOB_23_SEPT_2022
