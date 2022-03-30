#include "database/io/file_info.hpp"

namespace mega
{
namespace io
{

    FileInfo::FileInfo()
        : m_fileType( TOTAL_FILE_TYPES )
        , m_fileID( ObjectInfo::NO_FILE )
    {
    }

    FileInfo::FileInfo( Type                           fileType,
                        ObjectInfo::FileID             fileID,
                        const boost::filesystem::path& filePath )
        : m_fileType( fileType )
        , m_fileID( fileID )
        , m_filePath( filePath )
    {
    }

    FileInfo::FileInfo( Type                           fileType,
                        ObjectInfo::FileID             fileID,
                        const boost::filesystem::path& filePath,
                        const boost::filesystem::path& objectSourceFilePath )
        : m_fileType( fileType )
        , m_fileID( fileID )
        , m_filePath( filePath )
        , m_objectSourceFilePath( objectSourceFilePath )

    {
    }

} // namespace io
} // namespace mega

BOOST_CLASS_IMPLEMENTATION( mega::io::FileInfo, object_serializable )
