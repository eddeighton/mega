#ifndef INDEXED_FILE_25_MAR_2022
#define INDEXED_FILE_25_MAR_2022

#include "object.hpp"
#include "loader.hpp"

#include <memory>

namespace mega
{
namespace io
{

    namespace stage
    {
        class Test
        {
        public:
        };
    } // namespace stage

    namespace file
    {
        class File
        {
        public:
            using Ptr = std::shared_ptr< File >;
            using PtrCst = std::shared_ptr< const File >;

        private:
            const boost::filesystem::path m_filePath;
            const Object::FileID          m_fileID;
            Object::Array                 m_objects;

            std::unique_ptr< Loader > m_pLoader;

        public:
            File( const boost::filesystem::path& filePath,
                  Object::FileID                 fileID )
                : m_filePath( filePath )
                , m_fileID( fileID )
            {
            }

            Object::FileID                 getFileID() const { return m_fileID; }
            const boost::filesystem::path& getFilePath() const { return m_filePath; }

            void preload();
            void load();
            void store( const Manifest& manifest ) const;

            template < typename T, typename... Args >
            inline T* construct( Args... args )
            {
                T* pNewObject = new T( io::Object( T::Type, m_fileID, m_objects.size() ), args... );
                m_objects.push_back( pNewObject );
                return pNewObject;
            }

            template < typename T, typename TFunctor >
            inline auto collect( const TFunctor& functor )
            {
                return functor( m_objects );
            }

            template < typename T, typename TFunctor >
            inline auto collect( const TFunctor& functor ) const
            {
                return functor( m_objects );
            }
        };

        class FileTypes
        {
        public:
            enum
            {
                TestFile
            };
        };

        template < class TCreatingStage >
        class StagedFile : public File
        {
        public:
            using CreatingStage = TCreatingStage;
            StagedFile( const boost::filesystem::path& filePath, Object::FileID fileID )
                : File( filePath, fileID )
            {
            }
        };

        class TestFile : public StagedFile< stage::Test >
        {
        public:
            TestFile( const boost::filesystem::path& filePath, Object::FileID fileID )
                : StagedFile( filePath, fileID )
            {
            }
        };

    } // namespace file

} // namespace io
} // namespace mega

#endif // INDEXED_FILE_25_MAR_2022
