
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

#ifndef GUARD_2022_October_10_offset
#define GUARD_2022_October_10_offset

#include "mega/values/native_types.hpp"

#include <limits>
#include <functional>

namespace mega::event
{

static const U64 LogFileSize        = 1 << 26;
static const int LogFileIndexDigits = std::numeric_limits< U32 >::digits10 + 1;

class BufferIndex
{
public:
    inline BufferIndex()                                        = default;
    inline BufferIndex( const BufferIndex& fileIndex )            = default;
    inline BufferIndex& operator=( const BufferIndex& fileIndex ) = default;
    inline BufferIndex( U32 index )
        : m_index( index )
    {
    }
    inline U32 get() const { return m_index; }

    inline bool operator<( const BufferIndex& cmp ) const { return m_index < cmp.m_index; }
    inline bool operator==( const BufferIndex& cmp ) const { return m_index == cmp.m_index; }
    inline bool operator!=( const BufferIndex& cmp ) const { return !this->operator==( cmp ); }

    struct Hash
    {
        std::size_t operator()( const BufferIndex& fileIndex ) const { return std::hash< U32 >{}( fileIndex.m_index ); }
    };

private:
    U32 m_index = 0U;
};

class InterBufferOffset
{
public:
    inline InterBufferOffset()                                           = default;
    inline InterBufferOffset( const InterBufferOffset& offset )            = default;
    inline InterBufferOffset& operator=( const InterBufferOffset& offset ) = default;
    inline InterBufferOffset( U64 offset )
        : m_index( offset )
    {
    }
    inline U64 get() const { return m_index; }

    inline bool operator<( const InterBufferOffset& cmp ) const { return m_index < cmp.m_index; }
    inline bool operator==( const InterBufferOffset& cmp ) const { return m_index == cmp.m_index; }
    inline bool operator!=( const InterBufferOffset& cmp ) const { return !this->operator==( cmp ); }

    inline InterBufferOffset operator++() { return ++m_index; }
    inline InterBufferOffset operator++( int ) { return m_index++; }
    inline InterBufferOffset operator+=( InterBufferOffset amt ) { return m_index += amt.get(); }

private:
    U64 m_index = 0U;
};

class Offset
{
public:
    inline Offset()                                  = default;
    inline Offset( const Offset& offset )            = default;
    inline Offset& operator=( const Offset& offset ) = default;
    inline Offset( const BufferIndex& fileIndex, const InterBufferOffset& interFileOffset )
        : m_offset( fileIndex.get() * LogFileSize + interFileOffset.get() )
    {
    }
    inline operator BufferIndex() const { return BufferIndex{ static_cast< U32 >( m_offset / LogFileSize ) }; }
    inline operator InterBufferOffset() const { return InterBufferOffset{ m_offset % LogFileSize }; }

    inline bool operator<( const Offset& cmp ) const { return m_offset < cmp.m_offset; }
    inline bool operator==( const Offset& cmp ) const { return m_offset == cmp.m_offset; }
    inline bool operator!=( const Offset& cmp ) const { return !this->operator==( cmp ); }

    inline U64 get() const { return m_offset; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        archive& m_offset;
    }

private:
    U64 m_offset = 0U;
};

class BufferOffset
{
public:
    inline BufferOffset()                                          = default;
    inline BufferOffset( const BufferOffset& fileOffset )            = default;
    inline BufferOffset& operator=( const BufferOffset& fileOffset ) = default;
    inline BufferOffset( const Offset& offset )
        : m_fileIndex( offset )
        , m_offset( offset )
    {
    }
    inline const BufferIndex&      getFileIndex() const { return m_fileIndex; }
    inline const InterBufferOffset getInterFileOffset() const { return m_offset; }
    inline                       operator Offset() const { return Offset{ m_fileIndex, m_offset }; }

private:
    BufferIndex       m_fileIndex = 0U;
    InterBufferOffset m_offset    = 0U;
};

} // namespace mega::event

#endif // GUARD_2022_October_10_offset
