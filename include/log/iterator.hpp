
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

#ifndef GUARD_2023_July_31_iterator
#define GUARD_2023_July_31_iterator

#include "log/buffer.hpp"

namespace mega::log
{

template < class BufferFactory >
class Storage;

template < typename BufferFactory, typename RecordType >
class Iterator
{
    friend class Storage< BufferFactory >;

    inline const void* get() const
    {
        if( !m_pBuffer )
        {
            m_pBuffer = m_track.getBuffer( m_position );
        }
        ASSERT( m_pBuffer );
        const void* pData = m_pBuffer->read( m_position );

        bool bIsTerminated = false;
        if constexpr( RecordType::Variable )
        {
            bIsTerminated = m_pBuffer->template isTerminated< RecordType >( m_position );
        }
        else
        {
            bIsTerminated = m_pBuffer->template isTerminated< RecordType >();
        }

        if( bIsTerminated )
        {
            // then skip to next file
            BufferIndex fileIndex = m_position;
            m_position            = Offset( BufferIndex{ fileIndex.get() + 1 }, InterBufferOffset{} );

            m_pBuffer = m_track.getBuffer( m_position );
            ASSERT( m_pBuffer );
            pData = m_pBuffer->read( m_position );
        }
        return pData;
    }

    inline Iterator( const Track< BufferFactory >& track )
        : m_track( track )
    {
    }
    inline Iterator( const Track< BufferFactory >& track, Offset offset )
        : m_track( track )
        , m_position( offset )
    {
    }

public:
    inline bool operator==( const Iterator& cmp ) const noexcept { return m_position == cmp.m_position; }
    inline bool operator!=( const Iterator& cmp ) const noexcept { return !this->operator==( cmp ); }

    inline RecordType operator*() const noexcept { return RecordType( get() ); }

    inline void operator++() noexcept // pre increment only
    {
        if constexpr( RecordType::Variable )
        {
            m_position = Offset( m_position, InterBufferOffset{ InterBufferOffset( m_position ).get()
                                                                + RecordType::getVariableSize( get() ) } );
        }
        else
        {
            m_position
                = Offset( m_position, InterBufferOffset{ InterBufferOffset( m_position ).get() + RecordType::size() } );
        }
    }

    inline const Offset& position() const noexcept { return m_position; }

    const Buffer< BufferFactory >* getBuffer() const
    {
        ASSERT( m_pBuffer );
        return m_pBuffer;
    }

private:
    const Track< BufferFactory >&          m_track;
    mutable const Buffer< BufferFactory >* m_pBuffer  = nullptr;
    mutable Offset                         m_position = Offset{};
};

} // namespace mega::log

#endif // GUARD_2023_July_31_iterator
