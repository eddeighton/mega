
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

#ifndef GUARD_2022_October_10_log
#define GUARD_2022_October_10_log

#include "mega/native_types.hpp"

#include "log/offset.hpp"
#include "log/records.hxx"
#include "log/buffer.hpp"
#include "log/storage.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <memory>

namespace mega::log
{

namespace impl
{
class FileBufferFactory;

class FileBuffer
{
public:
    using Ptr = std::unique_ptr< FileBuffer >;

    FileBuffer( FileBufferFactory& bufferFactory, TrackID trackID, BufferIndex fileIndex );
    ~FileBuffer();

    void* get_address() const { return m_region.get_address(); }

private:
    boost::filesystem::path            m_filePath;
    boost::interprocess::file_mapping  m_fileMapping;
    boost::interprocess::mapped_region m_region;
};

class FileBufferFactory
{
public:
    using BufferImplType = FileBuffer;
    using BufferType     = Buffer< FileBufferFactory >;
    using TrackType      = Track< FileBufferFactory >;
    using IndexType      = Index< FileBufferFactory >;

    FileBufferFactory( FileBufferFactory& ) = delete;
    FileBufferFactory& operator=( FileBufferFactory& ) = delete;

    FileBufferFactory( const boost::filesystem::path logFolderPath, bool bLoad );

    const boost::filesystem::path& getLogFolderPath() const { return m_logFolderPath; }

    boost::filesystem::path constructLogFile( TrackID trackID, BufferIndex fileIndex );

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

private:
    const boost::filesystem::path m_logFolderPath;
};

} // namespace impl

using FileStorage = Storage< impl::FileBufferFactory >;

template < typename RecordType >
using FileIterator = Iterator< impl::FileBufferFactory, RecordType >;

} // namespace mega::log

#endif // GUARD_2022_October_10_log
