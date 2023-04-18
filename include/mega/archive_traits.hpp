
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

#ifndef GUARD_2023_April_14_archive_traits
#define GUARD_2023_April_14_archive_traits

#include "bin_archive.hpp"
#include "xml_archive.hpp"
#include "record_archive.hpp"

namespace mega
{

template< typename TArchiveType >
struct ArchiveTraits;

template<>
struct ArchiveTraits< boost::archive::SnapshotIArchive >
{
    static constexpr bool is_xml = false;
};

template<>
struct ArchiveTraits< boost::archive::SnapshotOArchive >
{
    static constexpr bool is_xml = false;
};

template<>
struct ArchiveTraits< mega::BinLoadArchive >
{
    static constexpr bool is_xml = false;
};
template<>
struct ArchiveTraits< mega::BinSaveArchive >
{
    static constexpr bool is_xml = false;
};

template<>
struct ArchiveTraits< mega::RecordLoadArchive >
{
    static constexpr bool is_xml = false;
};

template<>
struct ArchiveTraits< mega::RecordSaveArchive >
{
    static constexpr bool is_xml = false;
};

template<>
struct ArchiveTraits< mega::XMLLoadArchive >
{
    static constexpr bool is_xml = true;
};

template<>
struct ArchiveTraits< mega::XMLSaveArchive >
{
    static constexpr bool is_xml = true;
};

template<>
struct ArchiveTraits< boost::archive::XMLIArchive >
{
    static constexpr bool is_xml = true;
};

template<>
struct ArchiveTraits< boost::archive::XMLOArchive >
{
    static constexpr bool is_xml = true;
};

}

#endif //GUARD_2023_April_14_archive_traits