
#include "database/io/file.hpp"
#include "database/io/factory.hpp"
#include "database/io/loader.hpp"
#include "database/io/object_info.hpp"
#include "database/io/storer.hpp"

namespace mega
{
namespace io
{

    void File::preload( const FileAccess& fileAccess, const Manifest& manifest )
    {
        VERIFY_RTE( !m_pLoader );
        m_pLoader = std::make_unique< Loader >( manifest, fileAccess, m_info.getFilePath() );

        {
            std::size_t szNumObjects = 0U;
            m_pLoader->load( szNumObjects );
            m_objects.resize( szNumObjects );
            for ( std::size_t sz = 0U; sz < szNumObjects; ++sz )
            {
                ObjectInfo objectInfo;
                m_pLoader->load( objectInfo );

                // map the stored fileID to the runtime file ID
                // which basically means setting the fileID to this files
                objectInfo = ObjectInfo( objectInfo.getType(), getFileID(), objectInfo.getIndex() );

                // test the stored index is valid
                VERIFY_RTE( objectInfo.getIndex() < m_objects.size() );

                // test the object NOT already created
                Object* pObject = m_objects[ objectInfo.getIndex() ];
                VERIFY_RTE( !pObject );

                // create the object
                pObject = Factory::create( objectInfo );

                m_objects[ objectInfo.getIndex() ] = pObject;
            }
        }
    }

    void File::load( const Manifest& manifest )
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

        Storer storer( m_info.getFilePath(), manifest );

        storer.store( m_objects.size() );
        for ( Object* pObject : m_objects )
        {
            storer.store( pObject->getObjectInfo() );
        }
        for ( Object* pObject : m_objects )
        {
            pObject->store( storer );
        }
    }

} // namespace io
} // namespace mega
