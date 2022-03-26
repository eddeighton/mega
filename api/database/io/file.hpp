#ifndef INDEXED_FILE_25_MAR_2022
#define INDEXED_FILE_25_MAR_2022

#include "object.hpp"
#include "loader.hpp"

#include <memory>
#include <optional>

namespace mega
{
namespace io
{
    class File
    {
    public:
        using Ptr = std::shared_ptr< File >;
        using PtrCst = std::shared_ptr< const File >;

        class Info
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
        };

    private:
        Info                      m_info;
        Object::Array             m_objects;
        std::unique_ptr< Loader > m_pLoader;

    public:
        File( const Info& info )
            : m_info( info )
        {
        }

        Object::FileID                                  getFileID() const { return m_info.m_fileID; }
        const boost::filesystem::path&                  getFilePath() const { return m_info.m_filePath; }
        Info::Type                                      getType() const { return m_info.m_fileType; }
        const std::optional< boost::filesystem::path >& getObjectSourceFilePath() const { return m_info.m_objectSourceFilePath; }

        void preload();
        void load();
        void store( const Manifest& manifest ) const;

        template < typename T, typename... Args >
        inline T* construct( Args... args )
        {
            T* pNewObject = new T( io::Object( T::Type, m_info.m_fileID, m_objects.size() ), args... );
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

    std::ostream& operator<<( std::ostream& os, const File::Info& fileInfo );
    std::istream& operator>>( std::istream& is, File::Info& fileInfo );

} // namespace io
} // namespace mega

#endif // INDEXED_FILE_25_MAR_2022
