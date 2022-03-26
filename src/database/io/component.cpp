
#include "database/io/component.hpp"

#include <iostream>
#include <string>

namespace mega
{
namespace io
{

    Component::Component()
    {
    }

    void Component::addSourceFiles( const SourceListing::PathArray& sourceFiles )
    {
        std::copy( sourceFiles.begin(), sourceFiles.end(),
                   std::back_inserter( m_sourceFiles ) );
    }

    void Component::addComponent( Ptr pComponent )
    {
        m_components.push_back( pComponent );
    }

    void Component::load( std::istream& is )
    {
        std::string str;
        while ( std::getline( is, str ) )
        {
            if ( str == "component:" )
            {
                Component::Ptr pComponent = std::make_shared< Component >();
                m_components.push_back( pComponent );
                pComponent->load( is );
            }
            else
            {
                m_sourceFiles.push_back( str );
            }
        }
    }

    void Component::save( std::ostream& os ) const
    {
        os << "component:" << std::endl;
        for ( const boost::filesystem::path& sourceFile : m_sourceFiles )
        {
            os << sourceFile.generic_path() << std::endl;
        }

        for ( Component::Ptr pComponent : m_components )
        {
            pComponent->save( os );
        }
    }

} // namespace io
} // namespace mega