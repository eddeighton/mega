
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

#include "mega/reference.hpp"
#include "mega/event.hpp"

#include <boost/interprocess/streams/vectorstream.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <vector>

namespace mega::log
{

class Record
{
    using Buffer = std::vector< char >;

    const reference& getReference() const { return m_reference; }
    const Buffer&    getBuffer() const { return m_buffer; }

    inline Record( reference ref )
        : m_reference( ref )
    {
    }

    template < typename T >
    inline Record( reference ref, const T& data )
        : m_reference( ref )
    {
        setBuffer( data );
    }

    template < typename T >
    inline T getBuffer() const
    {
        T                                              data;
        boost::interprocess::basic_vectorbuf< Buffer > is( m_buffer );
        boost::archive::binary_iarchive                ia( is );
        ia&                                            data;
        return data;
    }

    template < typename T >
    inline void setBuffer( const T& data )
    {
        boost::interprocess::basic_vectorbuf< Buffer > os;
        {
            boost::archive::binary_oarchive oa( os );
            oa&                             data;
        }
        m_buffer = os.vector();
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_reference;
        archive& m_buffer;
    }

    inline operator Event() const { return Event{ m_reference }; }

private:
    mega::reference m_reference;
    Buffer          m_buffer;
};

class LogMsg
{
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

    Type        m_type;
    std::string m_msg;

    inline U64 size() const { return sizeof( Type ) + sizeof( U64 ) + m_msg.size() + 1; }

    LogMsg() = default;

    LogMsg( Type type, const std::string& str )
        : m_type( type )
        , m_msg( str )
    {
    }

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
};

class LogMsgRead
{
public:
    LogMsg::Type     m_type;
    std::string_view m_msg;

    LogMsgRead( const void* pData )
    {
        const char* p = reinterpret_cast< const char* >( pData );

        const U64 size = *reinterpret_cast< const U64* >( p );
        p += sizeof( U64 );

        m_type = *reinterpret_cast< const LogMsg::Type* >( p );
        p += sizeof( LogMsg::Type );

        m_msg = std::string_view( p, size - ( sizeof( U64 ) + sizeof( LogMsg::Type ) + 1 ) );
    }

    bool operator==( const LogMsg& cmp ) const
    {
        return m_type == cmp.m_type && m_msg == cmp.m_msg;
    }
};

} // namespace mega::log

#endif // GUARD_2022_October_01_record
