#ifndef DATABASE_DATA_GUARD_4_APRIL_2022
#define DATABASE_DATA_GUARD_4_APRIL_2022

#include "database/io/object_info.hpp"
#include "database/io/object.hpp"
#include "database/io/loader.hpp"
#include "database/io/storer.hpp"
#include "database/io/data_pointer.hpp"

#include "boost/filesystem/path.hpp"

#include <vector>
#include <string>
#include <cstddef>

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
    struct Context;
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
        Component( const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 0
        } size_t;

        std::string name;
        boost::filesystem::path directory;
        std::vector< boost::filesystem::path > includeDirectories;
        std::vector< boost::filesystem::path > sourceFiles;

        void* pView = nullptr;

        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
}
namespace AST
{
    struct Opaque : public mega::io::Object
    {
        Opaque( const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 0
        } size_t;

        std::string str;

        void* pView = nullptr;

        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Dimension : public mega::io::Object
    {
        Dimension( const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 0
        } size_t;

        bool isConst;
        std::string identifier;
        std::string type;

        Ptr< Tree::Dimension > p_Tree_Dimension;
        void* pView = nullptr;

        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Root : public mega::io::Object
    {
        Root( const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 1
        } size_t;


        Ptr< AST::Context > p_AST_Context;
        void* pView = nullptr;

        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Context : public mega::io::Object
    {
        Context( const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 1
        } size_t;

        std::string identifier;

        Ptr< Body::Context > p_Body_Context;
        void* pView = nullptr;

        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct FoobarRoot : public mega::io::Object
    {
        FoobarRoot( const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 2
        } size_t;


        Ptr< AST::TestRoot > p_AST_TestRoot;
        void* pView = nullptr;

        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct TestRoot : public mega::io::Object
    {
        TestRoot( const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 2
        } size_t;


        Ptr< AST::Root > p_AST_Root;
        void* pView = nullptr;

        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
}
namespace Body
{
    struct Context : public mega::io::Object
    {
        Context( const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 1
        } size_t;

        std::string body;

        Ptr< AST::Context > p_AST_Context;
        void* pView = nullptr;

        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
}
namespace Tree
{
    struct Dimension : public mega::io::Object
    {
        Dimension( const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 0
        } size_t;

        std::string foobar;

        Ptr< AST::Dimension > p_AST_Dimension;
        void* pView = nullptr;

        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Root : public mega::io::Object
    {
        Root( const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 2
        } size_t;


        void* pView = nullptr;

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
inline Ptr< Tree::Dimension > convert( Ptr< AST::Dimension >& from )
{
    return from->p_Tree_Dimension;
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
inline Ptr< Body::Context > convert( Ptr< AST::Context >& from )
{
    return from->p_Body_Context;
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
inline Ptr< Tree::Root > convert( Ptr< Tree::Root >& from )
{
    return from;
}


}
#endif //DATABASE_DATA_GUARD_4_APRIL_2022
