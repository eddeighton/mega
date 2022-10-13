
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

#ifndef GUARD_2022_October_01_record
#define GUARD_2022_October_01_record

#include "offset.hpp"

#include "mega/reference.hpp"
#include "mega/event.hpp"

#include <vector>
#include <string_view>

namespace mega::log
{

class MemoryRecord
{
    friend class MemoryRecordRead;

public:
    inline MemoryRecord( reference ref, bool bShared, std::string_view data )
        : m_reference( ref )
        , m_bShared( bShared )
        , m_data( data )
    {
    }

    inline U64 size() const { return sizeof( m_reference ) + sizeof( U64 ) + sizeof( bool ) + m_data.size(); }

    template < typename FileType >
    auto write( FileType& file ) const
    {
        const U64 sz = size();
        file.write( &sz, sizeof( sz ) );
        file.write( &m_bShared, sizeof( bool ) );
        file.write( &m_reference, sizeof( m_reference ) );
        return file.write( m_data.data(), m_data.size() );
    }

private:
    mega::reference  m_reference;
    bool             m_bShared;
    std::string_view m_data;
};

class MemoryRecordRead
{
    MemoryRecordRead( const void* pData )
    {
        const char* p = reinterpret_cast< const char* >( pData );

        const U64 size = *reinterpret_cast< const U64* >( p ) - ( sizeof( mega::reference ) + sizeof( U64 ) );
        p += sizeof( U64 );

        m_pReference = reinterpret_cast< const mega::reference* >( p );
        p += sizeof( mega::reference );

        m_data = std::string_view{ reinterpret_cast< const char* >( p ), size };
    }

    bool operator==( const MemoryRecord& cmp ) const
    {
        return ( *m_pReference == cmp.m_reference ) && m_data == cmp.m_data;
    }

    const mega::reference& getReference() const { return *m_pReference; }
    std::string_view       getData() const { return m_data; }

    inline operator Event() const { return Event{ getReference() }; }

private:
    const mega::reference* m_pReference;
    std::string_view       m_data;
};

class SchedulerRecord
{
    friend class SchedulerRecordRead;

    using Buffer = std::vector< char >;

public:
    inline SchedulerRecord( reference ref )
        : m_reference( ref )
    {
    }

    const reference& getReference() const { return m_reference; }
    const Buffer&    getBuffer() const { return m_buffer; }
    inline U64       size() const { return sizeof( m_reference ) + sizeof( U64 ) + m_buffer.size(); }

    template < typename FileType >
    auto write( FileType& file ) const
    {
        const U64 sz = size();
        file.write( &sz, sizeof( sz ) );
        file.write( &m_reference, sizeof( m_reference ) );
        return file.write( m_buffer.data(), m_buffer.size() );
    }

private:
    mega::reference m_reference;
    Buffer          m_buffer;
};

class SchedulerRecordRead
{
    SchedulerRecordRead( const void* pData )
    {
        const char* p = reinterpret_cast< const char* >( pData );

        const U64 size = *reinterpret_cast< const U64* >( p ) - ( sizeof( mega::reference ) + sizeof( U64 ) );
        p += sizeof( U64 );

        m_pReference = reinterpret_cast< const mega::reference* >( p );
        p += sizeof( mega::reference );

        m_data = std::string_view{ reinterpret_cast< const char* >( p ), size };
    }

    bool operator==( const SchedulerRecord& cmp ) const
    {
        return ( *m_pReference == cmp.m_reference ) && ( m_data.size() == cmp.m_buffer.size() )
               && ( memcmp( m_data.data(), cmp.m_buffer.data(), m_data.size() ) == 0 );
    }

    const mega::reference& getReference() const { return *m_pReference; }
    std::string_view       getData() const { return m_data; }

private:
    const mega::reference* m_pReference;
    std::string_view       m_data;
};

class LogMsg
{
    friend class LogMsgRead;

public:
    enum Type
    {
        eTrace,
        eDebug,
        eInfo,
        eWarn,
        eError,
        eFatal
    };

    LogMsg() = default;

