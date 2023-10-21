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

#include "environment/environment_stash.hpp"

#include "database/serialisation.hpp"
#include "database/file_header.hpp"
#include "database/object_loader.hpp"

#include <set>

namespace mega::io
{

StashEnvironment::StashEnvironment( mega::pipeline::Stash& stash, const Directories& directories )
    : BuildEnvironment( directories )
    , m_stash( stash )
{
}

bool StashEnvironment::restore( const CompilationFilePath& filePath, task::DeterminantHash hashCode ) const
{
    const Path actualPath = toPath( filePath );
    if ( m_stash.restore( actualPath, hashCode ) )
    {
        // check the file header for the correct version
        ::data::NullObjectPartLoader nullObjectPartLoader;

        std::unique_ptr< std::istream >   pFileStream = read( filePath );
        std::set< mega::io::ObjectInfo* > objectInfos;
        boost::archive::MegaIArchive      archive( *pFileStream, objectInfos, nullObjectPartLoader );

        mega::io::FileHeader fileHeader;
        archive >> fileHeader;
        if ( fileHeader.getVersion() == getVersion() )
        {
            return true;
        }
    }
    return false;
}

} // namespace mega::io
