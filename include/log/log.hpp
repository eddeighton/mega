
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
#include "index.hpp"
#include "range.hpp"

#include "records.hxx"

#include "mega/native_types.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <memory>

namespace mega::log
{
namespace impl
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

    inline const void* read( InterFileOffset offset ) const noexcept
    {
        return reinterpret_cast< char* >( m_region.get_address() ) + offset.get();
    }

    inline const void* getStart() const noexcept { return read( 0 ); }
    inline const void* getEnd() const noexcept { return read( LogFileSize ); }

    inline bool fit( U64 size ) const noexcept { return m_writePosition.get() + size <= LogFileSize; }

    inline InterFileOffset write( const void* pData, U64 size ) noexcept
    {
        std::memcpy( reinterpret_cast< char* >( m_region.get_address() ) + m_writePosition.get(),
                     reinterpret_cast< const char* >( pData ), size );
        m_writePosition = InterFileOffset{ m_writePosition.get() + size };
        return m_writePosition;
    }

    template < typename RecordType >
    void terminate() noexcept
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
    bool isTerminated() const noexcept
    {
        static_assert( !RecordType::Variable, "incorrect isTerminated function used" );
        return !fit( RecordType::size() );
    }

    template < typename RecordType >
    bool isTerminated( InterFileOffset readPosition ) const noexcept
    {
        static_assert( RecordType::Variable, "incorrect isTerminated function used" );
        return !fit( sizeof( SizeType ) ) || ( RecordType::getVariableSize( read( readPosition ) ) == 0U );
    }

private:
    boost::filesystem::path            m_filePath;
    boost::interprocess::file_mapping  m_fileMapping;
    boost::interprocess::mapped_region m_region;
    InterFileOffset                    m_writePosition;
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

    static FileIndex toFileIndex( TimeStamp timeStamp ) noexcept
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

} // namespace impl

template < typename RecordType >
class Iterator
{
    friend class Storage;

    inline const void* get() const
    {
        using namespace impl;
        if( !m_pFile )
        {
            m_pFile = m_track.getFile( m_position );
        }
        ASSERT( m_pFile );
        const void* pData = m_pFile->read( m_position );

        bool bIsTerminated = false;
        if constexpr( RecordType::Variable )
        {
            bIsTerminated = m_pFile->isTerminated< RecordType >( m_position );
        }
        else
        {
            bIsTerminated = m_pFile->isTerminated< RecordType >();
        }

        if( bIsTerminated )
        {
            // then skip to next file
            FileIndex fileIndex = m_position;
            m_position          = Offset( FileIndex{ fileIndex.get() + 1 }, InterFileOffset{} );

            m_pFile = m_track.getFile( m_position );
            ASSERT( m_pFile );
            pData = m_pFile->read( m_position );
        }
        return pData;
    }

    inline Iterator( const impl::Track& track )
        : m_track( track )
    {
    }
    inline Iterator( const impl::Track& track, Offset offset )
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
            m_position = Offset( m_position, InterFileOffset{ InterFileOffset( m_position ).get()
                                                              + RecordType::getVariableSize( get() ) } );
        }
        else
        {
            m_position
                = Offset( m_position, InterFileOffset{ InterFileOffset( m_position ).get() + RecordType::size() } );
        }
    }

    inline const Offset& position() const noexcept { return m_position; }

    const impl::File* getFile() const
    {
        ASSERT( m_pFile );
        return m_pFile;
    }

private:
    const impl::Track&        m_track;
    mutable const impl::File* m_pFile    = nullptr;
    mutable Offset            m_position = Offset{};
};

class Storage
{
    using TrackArray = std::array< impl::Track, toInt( TrackType::TOTAL ) >;

    inline const impl::Track& getTrack( TrackType trackType ) const
    {
        ASSERT( toInt( trackType ) < toInt( TrackType::TOTAL ) );
        return m_tracks[ toInt( trackType ) ];
    }
    inline impl::Track& getTrack( TrackType trackType )
    {
        ASSERT( toInt( trackType ) < toInt( TrackType::TOTAL ) );
        return m_tracks[ toInt( trackType ) ];
    }

    template < typename RecordType >
    inline void write( const RecordType& record, TrackType trackType )
    {
        using namespace impl;
        Offset&   offset    = m_iterator.get( trackType );
        FileIndex fileIndex = offset;

        Track& track = getTrack( trackType );
        File*  pFile = track.getFile( fileIndex );

        if( !pFile->fit( record.size() ) )
        {
            pFile->terminate< RecordType >();
            fileIndex = FileIndex{ fileIndex.get() + 1 };
            pFile     = track.getFile( fileIndex );
        }
        ASSERT( pFile->fit( record.size() ) );

        offset = Offset{ fileIndex, record.template write< File >( *pFile ) };
    }

public:
    Storage( const boost::filesystem::path& folderPath, bool bLoad = false );
    ~Storage();

public:
    // WRITE interface

    // per cycle update routine - generates index
    void cycle();

    template < typename RecordType >
    inline void record( const RecordType& record )
    {
        write( record, RecordType::Track );
    }

public:
    // READ interface
    Range getRange( TimeStamp timestamp ) const;

    // general access
    Offset get( TrackType track ) const;
    Offset get( TrackType track, TimeStamp timestamp ) const;

    template < typename RecordType >
    inline Iterator< RecordType > begin() const
    {
        return { getTrack( RecordType::Track ) };
    }
    template < typename RecordType >
    inline Iterator< RecordType > begin( const IndexRecord& position ) const
    {
        return { getTrack( RecordType::Track ), position.get< RecordType::Track >() };
    }
    template < typename RecordType >
    inline Iterator< RecordType > begin( TimeStamp timestamp ) const
    {
        return { getTrack( RecordType::Track ), get( RecordType::Track, timestamp ) };
    }
    template < typename RecordType >
    inline Iterator< RecordType > end() const
    {
        return { getTrack( RecordType::Track ), get( RecordType::Track ) };
    }

    // access current time stamp
    inline TimeStamp getTimeStamp() const { return m_timestamp; }

    // access current index
    inline const IndexRecord& getIterator() const { return m_iterator; }

    // get the current offset of a given track
    template < typename RecordType >
    inline const Offset& getTrackEnd() const
    {
        return m_iterator.get< RecordType::Track >();
    }

private:
    template < typename RecordType >
    inline TrackRange getTrackRange( const IndexRecord* pFrom, const IndexRecord* pTo ) const
    {
        const Offset offsetStart = pFrom->get( RecordType::Track );
        const Offset offsetEnd   = pTo->get( RecordType::Track );

        const auto& track = getTrack( RecordType::Track );

        const impl::File* pFileStart = track.getFile( offsetStart );
        const impl::File* pFileEnd   = track.getFile( offsetEnd );

        const void* pStart = pFileStart->read( offsetStart );
        const void* pEnd   = pFileEnd->read( offsetEnd );

        if( pFileStart == pFileEnd )
        {
            return { pStart, pEnd, nullptr, nullptr };
        }
        else
        {
            return { pStart, pFileStart->getEnd(), pFileEnd->getStart(), pEnd };
        }
    }

    const IndexRecord* getIndexRecord( TimeStamp timestamp ) const;

private:
    const boost::filesystem::path m_folderPath;
    impl::Index                   m_index;
    TrackArray                    m_tracks;
    TimeStamp                     m_timestamp;
    IndexRecord                   m_iterator;
};

} // namespace mega::log

#endif // GUARD_2022_October_10_log
