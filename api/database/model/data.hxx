#ifndef DATABASE_DATA_GUARD_4_APRIL_2022
#define DATABASE_DATA_GUARD_4_APRIL_2022

#include "database/io/object_info.hpp"
#include "database/io/object.hpp"
#include "database/io/loader.hpp"
#include "database/io/storer.hpp"

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
    struct Context;
    struct Root;
}
namespace Body
{
    struct Context;
}
namespace Tree
{
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
            Type = 1
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
            Type = 2
        } size_t;

        bool isConst;
        std::string identifier;
        std::string type;

        void* pView = nullptr;

        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Context : public mega::io::Object
    {
        Context( const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 3
        } size_t;

        std::string identifier;

        Body::Context* pBody_Context;
        void* pView = nullptr;

        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Root : public mega::io::Object
    {
        Root( const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 5
        } size_t;


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
            Type = 4
        } size_t;

        std::string body;

        AST::Context* pAST_Context;
        void* pView = nullptr;

        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
}
namespace Tree
{
    struct Root : public mega::io::Object
    {
        Root( const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 6
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

}
#endif //DATABASE_DATA_GUARD_4_APRIL_2022
