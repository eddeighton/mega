#include "database/io/manifest.hpp"
#include "database/io/component.hpp"
#include "database/io/environment.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

namespace mega
{
namespace io
{

    void Manifest::constructRecurse( Component::Ptr                 pComponent,
                                     const boost::filesystem::path& sourceDirectory,
                                     const boost::filesystem::path& buildDirectory,
                                     const boost::filesystem::path& iteratorDir )
    {
        const Environment environment( sourceDirectory, buildDirectory,
                                       sourceDirectory / boost::filesystem::relative( iteratorDir, buildDirectory ),
                                       iteratorDir );

        const boost::filesystem::path sourceListingPath = environment.source_list();
        if ( boost::filesystem::exists( sourceListingPath ) )
        {
            const io::SourceListing sourceListing = io::SourceListing::load( sourceListingPath );

            if ( sourceListing.isComponent() )
            {
                Component::Ptr pParentComponent = pComponent;
                Component::Ptr pComponent = std::make_shared< Component >();
                pParentComponent->addComponent( pComponent );
            }

            std::vector< File::Info > fileInfos;
            for ( const boost::filesystem::path& sourceFilePath : sourceListing.getSourceFiles() )
            {
                File::Info fileInfo;
                fileInfo.m_fileID = Object::NO_FILE;
                fileInfo.m_fileType = File::Info::ObjectSourceFile;
                fileInfo.m_filePath = sourceFilePath;
                fileInfo.m_objectSourceFilePath = sourceFilePath;
                fileInfos.push_back( fileInfo );
            }

            pComponent->addSourceFileInfos( fileInfos );
        }

        for ( boost::filesystem::directory_iterator iter( iteratorDir );
              iter != boost::filesystem::directory_iterator();
              ++iter )
        {
            const boost::filesystem::path& filePath = *iter;
            if ( boost::filesystem::is_directory( filePath ) )
            {
                constructRecurse( pComponent, sourceDirectory, buildDirectory, filePath );
            }
        }
    }

    Manifest::Manifest()
    {
        m_pComponent = std::make_shared< Component >();
    }

    Manifest::Manifest( const boost::filesystem::path& sourceDirectory,
                        const boost::filesystem::path& buildDirectory )
    {
        const Environment environment( sourceDirectory, buildDirectory, sourceDirectory, buildDirectory );

        m_pComponent = std::make_shared< Component >();
        constructRecurse( m_pComponent, sourceDirectory, buildDirectory, buildDirectory );

        m_pComponent->addCompilationFiles( sourceDirectory, buildDirectory );
        Object::FileID fileID = 0;
        m_pComponent->labelFiles( fileID );
    }

    void Manifest::load( std::istream& is )
    {
        VERIFY_RTE( m_pComponent );
        m_pComponent->load( is );
    }

    void Manifest::save( std::ostream& os ) const
    {
        VERIFY_RTE( m_pComponent );
        m_pComponent->save( os );
    }

    Manifest::PtrCst Manifest::load( const boost::filesystem::path& filepath )
    {
        std::ifstream inputFileStream( filepath.native().c_str(), std::ios::in );
        if ( !inputFileStream.good() )
        {
            THROW_RTE( "Failed to open file: " << filepath.string() );
        }

        std::shared_ptr< Manifest > pManifest( new Manifest );
        pManifest->load( inputFileStream );
        return Manifest::PtrCst( pManifest );
    }

    void Manifest::save( const boost::filesystem::path& filepath ) const
    {
        std::ofstream outputFileStream( filepath.native().c_str(), std::ios_base::trunc | std::ios_base::out );
        if ( !outputFileStream.good() )
        {
            THROW_RTE( "Failed to write to file: " << filepath.string() );
        }
        save( outputFileStream );
    }
} // namespace io
} // namespace mega
