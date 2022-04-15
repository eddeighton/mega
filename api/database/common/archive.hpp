//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#ifndef ARCHIVE_18_04_2019
#define ARCHIVE_18_04_2019

#include "object_loader.hpp"

#include <boost/filesystem/path.hpp>

#include <boost/filesystem/path_traits.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>

#include <optional>

namespace mega
{
    using InputArchiveType = boost::archive::xml_iarchive;
    using OutputArchiveType = boost::archive::xml_oarchive;
    // using InputArchiveType = boost::archive::text_iarchive;
    // using OutputArchiveType = boost::archive::text_oarchive;
} // namespace mega

namespace boost
{
    namespace archive
    {
        class MegaIArchive : public binary_iarchive
        {
        public:
            MegaIArchive( std::istream& os, ::data::ObjectPartLoader& loader );
            std::vector< mega::io::ObjectInfo::FileID > m_fileIDLoadedToRuntime;
            ::data::ObjectPartLoader&                   m_loader;
        };
    } // namespace archive
} // namespace boost

namespace boost
{
    namespace archive
    {
        class MegaOArchive : public binary_oarchive
        {
        public:
            MegaOArchive( std::ostream& os );
        };
    } // namespace archive
} // namespace boost

namespace boost
{
    namespace serialization
    {
        template < class Archive >
        inline void serialize( Archive& ar, boost::filesystem::path& p, const unsigned int version )
        {
            std::string s;
            if ( Archive::is_saving::value )
                s = p.generic_string();
            ar& boost::serialization::make_nvp( "string", s );
            if ( Archive::is_loading::value )
                p = s;
        }

        template < class Archive >
        inline void serialize( Archive& ar, std::optional< boost::filesystem::path >& p, const unsigned int version )
        {
            std::string s;
            if ( Archive::is_saving::value )
            {
                if ( p.has_value() )
                    s = p.value().generic_string();
                else
                    s = "";
            }
            ar& boost::serialization::make_nvp( "string", s );
            if ( Archive::is_loading::value )
            {
                if ( s.empty() )
                    p = std::optional< boost::filesystem::path >();
                else
                    p = s;
            }
        }

        template < class Archive, class T >
        inline void serialize( Archive& ar, std::optional< T >& optionalValue, const unsigned int version )
        {
            if ( Archive::is_saving::value )
            {
                VERIFY_RTE( optionalValue );
                ar& optionalValue.value();
            }

            if ( Archive::is_loading::value )
            {
                T   temp;
                ar& temp;
                optionalValue = temp;
            }
        }

    } // namespace serialization
} // namespace boost

#endif // ARCHIVE_18_04_2019
