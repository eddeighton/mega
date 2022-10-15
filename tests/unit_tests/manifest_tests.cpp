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

#include "database/model/file_info.hxx"
#include "database/model/manifest.hxx"

#include "database/common/serialisation.hpp"

#include <gtest/gtest.h>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <utility>
#include <sstream>

using namespace mega::io;

TEST( MegaManifest, Manifest_Empty )
{
    Manifest manifestFrom;

    std::stringstream ss;
    {
        boost::archive::xml_oarchive oa( ss );
        oa&                          boost::serialization::make_nvp( "manifest", manifestFrom );
    }

    Manifest manifestTo;
    {
        boost::archive::xml_iarchive ia( ss );
        ia&                          boost::serialization::make_nvp( "manifest", manifestTo );
    }

    std::vector< FileInfo > fileInfosFrom, fileInfosTo;

    ASSERT_TRUE( manifestFrom.getCompilationFileInfos().empty() );
    ASSERT_TRUE( manifestFrom.getMegaSourceFiles().empty() );
}
