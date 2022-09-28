


#ifndef ARCHIVE_ENVIRONMENT_18_JUNE_2022
#define ARCHIVE_ENVIRONMENT_18_JUNE_2022

#include "database/model/environment.hxx"

#include "database/common/archive.hpp"

namespace mega
{
namespace io
{

class ArchiveEnvironment : public Environment
{
    ReadArchive m_fileArchive;

public:
    ArchiveEnvironment( const boost::filesystem::path& archiveFilePath );

    // FileSystem
    virtual std::unique_ptr< std::istream > read( const BuildFilePath& filePath ) const;
    virtual std::unique_ptr< std::ostream > write_temp( const BuildFilePath&     filePath,
                                                        boost::filesystem::path& tempFilePath ) const;
    virtual void                            temp_to_real( const BuildFilePath& filePath ) const;

    virtual std::unique_ptr< std::istream > read( const SourceFilePath& filePath ) const;
    virtual std::unique_ptr< std::ostream > write_temp( const SourceFilePath&    filePath,
                                                        boost::filesystem::path& tempFilePath ) const;
    virtual void                            temp_to_real( const SourceFilePath& filePath ) const;
};

} // namespace io
} // namespace mega

#endif // ARCHIVE_ENVIRONMENT_18_JUNE_2022
