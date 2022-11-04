
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

#ifndef GUARD_2022_November_03_bin_archive
#define GUARD_2022_November_03_bin_archive

#include "mega/reference.hpp"
#include "mega/snapshot.hpp"

#include <boost/archive/binary_iarchive_impl.hpp>
#include <boost/archive/binary_oarchive_impl.hpp>
#include <boost/archive/detail/register_archive.hpp>
#include <boost/archive/impl/basic_binary_iprimitive.ipp>
#include <boost/archive/impl/basic_binary_oprimitive.ipp>

#include <boost/interprocess/streams/vectorstream.hpp>

#include <string>
#include <vector>

namespace boost::archive
{

class BOOST_SYMBOL_VISIBLE SnapshotIArchive
    : public binary_iarchive_impl< SnapshotIArchive, std::istream::char_type, std::istream::traits_type >
{
    using base
        = boost::archive::binary_iarchive_impl< SnapshotIArchive, std::istream::char_type, std::istream::traits_type >;

    friend class detail::common_oarchive< SnapshotIArchive >;
    friend class basic_xml_oarchive< SnapshotIArchive >;
    friend class boost::archive::load_access;

public:
    SnapshotIArchive( std::streambuf& bsb, const mega::Snapshot& snapshot )
        : base( bsb, no_header )
        , m_shapshot( snapshot )
    {
    }

    template< typename T >
    inline void load( T& value )
    {
        base::load( value );
    }

    inline void load( mega::reference& ref ) 
    {
        mega::Snapshot::Index index;
        base::load( index );
        ref = m_shapshot.indexToRef( index );
    }

private:
    const mega::Snapshot& m_shapshot;
};

class BOOST_SYMBOL_VISIBLE SnapshotOArchive
    : public binary_oarchive_impl< SnapshotOArchive, std::ostream::char_type, std::ostream::traits_type >
{
    using base
        = boost::archive::binary_oarchive_impl< SnapshotOArchive, std::ostream::char_type, std::ostream::traits_type >;

    friend class detail::common_oarchive< SnapshotOArchive >;
    friend class basic_xml_oarchive< SnapshotOArchive >;
    friend class boost::archive::save_access;

public:
    SnapshotOArchive( std::streambuf& bsb )
        : base( bsb, no_header )
    {
    }

    template< typename T >
    inline void save( const T& value )
    {
        base::save( value );
    }

    inline void save( const mega::reference& ref )
    {
        base::save( m_shapshot.refToIndex( ref ) );
    }

    mega::Snapshot& getSnapshot() { return m_shapshot; }

private:
    mega::Snapshot m_shapshot;
};

} // namespace boost::archive
BOOST_SERIALIZATION_REGISTER_ARCHIVE( boost::archive::SnapshotIArchive )
BOOST_SERIALIZATION_USE_ARRAY_OPTIMIZATION( boost::archive::SnapshotIArchive )
BOOST_SERIALIZATION_REGISTER_ARCHIVE( boost::archive::SnapshotOArchive )
BOOST_SERIALIZATION_USE_ARRAY_OPTIMIZATION( boost::archive::SnapshotOArchive )

namespace boost::serialization
{

inline void serialize( boost::archive::SnapshotIArchive& ar, mega::reference& value, const unsigned int version )
{
    ar.load( value );
}

inline void serialize( boost::archive::SnapshotOArchive& ar, mega::reference& value, const unsigned int version )
{
    ar.save( value );
}
}

namespace mega
{

class BinLoadArchive
{
public:
    using Buffer = std::vector< char >;

    BinLoadArchive( const Snapshot& snapshot )
        : m_shapshot( snapshot )
        , m_iVecStream( m_shapshot.buffer() )
        , m_archive( m_iVecStream, m_shapshot )
    {
    }

    template < typename T >
    inline void load( T& value )
    {
        m_archive& value;
    }

private:
    const Snapshot&                                m_shapshot;
    boost::interprocess::basic_vectorbuf< Buffer > m_iVecStream;
    boost::archive::SnapshotIArchive               m_archive;
};

class BinSaveArchive
{
public:
    using Buffer = std::vector< char >;

    BinSaveArchive()
        : m_archive( m_oVecStream )
    {
    }

    template < typename T >
    inline void save( T& value )
    {
        m_archive& value;
    }

    inline const Snapshot& makeSnapshot( TimeStamp timestamp )
    {
        Snapshot& snapshot = m_archive.getSnapshot();
        snapshot.timestamp( timestamp );
        snapshot.buffer( m_oVecStream.vector() );
        return snapshot;
    }

private:
    boost::interprocess::basic_vectorbuf< Buffer > m_oVecStream;
    boost::archive::SnapshotOArchive               m_archive;
};

} // namespace mega

#endif // GUARD_2022_November_03_bin_archive
