
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

#ifndef GUARD_2023_April_25_boost_serialization_xml_workaround
#define GUARD_2023_April_25_boost_serialization_xml_workaround

#include "boost_serialization_workaround.hpp"

#include <boost/archive/impl/basic_xml_grammar.hpp>

#include <boost/archive/impl/basic_xml_iarchive.ipp>
#include <boost/archive/impl/basic_xml_oarchive.ipp>

#include <boost/archive/impl/xml_iarchive_impl.ipp>
#include <boost/archive/impl/xml_oarchive_impl.ipp>

#include <boost/archive/basic_xml_archive.hpp>
#include <boost/archive/basic_xml_iarchive.hpp>
#include <boost/archive/basic_xml_oarchive.hpp>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>

#endif //GUARD_2023_April_25_boost_serialization_xml_workaround
