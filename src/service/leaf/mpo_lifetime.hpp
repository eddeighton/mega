
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

#ifndef GUARD_2022_October_22_mpo_lifetime
#define GUARD_2022_October_22_mpo_lifetime

#include "service/leaf.hpp"

#include "mega/default_traits.hpp"

namespace mega::service
{

class LeafRequestConversation;

class MPOLifetime
{
public:
    MPOLifetime( Leaf& leaf, LeafRequestConversation& conversation, const reference& root, const std::string& strMemory,
                 boost::asio::yield_context& yield_ctx );
    ~MPOLifetime();

    const reference&              getRoot() const { return m_root; }
    runtime::ManagedSharedMemory& getSharedMemory() { return m_sharedMemory; }

    std::string memoryAccess( MPO mpo );

private:
    Leaf&                         m_leaf;
    LeafRequestConversation&      m_conversation;
    boost::asio::yield_context&   m_yield_ctx;
    reference                     m_root;
    runtime::ManagedSharedMemory& m_sharedMemory;
};

} // namespace mega::service

#endif // GUARD_2022_October_22_mpo_lifetime
