#ifndef EXECUTION_CONTEXT_MANAGER_25_AUG_2022
#define EXECUTION_CONTEXT_MANAGER_25_AUG_2022

#include "mega/common.hpp"
#include "mega/allocator.hpp"

#include "service/protocol/common/header.hpp"
#include "service/network/log.hpp"

#include <set>
#include <map>

namespace mega
{
namespace service
{
class ExecutionContextManager
{
public:
    std::pair< bool, mega::ExecutionIndex > create( const network::ConversationID& conversationID )
    {
        if ( !m_allocations.full() )
        {
            mega::ExecutionIndex nextFree = m_allocations.nextFree();
            SPDLOG_TRACE( "Creating execution context: {} for conversation: {}", nextFree, conversationID );
            m_allocations.allocate( nextFree );
            m_simulations.insert( { nextFree, conversationID } );
            m_conversations.insert( { conversationID, nextFree } );
            return { true, nextFree };
        }
        else
        {
            return { false, 0U };
        }
    }

    void release( const network::ConversationID& conversationID )
    {
        auto iFind = m_conversations.find( conversationID );
        if ( iFind != m_conversations.end() )
        {
            const auto address = iFind->second;
            SPDLOG_TRACE( "Releasing execution context: {} for conversation: {}", address, conversationID );
            m_conversations.erase( iFind );
            m_allocations.free( address );
            m_simulations.erase( address );
        }
    }

    void release( mega::ExecutionIndex address )
    {
        m_allocations.free( address );
        m_simulations.erase( address );
    }

    const network::ConversationID& get( mega::ExecutionIndex executionIndex ) const
    {
        auto iFind = m_simulations.find( executionIndex );
        VERIFY_RTE_MSG( iFind != m_simulations.end(), "Failed to locate execution index: " << executionIndex );
        return iFind->second;
    }

private:
    mega::RingAllocator< mega::ExecutionIndex, mega::MAX_SIMULATIONS > m_allocations;
    std::map< mega::ExecutionIndex, network::ConversationID >          m_simulations;
    std::map< network::ConversationID, mega::ExecutionIndex >          m_conversations;
};
} // namespace service
} // namespace mega

#endif // EXECUTION_CONTEXT_MANAGER_25_AUG_2022
