
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

#include "event/file_log.hpp"
#include "event/filename.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/filesystem.hpp>

#include <string>
#include <optional>
#include <map>

namespace mega::event::impl
{

FileBuffer::FileBuffer( FileBufferFactory& bufferFactory, TrackID trackID, BufferIndex fileIndex )
    : m_filePath( bufferFactory.constructLogFile( trackID, fileIndex ) )
    , m_fileMapping( m_filePath.string().c_str(), boost::interprocess::read_write )
    , m_region( m_fileMapping, boost::interprocess::read_write, 0,
                LogFileSize /*, nullptr, boost::interprocess::default_map_options*/ )

{
    m_region.advise( boost::interprocess::mapped_region::advice_sequential );
}

FileBuffer::~FileBuffer()
{
    m_region.flush( 0U, m_region.get_size() );
}

FileBufferFactory::FileBufferFactory( const boost::filesystem::path logFolderPath, bool bLoad )
    : m_logFolderPath( logFolderPath )
    , m_index( *this )
{
    if( !boost::filesystem::is_directory( m_logFolderPath ) )
    {
        if( !boost::filesystem::create_directories( m_logFolderPath ) )
        {
            THROW_RTE( "Failed to create directories for: " << m_logFolderPath.string() );
            throw std::runtime_error( "Failed to create directories" );
        }
    }

    if( bLoad )
    {
        loadIterator();
    }
    else
    {
        // create first cycle at timestamp 0
        BufferType*             pBuffer = m_index.getBuffer( IndexType::toBufferIndex( m_timestamp ) );
        const InterBufferOffset offset  = pBuffer->write( &m_iterator, IndexType::RecordSize );
        ASSERT( offset.get() == IndexType::RecordSize );
    }
}

void FileBufferFactory::loadIterator()
{
    // collect all index files in order of BufferIndex
    std::map< BufferIndex, TrackID > files;
    {
        for( boost::filesystem::directory_entry& entry : boost::filesystem::directory_iterator( m_logFolderPath ) )
        {
            const boost::filesystem::path& filePath = entry.path();
            std::string                    diskFileName;
            BufferIndex                    diskFileIndex;
            if( fromFilePath( filePath, diskFileName, diskFileIndex ) )
            {
                if( diskFileName == toName( TrackID::TOTAL ) )
                {
                    VERIFY_RTE( files.insert( { diskFileIndex, TrackID::TOTAL } ).second );
                }
            }
        }
    }

    // now iterator through all index files in order and attempt to determine the
    // first point a cycle record was not written
    BufferIndex nextFileIndex;
    bool        bFoundLastCycle = false;
    for( auto i = files.begin(), iEnd = files.end(); i != iEnd && !bFoundLastCycle; ++i )
    {
        if( i->first != nextFileIndex )
            break;
        nextFileIndex = BufferIndex( nextFileIndex.get() + 1 );

        BufferType::Ptr pBuffer = std::make_unique< BufferType >( *this, i->second, i->first );

        m_timestamp = runtime::TimeStamp{
            static_cast< runtime::TimeStamp::ValueType >( IndexType::RecordsPerFile * i->first.get() ) };
        bool bFirst = true;
        for( InterBufferOffset offset = 0; offset != InterBufferOffset{ LogFileSize }; offset += IndexType::RecordSize )
        {
            auto pRecord = reinterpret_cast< const IndexRecord* >( pBuffer->read( offset ) );
            if( *pRecord < m_iterator )
            {
                bFoundLastCycle = true;
                break;
            }
            if( bFirst )
            {
                bFirst = false;
            }
            else
            {
                m_timestamp = runtime::TimeStamp{ m_timestamp.getValue() + 1 };
            }
            m_iterator = *pRecord;
        }
    }
}

boost::filesystem::path FileBufferFactory::constructLogFile( TrackID trackID, BufferIndex fileIndex )
{
    boost::filesystem::path filePath = toFilePath( m_logFolderPath, toName( trackID ), fileIndex );
    // ensure file exists
    if( !boost::filesystem::exists( filePath ) )
    {
        boost::filesystem::ensureFoldersExist( filePath );
        std::filebuf fileBuf;
        fileBuf.open(
            filePath.string(), std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary );
        fileBuf.pubseekoff( LogFileSize - 1, std::ios_base::beg );
        fileBuf.sputc( 0 );
    }
    else
    {
        // check size...
    }
    return filePath;
}
} // namespace mega::event::impl
