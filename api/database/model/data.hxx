#ifndef DATABASE_DATA_GUARD_4_APRIL_2022
#define DATABASE_DATA_GUARD_4_APRIL_2022

#include "database/common/object_info.hpp"
#include "database/common/object.hpp"
#include "database/common/loader.hpp"
#include "database/common/storer.hpp"
#include "database/common/data_pointer.hpp"
#include "database/common/object_loader.hpp"

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
    struct Identifier;
    struct ScopedIdentifier;
    struct ArgumentList;
    struct ReturnType;
    struct Inheritance;
    struct Size;
    struct Dimension;
    struct Include;
    struct SystemInclude;
    struct MegaInclude;
    struct MegaIncludeInline;
    struct MegaIncludeNested;
    struct CPPInclude;
    struct Dependency;
    struct ContextDef;
    struct AbstractDef;
    struct ActionDef;
    struct EventDef;
    struct FunctionDef;
    struct ObjectDef;
    struct LinkDef;
    struct SourceRoot;
    struct IncludeRoot;
    struct ObjectSourceRoot;
}
namespace Body
{
}
namespace Tree
{
    struct ContextGroup;
    struct Root;
    struct Context;
    struct Abstract;
    struct Action;
    struct Event;
    struct Function;
    struct Object;
    struct Link;
    struct Dimension;
}
namespace Analysis
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
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
}
namespace AST
{
    struct Identifier : public mega::io::Object
    {
        Identifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Identifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str);
        enum 
        {
            Type = 1
        };
        std::string str;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct ScopedIdentifier : public mega::io::Object
    {
        ScopedIdentifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        ScopedIdentifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Identifier > >& ids);
        enum 
        {
            Type = 2
        };
        std::vector< data::Ptr< data::AST::Identifier > > ids;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct ArgumentList : public mega::io::Object
    {
        ArgumentList( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        ArgumentList( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str);
        enum 
        {
            Type = 3
        };
        std::string str;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct ReturnType : public mega::io::Object
    {
        ReturnType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        ReturnType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str);
        enum 
        {
            Type = 4
        };
        std::string str;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Inheritance : public mega::io::Object
    {
        Inheritance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Inheritance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< std::string >& strings);
        enum 
        {
            Type = 5
        };
        std::vector< std::string > strings;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Size : public mega::io::Object
    {
        Size( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Size( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str);
        enum 
        {
            Type = 6
        };
        std::string str;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Dimension : public mega::io::Object
    {
        Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const bool& isConst, const data::Ptr< data::AST::Identifier >& id, const std::string& type);
        enum 
        {
            Type = 7
        };
        bool isConst;
        data::Ptr< data::AST::Identifier > id;
        std::string type;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Include : public mega::io::Object
    {
        Include( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 8
        };
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct SystemInclude : public mega::io::Object
    {
        SystemInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        SystemInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str);
        enum 
        {
            Type = 9
        };
        std::string str;
        Ptr< AST::Include > p_AST_Include;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct MegaInclude : public mega::io::Object
    {
        MegaInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        MegaInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& megaSourceFilePath);
        enum 
        {
            Type = 10
        };
        boost::filesystem::path megaSourceFilePath;
        std::optional< data::Ptr< data::AST::IncludeRoot > > root;
        Ptr< AST::Include > p_AST_Include;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct MegaIncludeInline : public mega::io::Object
    {
        MegaIncludeInline( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 11
        };
        Ptr< AST::MegaInclude > p_AST_MegaInclude;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct MegaIncludeNested : public mega::io::Object
    {
        MegaIncludeNested( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        MegaIncludeNested( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Identifier >& id);
        enum 
        {
            Type = 12
        };
        data::Ptr< data::AST::Identifier > id;
        Ptr< AST::MegaInclude > p_AST_MegaInclude;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct CPPInclude : public mega::io::Object
    {
        CPPInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        CPPInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& cppSourceFilePath);
        enum 
        {
            Type = 13
        };
        boost::filesystem::path cppSourceFilePath;
        Ptr< AST::Include > p_AST_Include;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Dependency : public mega::io::Object
    {
        Dependency( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Dependency( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str);
        enum 
        {
            Type = 14
        };
        std::string str;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct ContextDef : public mega::io::Object
    {
        ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::ContextDef > >& children, const std::vector< data::Ptr< data::AST::Dimension > >& dimensions, const std::vector< data::Ptr< data::AST::Include > >& includes, const std::vector< data::Ptr< data::AST::Dependency > >& dependencies, const std::string& body);
        enum 
        {
            Type = 15
        };
        std::vector< data::Ptr< data::AST::ContextDef > > children;
        std::vector< data::Ptr< data::AST::Dimension > > dimensions;
        std::vector< data::Ptr< data::AST::Include > > includes;
        std::vector< data::Ptr< data::AST::Dependency > > dependencies;
        std::string body;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct AbstractDef : public mega::io::Object
    {
        AbstractDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        AbstractDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::ScopedIdentifier >& id);
        enum 
        {
            Type = 16
        };
        data::Ptr< data::AST::ScopedIdentifier > id;
        Ptr< AST::ContextDef > p_AST_ContextDef;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct ActionDef : public mega::io::Object
    {
        ActionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        ActionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::ScopedIdentifier >& id, const data::Ptr< data::AST::Size >& size, const data::Ptr< data::AST::Inheritance >& inheritance);
        enum 
        {
            Type = 17
        };
        data::Ptr< data::AST::ScopedIdentifier > id;
        data::Ptr< data::AST::Size > size;
        data::Ptr< data::AST::Inheritance > inheritance;
        Ptr< AST::ContextDef > p_AST_ContextDef;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct EventDef : public mega::io::Object
    {
        EventDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        EventDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::ScopedIdentifier >& id, const data::Ptr< data::AST::Size >& size, const data::Ptr< data::AST::Inheritance >& inheritance);
        enum 
        {
            Type = 18
        };
        data::Ptr< data::AST::ScopedIdentifier > id;
        data::Ptr< data::AST::Size > size;
        data::Ptr< data::AST::Inheritance > inheritance;
        Ptr< AST::ContextDef > p_AST_ContextDef;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct FunctionDef : public mega::io::Object
    {
        FunctionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        FunctionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::ScopedIdentifier >& id, const data::Ptr< data::AST::ArgumentList >& argumentList, const data::Ptr< data::AST::ReturnType >& returnType);
        enum 
        {
            Type = 19
        };
        data::Ptr< data::AST::ScopedIdentifier > id;
        data::Ptr< data::AST::ArgumentList > argumentList;
        data::Ptr< data::AST::ReturnType > returnType;
        Ptr< AST::ContextDef > p_AST_ContextDef;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct ObjectDef : public mega::io::Object
    {
        ObjectDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        ObjectDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::ScopedIdentifier >& id, const data::Ptr< data::AST::Size >& size, const data::Ptr< data::AST::Inheritance >& inheritance);
        enum 
        {
            Type = 20
        };
        data::Ptr< data::AST::ScopedIdentifier > id;
        data::Ptr< data::AST::Size > size;
        data::Ptr< data::AST::Inheritance > inheritance;
        Ptr< AST::ContextDef > p_AST_ContextDef;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct LinkDef : public mega::io::Object
    {
        LinkDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        LinkDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::ScopedIdentifier >& id, const data::Ptr< data::AST::Size >& size, const data::Ptr< data::AST::Inheritance >& inheritance);
        enum 
        {
            Type = 21
        };
        data::Ptr< data::AST::ScopedIdentifier > id;
        data::Ptr< data::AST::Size > size;
        data::Ptr< data::AST::Inheritance > inheritance;
        Ptr< AST::ContextDef > p_AST_ContextDef;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct SourceRoot : public mega::io::Object
    {
        SourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        SourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& sourceFile, const data::Ptr< data::Components::Component >& component, const data::Ptr< data::AST::ContextDef >& ast);
        enum 
        {
            Type = 22
        };
        boost::filesystem::path sourceFile;
        data::Ptr< data::Components::Component > component;
        data::Ptr< data::AST::ContextDef > ast;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct IncludeRoot : public mega::io::Object
    {
        IncludeRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        IncludeRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::MegaInclude >& include);
        enum 
        {
            Type = 23
        };
        data::Ptr< data::AST::MegaInclude > include;
        Ptr< AST::SourceRoot > p_AST_SourceRoot;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct ObjectSourceRoot : public mega::io::Object
    {
        ObjectSourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Type = 24
        };
        Ptr< AST::SourceRoot > p_AST_SourceRoot;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
}
namespace Body
{
}
namespace Tree
{
    struct ContextGroup : public mega::io::Object
    {
        ContextGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        ContextGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Tree::Context > >& children);
        enum 
        {
            Type = 25
        };
        std::vector< data::Ptr< data::Tree::Context > > children;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Root : public mega::io::Object
    {
        Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::ObjectSourceRoot >& root);
        enum 
        {
            Type = 26
        };
        data::Ptr< data::AST::ObjectSourceRoot > root;
        Ptr< Tree::ContextGroup > p_Tree_ContextGroup;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Context : public mega::io::Object
    {
        Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& identifier, const data::Ptr< data::Tree::ContextGroup >& parent);
        enum 
        {
            Type = 27
        };
        std::string identifier;
        data::Ptr< data::Tree::ContextGroup > parent;
        Ptr< Tree::ContextGroup > p_Tree_ContextGroup;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Abstract : public mega::io::Object
    {
        Abstract( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Abstract( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::AbstractDef > >& abstract_defs, const std::vector< data::Ptr< data::Tree::Dimension > >& dimensions);
        enum 
        {
            Type = 28
        };
        std::vector< data::Ptr< data::AST::AbstractDef > > abstract_defs;
        std::vector< data::Ptr< data::Tree::Dimension > > dimensions;
        Ptr< Tree::Context > p_Tree_Context;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Action : public mega::io::Object
    {
        Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::ActionDef > >& action_defs, const std::vector< data::Ptr< data::Tree::Dimension > >& dimensions);
        enum 
        {
            Type = 29
        };
        std::vector< data::Ptr< data::AST::ActionDef > > action_defs;
        std::vector< data::Ptr< data::Tree::Dimension > > dimensions;
        Ptr< Tree::Context > p_Tree_Context;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Event : public mega::io::Object
    {
        Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::EventDef > >& event_defs, const std::vector< data::Ptr< data::Tree::Dimension > >& dimensions);
        enum 
        {
            Type = 30
        };
        std::vector< data::Ptr< data::AST::EventDef > > event_defs;
        std::vector< data::Ptr< data::Tree::Dimension > > dimensions;
        Ptr< Tree::Context > p_Tree_Context;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Function : public mega::io::Object
    {
        Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::FunctionDef > >& function_defs);
        enum 
        {
            Type = 31
        };
        std::vector< data::Ptr< data::AST::FunctionDef > > function_defs;
        Ptr< Tree::Context > p_Tree_Context;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Object : public mega::io::Object
    {
        Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::ObjectDef > >& object_defs, const std::vector< data::Ptr< data::Tree::Dimension > >& dimensions);
        enum 
        {
            Type = 32
        };
        std::vector< data::Ptr< data::AST::ObjectDef > > object_defs;
        std::vector< data::Ptr< data::Tree::Dimension > > dimensions;
        Ptr< Tree::Context > p_Tree_Context;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Link : public mega::io::Object
    {
        Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::LinkDef > >& link_defs);
        enum 
        {
            Type = 33
        };
        std::vector< data::Ptr< data::AST::LinkDef > > link_defs;
        Ptr< Tree::Context > p_Tree_Context;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
    struct Dimension : public mega::io::Object
    {
        Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Dimension >& parser_dimension, const std::string& identifier);
        enum 
        {
            Type = 34
        };
        data::Ptr< data::AST::Dimension > parser_dimension;
        std::string identifier;
        std::optional< std::string > type;
        mega::io::Object* m_pInheritance = nullptr;
        virtual void load( mega::io::Loader& loader );
        virtual void load_post( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
    };
}
namespace Analysis
{
}

template <>
inline Ptr< Components::Component > convert( const Ptr< Components::Component >& from )
{
    return from;
}

template <>
inline Ptr< AST::Identifier > convert( const Ptr< AST::Identifier >& from )
{
    return from;
}

template <>
inline Ptr< AST::ScopedIdentifier > convert( const Ptr< AST::ScopedIdentifier >& from )
{
    return from;
}

template <>
inline Ptr< AST::ArgumentList > convert( const Ptr< AST::ArgumentList >& from )
{
    return from;
}

template <>
inline Ptr< AST::ReturnType > convert( const Ptr< AST::ReturnType >& from )
{
    return from;
}

template <>
inline Ptr< AST::Inheritance > convert( const Ptr< AST::Inheritance >& from )
{
    return from;
}

template <>
inline Ptr< AST::Size > convert( const Ptr< AST::Size >& from )
{
    return from;
}

template <>
inline Ptr< AST::Dimension > convert( const Ptr< AST::Dimension >& from )
{
    return from;
}

template <>
inline Ptr< AST::Include > convert( const Ptr< AST::Include >& from )
{
    return from;
}

template <>
inline Ptr< AST::Include > convert( const Ptr< AST::SystemInclude >& from )
{
    return from->p_AST_Include;
}

template <>
inline Ptr< AST::SystemInclude > convert( const Ptr< AST::SystemInclude >& from )
{
    return from;
}

template <>
inline Ptr< AST::Include > convert( const Ptr< AST::MegaInclude >& from )
{
    return from->p_AST_Include;
}

template <>
inline Ptr< AST::MegaInclude > convert( const Ptr< AST::MegaInclude >& from )
{
    return from;
}

template <>
inline Ptr< AST::Include > convert( const Ptr< AST::MegaIncludeInline >& from )
{
    return from->p_AST_MegaInclude->p_AST_Include;
}

template <>
inline Ptr< AST::MegaInclude > convert( const Ptr< AST::MegaIncludeInline >& from )
{
    return from->p_AST_MegaInclude;
}

template <>
inline Ptr< AST::MegaIncludeInline > convert( const Ptr< AST::MegaIncludeInline >& from )
{
    return from;
}

template <>
inline Ptr< AST::Include > convert( const Ptr< AST::MegaIncludeNested >& from )
{
    return from->p_AST_MegaInclude->p_AST_Include;
}

template <>
inline Ptr< AST::MegaInclude > convert( const Ptr< AST::MegaIncludeNested >& from )
{
    return from->p_AST_MegaInclude;
}

template <>
inline Ptr< AST::MegaIncludeNested > convert( const Ptr< AST::MegaIncludeNested >& from )
{
    return from;
}

template <>
inline Ptr< AST::Include > convert( const Ptr< AST::CPPInclude >& from )
{
    return from->p_AST_Include;
}

template <>
inline Ptr< AST::CPPInclude > convert( const Ptr< AST::CPPInclude >& from )
{
    return from;
}

template <>
inline Ptr< AST::Dependency > convert( const Ptr< AST::Dependency >& from )
{
    return from;
}

template <>
inline Ptr< AST::ContextDef > convert( const Ptr< AST::ContextDef >& from )
{
    return from;
}

template <>
inline Ptr< AST::ContextDef > convert( const Ptr< AST::AbstractDef >& from )
{
    return from->p_AST_ContextDef;
}

template <>
inline Ptr< AST::AbstractDef > convert( const Ptr< AST::AbstractDef >& from )
{
    return from;
}

template <>
inline Ptr< AST::ContextDef > convert( const Ptr< AST::ActionDef >& from )
{
    return from->p_AST_ContextDef;
}

template <>
inline Ptr< AST::ActionDef > convert( const Ptr< AST::ActionDef >& from )
{
    return from;
}

template <>
inline Ptr< AST::ContextDef > convert( const Ptr< AST::EventDef >& from )
{
    return from->p_AST_ContextDef;
}

template <>
inline Ptr< AST::EventDef > convert( const Ptr< AST::EventDef >& from )
{
    return from;
}

template <>
inline Ptr< AST::ContextDef > convert( const Ptr< AST::FunctionDef >& from )
{
    return from->p_AST_ContextDef;
}

template <>
inline Ptr< AST::FunctionDef > convert( const Ptr< AST::FunctionDef >& from )
{
    return from;
}

template <>
inline Ptr< AST::ContextDef > convert( const Ptr< AST::ObjectDef >& from )
{
    return from->p_AST_ContextDef;
}

template <>
inline Ptr< AST::ObjectDef > convert( const Ptr< AST::ObjectDef >& from )
{
    return from;
}

template <>
inline Ptr< AST::ContextDef > convert( const Ptr< AST::LinkDef >& from )
{
    return from->p_AST_ContextDef;
}

template <>
inline Ptr< AST::LinkDef > convert( const Ptr< AST::LinkDef >& from )
{
    return from;
}

template <>
inline Ptr< AST::SourceRoot > convert( const Ptr< AST::SourceRoot >& from )
{
    return from;
}

template <>
inline Ptr< AST::SourceRoot > convert( const Ptr< AST::IncludeRoot >& from )
{
    return from->p_AST_SourceRoot;
}

template <>
inline Ptr< AST::IncludeRoot > convert( const Ptr< AST::IncludeRoot >& from )
{
    return from;
}

template <>
inline Ptr< AST::SourceRoot > convert( const Ptr< AST::ObjectSourceRoot >& from )
{
    return from->p_AST_SourceRoot;
}

template <>
inline Ptr< AST::ObjectSourceRoot > convert( const Ptr< AST::ObjectSourceRoot >& from )
{
    return from;
}

template <>
inline Ptr< Tree::ContextGroup > convert( const Ptr< Tree::ContextGroup >& from )
{
    return from;
}

template <>
inline Ptr< Tree::ContextGroup > convert( const Ptr< Tree::Root >& from )
{
    return from->p_Tree_ContextGroup;
}

template <>
inline Ptr< Tree::Root > convert( const Ptr< Tree::Root >& from )
{
    return from;
}

template <>
inline Ptr< Tree::ContextGroup > convert( const Ptr< Tree::Context >& from )
{
    return from->p_Tree_ContextGroup;
}

template <>
inline Ptr< Tree::Context > convert( const Ptr< Tree::Context >& from )
{
    return from;
}

template <>
inline Ptr< Tree::ContextGroup > convert( const Ptr< Tree::Abstract >& from )
{
    return from->p_Tree_Context->p_Tree_ContextGroup;
}

template <>
inline Ptr< Tree::Context > convert( const Ptr< Tree::Abstract >& from )
{
    return from->p_Tree_Context;
}

template <>
inline Ptr< Tree::Abstract > convert( const Ptr< Tree::Abstract >& from )
{
    return from;
}

template <>
inline Ptr< Tree::ContextGroup > convert( const Ptr< Tree::Action >& from )
{
    return from->p_Tree_Context->p_Tree_ContextGroup;
}

template <>
inline Ptr< Tree::Context > convert( const Ptr< Tree::Action >& from )
{
    return from->p_Tree_Context;
}

template <>
inline Ptr< Tree::Action > convert( const Ptr< Tree::Action >& from )
{
    return from;
}

template <>
inline Ptr< Tree::ContextGroup > convert( const Ptr< Tree::Event >& from )
{
    return from->p_Tree_Context->p_Tree_ContextGroup;
}

template <>
inline Ptr< Tree::Context > convert( const Ptr< Tree::Event >& from )
{
    return from->p_Tree_Context;
}

template <>
inline Ptr< Tree::Event > convert( const Ptr< Tree::Event >& from )
{
    return from;
}

template <>
inline Ptr< Tree::ContextGroup > convert( const Ptr< Tree::Function >& from )
{
    return from->p_Tree_Context->p_Tree_ContextGroup;
}

template <>
inline Ptr< Tree::Context > convert( const Ptr< Tree::Function >& from )
{
    return from->p_Tree_Context;
}

template <>
inline Ptr< Tree::Function > convert( const Ptr< Tree::Function >& from )
{
    return from;
}

template <>
inline Ptr< Tree::ContextGroup > convert( const Ptr< Tree::Object >& from )
{
    return from->p_Tree_Context->p_Tree_ContextGroup;
}

template <>
inline Ptr< Tree::Context > convert( const Ptr< Tree::Object >& from )
{
    return from->p_Tree_Context;
}

template <>
inline Ptr< Tree::Object > convert( const Ptr< Tree::Object >& from )
{
    return from;
}

template <>
inline Ptr< Tree::ContextGroup > convert( const Ptr< Tree::Link >& from )
{
    return from->p_Tree_Context->p_Tree_ContextGroup;
}

template <>
inline Ptr< Tree::Context > convert( const Ptr< Tree::Link >& from )
{
    return from->p_Tree_Context;
}

template <>
inline Ptr< Tree::Link > convert( const Ptr< Tree::Link >& from )
{
    return from;
}

template <>
inline Ptr< Tree::Dimension > convert( const Ptr< Tree::Dimension >& from )
{
    return from;
}


template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::SystemInclude >, Ptr< AST::Include > >
{
    inline TVariant operator()( Ptr< AST::Include >& from ) const
    {
        if( AST::SystemInclude* p1 = dynamic_cast< AST::SystemInclude* >( from->m_pInheritance ) )
        {
            return Ptr< AST::SystemInclude >( from, p1 );
        }
        else
        {
            return Ptr< AST::Include >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::MegaInclude >, Ptr< AST::Include > >
{
    inline TVariant operator()( Ptr< AST::Include >& from ) const
    {
        if( AST::MegaInclude* p1 = dynamic_cast< AST::MegaInclude* >( from->m_pInheritance ) )
        {
            return Ptr< AST::MegaInclude >( from, p1 );
        }
        else
        {
            return Ptr< AST::Include >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::MegaIncludeInline >, Ptr< AST::Include > >
{
    inline TVariant operator()( Ptr< AST::Include >& from ) const
    {
        if( AST::MegaInclude* p1 = dynamic_cast< AST::MegaInclude* >( from->m_pInheritance ) )
        {
        if( AST::MegaIncludeInline* p2 = dynamic_cast< AST::MegaIncludeInline* >( p1->m_pInheritance ) )
        {
            return Ptr< AST::MegaIncludeInline >( from, p2 );
        }
        else
        {
            return Ptr< AST::MegaInclude >( from, p1 );
        }
        }
        else
        {
            return Ptr< AST::Include >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::MegaIncludeNested >, Ptr< AST::Include > >
{
    inline TVariant operator()( Ptr< AST::Include >& from ) const
    {
        if( AST::MegaInclude* p1 = dynamic_cast< AST::MegaInclude* >( from->m_pInheritance ) )
        {
        if( AST::MegaIncludeNested* p2 = dynamic_cast< AST::MegaIncludeNested* >( p1->m_pInheritance ) )
        {
            return Ptr< AST::MegaIncludeNested >( from, p2 );
        }
        else
        {
            return Ptr< AST::MegaInclude >( from, p1 );
        }
        }
        else
        {
            return Ptr< AST::Include >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::CPPInclude >, Ptr< AST::Include > >
{
    inline TVariant operator()( Ptr< AST::Include >& from ) const
    {
        if( AST::CPPInclude* p1 = dynamic_cast< AST::CPPInclude* >( from->m_pInheritance ) )
        {
            return Ptr< AST::CPPInclude >( from, p1 );
        }
        else
        {
            return Ptr< AST::Include >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::MegaIncludeInline >, Ptr< AST::MegaInclude > >
{
    inline TVariant operator()( Ptr< AST::MegaInclude >& from ) const
    {
        if( AST::MegaIncludeInline* p1 = dynamic_cast< AST::MegaIncludeInline* >( from->m_pInheritance ) )
        {
            return Ptr< AST::MegaIncludeInline >( from, p1 );
        }
        else
        {
            return Ptr< AST::MegaInclude >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::MegaIncludeNested >, Ptr< AST::MegaInclude > >
{
    inline TVariant operator()( Ptr< AST::MegaInclude >& from ) const
    {
        if( AST::MegaIncludeNested* p1 = dynamic_cast< AST::MegaIncludeNested* >( from->m_pInheritance ) )
        {
            return Ptr< AST::MegaIncludeNested >( from, p1 );
        }
        else
        {
            return Ptr< AST::MegaInclude >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::AbstractDef >, Ptr< AST::ContextDef > >
{
    inline TVariant operator()( Ptr< AST::ContextDef >& from ) const
    {
        if( AST::AbstractDef* p1 = dynamic_cast< AST::AbstractDef* >( from->m_pInheritance ) )
        {
            return Ptr< AST::AbstractDef >( from, p1 );
        }
        else
        {
            return Ptr< AST::ContextDef >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::ActionDef >, Ptr< AST::ContextDef > >
{
    inline TVariant operator()( Ptr< AST::ContextDef >& from ) const
    {
        if( AST::ActionDef* p1 = dynamic_cast< AST::ActionDef* >( from->m_pInheritance ) )
        {
            return Ptr< AST::ActionDef >( from, p1 );
        }
        else
        {
            return Ptr< AST::ContextDef >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::EventDef >, Ptr< AST::ContextDef > >
{
    inline TVariant operator()( Ptr< AST::ContextDef >& from ) const
    {
        if( AST::EventDef* p1 = dynamic_cast< AST::EventDef* >( from->m_pInheritance ) )
        {
            return Ptr< AST::EventDef >( from, p1 );
        }
        else
        {
            return Ptr< AST::ContextDef >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::FunctionDef >, Ptr< AST::ContextDef > >
{
    inline TVariant operator()( Ptr< AST::ContextDef >& from ) const
    {
        if( AST::FunctionDef* p1 = dynamic_cast< AST::FunctionDef* >( from->m_pInheritance ) )
        {
            return Ptr< AST::FunctionDef >( from, p1 );
        }
        else
        {
            return Ptr< AST::ContextDef >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::ObjectDef >, Ptr< AST::ContextDef > >
{
    inline TVariant operator()( Ptr< AST::ContextDef >& from ) const
    {
        if( AST::ObjectDef* p1 = dynamic_cast< AST::ObjectDef* >( from->m_pInheritance ) )
        {
            return Ptr< AST::ObjectDef >( from, p1 );
        }
        else
        {
            return Ptr< AST::ContextDef >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::LinkDef >, Ptr< AST::ContextDef > >
{
    inline TVariant operator()( Ptr< AST::ContextDef >& from ) const
    {
        if( AST::LinkDef* p1 = dynamic_cast< AST::LinkDef* >( from->m_pInheritance ) )
        {
            return Ptr< AST::LinkDef >( from, p1 );
        }
        else
        {
            return Ptr< AST::ContextDef >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::IncludeRoot >, Ptr< AST::SourceRoot > >
{
    inline TVariant operator()( Ptr< AST::SourceRoot >& from ) const
    {
        if( AST::IncludeRoot* p1 = dynamic_cast< AST::IncludeRoot* >( from->m_pInheritance ) )
        {
            return Ptr< AST::IncludeRoot >( from, p1 );
        }
        else
        {
            return Ptr< AST::SourceRoot >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< AST::ObjectSourceRoot >, Ptr< AST::SourceRoot > >
{
    inline TVariant operator()( Ptr< AST::SourceRoot >& from ) const
    {
        if( AST::ObjectSourceRoot* p1 = dynamic_cast< AST::ObjectSourceRoot* >( from->m_pInheritance ) )
        {
            return Ptr< AST::ObjectSourceRoot >( from, p1 );
        }
        else
        {
            return Ptr< AST::SourceRoot >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< Tree::Root >, Ptr< Tree::ContextGroup > >
{
    inline TVariant operator()( Ptr< Tree::ContextGroup >& from ) const
    {
        if( Tree::Root* p1 = dynamic_cast< Tree::Root* >( from->m_pInheritance ) )
        {
            return Ptr< Tree::Root >( from, p1 );
        }
        else
        {
            return Ptr< Tree::ContextGroup >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< Tree::Context >, Ptr< Tree::ContextGroup > >
{
    inline TVariant operator()( Ptr< Tree::ContextGroup >& from ) const
    {
        if( Tree::Context* p1 = dynamic_cast< Tree::Context* >( from->m_pInheritance ) )
        {
            return Ptr< Tree::Context >( from, p1 );
        }
        else
        {
            return Ptr< Tree::ContextGroup >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< Tree::Abstract >, Ptr< Tree::ContextGroup > >
{
    inline TVariant operator()( Ptr< Tree::ContextGroup >& from ) const
    {
        if( Tree::Context* p1 = dynamic_cast< Tree::Context* >( from->m_pInheritance ) )
        {
        if( Tree::Abstract* p2 = dynamic_cast< Tree::Abstract* >( p1->m_pInheritance ) )
        {
            return Ptr< Tree::Abstract >( from, p2 );
        }
        else
        {
            return Ptr< Tree::Context >( from, p1 );
        }
        }
        else
        {
            return Ptr< Tree::ContextGroup >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< Tree::Action >, Ptr< Tree::ContextGroup > >
{
    inline TVariant operator()( Ptr< Tree::ContextGroup >& from ) const
    {
        if( Tree::Context* p1 = dynamic_cast< Tree::Context* >( from->m_pInheritance ) )
        {
        if( Tree::Action* p2 = dynamic_cast< Tree::Action* >( p1->m_pInheritance ) )
        {
            return Ptr< Tree::Action >( from, p2 );
        }
        else
        {
            return Ptr< Tree::Context >( from, p1 );
        }
        }
        else
        {
            return Ptr< Tree::ContextGroup >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< Tree::Event >, Ptr< Tree::ContextGroup > >
{
    inline TVariant operator()( Ptr< Tree::ContextGroup >& from ) const
    {
        if( Tree::Context* p1 = dynamic_cast< Tree::Context* >( from->m_pInheritance ) )
        {
        if( Tree::Event* p2 = dynamic_cast< Tree::Event* >( p1->m_pInheritance ) )
        {
            return Ptr< Tree::Event >( from, p2 );
        }
        else
        {
            return Ptr< Tree::Context >( from, p1 );
        }
        }
        else
        {
            return Ptr< Tree::ContextGroup >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< Tree::Function >, Ptr< Tree::ContextGroup > >
{
    inline TVariant operator()( Ptr< Tree::ContextGroup >& from ) const
    {
        if( Tree::Context* p1 = dynamic_cast< Tree::Context* >( from->m_pInheritance ) )
        {
        if( Tree::Function* p2 = dynamic_cast< Tree::Function* >( p1->m_pInheritance ) )
        {
            return Ptr< Tree::Function >( from, p2 );
        }
        else
        {
            return Ptr< Tree::Context >( from, p1 );
        }
        }
        else
        {
            return Ptr< Tree::ContextGroup >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< Tree::Object >, Ptr< Tree::ContextGroup > >
{
    inline TVariant operator()( Ptr< Tree::ContextGroup >& from ) const
    {
        if( Tree::Context* p1 = dynamic_cast< Tree::Context* >( from->m_pInheritance ) )
        {
        if( Tree::Object* p2 = dynamic_cast< Tree::Object* >( p1->m_pInheritance ) )
        {
            return Ptr< Tree::Object >( from, p2 );
        }
        else
        {
            return Ptr< Tree::Context >( from, p1 );
        }
        }
        else
        {
            return Ptr< Tree::ContextGroup >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< Tree::Link >, Ptr< Tree::ContextGroup > >
{
    inline TVariant operator()( Ptr< Tree::ContextGroup >& from ) const
    {
        if( Tree::Context* p1 = dynamic_cast< Tree::Context* >( from->m_pInheritance ) )
        {
        if( Tree::Link* p2 = dynamic_cast< Tree::Link* >( p1->m_pInheritance ) )
        {
            return Ptr< Tree::Link >( from, p2 );
        }
        else
        {
            return Ptr< Tree::Context >( from, p1 );
        }
        }
        else
        {
            return Ptr< Tree::ContextGroup >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< Tree::Abstract >, Ptr< Tree::Context > >
{
    inline TVariant operator()( Ptr< Tree::Context >& from ) const
    {
        if( Tree::Abstract* p1 = dynamic_cast< Tree::Abstract* >( from->m_pInheritance ) )
        {
            return Ptr< Tree::Abstract >( from, p1 );
        }
        else
        {
            return Ptr< Tree::Context >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< Tree::Action >, Ptr< Tree::Context > >
{
    inline TVariant operator()( Ptr< Tree::Context >& from ) const
    {
        if( Tree::Action* p1 = dynamic_cast< Tree::Action* >( from->m_pInheritance ) )
        {
            return Ptr< Tree::Action >( from, p1 );
        }
        else
        {
            return Ptr< Tree::Context >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< Tree::Event >, Ptr< Tree::Context > >
{
    inline TVariant operator()( Ptr< Tree::Context >& from ) const
    {
        if( Tree::Event* p1 = dynamic_cast< Tree::Event* >( from->m_pInheritance ) )
        {
            return Ptr< Tree::Event >( from, p1 );
        }
        else
        {
            return Ptr< Tree::Context >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< Tree::Function >, Ptr< Tree::Context > >
{
    inline TVariant operator()( Ptr< Tree::Context >& from ) const
    {
        if( Tree::Function* p1 = dynamic_cast< Tree::Function* >( from->m_pInheritance ) )
        {
            return Ptr< Tree::Function >( from, p1 );
        }
        else
        {
            return Ptr< Tree::Context >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< Tree::Object >, Ptr< Tree::Context > >
{
    inline TVariant operator()( Ptr< Tree::Context >& from ) const
    {
        if( Tree::Object* p1 = dynamic_cast< Tree::Object* >( from->m_pInheritance ) )
        {
            return Ptr< Tree::Object >( from, p1 );
        }
        else
        {
            return Ptr< Tree::Context >( from, from );
        }
    }
};

template < typename TVariant >
struct UpCast< TVariant, Ptr< Tree::Link >, Ptr< Tree::Context > >
{
    inline TVariant operator()( Ptr< Tree::Context >& from ) const
    {
        if( Tree::Link* p1 = dynamic_cast< Tree::Link* >( from->m_pInheritance ) )
        {
            return Ptr< Tree::Link >( from, p1 );
        }
        else
        {
            return Ptr< Tree::Context >( from, from );
        }
    }
};


class Factory
{
public:
    static mega::io::Object* create( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
};

}
#endif //DATABASE_DATA_GUARD_4_APRIL_2022
