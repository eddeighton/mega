
#ifndef SHARED_MEMORY_MANAGER_3_SEPT_2022
#define SHARED_MEMORY_MANAGER_3_SEPT_2022

#include "mega/default_traits.hpp"

#include "service/protocol/common/header.hpp"

#include "service/network/log.hpp"

#include <string>
#include <sstream>
#include <map>

namespace mega
{
namespace service
{
class SharedMemoryManager
{
    class SharedMemory
    {
        static constexpr std::size_t SIZE = 1024 * 1024 * 4;

        std::string memoryName( const network::ConversationID& conversationID ) const
        {
            std::ostringstream os;
            os << "memory_" << conversationID;
            return os.str();
        }

    public:
        using Ptr = std::unique_ptr< SharedMemory >;

        SharedMemory( const network::ConversationID& conversationID )
            : m_strName( memoryName( conversationID ) )
            , m_memory( boost::interprocess::create_only, m_strName.c_str(), SIZE )
        {
        }
        ~SharedMemory() { boost::interprocess::shared_memory_object::remove( m_strName.c_str() ); }

        const std::string& getName() const { return m_strName; }

    private:
        std::string                  m_strName;
        runtime::ManagedSharedMemory m_memory;
    };

    using SharedMemoryMap = std::map< network::ConversationID, SharedMemory::Ptr >;

public:
    const std::string& acquire( const network::ConversationID& conversationID )
    {
        auto iFind = m_memory.find( conversationID );
        if ( iFind != m_memory.end() )
        {
            return iFind->second->getName();
        }
        else
        {
            SharedMemory::Ptr  pNewMemory = std::make_unique< SharedMemory >( conversationID );
            const std::string& strName    = pNewMemory->getName();
            m_memory.insert( { conversationID, std::move( pNewMemory ) } );
            return strName;
        }
    }

    void release( const network::ConversationID& conversationID )
    {
        auto iFind = m_memory.find( conversationID );
        if ( iFind != m_memory.end() )
        {
            m_memory.erase( iFind );
            SPDLOG_INFO( "Daemon released memory for {}", conversationID );
        }
    }

private:
    SharedMemoryMap m_memory;
};
} // namespace service
} // namespace mega

#endif // SHARED_MEMORY_MANAGER_3_SEPT_2022
