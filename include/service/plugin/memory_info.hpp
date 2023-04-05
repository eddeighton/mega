
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

#ifndef GUARD_2023_April_05_memory_info
#define GUARD_2023_April_05_memory_info

#include "service/protocol/common/sender_ref.hpp"

#include <array>
#include <vector>
#include <algorithm>

namespace mega::service
{

class MemoryDescription
{
    struct MPOInfo
    {
        MPO                                    mpo;
        network::SenderRef::AllocatorBaseArray allocators;
    };
    using MPOInfoVector = std::vector< MPOInfo >;
    MPOInfoVector m_mpoInfos;
    U64           m_state = 1U;

public:
    void onRegister( const network::SenderRef& senderRef )
    {
        ++m_state;
        m_mpoInfos.push_back( MPOInfo{ senderRef.m_mpo, senderRef.m_allocators } );
    }

    void onUnregister( MPO mpo )
    {
        ++m_state;
        m_mpoInfos.erase( std::remove_if( m_mpoInfos.begin(), m_mpoInfos.end(),
                                          [ mpo ]( const MPOInfo& mpoInfo ) { return mpoInfo.mpo == mpo; } ),
                          m_mpoInfos.end() );
    }

    U64         getState() const { return m_state; }
    U64         getSize() const { return m_mpoInfos.size(); }
    const void* getData() const { return reinterpret_cast< const void* >( m_mpoInfos.data() ); }
};
} // namespace mega::service

#endif // GUARD_2023_April_05_memory_info
