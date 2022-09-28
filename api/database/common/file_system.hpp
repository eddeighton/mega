
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



#ifndef FILE_SYSTEM_21_APRIL_2022
#define FILE_SYSTEM_21_APRIL_2022

#include "database/types/sources.hpp"

#include "common/hash.hpp"

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>

#include <memory>

namespace mega
{
namespace io
{
class FileSystem
{
public:
    virtual ~FileSystem() {}

    // file io
    virtual std::unique_ptr< std::istream > read( const BuildFilePath& filePath ) const               = 0;
    virtual std::unique_ptr< std::ostream > write_temp( const BuildFilePath&     filePath,
                                                        boost::filesystem::path& tempFilePath ) const = 0;
    virtual void                            temp_to_real( const BuildFilePath& filePath ) const       = 0;

    virtual std::unique_ptr< std::istream > read( const SourceFilePath& filePath ) const              = 0;
    virtual std::unique_ptr< std::ostream > write_temp( const SourceFilePath&    filePath,
                                                        boost::filesystem::path& tempFilePath ) const = 0;
    virtual void                            temp_to_real( const SourceFilePath& filePath ) const      = 0;
};

} // namespace io
} // namespace mega

#endif // FILE_SYSTEM_21_APRIL_2022
