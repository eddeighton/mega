#ifndef IO_COMPONENT_25_MARCH_2022
#define IO_COMPONENT_25_MARCH_2022

#include "source_listing.hpp"
#include "file_info.hpp"
#include "file.hpp"

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

        void collectFileInfos( std::vector< FileInfo >& fileInfos ) const;

        void addSourceFileInfos( const std::vector< FileInfo >& sourceFileInfos );
        void addComponent( Ptr pComponent );
        void addCompilationFiles( const Environment& environment );
        void labelFiles( Object::FileID& fileID );

        const std::vector< FileInfo >&       getSourceFileInfos() const { return m_fileInfos; }
        const std::vector< Component::Ptr >& getComponents() const { return m_components; }

        void load( std::istream& is );
        void save( std::ostream& os ) const;

    private:
        std::vector< FileInfo >       m_fileInfos;
        std::vector< Component::Ptr > m_components;
    };
} // namespace io
} // namespace mega

#endif // IO_COMPONENT_25_MARCH_2022
