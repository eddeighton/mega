#include "database/io/manifest.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>

namespace mega
{
namespace io
{

    boost::filesystem::path Manifest::filepath( const boost::filesystem::path& buildDir )
    {
        return buildDir / "project_manifest.db";
    }

    void Manifest::constructRecurse( Component::Ptr pComponent, const boost::filesystem::path& directory )
    {
        const boost::filesystem::path sourceListingPath = io::SourceListing::filepath( directory );
        if ( boost::filesystem::exists( sourceListingPath ) )
        {
            const io::SourceListing sourceListing = io::SourceListing::load( sourceListingPath );

            if ( sourceListing.isComponent() )
            {
                Component::Ptr pParentComponent = pComponent;
                Component::Ptr pComponent = std::make_shared< Component >();
                pParentComponent->addComponent( pComponent );
            }

            pComponent->addSourceFiles( sourceListing.getSourceFiles() );
        }

        for ( boost::filesystem::directory_iterator iter( directory );
              iter != boost::filesystem::directory_iterator();
              ++iter )
        {
            const boost::filesystem::path& filePath = *iter;
            if ( boost::filesystem::is_directory( filePath ) )
            {
                constructRecurse( pComponent, filePath );
            }
        }
    }

    Manifest::Manifest()
    {
        m_pComponent = std::make_shared< Component >();
    }

    Manifest::Manifest( const boost::filesystem::path& buildDirectory )
    {
        m_pComponent = std::make_shared< Component >();
        constructRecurse( m_pComponent, buildDirectory );
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
