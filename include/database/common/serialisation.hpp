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

#include "database/common/api.hpp"
#include "object_loader.hpp"

#include "utilities/serialization_helpers.hpp"

#include <boost/filesystem/path.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <optional>
#include <type_traits>
#include <set>

namespace boost::archive
{

class EGDB_EXPORT MegaIArchive : public binary_iarchive
{
    std::set< ::mega::io::ObjectInfo* >& m_objectInfos;

public:
    MegaIArchive( ::std::istream& os, ::std::set< ::mega::io::ObjectInfo* >& objectInfos,
                  ::data::ObjectPartLoader& loader );
    void objectInfo( ::mega::io::ObjectInfo* pObjectInfo );

    ::data::ObjectPartLoader& m_loader;
};

class EGDB_EXPORT MegaOArchive : public binary_oarchive
{
    std::set< const ::mega::io::ObjectInfo* >& m_objectInfos;

public:
    MegaOArchive( ::std::ostream& os, ::std::set< const ::mega::io::ObjectInfo* >& objectInfos );
    void objectInfo( const ::mega::io::ObjectInfo* pObjectInfo );
};

} // namespace boost::archive

#endif // ARCHIVE_18_04_2019
