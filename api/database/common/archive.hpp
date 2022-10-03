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

#ifndef ARCHIVE_21_APRIL_2022
#define ARCHIVE_21_APRIL_2022

#include "database/types/sources.hpp"
#include "database/model/manifest.hxx"

#include <boost/filesystem/path.hpp>

#include <memory>

namespace mega::io
{

class ReadArchive
{
public:
    ReadArchive( const boost::filesystem::path& filePath );

    std::unique_ptr< std::istream > read( const SourceFilePath& filePath ) const;
    std::unique_ptr< std::istream > read( const BuildFilePath& filePath ) const;

    void verify();

    static void compile_archive( const boost::filesystem::path& filePath, const Manifest& manifest,
                                 const boost::filesystem::path& srcDir, const boost::filesystem::path& buildDir );

private:
    struct Pimpl;
    std::shared_ptr< Pimpl > m_pImpl;
};

} // namespace mega::io

#endif // ARCHIVE_21_APRIL_2022
