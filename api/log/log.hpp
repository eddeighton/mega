
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

#include "offset.hpp"
#include "filename.hpp"
#include "record.hpp"

#include "mega/native_types.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <memory>

namespace mega::log
{

class File
{
public:
    using Ptr       = std::unique_ptr< File >;
    using PtrVector = std::vector< Ptr >;

    static boost::filesystem::path constructLogFile( const boost::filesystem::path& logFolderPath,
                                                     const std::string& strFileType, FileIndex fileIndex );

    File( const boost::filesystem::path& logFolderPath, const std::string& strFileType, FileIndex fileIndex );
    ~File();

    const void* read( InterFileOffset offset ) const;
    bool        fit( U64 size ) const;

    InterFileOffset write( const void* pData, U64 size );
    void            terminate();

private:
    boost::filesystem::path            m_filePath;
    boost::interprocess::file_mapping  m_fileMapping;
    boost::interprocess::mapped_region m_region;
    InterFileOffset                    m_iterator;
};

class FileSequence
{
    using FilePtrMap = std::unordered_map< FileIndex, File::Ptr, FileIndex::Hash >;

public:
    FileSequence( const boost::filesystem::path& folderPath, const std::string& strName );
    ~FileSequence();

    File*       getFile( FileIndex fileIndex );
    const File* getFile( FileIndex fileIndex ) const;

private:
    const boost::filesystem::path& m_folderPath;
    std::string                    m_strName;
    mutable FilePtrMap             m_files;
};

class Index : public FileSequence
{
public:
    using Ptr = std::unique_ptr< Index >;

    static const char* INDEX_TRACE_NAME;

    Index( const boost::filesystem::path& folderPath )
        : FileSequence( folderPath, INDEX_TRACE_NAME )
    {
    }

    static constexpr U64 RecordSize = sizeof( IndexRecord );
    static_assert( RecordSize == 64U, "Unexpected record size" );
    static constexpr auto RecordsPerFile = LogFileSize / RecordSize;

    static FileIndex toFileIndex( TimeStamp timeStamp )
    {
        return FileIndex{ static_cast< U32 >( timeStamp / RecordsPerFile ) };
    }
};

class Track : public FileSequence
{
public:
    using Ptr = std::unique_ptr< Track >;
    Track( const boost::filesystem::path& folderPath, TrackType trackType )
        : FileSequence( folderPath, toName( trackType ) )
        , m_trackType( trackType )
    {
    }

private:
    TrackType m_trackType;
};

template < typename RecordType >
class Iterator
{
    const void* get() const
    {
        if ( !m_pFile )
        {
            m_pFile = m_track.getFile( m_position );
        }
        ASSERT( m_pFile );
        const void* pData = m_pFile->read( m_position );

        // if there is no space for nullsize or nullsize is present
        if ( ( ( LogFileSize - InterFileOffset( m_position ).get() ) < sizeof( U64 ) )
             || *reinterpret_cast< const U64* >( pData ) == 0U )
        {
            // then skip to next file
            FileIndex fileIndex = m_position;
            m_position          = Offset( FileIndex{ fileIndex.get() + 1 }, InterFileOffset{} );
            m_pFile             = nullptr;
            return get();
        }
        return pData;
    }

public:
    Iterator( const Track& track )
        : m_track( track )
    {
    }
    Iterator( const Track& track, Offset offset )
        : m_track( track )
        , m_position( offset )
    {
    }

    bool operator==( const Iterator& cmp ) const { return m_position == cmp.m_position; }
    bool operator!=( const Iterator& cmp ) const { return !this->operator==( cmp ); }

    RecordType operator*() const { return RecordType( get() ); }

    RecordType operator++()
    {
        RecordType temp( get() );
        const U64* pSize = reinterpret_cast< const U64* >( get() );
        m_position       = Offset( m_position, InterFileOffset{ InterFileOffset( m_position ).get() + *pSize } );
        return temp;
    }

    RecordType operator++( int )
    {
        const U64* pSize = reinterpret_cast< const U64* >( get() );
        m_position       = Offset( m_position, InterFileOffset{ InterFileOffset( m_position ).get() + *pSize } );
        return RecordType( get() );
    }

