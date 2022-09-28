




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
