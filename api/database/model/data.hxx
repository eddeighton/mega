#ifndef DATABASE_DATA_GUARD_4_APRIL_2022
#define DATABASE_DATA_GUARD_4_APRIL_2022

#include "database/io/object_info.hpp"
#include "database/io/object.hpp"
#include "database/io/loader.hpp"
#include "database/io/storer.hpp"
#include "database/io/data_pointer.hpp"
#include "database/io/object_loader.hpp"

#include "boost/filesystem/path.hpp"

#include <string>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>

namespace data
{

// declarations
namespace Components
{
    struct Component;
}
namespace AST
{
    struct Opaque;
    struct Dimension;
    struct Root;
    struct Context;
    struct FoobarRoot;
    struct TestRoot;
}
namespace Body
{
}
namespace Extra
{
}
namespace Tree
{
    struct Dimension;
    struct Root;
}
namespace DependencyAnalysis
{
}
namespace ObjectCompile
{
}
namespace ObjectAnalysis
{
}
namespace Tree
{
}
namespace Buffers
{
}
namespace Graph
{
}
namespace Invocations
{
}

// definitions
namespace Components
{
    struct Component : public mega::io::Object
    {
        Component( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Component( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& name, const boost::filesystem::path& directory, const std::vector< boost::filesystem::path >& includeDirectories, const std::vector< boost::filesystem::path >& sourceFiles);
        enum 
        {
            Type = 0
        };
        std::string name;
        boost::filesystem::path directory;
        std::vector< boost::filesystem::path > includeDirectories;
        std::vector< boost::filesystem::path > sourceFiles;
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
}
namespace AST
{
    struct Opaque : public mega::io::Object
    {
        Opaque( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Opaque( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str);
        enum 
        {
            Type = 1
        };
        std::string str;
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Dimension : public mega::io::Object
    {
        Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const bool& isConst, const std::string& identifier, const std::string& type, const std::string& test, data::Ptr< data::AST::Opaque > cppTypeName, const std::vector< data::Ptr< data::AST::Opaque > >& children1, std::map< int, data::Ptr< data::AST::Opaque > > map1, std::map< data::Ptr< data::AST::Opaque >, int > map2);
        enum 
        {
            Type = 2
        };
        bool isConst;
        std::string identifier;
        std::string type;
        std::string test;
        data::Ptr< data::AST::Opaque > cppTypeName;
        std::vector< data::Ptr< data::AST::Opaque > > children1;
        std::map< int, data::Ptr< data::AST::Opaque > > map1;
        std::map< data::Ptr< data::AST::Opaque >, int > map2;
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Root : public mega::io::Object
    {
        Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 3
        };
        Ptr< AST::Context > p_AST_Context;
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Context : public mega::io::Object
    {
        Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& identifier, const std::string& body);
        enum 
        {
            Type = 4
        };
        std::string identifier;
        std::string body;
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct FoobarRoot : public mega::io::Object
    {
        FoobarRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 5
        };
        Ptr< AST::TestRoot > p_AST_TestRoot;
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct TestRoot : public mega::io::Object
    {
        TestRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 6
        };
        Ptr< AST::Root > p_AST_Root;
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
}
namespace Body
{
}
namespace Extra
{
}
namespace Tree
{
    struct Dimension : public mega::io::Object
    {
        Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const bool& more);
        enum 
        {
            Type = 7
        };
        bool more;
        Ptr< AST::Dimension > p_AST_Dimension;
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Root : public mega::io::Object
    {
        Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 8
        };
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
}
namespace DependencyAnalysis
{
}
namespace ObjectCompile
{
}
namespace ObjectAnalysis
{
}
namespace Tree
{
}
namespace Buffers
{
}
namespace Graph
{
}
namespace Invocations
{
}

template <>
inline Ptr< Components::Component > convert( Ptr< Components::Component >& from )
{
    return from;
}

template <>
inline Ptr< AST::Opaque > convert( Ptr< AST::Opaque >& from )
{
    return from;
}

template <>
inline Ptr< AST::Dimension > convert( Ptr< AST::Dimension >& from )
{
    return from;
}

template <>
inline Ptr< AST::Root > convert( Ptr< AST::Root >& from )
{
    return from;
}

template <>
inline Ptr< AST::Context > convert( Ptr< AST::Root >& from )
{
    return from->p_AST_Context;
}

template <>
inline Ptr< AST::Context > convert( Ptr< AST::Context >& from )
{
    return from;
}

template <>
inline Ptr< AST::Root > convert( Ptr< AST::FoobarRoot >& from )
{
    return from->p_AST_TestRoot->p_AST_Root;
}

template <>
inline Ptr< AST::Context > convert( Ptr< AST::FoobarRoot >& from )
{
    return from->p_AST_TestRoot->p_AST_Root->p_AST_Context;
}

template <>
inline Ptr< AST::FoobarRoot > convert( Ptr< AST::FoobarRoot >& from )
{
    return from;
}

template <>
inline Ptr< AST::TestRoot > convert( Ptr< AST::FoobarRoot >& from )
{
    return from->p_AST_TestRoot;
}

template <>
inline Ptr< AST::Root > convert( Ptr< AST::TestRoot >& from )
{
    return from->p_AST_Root;
}

template <>
inline Ptr< AST::Context > convert( Ptr< AST::TestRoot >& from )
{
    return from->p_AST_Root->p_AST_Context;
}

template <>
inline Ptr< AST::TestRoot > convert( Ptr< AST::TestRoot >& from )
{
    return from;
}

template <>
inline Ptr< AST::Dimension > convert( Ptr< Tree::Dimension >& from )
{
    return from->p_AST_Dimension;
}

template <>
inline Ptr< Tree::Dimension > convert( Ptr< Tree::Dimension >& from )
{
    return from;
}

template <>
inline Ptr< Tree::Root > convert( Ptr< Tree::Root >& from )
{
    return from;
}


class Factory
{
public:
    static mega::io::Object* create( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
};

}
#endif //DATABASE_DATA_GUARD_4_APRIL_2022
