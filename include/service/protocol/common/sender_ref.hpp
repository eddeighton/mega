
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

#include "mega/mpo.hpp"

#include "common/assert_verify.hpp"

#include <array>

namespace mega::network
{

class LogicalThreadBase;

struct ClockTick
{
public:
    mega::TimeStamp m_cycle = {};
    mega::F32       m_ct = {}, m_dt = {};

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        THROW_RTE( "Attempted to serialize ClockTick" );
    }
};

class SenderRef
{
public:
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        THROW_RTE( "Attempted to serialize SenderRef" );
    }

    MPO               m_mpo;
    LogicalThreadBase* m_pSender;

    struct AllocatorBase
    {
        TypeID type  = {};
        void*  pBase = nullptr;
    };
    static constexpr U64 MAX_ALLOCATORS = 8;
    using AllocatorBaseArray            = std::array< AllocatorBase, MAX_ALLOCATORS >;
    AllocatorBaseArray m_allocators;
};

struct AllocatorStatus
{
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& total;
        archive& blockSize;
        archive& allocations;
        archive& free;
    }

    U64 total;
    U64 blockSize;
    U64 allocations;
    U64 free;
};

struct MemoryStatus
{
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_heap;
        archive& m_object;
        archive& m_allocators;
    }

    U64 m_heap;
    U64 m_object;

    struct TypedAllocatorStatus
    {
        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& typeID;
            archive& status;
        }
        TypeID          typeID;
        AllocatorStatus status;
    };
    using TypedAllocatorStatusArray = std::array< TypedAllocatorStatus, network::SenderRef::MAX_ALLOCATORS >;
    TypedAllocatorStatusArray m_allocators;
};

} // namespace mega::network

#endif // GUARD_2023_March_21_sender_ref
