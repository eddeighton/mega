
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

#include "log/log.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/filesystem.hpp>

#include <string>
#include <optional>
#include <map>

namespace mega::log
{
using namespace impl;

boost::filesystem::path File::constructLogFile( const boost::filesystem::path& logFolderPath,
                                                const std::string& strFileType, FileIndex fileIndex )
{
    boost::filesystem::path filePath = toFilePath( logFolderPath, strFileType, fileIndex );
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

File::File( const boost::filesystem::path& logFolderPath, const std::string& strFileType, FileIndex fileIndex )
    : m_filePath( constructLogFile( logFolderPath, strFileType, fileIndex ) )
    , m_fileMapping( m_filePath.string().c_str(), boost::interprocess::read_write )
    , m_region( m_fileMapping, boost::interprocess::read_write, 0,
                LogFileSize /*, nullptr, boost::interprocess::default_map_options*/ )
    , m_iterator( 0 )

{
    m_region.advise( boost::interprocess::mapped_region::advice_sequential );
}

File::~File()
{
    m_region.flush( 0U, m_region.get_size() );
}

const void* File::read( InterFileOffset offset ) const
{
    return reinterpret_cast< char* >( m_region.get_address() ) + offset.get();
}

bool File::fit( U64 size ) const
{
    return m_iterator.get() + size <= LogFileSize;
}

InterFileOffset File::write( const void* pData, U64 size )
{
    std::memcpy( reinterpret_cast< char* >( m_region.get_address() ) + m_iterator.get(),
                 reinterpret_cast< const char* >( pData ), size );
    m_iterator = InterFileOffset{ m_iterator.get() + size };
    return m_iterator;
}

void File::terminate()
{
    if( m_iterator.get() + sizeof( U64 ) <= LogFileSize )
    {
        static const U64 nullsize = 0U;
        write( &nullsize, sizeof( U64 ) );
    }
}
bool File::isTerminate() const
{
    return ( ( LogFileSize - m_iterator.get() ) < sizeof( U64 ) )
           || ( ( *reinterpret_cast< U64* >( m_region.get_address() ) + m_iterator.get() ) == 0U );
}

FileSequence::FileSequence( const boost::filesystem::path& folderPath, const std::string& strName )
    : m_folderPath( folderPath )
    , m_strName( strName )
{
}

File* FileSequence::getFile( FileIndex fileIndex )
{
    auto iFind = m_files.find( fileIndex );
    if( iFind != m_files.end() )
    {
        return iFind->second.get();
    }

    // create new file
    {
        File::Ptr pFile = std::make_unique< File >( m_folderPath, m_strName, fileIndex );
        auto      ib    = m_files.insert( { fileIndex, std::move( pFile ) } );
        VERIFY_RTE( ib.second );
        return ib.first->second.get();
    }
}

const File* FileSequence::getFile( FileIndex fileIndex ) const
{
    auto iFind = m_files.find( fileIndex );
    if( iFind != m_files.end() )
    {
        return iFind->second.get();
    }
    // create new file
    {
        File::Ptr pFile = std::make_unique< File >( m_folderPath, m_strName, fileIndex );
        auto      ib    = m_files.insert( { fileIndex, std::move( pFile ) } );
        VERIFY_RTE( ib.second );
        return ib.first->second.get();
    }
}

FileSequence::~FileSequence() = default;

const char* Index::INDEX_TRACE_NAME = "index";

Storage::Storage( const boost::filesystem::path& folderPath, bool bLoad )
    : m_folderPath( folderPath )
    , m_index( m_folderPath )
    , m_tracks{ Track{ m_folderPath, TrackType( 0 ) }, Track{ m_folderPath, TrackType( 1 ) },
                Track{ m_folderPath, TrackType( 2 ) }, Track{ m_folderPath, TrackType( 3 ) },
                Track{ m_folderPath, TrackType( 4 ) }, Track{ m_folderPath, TrackType( 5 ) },
                Track{ m_folderPath, TrackType( 6 ) }, Track{ m_folderPath, TrackType( 7 ) } }
    , m_timestamp( 0U )
{
    if( !boost::filesystem::is_directory( m_folderPath ) )
    {
        if( !boost::filesystem::create_directories( m_folderPath ) )
        {
            THROW_RTE( "Failed to create directories for: " << m_folderPath.string() );
            throw std::runtime_error( "Failed to create directories" );
        }
    }

    if( bLoad )
    {
        // collect all index files in order of FileIndex
        std::map< FileIndex, std::string > files;
        {
            for( boost::filesystem::directory_entry& entry : boost::filesystem::directory_iterator( m_folderPath ) )
            {
                const boost::filesystem::path& filePath = entry.path();
                std::string                    diskFileName;
                FileIndex                      diskFileIndex;
                if( fromFilePath( filePath, diskFileName, diskFileIndex ) )
                {
                    if( diskFileName == Index::INDEX_TRACE_NAME )
                    {
                        ASSERT( files.insert( { diskFileIndex, diskFileName } ).second );
                    }
                }
            }
        }

        // now iterator through all index files in order and attempt to determine the
        // first point a cycle record was not written
        FileIndex nextFileIndex;
        bool      bFoundLastCycle = false;
        for( auto i = files.begin(), iEnd = files.end(); i != iEnd && !bFoundLastCycle; ++i )
        {
            if( i->first != nextFileIndex )
                break;
            nextFileIndex = FileIndex( nextFileIndex.get() + 1 );

            File::Ptr pFile = std::make_unique< File >( m_folderPath, i->second, i->first );

            m_timestamp = Index::RecordsPerFile * i->first.get();
            for( InterFileOffset offset = 0; offset != InterFileOffset{ LogFileSize }; offset += Index::RecordSize )
            {
                auto pRecord = reinterpret_cast< const IndexRecord* >( pFile->read( offset ) );
                if( *pRecord < m_iterator )
                {
                    bFoundLastCycle = true;
                    break;
                }
                ++m_timestamp;
                m_iterator = *pRecord;
            }
        }
    }
    else
    {
        // create first cycle at timestamp 0
        File*                 pFile  = m_index.getFile( m_index.toFileIndex( m_timestamp ) );
        const InterFileOffset offset = pFile->write( &m_iterator, Index::RecordSize );
        ASSERT( offset.get() == Index::RecordSize );
    }
}

Storage::~Storage()
{
    //
}

void Storage::cycle()
{
    ++m_timestamp;
    File*                 pFile  = m_index.getFile( m_index.toFileIndex( m_timestamp ) );
    const InterFileOffset offset = pFile->write( &m_iterator, Index::RecordSize );
    ASSERT( offset.get() % Index::RecordSize == 0U );
    // this looks wierd but is correct - the timestamp record has identity that matches
    // the record start position within file / Index::RecordSize.
    // So if 4 records per file then timestamp 4 would be first record in second file
    // such that ( 4 % 4 ) + 1 == end of timestamp 4's data in second file
    ASSERT( ( m_timestamp % Index::RecordsPerFile ) + 1 == ( offset.get() / Index::RecordSize ) );
}

Offset Storage::get( TrackType track ) const
{
    //
    return m_iterator.get( track );
}

Offset Storage::get( TrackType track, TimeStamp timestamp ) const
{
    if( timestamp <= m_timestamp )
    {
        const File* pFile   = m_index.getFile( m_index.toFileIndex( timestamp ) );
        const void* pData   = pFile->read( ( timestamp % Index::RecordsPerFile ) * Index::RecordSize );
        auto        pRecord = reinterpret_cast< const IndexRecord* >( pData );
        return pRecord->get( track );
    }
    else
    {
        return get( track );
    }
}

} // namespace mega::log
