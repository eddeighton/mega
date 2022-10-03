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

#ifndef ARCHIVE_18_04_2019
#define ARCHIVE_18_04_2019

#include "object_loader.hpp"

#include "utilities/serialization_helpers.hpp"

#include "nlohmann/json.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/path_traits.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

// #include <boost/archive/text_oarchive.hpp>
// #include <boost/archive/text_iarchive.hpp>

#include <optional>
#include <type_traits>
#include <set>

namespace mega
{
using InputArchiveType  = boost::archive::xml_iarchive;
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
    std::set< mega::io::ObjectInfo* >& m_objectInfos;

public:
    MegaIArchive( std::istream& os, std::set< mega::io::ObjectInfo* >& objectInfos, ::data::ObjectPartLoader& loader );
    void objectInfo( mega::io::ObjectInfo* pObjectInfo );

    ::data::ObjectPartLoader& m_loader;
};

class MegaOArchive : public binary_oarchive
{
    std::set< const mega::io::ObjectInfo* >& m_objectInfos;

public:
    MegaOArchive( std::ostream& os, std::set< const mega::io::ObjectInfo* >& objectInfos );
    void objectInfo( const mega::io::ObjectInfo* pObjectInfo );
};
} // namespace archive

namespace filesystem
{
inline void to_json( nlohmann::json& j, const boost::filesystem::path& p )
{
    j = nlohmann::json{ { "path", p.string() } };
}
} // namespace filesystem
} // namespace boost

#endif // ARCHIVE_18_04_2019
