
#include "database/io/file.hpp"
#include "database/io/factory.hpp"
#include "database/io/loader.hpp"
#include "database/io/storer.hpp"

namespace
{
class FakeObjectFactory : public mega::io::Factory
{
public:
    virtual mega::io::Object* create( const mega::io::Object& objectSpec )
    {
        THROW_RTE( "UNREACHABLE" );
    }
};
} // namespace

namespace mega
{
namespace io
{
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
    }
} // namespace io
} // namespace mega
