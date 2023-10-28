
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

#ifndef GUARD_2023_July_31_storage
#define GUARD_2023_July_31_storage

#include "log/range.hpp"
#include "log/iterator.hpp"

#include <array>

namespace mega::log
{

template < class BufferFactory >
class Storage : public BufferFactory
{
public:
    using BufferType = typename BufferFactory::BufferType;
    using TrackType  = typename BufferFactory::TrackType;
    using IndexType  = typename BufferFactory::IndexType;

private:
    using TrackArray = std::array< TrackType, toInt( TrackID::TOTAL ) >;

    inline const TrackType& getTrack( TrackID trackID ) const
    {
        ASSERT( toInt( trackID ) < toInt( TrackID::TOTAL ) );
        return m_tracks[ toInt( trackID ) ];
    }

    inline TrackType& getTrack( TrackID trackID )
    {
        ASSERT( toInt( trackID ) < toInt( TrackID::TOTAL ) );
        return m_tracks[ toInt( trackID ) ];
    }

    template < typename RecordType >
    inline void write( const RecordType& record, TrackID trackID )
    {
        Offset&     offset      = BufferFactory::m_iterator.get( trackID );
        BufferIndex bufferIndex = offset;

        TrackType&  track   = getTrack( trackID );
        BufferType* pBuffer = track.getBuffer( bufferIndex );

        if( !pBuffer->fit( record.size() ) )
        {
            pBuffer->template terminate< RecordType >();
            bufferIndex = BufferIndex{ bufferIndex.get() + 1 };
            pBuffer     = track.getBuffer( bufferIndex );
        }
        ASSERT( pBuffer->fit( record.size() ) );

        offset = Offset{ bufferIndex, record.template write< BufferType >( *pBuffer ) };
    }

public:
    template < typename... Args >
    Storage( Args... args )
        : BufferFactory( args... )
        , m_tracks{ TrackType{ *this, TrackID( 0 ) }, TrackType{ *this, TrackID( 1 ) },
                    TrackType{ *this, TrackID( 2 ) }, TrackType{ *this, TrackID( 3 ) },
                    TrackType{ *this, TrackID( 4 ) }, TrackType{ *this, TrackID( 5 ) },
                    TrackType{ *this, TrackID( 6 ) }, TrackType{ *this, TrackID( 7 ) } }
    {
    }

public:
    // WRITE interface
    template < typename RecordType >
    inline void record( const RecordType& record )
    {
        write( record, RecordType::TRACKID );
    }

public:
    // READ interface
    Range getRange( TimeStamp timestamp ) const
    {
        if( timestamp <= BufferFactory::m_timestamp )
        {
            const IndexRecord* pStart = getIndexRecord( timestamp );
            const IndexRecord* pEnd   = getIndexRecord( BufferFactory::m_timestamp );

            return Range{ getTrackRange< Structure::Read >( pStart, pEnd ),
                          getTrackRange< Memory::Read >( pStart, pEnd ) };
        }
        else
        {
            return {};
        }
    }

    // general access
    Offset get( TrackID trackID ) const
    {
        //
        return BufferFactory::m_iterator.get( trackID );
    }

    Offset get( TrackID trackID, TimeStamp timestamp ) const
    {
        if( timestamp <= BufferFactory::m_timestamp )
        {
            return getIndexRecord( timestamp )->get( trackID );
        }
        else
        {
            return get( trackID );
        }
    }

    template < typename RecordType >
    inline Iterator< BufferFactory, RecordType > begin() const
    {
        return { getTrack( RecordType::TRACKID ) };
    }
    template < typename RecordType >
    inline Iterator< BufferFactory, RecordType > begin( const IndexRecord& position ) const
    {
        return { getTrack( RecordType::TRACKID ), position.get< RecordType::TRACKID >() };
    }
    template < typename RecordType >
    inline Iterator< BufferFactory, RecordType > begin( TimeStamp timestamp ) const
    {
        return { getTrack( RecordType::TRACKID ), get( RecordType::TRACKID, timestamp ) };
    }
    template < typename RecordType >
    inline Iterator< BufferFactory, RecordType > end() const
    {
        return { getTrack( RecordType::TRACKID ), get( RecordType::TRACKID ) };
    }

    // access current time stamp
    inline TimeStamp getTimeStamp() const { return BufferFactory::m_timestamp; }

    // access current index
    inline const IndexRecord& getIterator() const { return BufferFactory::m_iterator; }

    // get the current offset of a given track
    template < typename RecordType >
    inline const Offset& getTrackEnd() const
    {
        return BufferFactory::m_iterator.template get< RecordType::TRACKID >();
    }

private:
    template < typename RecordType >
    inline TrackRange getTrackRange( const IndexRecord* pFrom, const IndexRecord* pTo ) const
    {
        const Offset offsetStart = pFrom->get( RecordType::TRACKID );
        const Offset offsetEnd   = pTo->get( RecordType::TRACKID );

        const auto& track = getTrack( RecordType::TRACKID );

        const BufferType* pFileStart = track.getBuffer( offsetStart );
        const BufferType* pFileEnd   = track.getBuffer( offsetEnd );

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

    const IndexRecord* getIndexRecord( TimeStamp timestamp ) const
    {
        const BufferType* pBuffer
            = BufferFactory::m_index.getBuffer( BufferFactory::m_index.toBufferIndex( timestamp ) );
        const void* pData = pBuffer->read( ( timestamp % IndexType::RecordsPerFile ) * IndexType::RecordSize );
        return reinterpret_cast< const IndexRecord* >( pData );
    }

private:
    TrackArray m_tracks;
};

} // namespace mega::log

#endif // GUARD_2023_July_31_storage