    const Offset& position() const { return m_position; }

private:
    const Track&        m_track;
    mutable const File* m_pFile    = nullptr;
    mutable Offset      m_position = Offset{};
};

class Storage
{
    using TrackArray = std::array< Track, toInt( TrackType::TOTAL ) >;

    inline const Track& getTrack( TrackType trackType ) const
    {
        ASSERT( toInt( trackType ) < toInt( TrackType::TOTAL ) );
        return m_tracks[ toInt( trackType ) ];
    }
    inline Track& getTrack( TrackType trackType )
    {
        ASSERT( toInt( trackType ) < toInt( TrackType::TOTAL ) );
        return m_tracks[ toInt( trackType ) ];
    }

    template < typename MsgType >
    inline void write( const MsgType& msg, TrackType trackType )
    {
        Offset&   offset    = m_iterator.get( trackType );
        FileIndex fileIndex = offset;

        Track& track = getTrack( trackType );
        File*  pFile = track.getFile( fileIndex );

        if ( !pFile->fit( msg.size() ) )
        {
            pFile->terminate();
            fileIndex = FileIndex{ fileIndex.get() + 1 };
            pFile     = track.getFile( fileIndex );
        }
        ASSERT( pFile->fit( msg.size() ) );

        offset = Offset{ fileIndex, msg.template write< File >( *pFile ) };
    }

public:
    Storage( const boost::filesystem::path& folderPath, bool bLoad = false );
    ~Storage();

    void cycle();

    inline void log( const LogMsg& logMsg ) { write( logMsg, TrackType::LOG ); }
    inline void record( RecordTrackType recordType, const MemoryRecord& record )
    {
        write( record, toTrackType( recordType ) );
    }
    inline void record( SchedulerTrackType schedType, const SchedulerRecord& record )
    {
        write( record, toTrackType( schedType ) );
    }

    Offset get( TrackType track ) const;
    Offset get( TrackType track, TimeStamp timestamp ) const;

    inline Iterator< LogMsgRead > logBegin() const { return { getTrack( TrackType::LOG ) }; }
    inline Iterator< LogMsgRead > logBegin( TimeStamp timestamp ) const
    {
        return { getTrack( TrackType::LOG ), get( TrackType::LOG, timestamp ) };
    }
    inline Iterator< LogMsgRead > logEnd() const { return { getTrack( TrackType::LOG ), get( TrackType::LOG ) }; }

    inline Iterator< MemoryRecordRead > recordBegin( RecordTrackType recordType ) const
    {
        return { getTrack( toTrackType( recordType ) ) };
    }
    inline Iterator< MemoryRecordRead > recordBegin( RecordTrackType recordType, TimeStamp timestamp ) const
    {
        return { getTrack( toTrackType( recordType ) ), get( toTrackType( recordType ), timestamp ) };
    }
    inline Iterator< MemoryRecordRead > recordEnd( RecordTrackType recordType ) const
    {
        return { getTrack( toTrackType( recordType ) ), get( toTrackType( recordType ) ) };
    }

    inline Iterator< SchedulerRecordRead > schedBegin( SchedulerTrackType recordType ) const
    {
        return { getTrack( toTrackType( recordType ) ) };
    }
    inline Iterator< SchedulerRecordRead > schedBegin( SchedulerTrackType recordType, TimeStamp timestamp ) const
    {
        return { getTrack( toTrackType( recordType ) ), get( toTrackType( recordType ), timestamp ) };
    }
    inline Iterator< SchedulerRecordRead > schedEnd( SchedulerTrackType recordType ) const
    {
        return { getTrack( toTrackType( recordType ) ), get( toTrackType( recordType ) ) };
    }

    TimeStamp          getTimeStamp() const { return m_timestamp; }
    const IndexRecord& getIterator() const { return m_iterator; }

private:
    const boost::filesystem::path m_folderPath;
    Index                         m_index;
    TrackArray                    m_tracks;
    TimeStamp                     m_timestamp;
    IndexRecord                   m_iterator;
};

} // namespace mega::log

#endif // GUARD_2022_October_10_log
