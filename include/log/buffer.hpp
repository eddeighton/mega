
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

#ifndef GUARD_2023_July_31_buffer
#define GUARD_2023_July_31_buffer

#include "log/records.hxx"
#include "log/index_record.hpp"
#include "log/offset.hpp"

#include <memory>
#include <vector>
#include <cstring>

namespace mega::log
{

template < class BufferFactory >
class Buffer : public BufferFactory::BufferImplType
{
    using BufferImplType = typename BufferFactory::BufferImplType;
    inline void* getAddress() const noexcept { return BufferImplType::get_address(); }

public:
    using Ptr       = std::unique_ptr< Buffer >;
    using PtrVector = std::vector< Ptr >;

    Buffer( BufferFactory& bufferFactory, TrackID trackID, BufferIndex fileIndex )
        : BufferImplType( bufferFactory, trackID, fileIndex )
        , m_writePosition( 0 )
    {
    }

    inline const void* read( InterBufferOffset offset ) const noexcept
    {
        return reinterpret_cast< char* >( getAddress() ) + offset.get();
    }

    inline const void* getStart() const noexcept { return read( 0 ); }
    inline const void* getEnd() const noexcept { return read( LogFileSize ); }

    inline bool fit( U64 size ) const noexcept { return m_writePosition.get() + size <= LogFileSize; }

    inline InterBufferOffset write( const void* pData, U64 size ) noexcept
    {
        std::memcpy( reinterpret_cast< char* >( getAddress() ) + m_writePosition.get(),
                     reinterpret_cast< const char* >( pData ), size );
        m_writePosition = InterBufferOffset{ m_writePosition.get() + size };
        return m_writePosition;
    }

    template < typename RecordType >
    inline void terminate() noexcept
    {
        if constexpr( RecordType::Variable )
        {
            if( fit( sizeof( SizeType ) ) )
            {
                static const SizeType nullsize = 0U;
                write( &nullsize, sizeof( SizeType ) );
            }
        }
    }

    template < typename RecordType >
    inline bool isTerminated() const noexcept
    {
        static_assert( !RecordType::Variable, "incorrect isTerminated function used" );
        return !fit( RecordType::size() );
    }

    template < typename RecordType >
    inline bool isTerminated( InterBufferOffset readPosition ) const noexcept
    {
        static_assert( RecordType::Variable, "incorrect isTerminated function used" );
        return !fit( sizeof( SizeType ) ) || ( RecordType::getVariableSize( read( readPosition ) ) == 0U );
    }

private:
    InterBufferOffset m_writePosition;
};

template < class BufferFactory >
class BufferSequence
{
    using BufferType   = Buffer< BufferFactory >;
    using BufferPtrMap = std::unordered_map< BufferIndex, typename BufferType::Ptr, BufferIndex::Hash >;

public:
    inline BufferSequence( BufferFactory& bufferFactory, TrackID trackID )
        : m_bufferStorage( bufferFactory )
        , m_trackID( trackID )
    {
    }

    ~BufferSequence() = default;

    inline BufferType* getBuffer( BufferIndex fileIndex )
    {
        auto iFind = m_buffers.find( fileIndex );
        if( iFind != m_buffers.end() )
        {
            return iFind->second.get();
        }

        // create new file
        {
            typename BufferType::Ptr pFile = std::make_unique< BufferType >( m_bufferStorage, m_trackID, fileIndex );
            auto                     ib    = m_buffers.insert( { fileIndex, std::move( pFile ) } );
            VERIFY_RTE( ib.second );
            return ib.first->second.get();
        }
    }

    inline const BufferType* getBuffer( BufferIndex fileIndex ) const
    {
        auto iFind = m_buffers.find( fileIndex );
        if( iFind != m_buffers.end() )
        {
            return iFind->second.get();
        }
        // create new file
        {
            typename BufferType::Ptr pFile = std::make_unique< BufferType >( m_bufferStorage, m_trackID, fileIndex );
            auto                     ib    = m_buffers.insert( { fileIndex, std::move( pFile ) } );
            VERIFY_RTE( ib.second );
            return ib.first->second.get();
        }
    }

private:
    BufferFactory&       m_bufferStorage;
    TrackID              m_trackID;
    mutable BufferPtrMap m_buffers;
};

template < class BufferFactory >
class Index : public BufferSequence< BufferFactory >
{
public:
    using Ptr = std::unique_ptr< Index >;

    inline Index( BufferFactory& bufferFactory )
        : BufferSequence< BufferFactory >( bufferFactory, TrackID::TOTAL )
    {
    }

    static constexpr U64 RecordSize = sizeof( IndexRecord );
    static_assert( RecordSize == 64U, "Unexpected record size" );
    static constexpr auto RecordsPerFile = LogFileSize / RecordSize;

    static inline BufferIndex toBufferIndex( TimeStamp timeStamp ) noexcept
    {
        return BufferIndex{ static_cast< U32 >( timeStamp / RecordsPerFile ) };
    }
    static inline InterBufferOffset toInterBufferOffset( TimeStamp timeStamp ) noexcept
    {
        return ( timeStamp % RecordsPerFile ) * RecordSize;
    }
};

template < class BufferFactory >
class Track : public BufferSequence< BufferFactory >
{
public:
    using Ptr = std::unique_ptr< Track >;
    inline Track( BufferFactory& bufferFactory, TrackID trackID )
        : BufferSequence< BufferFactory >( bufferFactory, trackID )
    {
    }
};

} // namespace mega::log

#endif // GUARD_2023_July_31_buffer
