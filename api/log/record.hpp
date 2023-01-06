
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

#include <utility>
#include <vector>
#include <string_view>
#include <string>
#include <array>

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

    inline U64 size() const { return sizeof( U64 ) + sizeof( reference ) + sizeof( bool ) + m_data.size(); }

    template < typename FileType >
    inline auto write( FileType& file ) const
    {
        const U64 sz = size();
        file.write( &sz, sizeof( sz ) );
        file.write( &m_bShared, sizeof( bool ) );
        file.write( &m_reference, sizeof( reference ) );
        return file.write( m_data.data(), m_data.size() );
    }

private:
    mega::reference  m_reference;
    bool             m_bShared;
    std::string_view m_data;
};

class MemoryRecordRead
{
public:
    inline MemoryRecordRead( const void* pData )
    {
        const char* p = reinterpret_cast< const char* >( pData );

        const U64 size = ( *reinterpret_cast< const U64* >( p ) ) - ( sizeof( U64 ) + sizeof( reference ) + sizeof( bool ) );
        p += sizeof( U64 );

        m_pShared = reinterpret_cast< const bool* >( p );
        p += sizeof( bool );

        m_pReference = reinterpret_cast< const mega::reference* >( p );
        p += sizeof( reference );

        m_data = std::string_view{ reinterpret_cast< const char* >( p ), size };
    }

    inline bool operator==( const MemoryRecord& cmp ) const
    {
        return ( *m_pReference == cmp.m_reference ) && m_data == cmp.m_data;
    }

    inline const mega::reference& getReference() const { return *m_pReference; }
    inline std::string_view       getData() const { return m_data; }

    inline operator Event() const { return Event{ getReference() }; }

private:
    const mega::reference* m_pReference;
    const bool*            m_pShared;
    std::string_view       m_data;
};

class SchedulerRecord
{
    friend class SchedulerRecordRead;

    using Buffer = std::vector< char >;

public:
    enum Type : U8
    {
        Start,
        Stop
    };

    inline SchedulerRecord( reference ref, Type type )
        : m_reference( ref )
        , m_type( type )
    {
    }

    inline const reference& getReference() const { return m_reference; }
    inline const Type       getType() const { return m_type; }
    inline U64              size() const { return sizeof( m_reference ) + sizeof( U64 ) + sizeof( Type ); }

    template < typename FileType >
    inline auto write( FileType& file ) const
    {
        const U64 sz = size();
        file.write( &sz, sizeof( sz ) );
        file.write( &m_reference, sizeof( m_reference ) );
        return file.write( &m_type, sizeof( m_type ) );
    }

private:
    mega::reference m_reference;
    Type            m_type;
};

class SchedulerRecordRead
{
public:
    inline SchedulerRecordRead( const void* pData )
    {
        const char* p = reinterpret_cast< const char* >( pData );

        const U64 size = *reinterpret_cast< const U64* >( p ) - ( sizeof( mega::reference ) + sizeof( U64 ) );
        p += sizeof( U64 );

        m_pReference = reinterpret_cast< const mega::reference* >( p );
        p += sizeof( mega::reference );

        m_pType = reinterpret_cast< const SchedulerRecord::Type* >( p );
    }

    inline bool operator==( const SchedulerRecord& cmp ) const
    {
        return getReference() == cmp.getReference() && getType() == cmp.getType();
    }

    inline const mega::reference& getReference() const { return *m_pReference; }
    inline SchedulerRecord::Type  getType() const { return *m_pType; }

private:
    const mega::reference*       m_pReference;
    const SchedulerRecord::Type* m_pType;
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

    inline LogMsg( Type type, std::string str )
        : m_type( type )
        , m_msg( std::move( str ) )
    {
    }

    inline U64 size() const { return sizeof( Type ) + sizeof( U64 ) + m_msg.size() + 1; }

    template < typename FileType >
    inline auto write( FileType& file ) const
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
    inline LogMsgRead( const void* pData )
    {
        const char* p = reinterpret_cast< const char* >( pData );

        const U64 size = *reinterpret_cast< const U64* >( p );
        p += sizeof( U64 );

        m_type = *reinterpret_cast< const LogMsg::Type* >( p );
        p += sizeof( LogMsg::Type );

        m_msg = std::string_view( p, size - ( sizeof( U64 ) + sizeof( LogMsg::Type ) + 1 ) );
    }

    inline bool operator==( const LogMsg& cmp ) const { return m_type == cmp.m_type && m_msg == cmp.m_msg; }

    inline LogMsg::Type            getType() const { return m_type; }
    inline const std::string_view& getMsg() const { return m_msg; }

private:
    LogMsg::Type     m_type;
    std::string_view m_msg;
};

enum class TrackType : U8
{
    Log,
    Scheduler,
    Simulation,
    UserInterface,
    Analysis,
    Resources,
    Graphics,
    Audio,
    TOTAL
};

inline constexpr typename std::underlying_type< TrackType >::type toInt( TrackType e ) noexcept
{
    return static_cast< typename std::underlying_type< TrackType >::type >( e );
}

enum class MemoryTrackType : U8
{
    Simulation    = toInt( TrackType::Simulation ),
    UserInterface = toInt( TrackType::UserInterface ),
    Analysis      = toInt( TrackType::Analysis ),
    Resources     = toInt( TrackType::Resources ),
    Graphics      = toInt( TrackType::Graphics ),
    Audio         = toInt( TrackType::Audio )
};

inline TrackType toTrackType( MemoryTrackType e ) noexcept
{
    return TrackType{ static_cast< typename std::underlying_type< TrackType >::type >( e ) };
}

inline const std::string& toName( TrackType trackType )
{
    using namespace std::string_literals;
    static const std::array< std::string, toInt( TrackType::TOTAL ) > strings
        // clang-format off
    = { 
        "Log"s,
        "Scheduler"s,
        "Simulation"s,
        "UserInterface"s,
        "Analysis"s,
        "Resources"s,
        "Graphics"s,
        "Audio"s
    };
    // clang-format on
    return strings[ toInt( trackType ) ];
}

class IndexRecord
{
    static_assert( toInt( TrackType::TOTAL ) == 8, "Unexpected number of tracks" );

public:
    inline const Offset& get( TrackType track ) const { return m_offsets[ toInt( track ) ]; }
    inline Offset&       get( TrackType track ) { return m_offsets[ toInt( track ) ]; }

    inline bool operator==( const IndexRecord& cmp ) const { return m_offsets == cmp.m_offsets; }
    inline bool operator!=( const IndexRecord& cmp ) const { return m_offsets != cmp.m_offsets; }
    inline bool operator<( const IndexRecord& cmp ) const { return m_offsets < cmp.m_offsets; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        for ( auto i = 0; i != log::toInt( log::TrackType::TOTAL ); ++i )
        {
            archive& m_offsets[ i ];
        }
    }

private:
    std::array< Offset, toInt( TrackType::TOTAL ) > m_offsets;
};

} // namespace mega::log

#endif // GUARD_2022_October_01_record
