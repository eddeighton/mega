#ifndef IO_FILEINFO_28_MARCH_2022
#define IO_FILEINFO_28_MARCH_2022

#include "database/io/object_info.hpp"
#include "object.hpp"
#include "database/io/archive.hpp"

#include "boost/filesystem/path.hpp"

#include <optional>

namespace mega
{
namespace io
{
    class FileInfo
    {
        using OptionalPath = std::optional< boost::filesystem::path >;

    public:
        // clang-format off
        enum Stage
        {
#define STAGE_TYPE( stagetype ) stagetype,
#include "stage_types.hxx"
#undef STAGE_TYPE
            TOTAL_STAGE_TYPES
        };
        // clang-format on

        // clang-format off
        // clang-format on
        enum Type
        {
#define FILE_TYPE( filetype, stage ) filetype,
#include "file_types_global.hxx"
#include "file_types_object.hxx"
#undef FILE_TYPE
            TOTAL_FILE_TYPES
        };

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

    public:
        FileInfo();

        FileInfo( Type                           fileType,
                  ObjectInfo::FileID             fileID,
                  const boost::filesystem::path& filePath );

        FileInfo( Type                           fileType,
                  ObjectInfo::FileID             fileID,
                  const boost::filesystem::path& filePath,
                  const boost::filesystem::path& objectSourceFilePath );

        Type                           getFileType() const { return m_fileType; }
        ObjectInfo::FileID             getFileID() const { return m_fileID; }
        const boost::filesystem::path& getFilePath() const { return m_filePath; }
        const OptionalPath& getObjectSourceFilePath() const { return m_objectSourceFilePath; }

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& boost::serialization::make_nvp( "fileType", m_fileType );
            archive& boost::serialization::make_nvp( "fileID", m_fileID );
            archive& boost::serialization::make_nvp( "filePath", m_filePath );
            archive& boost::serialization::make_nvp(
                "objectSourceFilePath", m_objectSourceFilePath );
        }

    private:
        Type                    m_fileType;
        ObjectInfo::FileID      m_fileID;
        boost::filesystem::path m_filePath;
        OptionalPath            m_objectSourceFilePath;
    };

} // namespace io
} // namespace mega

#endif // IO_FILEINFO_28_MARCH_2022
