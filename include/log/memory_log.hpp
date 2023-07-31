
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

#ifndef GUARD_2023_July_31_memory_log
#define GUARD_2023_July_31_memory_log

#include "mega/native_types.hpp"

#include "log/offset.hpp"
#include "log/records.hxx"
#include "log/buffer.hpp"
#include "log/storage.hpp"

#include <memory>
#include <vector>

namespace mega::log
{

namespace impl
{
class MemoryBufferFactory;

class MemoryBuffer
{
public:
    using Ptr = std::unique_ptr< MemoryBuffer >;

    MemoryBuffer( MemoryBufferFactory&, TrackID, BufferIndex ) {}
    void* get_address() const { return const_cast< char* >( m_buffer.data() ); }

private:
    std::array< char, LogFileSize > m_buffer = { 0 };
};

class MemoryBufferFactory
{
public:
    using BufferImplType = MemoryBuffer;
    using BufferType     = Buffer< MemoryBufferFactory >;
    using TrackType      = Track< MemoryBufferFactory >;
    using IndexType      = Index< MemoryBufferFactory >;

    MemoryBufferFactory()
        : m_index( *this )
    {
    }

    void cycle()
    {
        ++m_timestamp;
        BufferType*             pBuffer = m_index.getBuffer( m_index.toBufferIndex( m_timestamp ) );
        const InterBufferOffset offset  = pBuffer->write( &m_iterator, IndexType::RecordSize );
        ASSERT( offset.get() % IndexType::RecordSize == 0U );
        // this looks wierd but is correct - the timestamp record has identity that matches
        // the record start position within file / Index::RecordSize.
        // So if 4 records per file then timestamp 4 would be first record in second file
        // such that ( 4 % 4 ) + 1 == end of timestamp 4's data in second file
        ASSERT( ( m_timestamp % IndexType::RecordsPerFile ) + 1 == ( offset.get() / IndexType::RecordSize ) );
    }

protected:
    TimeStamp   m_timestamp = 0U;
    IndexRecord m_iterator;
    IndexType   m_index;
};
} // namespace impl

using MemoryStorage = Storage< impl::MemoryBufferFactory >;

template < typename RecordType >
using MemoryIterator = Iterator< impl::MemoryBufferFactory, RecordType >;

} // namespace mega::log

#endif // GUARD_2023_July_31_memory_log
