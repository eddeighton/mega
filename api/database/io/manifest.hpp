#ifndef IO_MANIFEST_26_MAR_2022
#define IO_MANIFEST_26_MAR_2022

#include "component.hpp"

#include <boost/filesystem/path.hpp>

#include <memory>

namespace mega
{
namespace io
{

    class Loader;
    class Manifest
    {
    public:
        using PtrCst = std::shared_ptr< const Manifest >;

        static boost::filesystem::path filepath( const boost::filesystem::path& buildDir );

        Manifest();
        Manifest( const boost::filesystem::path& buildDirectory );

        Component::Ptr getComponent() const { return m_pComponent; }

        void load( std::istream& is );
        void save( std::ostream& os ) const;

        static Manifest::PtrCst load( const boost::filesystem::path& filepath );
        void                    save( const boost::filesystem::path& filepath ) const;

    private:
        void addComponent( Component::Ptr pComponent );
        void constructRecurse( Component::Ptr pComponent, const boost::filesystem::path& directory );

    private:
        Component::Ptr m_pComponent;
    };
} // namespace io
} // namespace mega

#endif // IO_MANIFEST_26_MAR_2022