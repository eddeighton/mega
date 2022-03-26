#ifndef IO_COMPONENT_25_MARCH_2022
#define IO_COMPONENT_25_MARCH_2022

#include "source_listing.hpp"

#include <boost/filesystem/path.hpp>

#include <memory>

namespace mega
{
namespace io
{
    class Component
    {
    public:
        using Ptr = std::shared_ptr< Component >;

        Component();

        void addSourceFiles( const SourceListing::PathArray& sourceFiles );
        void addComponent( Ptr pComponent );

        const SourceListing::PathArray&      getSourceFiles() const { return m_sourceFiles; }
        const std::vector< Component::Ptr >& getComponents() const { return m_components; }

        void load( std::istream& is );
        void save( std::ostream& os ) const;

    private:
        SourceListing::PathArray      m_sourceFiles;
        std::vector< Component::Ptr > m_components;
    };
} // namespace io
} // namespace mega

#endif // IO_COMPONENT_25_MARCH_2022
