#ifndef COMPONENT_30_MAR_2022
#define COMPONENT_30_MAR_2022

#include "database/io/stages.hpp"
#include "database/io/object.hpp"
#include "database/io/loader.hpp"
#include "database/io/storer.hpp"
#include "database/io/component_info.hpp"

#include "database/model/objects.hpp"

namespace mega
{
class Component : public io::FileObject< io::file::Component >
{
    friend class io::File;
    friend class io::Factory;
    using Base = io::FileObject< io::file::Component >;

protected:
    Component( const io::Object& object );

public:
    Component( const io::Object&        object,
               const io::ComponentInfo& componentInfo );

    static const ObjectType Type = eComponent;

    virtual void load( io::Loader& loader );
    virtual void store( io::Storer& storer ) const;

    const std::string&                            getStrName() const { return m_info.getName(); }
    const boost::filesystem::path&                getDirectory() const { return m_info.getDirectory(); }
    const std::vector< boost::filesystem::path >& getIncludeDirectories() const { return m_info.getIncludeDirectories(); }
    const std::vector< boost::filesystem::path >& getSourceFiles() const { return m_info.getSourceFiles(); }

private:
    io::ComponentInfo m_info;
};

} // namespace mega

#endif // COMPONENT_30_MAR_2022
