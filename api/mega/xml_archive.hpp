
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

#include "common/file.hpp"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>

#include <string>

namespace mega
{

class XMLSaveArchive
{
public:
    XMLSaveArchive( const boost::filesystem::path& filePath )
        : m_pFileStream( boost::filesystem::createBinaryOutputFileStream( filePath ) )
        , m_archive( *m_pFileStream )
    {
    }

    template < typename T >
    void save( T& value, const char* name )
    {
        m_archive& boost::serialization::make_nvp( name, value );
    }

    void beginPart( const char* name )
    {
        std::ostringstream os;
        os << "<" << name << " >";
        m_archive.put( os.str().c_str() );
    }
    void endPart( const char* name )
    {
        std::ostringstream os;
        os << "</" << name << " >";
        m_archive.put( os.str().c_str() );
    }

private:
    std::unique_ptr< std::ostream > m_pFileStream;
    boost::archive::xml_oarchive    m_archive;
};

class XMLLoadArchive
{
public:
    XMLLoadArchive( const boost::filesystem::path& filePath )
        : m_pFileStream( boost::filesystem::createBinaryInputFileStream( filePath ) )
        , m_archive( *m_pFileStream )
    {
    }

    template < typename T >
    void load( T& value, const char* name )
    {
        m_archive& boost::serialization::make_nvp( name, value );
    }

    void beginPart( const char* name ) {}
    void endPart( const char* name ) {}

private:
    std::unique_ptr< std::istream > m_pFileStream;
    boost::archive::xml_iarchive    m_archive;
};

} // namespace mega

#endif // GUARD_2022_October_31_archive
