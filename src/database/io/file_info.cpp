#include "database/io/file_info.hpp"

namespace mega
{
namespace io
{

    FileInfo::FileInfo()
        : m_fileType( TOTAL_FILE_TYPES )
        , m_fileID( Object::NO_FILE )
    {
    }

    FileInfo::FileInfo( Type                     fileType,
                        Object::FileID           fileID,
                        const boost::filesystem::path& filePath )
        : m_fileType( fileType )
        , m_fileID( fileID )
        , m_filePath( filePath )
    {
    }

    FileInfo::FileInfo( Type                           fileType,
                        Object::FileID                 fileID,
                        const boost::filesystem::path&       filePath,
                        const boost::filesystem::path& objectSourceFilePath )
        : m_fileType( fileType )
        , m_fileID( fileID )
        , m_filePath( filePath )
        , m_objectSourceFilePath( objectSourceFilePath )

    {
    }

    // clang-format off
    static const char* g_fileTypes[] = 
    {
        #define FILE_TYPE( filetype, stage ) #filetype,
        #include "database/io/file_types.hxx"
        #undef FILE_TYPE
    };
    // clang-format on

    const char* getFileTypeString( FileInfo::Type fileType )
    {
        static_assert( sizeof( g_fileTypes ) / sizeof( char* ) == FileInfo::TOTAL_FILE_TYPES );
        if ( ( fileType >= 0 ) && ( ( std::size_t )fileType < FileInfo::TOTAL_FILE_TYPES ) )
            return g_fileTypes[ fileType ];
        else
            return "TOTAL_FILE_TYPES";
    }

    static const char path_delimiter = '*';

    void parseSurroundedString( std::istream& is, std::string& str )
    {
        int  state = 0;
        char c;
        while ( is )
        {
            is >> c;
            switch ( state )
            {
            case 0:
                if ( c == path_delimiter )
                {
                    state = 1;
                }
                break;
            case 1:
                if ( c == path_delimiter )
                    return;
                else
                    str.push_back( c );
            }
        }
    }

    std::ostream& operator<<( std::ostream& os, const FileInfo& fileInfo )
    {
        std::string strObjectPath;
        if ( fileInfo.m_objectSourceFilePath.has_value() )
            strObjectPath = fileInfo.m_objectSourceFilePath.value().generic_string();

        os << getFileTypeString( fileInfo.m_fileType ) << ' '
           << fileInfo.m_fileID << ' '
           << path_delimiter << fileInfo.m_filePath.generic_string() << path_delimiter << ' '
           << path_delimiter << strObjectPath << path_delimiter;

        return os;
    }

    std::istream& operator>>( std::istream& is, FileInfo& fileInfo )
    {
        {
            std::string str;
            is >> str;

            fileInfo.m_fileType = FileInfo::TOTAL_FILE_TYPES;
            for ( std::size_t sz = 0; sz != FileInfo::TOTAL_FILE_TYPES; ++sz )
            {
                if ( str == g_fileTypes[ sz ] )
                {
                    fileInfo.m_fileType = static_cast< FileInfo::Type >( sz );
                    break;
                }
            }
            VERIFY_RTE_MSG( fileInfo.m_fileType != FileInfo::TOTAL_FILE_TYPES, "Failed to serialize file type" );
        }

        is >> fileInfo.m_fileID;

        {
            std::string str;
            parseSurroundedString( is, str );
            fileInfo.m_filePath = str;
        }

        {
            std::string str;
            parseSurroundedString( is, str );
            fileInfo.m_objectSourceFilePath = str;
        }

        return is;
    }

} // namespace io
} // namespace mega
