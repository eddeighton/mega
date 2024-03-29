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

#include "common/assert_verify.hpp"
#include "common/file.hpp"
#include "common/terminal.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace
{
nlohmann::json loadJson( const boost::filesystem::path& filePath )
{
    VERIFY_RTE_MSG(
        boost::filesystem::exists( filePath ),
        common::COLOUR_RED_BEGIN << "ERROR: Failed to locate: " << filePath.string() << common::COLOUR_END );
    std::cout << "Reading: " << filePath.string() << std::endl;
    std::ifstream file( filePath.string(), std::ios_base::in );
    VERIFY_RTE_MSG( !file.fail(), "Failed to open json file: " << filePath.string() );
    return nlohmann::json::parse( std::istreambuf_iterator< char >( file ), std::istreambuf_iterator< char >() );
}
} // namespace

void run_inja( const boost::filesystem::path& jsonFilePath,
               const boost::filesystem::path& injaTemplateFilePath,
               const boost::filesystem::path& outputFilePath )
{
    try
    {
        std::ostringstream osOutput;
        {
            const boost::filesystem::path jsonFilePathAbs
                = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( jsonFilePath ) );

            const auto data = loadJson( jsonFilePathAbs );

            ::inja::Environment injaEnv;
            injaEnv.set_trim_blocks( true );

            const boost::filesystem::path injaTemplateFilePathAbs
                = boost::filesystem::edsCannonicalise( boost::filesystem::absolute( injaTemplateFilePath ) );
            VERIFY_RTE_MSG( boost::filesystem::exists( injaTemplateFilePathAbs ),
                            common::COLOUR_RED_BEGIN << "ERROR: Failed to locate: " << injaTemplateFilePath.string()
                                                     << common::COLOUR_END );

            ::inja::Template injaTemplate = injaEnv.parse_template( injaTemplateFilePathAbs.string() );
            injaEnv.render_to( osOutput, injaTemplate, data );
        }

        boost::filesystem::updateFileIfChanged( outputFilePath, osOutput.str() );
    }
    catch( std::exception& ex )
    {
        THROW_RTE( "Error processing template: " << injaTemplateFilePath.string() << " with data: "
                                                 << jsonFilePath.string() << " Error: " << ex.what() );
    }
}