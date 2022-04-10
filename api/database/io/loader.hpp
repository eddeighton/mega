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

#ifndef LOADER_18_04_2019
#define LOADER_18_04_2019

#include "archive.hpp"
#include "object_info.hpp"
#include "object.hpp"
#include "manifest.hpp"
#include "data_pointer.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

#include <memory>
#include <optional>
#include <set>
#include <map>
#include <vector>

namespace mega
{
    namespace io
    {
        class Loader
        {
        public:
            Loader( const Manifest& manifest, const boost::filesystem::path& filePath, ::data::ObjectPartLoader& loader );

            template < typename T >
            void load( T& value )
            {
                m_archive& value;
            }

        private:
            const Manifest&                                m_runtimeManifest;
            std::unique_ptr< boost::filesystem::ifstream > m_pFileStream;
            boost::archive::MegaIArchive                   m_archive;
        };

    } // namespace io
} // namespace mega

#endif // LOADER_18_04_2019