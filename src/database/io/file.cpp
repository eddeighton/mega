
#include "database/io/file.hpp"
#include "database/io/factory.hpp"
#include "database/io/loader.hpp"
#include "database/io/storer.hpp"

namespace mega
{
namespace io
{
    // clang-format off
    static const char* g_fileTypes[] = 
    {
        #define FILE_TYPE(filetype) #filetype,
        #include "database/io/file_types.hxx"
        #undef FILE_TYPE
    };
    // clang-format on
    
    const char* getFileTypeString( File::Info::Type fileType )
    {
        static_assert( sizeof( g_fileTypes ) / sizeof( char* ) == File::Info::TOTAL_FILE_TYPES );
        if ( ( fileType >= 0 ) && ( ( std::size_t )fileType < File::Info::TOTAL_FILE_TYPES ) )
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
                    str.clear();
                    state = 1;
                }
                break;
            case 1:
                if ( c == path_delimiter )
                    break;
                else
                    str.push_back( c );
            }
        }
    }

    std::ostream& operator<<( std::ostream& os, const File::Info& fileInfo )
    {
        std::string strObjectPath;
        if ( fileInfo.m_objectSourceFilePath.has_value() )
            strObjectPath = fileInfo.m_objectSourceFilePath.value().generic_string();

        os << getFileTypeString( fileInfo.m_fileType ) << ' '
           << fileInfo.m_fileID << ' '
           << path_delimiter << fileInfo.m_filePath.generic_string() << path_delimiter
           << path_delimiter << strObjectPath << path_delimiter;

        return os;
    }

    std::istream& operator>>( std::istream& is, File::Info& fileInfo )
    {
        std::string str;
        is >> str;

        fileInfo.m_fileType = File::Info::TOTAL_FILE_TYPES;
        for ( std::size_t sz = 0; sz != File::Info::TOTAL_FILE_TYPES; ++sz )
        {
            if ( str == g_fileTypes[ sz ] )
            {
                fileInfo.m_fileType = static_cast< File::Info::Type >( sz );
                break;
            }
        }
        VERIFY_RTE_MSG( fileInfo.m_fileType != File::Info::TOTAL_FILE_TYPES, "Failed to serialize file type" );

        is >> fileInfo.m_fileID;

        parseSurroundedString( is, str );
        fileInfo.m_filePath = str;

        parseSurroundedString( is, str );
        fileInfo.m_objectSourceFilePath = str;

        return is;
    }

    void File::preload()
    {
        m_pLoader = std::make_unique< Loader >( m_info.m_filePath );

        {
            std::size_t szNumObjects = 0U;
            m_pLoader->load( szNumObjects );
            m_objects.resize( szNumObjects );
            for ( std::size_t sz = 0U; sz < szNumObjects; ++sz )
            {
                Object object;
                object.load( *m_pLoader );

                // test the stored index is valid
                std::size_t szIndex = object.getIndex();
                VERIFY_RTE( szIndex < m_objects.size() );

                // test the object NOT already created
                Object* pObject = m_objects[ szIndex ];
                VERIFY_RTE( !pObject );

                // create the object
                pObject = Factory::create( object );

                m_objects[ szIndex ] = pObject;
            }
        }
    }

    void File::load()
    {
        VERIFY_RTE( m_pLoader );
        for ( Object* pObject : m_objects )
        {
            pObject->load( *m_pLoader );
        }
        m_pLoader.reset();
    }

    void File::store( const Manifest& manifest ) const
    {
        VERIFY_RTE( !m_pLoader );

        Storer storer( m_info.m_filePath, manifest );

        for ( Object* pObject : m_objects )
        {
            Object object = *pObject;
            object.store( storer );
        }
        for ( Object* pObject : m_objects )
        {
            pObject->store( storer );
        }
    }

    /*
    File::File( const boost::filesystem::path& filePath, Object::FileID fileID )
        : m_filePath( filePath )
        , m_fileID( fileID )
    {
    }

    Object::FileID File::readFileID( const boost::filesystem::path& filePath )
    {
        FakeObjectFactory fakeFactory;
        FileIDToFileMap   fileMap;
        Loader            loader( filePath, fakeFactory, fileMap );

        Object::FileID loadedFileID = Object::NO_FILE;
        loader.load( loadedFileID );

        return loadedFileID;
    }

    void File::beginLoad( Factory& factory, FileIDToFileMap& fileMap )
    {
        m_pLoader = std::make_shared< Loader >( m_filePath, factory, fileMap );
        Loader& loader = *m_pLoader;

        // load and sanity check the file ID
        Object::FileID loadedFileID = Object::NO_FILE;
        loader.load( loadedFileID );

        VERIFY_RTE_MSG( loadedFileID == m_fileID, "File corruption detected. File ID does not match for: " << m_filePath );

        // load the file table
        {
            std::size_t szNumFiles = 0U;
            loader.load( szNumFiles );
            for ( std::size_t sz = 0U; sz < szNumFiles; ++sz )
            {
                FileEntry fileEntry;
                loader.load( fileEntry.second );
                loader.load( fileEntry.first );
                m_files.push_back( fileEntry );
            }
        }
        // load the object table
        {
            std::size_t szNumObjects = 0U;
            loader.load( szNumObjects );
            m_objects.resize( szNumObjects );
            for ( std::size_t sz = 0U; sz < szNumObjects; ++sz )
            {
                loader.loadObject();
            }
        }

        // load the files in the file table
        for ( const FileEntry& fileEntry : m_files )
        {
            beginLoadingFile( factory, fileMap, fileEntry.first, fileEntry.second );
        }
    }

    void File::endLoad()
    {
        // actually load the objects
        VERIFY_RTE( m_pLoader );
        if ( m_pLoader )
        {
            for ( Object* pObject : m_objects )
            {
                pObject->load( *m_pLoader );
            }
            m_pLoader.reset();
        }
    }

    void File::beginLoadingFile( Factory& factory, FileIDToFileMap& fileMap,
                                 const boost::filesystem::path& filePath, Object::FileID fileID )
    {
        File::FileIDToFileMap::iterator iFind = fileMap.find( fileID );
        if ( iFind == fileMap.end() )
        {
            File* pIndexedFile = new File( filePath, fileID );
            fileMap.insert( std::make_pair( fileID, pIndexedFile ) );
            pIndexedFile->beginLoad( factory, fileMap );
        }
    }

    void File::load( Factory& factory, FileIDToFileMap& fileMap,
                     const boost::filesystem::path& filePath, Object::FileID fileID )
    {
        beginLoadingFile( factory, fileMap, filePath, fileID );
        for ( FileIDToFileMap::iterator i = fileMap.begin(),
                                        iEnd = fileMap.end();
              i != iEnd;
              ++i )
        {
            i->second->endLoad();
        }
    }

    void File::store( const boost::filesystem::path& filePath, Object::FileID fileID,
                      const FileIDtoPathMap& files, const Object::Array& objects )
    {
        Storer storer( filePath );

        // store the file ID of this file
        storer.store( fileID );

        // store the file table
        {
            storer.store( files.size() );
            for ( FileIDtoPathMap::const_iterator i = files.begin(),
                                                  iEnd = files.end();
                  i != iEnd;
                  ++i )
            {
                storer.store( i->first );
                storer.store( i->second );
            }
        }

        // store the other files
        //... actually no need

        // store the object table
        {
            storer.store( objects.size() );
            for ( const Object* pObject : objects )
            {
                storer.storeObject( pObject );
            }
        }

        // store the objects
        {
            for ( const Object* pObject : objects )
            {
                pObject->store( storer );
            }
        }
    }*/
} // namespace io
} // namespace mega
