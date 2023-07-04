
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

#ifndef GUARD_2023_January_16_record_archive
#define GUARD_2023_January_16_record_archive

#include "mega/reference.hpp"
#include "mega/reference_io.hpp"

#include "common/assert_verify.hpp"

#include "mega/boost_serialization_workaround.hpp"

#include <boost/interprocess/streams/vectorstream.hpp>
#include <boost/interprocess/streams/bufferstream.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace boost::archive
{
class RecordIArchive
    : public binary_iarchive_impl< RecordIArchive, std::istream::char_type, std::istream::traits_type >
{
    using base
        = boost::archive::binary_iarchive_impl< RecordIArchive, std::istream::char_type, std::istream::traits_type >;

    friend class detail::common_iarchive< RecordIArchive >;
    friend class basic_binary_iarchive< RecordIArchive >;
    friend class boost::archive::load_access;

public:
    RecordIArchive( std::streambuf& bsb )
        : base( bsb, no_header )
    {
    }

    template < typename T >
    inline void load( T& value )
    {
        base::load( value );
    }
};

class RecordOArchive
    : public binary_oarchive_impl< RecordOArchive, std::ostream::char_type, std::ostream::traits_type >
{
    using base
        = boost::archive::binary_oarchive_impl< RecordOArchive, std::ostream::char_type, std::ostream::traits_type >;

    friend class detail::common_oarchive< RecordOArchive >;
    friend class basic_binary_oarchive< RecordOArchive >;
    friend class boost::archive::save_access;

public:
    RecordOArchive( std::streambuf& bsb )
        : base( bsb, no_header )
    {
    }

    template < typename T >
    inline void save( const T& value )
    {
        base::save( value );
    }

    inline void save( const mega::reference& ref ) 
    { 
        if( ref.isHeapAddress() )
        {
            base::save( ref.getNetworkAddress() );
        }
        else
        {
            base::save( ref );
        }
    }
    
};

} // namespace boost::archive

BOOST_SERIALIZATION_USE_ARRAY_OPTIMIZATION( boost::archive::RecordIArchive )
BOOST_SERIALIZATION_USE_ARRAY_OPTIMIZATION( boost::archive::RecordOArchive )

namespace boost::serialization
{
inline void serialize( boost::archive::RecordIArchive& ar, ::mega::reference& value, const unsigned int version )
{
    ar.load( value );
}

inline void serialize( boost::archive::RecordOArchive& ar, ::mega::reference& value, const unsigned int version )
{
    ar.save( value );
}
} // namespace boost::serialization

namespace mega
{

class RecordLoadArchive
{
public:
    using Buffer = std::vector< char >;

    RecordLoadArchive( const std::string_view& buffer )
        : m_bufferStream( const_cast< char* >( buffer.data() ), buffer.size() )
        , m_archive( (std::streambuf&)m_bufferStream )
    {
    }

    template < typename T >
    inline void load( T& value )
    {
        m_archive& value;
    }

private:
    boost::interprocess::bufferstream   m_bufferStream;
    boost::archive::RecordIArchive      m_archive;
};

class RecordSaveArchive
{
public:
    using Buffer = std::vector< char >;

    RecordSaveArchive()
        : m_archive( m_oVecStream )
    {
    }

    template < typename T >
    inline void save( T& value )
    {
        m_archive& value;
    }

    std::string_view get() const
    {
        return std::string_view{ m_oVecStream.vector().data(), m_oVecStream.vector().size() };
    }

private:
    boost::interprocess::basic_vectorbuf< Buffer >  m_oVecStream;
    boost::archive::RecordOArchive                  m_archive;
};

} // namespace mega

#endif //GUARD_2023_January_16_record_archive
