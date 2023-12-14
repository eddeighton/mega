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

#ifndef COMPILER_CONFIGURATION_21_OCT_2023
#define COMPILER_CONFIGURATION_21_OCT_2023

#include "database/directories.hpp"
#include "database/manifest_data.hpp"
#include "database/component_info.hpp"

#include "pipeline/configuration.hpp"

#include <boost/filesystem/path.hpp>

#include <vector>
#include <string>

namespace mega::compiler
{

struct Configuration
{
    pipeline::ConfigurationHeader    header;
    std::string                      projectName;
    std::vector< io::ComponentInfo > componentInfos;
    io::Directories                  directories;
    io::ManifestData                 manifestData;
    boost::filesystem::path          unityProjectDir;
    boost::filesystem::path          unityEditor;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        // NOTE: header serialization handled seperately so can access in pipeline abstraction
        // archive& boost::serialization::make_nvp( "header", header );
        archive& boost::serialization::make_nvp( "projectName", projectName );
        archive& boost::serialization::make_nvp( "componentInfos", componentInfos );
        archive& boost::serialization::make_nvp( "directories", directories );
        archive& boost::serialization::make_nvp( "manifestData", manifestData );
        archive& boost::serialization::make_nvp( "unityProjectDir", unityProjectDir );
        archive& boost::serialization::make_nvp( "unityEditor", unityEditor );
    }
};

} // namespace mega::compiler

#endif // COMPILER_CONFIGURATION_21_OCT_2023
