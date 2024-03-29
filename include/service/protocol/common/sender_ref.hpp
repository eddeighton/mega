
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

#ifndef GUARD_2023_March_21_sender_ref
#define GUARD_2023_March_21_sender_ref

#include "mega/values/compilation/concrete/type_id.hpp"
#include "mega/values/runtime/timestamp.hpp"
#include "mega/values/runtime/mpo.hpp"
#include "mega/values/native_types.hpp"

#include "common/assert_verify.hpp"

#include <array>

namespace mega::network
{

class LogicalThreadBase;

struct ClockTick
{
public:
    mega::runtime::TimeStamp m_cycle = {};
    mega::F32                m_ct = {}, m_dt = {};

    template < class Archive >
    inline void serialize( Archive&, const unsigned int )
    {
        THROW_RTE( "Attempted to serialize ClockTick" );
    }
};

class SenderRef
{
public:
    template < class Archive >
    inline void serialize( Archive&, const unsigned int )
    {
        THROW_RTE( "Attempted to serialize SenderRef" );
    }

    runtime::MPO       m_mpo;
    LogicalThreadBase* m_pSender;

    struct AllocatorBase
    {
        concrete::TypeID type  = {};
        void*            pBase = nullptr;
    };
    static constexpr U64 MAX_ALLOCATORS = 8;
    using AllocatorBaseArray            = std::array< AllocatorBase, MAX_ALLOCATORS >;
    AllocatorBaseArray m_allocators;
};

struct HTTPRequestData
{
    unsigned    verb    = 0;
    unsigned    version = 0;
    std::string request;
    bool        keep_alive = false;

    template < class Archive >
    inline void serialize( Archive&, const unsigned int )
    {
        THROW_RTE( "Attempted to serialize HTTPRequestData" );
    }
};

} // namespace mega::network

#endif // GUARD_2023_March_21_sender_ref
