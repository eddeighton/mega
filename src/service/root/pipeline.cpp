
#include "pipeline.hpp"

#include "root.hpp"

namespace mega
{
    namespace service
    {

    RootPipelineConversation::RootPipelineConversation( Root&                          root,
                              const network::ConversationID& conversationID,
                              const network::ConnectionID&   originatingConnectionID )
        : RootRequestConversation( root, conversationID, originatingConnectionID )
        , m_taskReady( root.getIOContext(), CHANNEL_SIZE )
        , m_taskComplete( root.getIOContext(), CHANNEL_SIZE )
    {
    }

    }
}