    LogMsg( Type type, const std::string& str )
        : m_type( type )
        , m_msg( str )
    {
    }

    inline U64 size() const { return sizeof( Type ) + sizeof( U64 ) + m_msg.size() + 1; }

    template < typename FileType >
    auto write( FileType& file ) const
    {
        const U64 sz = size();
        file.write( &sz, sizeof( sz ) );
        file.write( &m_type, sizeof( Type ) );
        file.write( m_msg.data(), m_msg.size() );
        static const char nullbyte = '\0';
        return file.write( &nullbyte, 1 );
    }

private:
    Type        m_type;
    std::string m_msg;
};

class LogMsgRead
{
public:
    LogMsgRead( const void* pData )
    {
        const char* p = reinterpret_cast< const char* >( pData );

        const U64 size = *reinterpret_cast< const U64* >( p );
        p += sizeof( U64 );

        m_type = *reinterpret_cast< const LogMsg::Type* >( p );
        p += sizeof( LogMsg::Type );

        m_msg = std::string_view( p, size - ( sizeof( U64 ) + sizeof( LogMsg::Type ) + 1 ) );
    }

    bool operator==( const LogMsg& cmp ) const { return m_type == cmp.m_type && m_msg == cmp.m_msg; }

    LogMsg::Type            getType() const { return m_type; }
    const std::string_view& getMsg() const { return m_msg; }

private:
    LogMsg::Type     m_type;
    std::string_view m_msg;
};

enum class TrackType : U8
{
    LOG,
    SIM_Writes,
    SIM_Scheduler,
    GUI_Writes,
    GUI_Scheduler,
    DEV_Writes,
    DEV_Scheduler,
    RES,
    TOTAL
};

inline constexpr typename std::underlying_type< TrackType >::type toInt( TrackType e ) noexcept
{
    return static_cast< typename std::underlying_type< TrackType >::type >( e );
}

enum class RecordTrackType : U8
{
    SIM_Writes = toInt( TrackType::SIM_Writes ),
    GUI_Writes = toInt( TrackType::GUI_Writes ),
    DEV_Writes = toInt( TrackType::DEV_Writes )
};

inline TrackType toTrackType( RecordTrackType e ) noexcept
{
    return TrackType{ static_cast< typename std::underlying_type< TrackType >::type >( e ) };
}

enum class SchedulerTrackType : U8
{
    SIM_Scheduler = toInt( TrackType::SIM_Scheduler ),
    GUI_Scheduler = toInt( TrackType::GUI_Scheduler ),
    DEV_Scheduler = toInt( TrackType::DEV_Scheduler )
};

inline TrackType toTrackType( SchedulerTrackType e ) noexcept
{
    return TrackType{ static_cast< typename std::underlying_type< TrackType >::type >( e ) };
}

inline const std::string& toName( TrackType trackType )
{
    using namespace std::string_literals;
    static const std::array< std::string, toInt( TrackType::TOTAL ) > strings
        // clang-format off
    = { 
        "LOG"s,        
        "SIM_Writes"s, 
        "SIM_Scheduler"s, 
        "GUI_Writes"s, 
        "GUI_Scheduler"s,
        "DEV_Writes"s, 
        "DEV_Scheduler"s, 
        "RES"s 
    };
    // clang-format on
    return strings[ toInt( trackType ) ];
}

struct IndexRecord
{
    static_assert( toInt( TrackType::TOTAL ) == 8, "Unexpected number of tracks" );
    std::array< Offset, toInt( TrackType::TOTAL ) > m_offsets;

    const Offset& get( TrackType track ) const { return m_offsets[ toInt( track ) ]; }
    Offset&       get( TrackType track ) { return m_offsets[ toInt( track ) ]; }

    bool operator<( const IndexRecord& cmp ) const { return m_offsets < cmp.m_offsets; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        for ( auto i = 0; i != log::toInt( log::TrackType::TOTAL ); ++i )
        {
            archive& m_offsets[ i ];
        }
    }
};

} // namespace mega::log

#endif // GUARD_2022_October_01_record
