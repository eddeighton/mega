
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

#include "service/reporters.hpp"

#include "report/url.hpp"
#include "mega/reports.hpp"

#include "common/file.hpp"

#include <sstream>

namespace mega::reports
{

Report getFileReport( const URL& url )
{
    Branch result;

    if( const auto fileOpt = report::getFile( url ); fileOpt.has_value() )
    {
        if( boost::filesystem::exists( fileOpt.value() ) )
        {
            const auto filePath = fileOpt.value();

            if( boost::filesystem::is_regular_file( filePath ) )
            {
                std::stringstream ss;
                boost::filesystem::loadAsciiFile( filePath, ss );

                std::string strLine;
                while( ss && ss.good() )
                {
                    std::getline( ss, strLine );
                    result.m_elements.push_back( Line{ strLine } );
                }
            }
            else if( boost::filesystem::is_directory( filePath ) )
            {
                // create directory listing
                for( boost::filesystem::directory_iterator iter( filePath );
                     iter != boost::filesystem::directory_iterator();
                     ++iter )
                {
                    const boost::filesystem::path& dirItem = *iter;
                    result.m_elements.push_back(
                        Line{ dirItem, report::makeFileURL( url, dirItem ) } );
                }
            }
            else
            {
                // do nothing
            }
        }
    }

    return result;
}

} // namespace mega::reports
