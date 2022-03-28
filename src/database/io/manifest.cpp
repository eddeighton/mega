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

    void Manifest::removeManifestAndSourceListingFilesRecurse( const Environment&             environment,
                                                               const boost::filesystem::path& iteratorDir )
    {
        {
            const boost::filesystem::path manifestPath = environment.project_manifest();
            if ( boost::filesystem::exists( manifestPath ) )
            {
                boost::filesystem::remove( manifestPath );
            }
        }

        {
            const boost::filesystem::path sourceListingPath = environment.source_list( iteratorDir );
            if ( boost::filesystem::exists( sourceListingPath ) )
            {
                boost::filesystem::remove( sourceListingPath );
            }
        }

        for ( boost::filesystem::directory_iterator iter( iteratorDir );
              iter != boost::filesystem::directory_iterator();
              ++iter )
        {
            const boost::filesystem::path& filePath = *iter;
            if ( boost::filesystem::is_directory( filePath ) )
            {
                removeManifestAndSourceListingFilesRecurse( environment, filePath );
            }
        }
    }

    void Manifest::constructRecurse( Component::Ptr                 pComponent,
                                     const Environment&             environment,
                                     const boost::filesystem::path& buildDirIter )
    {
        const boost::filesystem::path sourceListingPath = environment.source_list( buildDirIter );
        if ( boost::filesystem::exists( sourceListingPath ) )
        {
            const io::SourceListing sourceListing = io::SourceListing::load( sourceListingPath );

            if ( sourceListing.isComponent() )
            {
                Component::Ptr pParentComponent = pComponent;
                pComponent = std::make_shared< Component >();
                if ( pParentComponent )
                    pParentComponent->addComponent( pComponent );
                else
                    m_pComponent = pComponent;
            }

            std::vector< FileInfo > fileInfos;
            for ( const boost::filesystem::path& sourceFilePath : sourceListing.getSourceFiles() )
            {
                FileInfo fileInfo;
                fileInfo.m_fileID = Object::NO_FILE;
                fileInfo.m_fileType = FileInfo::ObjectSourceFile;
                fileInfo.m_filePath = sourceFilePath;
                fileInfo.m_objectSourceFilePath = sourceFilePath;
                fileInfos.push_back( fileInfo );
            }

            if ( pComponent )
            {
                pComponent->addSourceFileInfos( fileInfos );
            }
        }

        for ( boost::filesystem::directory_iterator iter( buildDirIter );
              iter != boost::filesystem::directory_iterator();
              ++iter )
        {
            const boost::filesystem::path& buildDirIterNested = *iter;
            if ( boost::filesystem::is_directory( buildDirIterNested ) )
            {
                constructRecurse( pComponent, environment, buildDirIterNested );
            }
        }
    }

    Manifest::Manifest()
    {
        m_pComponent = std::make_shared< Component >();
    }

    Manifest::Manifest( const boost::filesystem::path& filepath )
    {
        m_pComponent = std::make_shared< Component >();
        load( filepath );
    }
    Manifest::Manifest( const Environment& environment )
    {
        constructRecurse( Component::Ptr(), environment, environment.rootBuildDir() );
        VERIFY_RTE_MSG( m_pComponent, "Could not find component in source tree" );

        m_pComponent->addCompilationFiles( environment );
        Object::FileID fileID = 0;
        m_pComponent->labelFiles( fileID );
    }

    void Manifest::collectFileInfos( std::vector< FileInfo >& fileInfos ) const
    {
        VERIFY_RTE( m_pComponent );
        m_pComponent->collectFileInfos( fileInfos );
    }

    void Manifest::load( std::istream& is )
    {
        VERIFY_RTE( m_pComponent );
        m_pComponent->load( is );
    }

    void Manifest::load( const boost::filesystem::path& filepath )
    {
        VERIFY_RTE_MSG( boost::filesystem::exists( filepath ),
                        "Failed to locate file: " << filepath.string() );
        std::ifstream inputFileStream( filepath.native().c_str(), std::ios::in );
        if ( !inputFileStream.good() )
        {
            THROW_RTE( "Failed to open file: " << filepath.string() );
        }
        load( inputFileStream );
    }

    void Manifest::save( std::ostream& os ) const
    {
        VERIFY_RTE( m_pComponent );
        m_pComponent->save( os );
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

    void Manifest::removeManifestAndSourceListingFiles( const boost::filesystem::path& sourceDirectory,
                                                        const boost::filesystem::path& buildDirectory )
    {
        const Environment environment( sourceDirectory, buildDirectory );
        removeManifestAndSourceListingFilesRecurse( environment, buildDirectory );
    }
} // namespace io
} // namespace mega
