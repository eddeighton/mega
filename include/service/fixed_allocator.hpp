
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

#ifndef GUARD_2023_April_05_fixed_allocator
#define GUARD_2023_April_05_fixed_allocator

#include "mega/values/native_types.hpp"

#include "mega/values/compilation/size_alignment.hpp"

#include "mega/values/runtime/pointer.hpp"

#include "mega/memory.hpp"

#include "service/protocol/common/sender_ref.hpp"
#include "service/network/log.hpp"

#include "common/assert_verify.hpp"

#include <boost/circular_buffer.hpp>

#include <memory>
#include <optional>

namespace mega::runtime
{

class FixedAllocator
{
    friend class FixedPtr;
    using FreeList = boost::circular_buffer< AllocationID >;

    U64           m_blockSize;
    U64           m_allocations;
    SizeAlignment m_sizeAlignment;
    HeapBufferPtr m_memory;
    HeapBufferPtr m_double;
    FreeList      m_free;

public:
    using Ptr = std::unique_ptr< FixedAllocator >;

    FixedAllocator( U64 blockSize, U64 allocations, U64 alignment )
        : m_blockSize( blockSize )
        , m_allocations( allocations )
        , m_sizeAlignment( { m_blockSize * m_allocations, alignment } )
        , m_memory( m_sizeAlignment )
        , m_double( m_sizeAlignment )
        , m_free( m_allocations )
    {
        // establish all free indicies
        for( int i = 0; i != m_allocations; ++i )
        {
            m_free.push_back( i );
        }
    }

    network::AllocatorStatus getStatus() const
    {
        return { m_blockSize * m_allocations, m_blockSize, m_allocations, m_free.size() };
    }

    inline void* getDoubleBuffer() const { return m_double.get(); }

    inline void copyToDoubleBuffer() { memcpy( m_double.get(), m_memory.get(), m_sizeAlignment.size ); }

    inline AllocationID malloc()
    {
        VERIFY_RTE_MSG( !m_free.empty(), "Fixed memory overflow" );
        AllocationID result = m_free.front();
        SPDLOG_TRACE( "FixedAllocator::malloc {}", result );
        m_free.pop_front();
        return result;
    }

    inline void free( AllocationID id )
    {
        SPDLOG_TRACE( "FixedAllocator::free: {}", id );
        //
        m_free.push_front( id );
    }

    inline void* get( AllocationID allocationID ) const
    {
        //
        return reinterpret_cast< char* >( m_memory.get() ) + ( m_blockSize * allocationID );
    }

public:
    class FixedPtr
    {
        FixedAllocator&               m_allocator;
        std::optional< AllocationID > m_allocationIDOpt;

    public:
        inline FixedPtr( FixedAllocator& allocator )
            : m_allocator( allocator )
        {
            m_allocationIDOpt = m_allocator.malloc();
        }
        inline FixedPtr( FixedPtr&& mv )
            : m_allocator( mv.m_allocator )
        {
            m_allocationIDOpt.swap( mv.m_allocationIDOpt );
        }
        inline FixedPtr( FixedPtr& )            = delete;
        inline FixedPtr& operator=( FixedPtr& ) = delete;
        inline ~FixedPtr()
        {
            if( m_allocationIDOpt.has_value() )
            {
                m_allocator.free( m_allocationIDOpt.value() );
            }
        }
        inline AllocationID getID() const { return m_allocationIDOpt.value(); }
        inline void*        get() const { return m_allocator.get( m_allocationIDOpt.value() ); }
    };

    FixedPtr allocate() { return FixedPtr{ *this }; }
};

} // namespace mega::runtime

#endif // GUARD_2023_April_05_fixed_allocator
