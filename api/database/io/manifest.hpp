#ifndef IO_MANIFEST_26_MAR_2022
#define IO_MANIFEST_26_MAR_2022

#include "object.hpp"

#include <boost/filesystem/path.hpp>

#include <memory>

namespace mega
{
namespace io
{
    class Component;
    class Manifest
    {
    public:
        using PtrCst = std::shared_ptr< const Manifest >;

        Manifest();
        Manifest( const boost::filesystem::path& sourceDirectory,
                  const boost::filesystem::path& buildDirectory );

        std::shared_ptr< Component > getComponent() const { return m_pComponent; }

        void load( std::istream& is );
        void save( std::ostream& os ) const;

        static void             reset( const boost::filesystem::path& sourceDirectory,
                                       const boost::filesystem::path& buildDirectory );
        static Manifest::PtrCst load( const boost::filesystem::path& filepath );
        void                    save( const boost::filesystem::path& filepath ) const;

    private:
        void        addComponent( std::shared_ptr< Component > pComponent );
        static void resetRecurse( const boost::filesystem::path& sourceDirectory,
                                  const boost::filesystem::path& buildDirectory,
                                  const boost::filesystem::path& iteratorDir );
        void        constructRecurse( std::shared_ptr< Component >   pComponent,
                                      const boost::filesystem::path& sourceDirectory,
                                      const boost::filesystem::path& buildDirectory,
                                      const boost::filesystem::path& iteratorDir );

    private:
        std::shared_ptr< Component > m_pComponent;
    };
} // namespace io
} // namespace mega

#endif // IO_MANIFEST_26_MAR_2022