#ifndef INDEXED_FILE_25_MAR_2022
#define INDEXED_FILE_25_MAR_2022

#include "object.hpp"
#include "factory.hpp"

namespace mega
{
namespace io
{
    class File
    {
        friend class Loader;

    public:
        using FileEntry = std::pair< boost::filesystem::path, Object::FileID >;
        using FileTable = std::vector< FileEntry >;
        using FileIDToFileMap = std::map< Object::FileID, File* >;
        using FileIDtoPathMap = std::map< Object::FileID, boost::filesystem::path >;

        File( const boost::filesystem::path& filePath, Object::FileID fileID );

        static Object::FileID readFileID( const boost::filesystem::path& filePath );

        static void load( Factory& factory, FileIDToFileMap& fileMap,
                          const boost::filesystem::path& filePath, Object::FileID fileID );

        static void store( const boost::filesystem::path& filePath, Object::FileID fileID,
                           const FileIDtoPathMap& files, const Object::Array& objects );

        const boost::filesystem::path& getPath() const { return m_filePath; }
        const Object::FileID           getFileID() const { return m_fileID; }
        const FileTable&               getFiles() const { return m_files; }
        const Object::Array&           getObjects() const { return m_objects; }

        Object::Array& getObjects() { return m_objects; }

    private:
        static void beginLoadingFile( Factory& factory, FileIDToFileMap& fileMap,
                                      const boost::filesystem::path& filePath, Object::FileID fileID );
        void        beginLoad( Factory& factory, FileIDToFileMap& fileMap );
        void        endLoad();

    private:
        const boost::filesystem::path m_filePath;
        const Object::FileID          m_fileID;
        FileTable                     m_files;
        Object::Array                 m_objects;

        std::shared_ptr< Loader > m_pLoader;
    };

} // namespace io
} // namespace mega

#endif // INDEXED_FILE_25_MAR_2022
