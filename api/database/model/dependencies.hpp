#ifndef DEPENDENCIES_30_MAR_2022
#define DEPENDENCIES_30_MAR_2022

#include "database/model/input.hpp"
#include "database/model/objects.hpp"

#include "database/io/stages.hpp"
#include "database/io/object.hpp"
#include "database/io/loader.hpp"
#include "database/io/storer.hpp"
#include "database/io/component_info.hpp"

namespace mega
{
class DependencyAnalysis : public io::FileObject< io::file::DependencyAnalysis >
{
    friend class io::File;
    friend class io::Factory;
    using Base = io::FileObject< io::file::DependencyAnalysis >;
    using DependencyGraph = std::multimap< const input::Root*, const input::Root*, io::CompareIndexedObjects >;

protected:
    DependencyAnalysis( const io::ObjectInfo& object );

    static const ObjectType Type = eDependencyAnalysis;

public:
    virtual void load( io::Loader& loader );
    virtual void store( io::Storer& storer ) const;

    DependencyGraph m_dependencies;
};

} // namespace mega

#endif // DEPENDENCIES_30_MAR_2022
