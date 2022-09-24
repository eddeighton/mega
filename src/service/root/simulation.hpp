#ifndef ROOT_SIM_24_SEPT_2022
#define ROOT_SIM_24_SEPT_2022

#include "request.hpp"

#include "service/network/log.hpp"

#include "service/protocol/model/sim.hxx"

namespace mega
{
namespace service
{

class RootSimulation : public RootRequestConversation, public network::sim::Impl
{
public:
    RootSimulation( Root&                          root,
                    const network::ConversationID& conversationID,
                    const network::ConnectionID&   originatingConnectionID,
                    mega::MPO                      leafMPO );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;

    virtual void SimStart( boost::asio::yield_context& yield_ctx ) override;

private:

    mega::MPO m_mpo;
};

} // namespace service
} // namespace mega
#endif // ROOT_SIM_24_SEPT_2022
