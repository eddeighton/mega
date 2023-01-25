
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

#ifndef GUARD_2022_October_31_archive
#define GUARD_2022_October_31_archive

#include "mega/reference.hpp"
#include "mega/address_table.hpp"

#include "common/file.hpp"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>

#include <string>
#include <sstream>

namespace mega
{
static constexpr auto boostXMLArchiveFlags = boost::archive::no_header | boost::archive::no_codecvt
                                             | boost::archive::no_xml_tag_checking | boost::archive::no_tracking;

class XMLSaveArchive
{
public:
    XMLSaveArchive( const boost::filesystem::path& filePath )
        : m_pFileStream( boost::filesystem::createNewFileStream( filePath ) )
        , m_archive( *m_pFileStream, boostXMLArchiveFlags )
    {
    }

    template < typename T >
    inline void save( const char* name, T& value )
    {
        m_archive& boost::serialization::make_nvp( name, value );
    }

    inline void save( const char* name, const mega::reference& ref )
    {
        if( auto indexOpt = m_table.refToIndexIfExist( ref ) )
        {
            m_archive& boost::serialization::make_nvp( name, indexOpt.value() );
        }
        else
        {
            m_archive& boost::serialization::make_nvp( name, ref );
        }
    }

    inline void save( const mega::reference& ref )
    {
        if( auto indexOpt = m_table.refToIndexIfExist( ref ) )
        {
            m_archive& boost::serialization::make_nvp( "ref", indexOpt.value() );
        }
        else
        {
            m_archive& boost::serialization::make_nvp( "ref", ref );
        }
    }

    inline void internalReference( const reference& ref ) { m_table.refToIndex( ref ); }

    void beginPart( const char* name, bool bIsObject, const reference& ref )
    {
        auto indexOpt = m_table.refToIndexIfExist( ref );
        VERIFY_RTE( indexOpt.has_value() );

        if( bIsObject )
        {
            std::ostringstream os;
            os << "<" << name << " index=\"" << indexOpt.value() << "\" >";
            m_archive.put( os.str().c_str() );
        }
        else
        {
            std::ostringstream os;
            os << "<" << name << ">";
            m_archive.put( os.str().c_str() );
        }

    }
    void endPart( const char* name, bool bIsObject, const reference& ref )
    {
        auto indexOpt = m_table.refToIndexIfExist( ref );
        VERIFY_RTE( indexOpt.has_value() );

        std::ostringstream os;
        os << "</" << name << " >";
        m_archive.put( os.str().c_str() );
    }

private:
    std::unique_ptr< std::ostream > m_pFileStream;
    boost::archive::xml_oarchive    m_archive;
    AddressTable                    m_table;
};

class XMLLoadArchive
{
public:
    XMLLoadArchive( const boost::filesystem::path& filePath )
        : m_pFileStream( boost::filesystem::loadFileStream( filePath ) )
    {
    }

    template < typename T >
    void load( const char* name, T& value )
    {
        if( m_pArchive )
        {
            m_pFileStream->seekg( 0 );
            *m_pArchive& boost::serialization::make_nvp( name, value );
        }
        else
        {
        }
    }



    void initArchive()
    {
        if( !m_pArchive )
        {
            m_pArchive = std::make_unique< boost::archive::xml_iarchive >( *m_pFileStream, boostXMLArchiveFlags );
        }
    }

private:
    std::unique_ptr< std::istream >                 m_pFileStream;
    std::unique_ptr< boost::archive::xml_iarchive > m_pArchive;
};

} // namespace mega

#endif // GUARD_2022_October_31_archive
