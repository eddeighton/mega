
#include "database/common/file.hpp"
#include "database/common/loader.hpp"
#include "database/common/object_info.hpp"
#include "database/common/storer.hpp"

#include "database/model/data.hxx"

namespace mega
{
namespace io
{

    void File::preload( Loader& loader, const Manifest& manifest )
    {
        try
        {
            {
                std::size_t szNumObjects = 0U;
                loader.load( szNumObjects );
                m_objects.resize( szNumObjects );
                for ( std::size_t sz = 0U; sz < szNumObjects; ++sz )
                {
                    ObjectInfo objectInfo;
                    loader.load( objectInfo );

                    // map the stored fileID to the runtime file ID
                    // which basically means setting the fileID to this files
                    objectInfo = ObjectInfo( objectInfo.getType(), getFileID(), objectInfo.getIndex() );

                    // test the stored index is valid
                    VERIFY_RTE( objectInfo.getIndex() < m_objects.size() );

                    // test the object NOT already created
                    Object* pObject = m_objects[ objectInfo.getIndex() ];
                    VERIFY_RTE( !pObject );

                    // create the object
                    pObject = data::Factory::create( m_objectLoader, objectInfo );

                    m_objects[ objectInfo.getIndex() ] = pObject;
                }
            }
        }
        catch( boost::archive::archive_exception& ex )
        {
            THROW_RTE( "Exception from boost archive when loading: " << m_info.getFilePath().string() );
        }
    }

    void File::load( const Manifest& manifest )
    {
        VERIFY_RTE( !m_pLoader );
        m_pLoader = std::make_shared< Loader >( manifest, m_info.getFilePath(), m_objectLoader );
        preload( *m_pLoader, manifest );
        for ( Object* pObject : m_objects )
        {
            pObject->load( *m_pLoader );
        }
    }

    void File::load_post( const Manifest& manifest )
    {
        VERIFY_RTE( m_pLoader );
        for ( Object* pObject : m_objects )
        {
            pObject->load_post( *m_pLoader );
        }
        m_pLoader.reset();
    }

    void File::store( const Manifest& manifest ) const
    {
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
