#ifndef IO_FILEINFO_28_MARCH_2022
#define IO_FILEINFO_28_MARCH_2022

#include "object.hpp"

#include "boost/filesystem/path.hpp"

#include <optional>

namespace mega
{
namespace io
{
    class FileInfo
    {
    public:
        // clang-format off
            enum Type
            {
                #define FILE_TYPE(filetype) filetype,
                #include "file_types.hxx"
                #undef FILE_TYPE
                TOTAL_FILE_TYPES
            };
        // clang-format on

        Type                                     m_fileType;
        Object::FileID                           m_fileID;
        boost::filesystem::path                  m_filePath;
        std::optional< boost::filesystem::path > m_objectSourceFilePath;

        inline bool operator==( const FileInfo& cmp ) const
        {
            if ( this == &cmp )
                return true;

            // clang-format off
            return  m_fileType                 == cmp.m_fileType && 
                    m_fileID                   == cmp.m_fileID && 
                    m_filePath                 == cmp.m_filePath && 
                    m_objectSourceFilePath     == cmp.m_objectSourceFilePath;
            // clang-format on
        }
    };

    std::ostream& operator<<( std::ostream& os, const FileInfo& fileInfo );
    std::istream& operator>>( std::istream& is, FileInfo& fileInfo );


} // namespace io
} // namespace mega

#endif // IO_FILEINFO_28_MARCH_2022
