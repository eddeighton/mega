#include "request.hpp"

#include "root.hpp"

namespace mega
{
namespace service
{

// network::address::Impl
mega::MPO RootRequestConversation::GetNetworkAddressMPO( const mega::NetworkAddress& networkAddress,
                                                         boost::asio::yield_context& yield_ctx )
{
    return m_root.m_logicalAddressSpace.getNetworkAddressMPO( networkAddress );
}
mega::NetworkAddress RootRequestConversation::GetRootNetworkAddress( const mega::MPO&            mpo,
                                                                     boost::asio::yield_context& yield_ctx )
{
    return m_root.m_logicalAddressSpace.getRootNetworkAddress( mpo );
}
mega::NetworkAddress RootRequestConversation::AllocateNetworkAddress( const mega::MPO&            mpo,
                                                                      const mega::TypeID&         objectTypeID,
                                                                      boost::asio::yield_context& yield_ctx )
{
    return m_root.m_logicalAddressSpace.allocateNetworkAddress( mpo, objectTypeID );
}
void RootRequestConversation::DeAllocateNetworkAddress( const MPO&                  mpo,
                                                        const NetworkAddress&       networkAddress,
                                                        boost::asio::yield_context& yield_ctx )
{
    return m_root.m_logicalAddressSpace.deAllocateNetworkAddress( mpo, networkAddress );
}
} // namespace service
} // namespace mega
