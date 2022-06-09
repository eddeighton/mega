#ifndef DATABASE_DATA_GUARD_4_APRIL_2022
#define DATABASE_DATA_GUARD_4_APRIL_2022

#include "database/common/object_info.hpp"
#include "database/common/object.hpp"
#include "database/common/loader.hpp"
#include "database/common/storer.hpp"
#include "database/common/data_pointer.hpp"
#include "database/common/object_loader.hpp"

#include "nlohmann/json.hpp"

#include "mega/common.hpp"
#include "database/types/invocation_id.hpp"

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
        struct Link;
        struct Dimension;
        struct Include;
        struct SystemInclude;
        struct MegaInclude;
        struct MegaIncludeInline;
        struct MegaIncludeNested;
        struct CPPInclude;
        struct Dependency;
        struct ContextDef;
        struct NamespaceDef;
        struct AbstractDef;
        struct ActionDef;
        struct EventDef;
        struct FunctionDef;
        struct ObjectDef;
        struct LinkDef;
        struct TableDef;
        struct SourceRoot;
        struct IncludeRoot;
        struct ObjectSourceRoot;
    } // namespace AST
    namespace Body
    {
        struct ContextDef;
    }
    namespace Tree
    {
        struct DimensionTrait;
        struct InheritanceTrait;
        struct LinkTrait;
        struct ReturnTypeTrait;
        struct ArgumentListTrait;
        struct SizeTrait;
        struct ContextGroup;
        struct Root;
        struct IContext;
        struct Namespace;
        struct Abstract;
        struct Action;
        struct Event;
        struct Function;
        struct Object;
        struct Link;
        struct Table;
    } // namespace Tree
    namespace DPGraph
    {
        struct Glob;
        struct ObjectDependencies;
        struct Analysis;
    } // namespace DPGraph
    namespace SymbolTable
    {
        struct Symbol;
        struct SymbolSet;
        struct SymbolTable;
    } // namespace SymbolTable
    namespace PerSourceSymbols
    {
        struct DimensionTrait;
        struct IContext;
    } // namespace PerSourceSymbols
    namespace Clang
    {
        struct DimensionTrait;
        struct InheritanceTrait;
    } // namespace Clang
    namespace Concrete
    {
        struct DimensionTrait;
        struct IContext;
        struct Dimension;
        struct Context;
        struct Namespace;
        struct Action;
        struct Event;
        struct Function;
        struct Object;
        struct Link;
        struct Table;
        struct Root;
    } // namespace Concrete
    namespace Model
    {
        struct ObjectGraph;
        struct Graph;
    } // namespace Model
    namespace Derivations
    {
        struct ObjectMapping;
        struct Mapping;
    } // namespace Derivations
    namespace Operations
    {
        struct InterfaceVariant;
        struct ConcreteVariant;
        struct Element;
        struct ElementVector;
        struct Context;
        struct TypePath;
        struct NameRoot;
        struct Name;
        struct NameResolution;
        struct Invocation;
        struct Invocations;
    } // namespace Operations

    // definitions
    namespace Components
    {
        struct Component : public mega::io::Object
        {
            Component( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Component( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &name, const boost::filesystem::path &directory,
                       const std::vector< std::string > &cpp_flags, const std::vector< std::string > &cpp_defines,
                       const std::vector< boost::filesystem::path > &includeDirectories, const std::vector< boost::filesystem::path > &sourceFiles );
            enum
            {
                Object_Part_Type_ID = 0
            };
            std::string                                              name;
            boost::filesystem::path                                  directory;
            std::vector< std::string >                               cpp_flags;
            std::vector< std::string >                               cpp_defines;
            std::vector< boost::filesystem::path >                   includeDirectories;
            std::vector< boost::filesystem::path >                   sourceFiles;
            std::variant< data::Ptr< data::Components::Component > > m_inheritance;
            virtual bool                                             test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                             set_inheritance_pointer();
            virtual void                                             load( mega::io::Loader &loader );
            virtual void                                             store( mega::io::Storer &storer ) const;
            virtual void                                             to_json( nlohmann::json &data ) const;
        };
    } // namespace Components
    namespace AST
    {
        struct Identifier : public mega::io::Object
        {
            Identifier( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Identifier( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &str );
            enum
            {
                Object_Part_Type_ID = 1
            };
            std::string                                        str;
            std::variant< data::Ptr< data::AST::Identifier > > m_inheritance;
            virtual bool                                       test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                       set_inheritance_pointer();
            virtual void                                       load( mega::io::Loader &loader );
            virtual void                                       store( mega::io::Storer &storer ) const;
            virtual void                                       to_json( nlohmann::json &data ) const;
        };
        struct ScopedIdentifier : public mega::io::Object
        {
            ScopedIdentifier( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            ScopedIdentifier( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::AST::Identifier > > &ids,
                              const std::string &source_file, const std::size_t &line_number );
            enum
            {
                Object_Part_Type_ID = 2
            };
            std::vector< data::Ptr< data::AST::Identifier > >        ids;
            std::string                                              source_file;
            std::size_t                                              line_number;
            std::variant< data::Ptr< data::AST::ScopedIdentifier > > m_inheritance;
            virtual bool                                             test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                             set_inheritance_pointer();
            virtual void                                             load( mega::io::Loader &loader );
            virtual void                                             store( mega::io::Storer &storer ) const;
            virtual void                                             to_json( nlohmann::json &data ) const;
        };
        struct ArgumentList : public mega::io::Object
        {
            ArgumentList( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            ArgumentList( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &str );
            enum
            {
                Object_Part_Type_ID = 3
            };
            std::string                                                                                      str;
            std::variant< data::Ptr< data::AST::ArgumentList >, data::Ptr< data::Tree::ArgumentListTrait > > m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct ReturnType : public mega::io::Object
        {
            ReturnType( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            ReturnType( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &str );
            enum
            {
                Object_Part_Type_ID = 4
            };
            std::string                                                                                  str;
            std::variant< data::Ptr< data::AST::ReturnType >, data::Ptr< data::Tree::ReturnTypeTrait > > m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Inheritance : public mega::io::Object
        {
            Inheritance( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Inheritance( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< std::string > &strings );
            enum
            {
                Object_Part_Type_ID = 5
            };
            std::vector< std::string >                                                                     strings;
            std::variant< data::Ptr< data::AST::Inheritance >, data::Ptr< data::Tree::InheritanceTrait > > m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Size : public mega::io::Object
        {
            Size( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Size( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &str );
            enum
            {
                Object_Part_Type_ID = 6
            };
            std::string                                                                      str;
            std::variant< data::Ptr< data::AST::Size >, data::Ptr< data::Tree::SizeTrait > > m_inheritance;
            virtual bool                                                                     test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                                     set_inheritance_pointer();
            virtual void                                                                     load( mega::io::Loader &loader );
            virtual void                                                                     store( mega::io::Storer &storer ) const;
            virtual void                                                                     to_json( nlohmann::json &data ) const;
        };
        struct Link : public mega::io::Object
        {
            Link( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Link( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const mega::CardinalityRange &linker, const mega::CardinalityRange &linkee,
                  const bool &derive_from, const bool &derive_to, const mega::Ownership &ownership );
            enum
            {
                Object_Part_Type_ID = 7
            };
            mega::CardinalityRange                                                           linker;
            mega::CardinalityRange                                                           linkee;
            bool                                                                             derive_from;
            bool                                                                             derive_to;
            mega::Ownership                                                                  ownership;
            std::variant< data::Ptr< data::AST::Link >, data::Ptr< data::Tree::LinkTrait > > m_inheritance;
            virtual bool                                                                     test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                                     set_inheritance_pointer();
            virtual void                                                                     load( mega::io::Loader &loader );
            virtual void                                                                     store( mega::io::Storer &storer ) const;
            virtual void                                                                     to_json( nlohmann::json &data ) const;
        };
        struct Dimension : public mega::io::Object
        {
            Dimension( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Dimension( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const bool &isConst, const data::Ptr< data::AST::Identifier > &id,
                       const std::string &type );
            enum
            {
                Object_Part_Type_ID = 8
            };
            bool                                                                                       isConst;
            data::Ptr< data::AST::Identifier >                                                         id;
            std::string                                                                                type;
            std::variant< data::Ptr< data::AST::Dimension >, data::Ptr< data::Tree::DimensionTrait > > m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Include : public mega::io::Object
        {
            Include( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            enum
            {
                Object_Part_Type_ID = 9
            };
            std::variant< data::Ptr< data::AST::Include >, data::Ptr< data::AST::SystemInclude >, data::Ptr< data::AST::MegaInclude >,
                          data::Ptr< data::AST::MegaIncludeInline >, data::Ptr< data::AST::MegaIncludeNested >, data::Ptr< data::AST::CPPInclude > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct SystemInclude : public mega::io::Object
        {
            SystemInclude( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            SystemInclude( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &str );
            enum
            {
                Object_Part_Type_ID = 10
            };
            std::string         str;
            Ptr< AST::Include > p_AST_Include;
            std::variant< data::Ptr< data::AST::Include >, data::Ptr< data::AST::SystemInclude >, data::Ptr< data::AST::MegaInclude >,
                          data::Ptr< data::AST::MegaIncludeInline >, data::Ptr< data::AST::MegaIncludeNested >, data::Ptr< data::AST::CPPInclude > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct MegaInclude : public mega::io::Object
        {
            MegaInclude( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            MegaInclude( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const boost::filesystem::path &megaSourceFilePath );
            enum
            {
                Object_Part_Type_ID = 11
            };
            boost::filesystem::path                              megaSourceFilePath;
            std::optional< data::Ptr< data::AST::IncludeRoot > > root;
            Ptr< AST::Include >                                  p_AST_Include;
            std::variant< data::Ptr< data::AST::Include >, data::Ptr< data::AST::SystemInclude >, data::Ptr< data::AST::MegaInclude >,
                          data::Ptr< data::AST::MegaIncludeInline >, data::Ptr< data::AST::MegaIncludeNested >, data::Ptr< data::AST::CPPInclude > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct MegaIncludeInline : public mega::io::Object
        {
            MegaIncludeInline( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            enum
            {
                Object_Part_Type_ID = 12
            };
            Ptr< AST::MegaInclude > p_AST_MegaInclude;
            std::variant< data::Ptr< data::AST::Include >, data::Ptr< data::AST::SystemInclude >, data::Ptr< data::AST::MegaInclude >,
                          data::Ptr< data::AST::MegaIncludeInline >, data::Ptr< data::AST::MegaIncludeNested >, data::Ptr< data::AST::CPPInclude > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct MegaIncludeNested : public mega::io::Object
        {
            MegaIncludeNested( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            MegaIncludeNested( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::ScopedIdentifier > &id );
            enum
            {
                Object_Part_Type_ID = 13
            };
            data::Ptr< data::AST::ScopedIdentifier > id;
            Ptr< AST::MegaInclude >                  p_AST_MegaInclude;
            std::variant< data::Ptr< data::AST::Include >, data::Ptr< data::AST::SystemInclude >, data::Ptr< data::AST::MegaInclude >,
                          data::Ptr< data::AST::MegaIncludeInline >, data::Ptr< data::AST::MegaIncludeNested >, data::Ptr< data::AST::CPPInclude > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct CPPInclude : public mega::io::Object
        {
            CPPInclude( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            CPPInclude( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const boost::filesystem::path &cppSourceFilePath );
            enum
            {
                Object_Part_Type_ID = 14
            };
            boost::filesystem::path cppSourceFilePath;
            Ptr< AST::Include >     p_AST_Include;
            std::variant< data::Ptr< data::AST::Include >, data::Ptr< data::AST::SystemInclude >, data::Ptr< data::AST::MegaInclude >,
                          data::Ptr< data::AST::MegaIncludeInline >, data::Ptr< data::AST::MegaIncludeNested >, data::Ptr< data::AST::CPPInclude > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Dependency : public mega::io::Object
        {
            Dependency( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Dependency( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &str );
            enum
            {
                Object_Part_Type_ID = 15
            };
            std::string                                        str;
            std::variant< data::Ptr< data::AST::Dependency > > m_inheritance;
            virtual bool                                       test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                       set_inheritance_pointer();
            virtual void                                       load( mega::io::Loader &loader );
            virtual void                                       store( mega::io::Storer &storer ) const;
            virtual void                                       to_json( nlohmann::json &data ) const;
        };
        struct ContextDef : public mega::io::Object
        {
            ContextDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            ContextDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::ScopedIdentifier > &id,
                        const std::vector< data::Ptr< data::AST::ContextDef > > &children, const std::vector< data::Ptr< data::AST::Dimension > > &dimensions,
                        const std::vector< data::Ptr< data::AST::Include > > &includes, const std::vector< data::Ptr< data::AST::Dependency > > &dependencies );
            enum
            {
                Object_Part_Type_ID = 16
            };
            data::Ptr< data::AST::ScopedIdentifier >          id;
            std::vector< data::Ptr< data::AST::ContextDef > > children;
            std::vector< data::Ptr< data::AST::Dimension > >  dimensions;
            std::vector< data::Ptr< data::AST::Include > >    includes;
            std::vector< data::Ptr< data::AST::Dependency > > dependencies;
            Ptr< Body::ContextDef >                           p_Body_ContextDef;
            std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                          data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                          data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct NamespaceDef : public mega::io::Object
        {
            NamespaceDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            enum
            {
                Object_Part_Type_ID = 18
            };
            Ptr< AST::ContextDef > p_AST_ContextDef;
            std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                          data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                          data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct AbstractDef : public mega::io::Object
        {
            AbstractDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            AbstractDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::Inheritance > &inheritance );
            enum
            {
                Object_Part_Type_ID = 19
            };
            data::Ptr< data::AST::Inheritance > inheritance;
            Ptr< AST::ContextDef >              p_AST_ContextDef;
            std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                          data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                          data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct ActionDef : public mega::io::Object
        {
            ActionDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            ActionDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::Size > &size,
                       const data::Ptr< data::AST::Inheritance > &inheritance );
            enum
            {
                Object_Part_Type_ID = 20
            };
            data::Ptr< data::AST::Size >        size;
            data::Ptr< data::AST::Inheritance > inheritance;
            Ptr< AST::ContextDef >              p_AST_ContextDef;
            std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                          data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                          data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct EventDef : public mega::io::Object
        {
            EventDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            EventDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::Size > &size,
                      const data::Ptr< data::AST::Inheritance > &inheritance );
            enum
            {
                Object_Part_Type_ID = 21
            };
            data::Ptr< data::AST::Size >        size;
            data::Ptr< data::AST::Inheritance > inheritance;
            Ptr< AST::ContextDef >              p_AST_ContextDef;
            std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                          data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                          data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct FunctionDef : public mega::io::Object
        {
            FunctionDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            FunctionDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::ArgumentList > &argumentList,
                         const data::Ptr< data::AST::ReturnType > &returnType );
            enum
            {
                Object_Part_Type_ID = 22
            };
            data::Ptr< data::AST::ArgumentList > argumentList;
            data::Ptr< data::AST::ReturnType >   returnType;
            Ptr< AST::ContextDef >               p_AST_ContextDef;
            std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                          data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                          data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct ObjectDef : public mega::io::Object
        {
            ObjectDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            ObjectDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::Inheritance > &inheritance );
            enum
            {
                Object_Part_Type_ID = 23
            };
            data::Ptr< data::AST::Inheritance > inheritance;
            Ptr< AST::ContextDef >              p_AST_ContextDef;
            std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                          data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                          data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct LinkDef : public mega::io::Object
        {
            LinkDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            LinkDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::Link > &link,
                     const data::Ptr< data::AST::Inheritance > &target );
            enum
            {
                Object_Part_Type_ID = 24
            };
            data::Ptr< data::AST::Link >        link;
            data::Ptr< data::AST::Inheritance > target;
            Ptr< AST::ContextDef >              p_AST_ContextDef;
            std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                          data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                          data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct TableDef : public mega::io::Object
        {
            TableDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            TableDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::Inheritance > &inheritance );
            enum
            {
                Object_Part_Type_ID = 25
            };
            data::Ptr< data::AST::Inheritance > inheritance;
            Ptr< AST::ContextDef >              p_AST_ContextDef;
            std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                          data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                          data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct SourceRoot : public mega::io::Object
        {
            SourceRoot( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            SourceRoot( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const boost::filesystem::path &sourceFile,
                        const data::Ptr< data::Components::Component > &component, const data::Ptr< data::AST::ContextDef > &ast );
            enum
            {
                Object_Part_Type_ID = 26
            };
            boost::filesystem::path                                                                                                           sourceFile;
            data::Ptr< data::Components::Component >                                                                                          component;
            data::Ptr< data::AST::ContextDef >                                                                                                ast;
            std::variant< data::Ptr< data::AST::SourceRoot >, data::Ptr< data::AST::IncludeRoot >, data::Ptr< data::AST::ObjectSourceRoot > > m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct IncludeRoot : public mega::io::Object
        {
            IncludeRoot( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            enum
            {
                Object_Part_Type_ID = 27
            };
            Ptr< AST::SourceRoot >                                                                                                            p_AST_SourceRoot;
            std::variant< data::Ptr< data::AST::SourceRoot >, data::Ptr< data::AST::IncludeRoot >, data::Ptr< data::AST::ObjectSourceRoot > > m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct ObjectSourceRoot : public mega::io::Object
        {
            ObjectSourceRoot( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            enum
            {
                Object_Part_Type_ID = 28
            };
            Ptr< AST::SourceRoot >                                                                                                            p_AST_SourceRoot;
            std::variant< data::Ptr< data::AST::SourceRoot >, data::Ptr< data::AST::IncludeRoot >, data::Ptr< data::AST::ObjectSourceRoot > > m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
    } // namespace AST
    namespace Body
    {
        struct ContextDef : public mega::io::Object
        {
            ContextDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            ContextDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &body );
            enum
            {
                Object_Part_Type_ID = 17
            };
            std::string  body;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
    } // namespace Body
    namespace Tree
    {
        struct DimensionTrait : public mega::io::Object
        {
            DimensionTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            enum
            {
                Object_Part_Type_ID = 29
            };
            Ptr< AST::Dimension >                                                                      p_AST_Dimension;
            Ptr< PerSourceSymbols::DimensionTrait >                                                    p_PerSourceSymbols_DimensionTrait;
            Ptr< Concrete::DimensionTrait >                                                            p_Concrete_DimensionTrait;
            Ptr< Clang::DimensionTrait >                                                               p_Clang_DimensionTrait;
            std::variant< data::Ptr< data::AST::Dimension >, data::Ptr< data::Tree::DimensionTrait > > m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct InheritanceTrait : public mega::io::Object
        {
            InheritanceTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            enum
            {
                Object_Part_Type_ID = 33
            };
            Ptr< AST::Inheritance >                                                                        p_AST_Inheritance;
            Ptr< Clang::InheritanceTrait >                                                                 p_Clang_InheritanceTrait;
            std::variant< data::Ptr< data::AST::Inheritance >, data::Ptr< data::Tree::InheritanceTrait > > m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct LinkTrait : public mega::io::Object
        {
            LinkTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            enum
            {
                Object_Part_Type_ID = 35
            };
            Ptr< AST::Link >                                                                 p_AST_Link;
            std::variant< data::Ptr< data::AST::Link >, data::Ptr< data::Tree::LinkTrait > > m_inheritance;
            virtual bool                                                                     test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                                     set_inheritance_pointer();
            virtual void                                                                     load( mega::io::Loader &loader );
            virtual void                                                                     store( mega::io::Storer &storer ) const;
            virtual void                                                                     to_json( nlohmann::json &data ) const;
        };
        struct ReturnTypeTrait : public mega::io::Object
        {
            ReturnTypeTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            enum
            {
                Object_Part_Type_ID = 36
            };
            Ptr< AST::ReturnType >                                                                       p_AST_ReturnType;
            std::variant< data::Ptr< data::AST::ReturnType >, data::Ptr< data::Tree::ReturnTypeTrait > > m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct ArgumentListTrait : public mega::io::Object
        {
            ArgumentListTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            enum
            {
                Object_Part_Type_ID = 37
            };
            Ptr< AST::ArgumentList >                                                                         p_AST_ArgumentList;
            std::variant< data::Ptr< data::AST::ArgumentList >, data::Ptr< data::Tree::ArgumentListTrait > > m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct SizeTrait : public mega::io::Object
        {
            SizeTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            enum
            {
                Object_Part_Type_ID = 38
            };
            Ptr< AST::Size >                                                                 p_AST_Size;
            std::variant< data::Ptr< data::AST::Size >, data::Ptr< data::Tree::SizeTrait > > m_inheritance;
            virtual bool                                                                     test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                                     set_inheritance_pointer();
            virtual void                                                                     load( mega::io::Loader &loader );
            virtual void                                                                     store( mega::io::Storer &storer ) const;
            virtual void                                                                     to_json( nlohmann::json &data ) const;
        };
        struct ContextGroup : public mega::io::Object
        {
            ContextGroup( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            ContextGroup( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::Tree::IContext > > &children );
            enum
            {
                Object_Part_Type_ID = 39
            };
            std::vector< data::Ptr< data::Tree::IContext > > children;
            std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::IContext >,
                          data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                          data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >, data::Ptr< data::Tree::Link >,
                          data::Ptr< data::Tree::Table > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Root : public mega::io::Object
        {
            Root( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Root( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::ObjectSourceRoot > &root );
            enum
            {
                Object_Part_Type_ID = 40
            };
            data::Ptr< data::AST::ObjectSourceRoot > root;
            Ptr< Tree::ContextGroup >                p_Tree_ContextGroup;
            std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::IContext >,
                          data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                          data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >, data::Ptr< data::Tree::Link >,
                          data::Ptr< data::Tree::Table > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct IContext : public mega::io::Object
        {
            IContext( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            IContext( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &identifier,
                      const data::Ptr< data::Tree::ContextGroup > &parent );
            enum
            {
                Object_Part_Type_ID = 41
            };
            std::string                           identifier;
            data::Ptr< data::Tree::ContextGroup > parent;
            Ptr< Tree::ContextGroup >             p_Tree_ContextGroup;
            Ptr< PerSourceSymbols::IContext >     p_PerSourceSymbols_IContext;
            Ptr< Concrete::IContext >             p_Concrete_IContext;
            std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::IContext >,
                          data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                          data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >, data::Ptr< data::Tree::Link >,
                          data::Ptr< data::Tree::Table > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Namespace : public mega::io::Object
        {
            Namespace( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Namespace( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const bool &is_global,
                       const std::vector< data::Ptr< data::AST::ContextDef > > &namespace_defs );
            enum
            {
                Object_Part_Type_ID = 44
            };
            bool                                                                    is_global;
            std::vector< data::Ptr< data::AST::ContextDef > >                       namespace_defs;
            std::optional< std::vector< data::Ptr< data::Tree::DimensionTrait > > > dimension_traits;
            Ptr< Tree::IContext >                                                   p_Tree_IContext;
            std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::IContext >,
                          data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                          data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >, data::Ptr< data::Tree::Link >,
                          data::Ptr< data::Tree::Table > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Abstract : public mega::io::Object
        {
            Abstract( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Abstract( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                      const std::vector< data::Ptr< data::AST::AbstractDef > > &abstract_defs );
            enum
            {
                Object_Part_Type_ID = 45
            };
            std::vector< data::Ptr< data::AST::AbstractDef > >                          abstract_defs;
            std::optional< std::vector< data::Ptr< data::Tree::DimensionTrait > > >     dimension_traits;
            std::optional< std::optional< data::Ptr< data::Tree::InheritanceTrait > > > inheritance_trait;
            Ptr< Tree::IContext >                                                       p_Tree_IContext;
            std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::IContext >,
                          data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                          data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >, data::Ptr< data::Tree::Link >,
                          data::Ptr< data::Tree::Table > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Action : public mega::io::Object
        {
            Action( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Action( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::AST::ActionDef > > &action_defs );
            enum
            {
                Object_Part_Type_ID = 46
            };
            std::vector< data::Ptr< data::AST::ActionDef > >                            action_defs;
            std::optional< std::vector< data::Ptr< data::Tree::DimensionTrait > > >     dimension_traits;
            std::optional< std::optional< data::Ptr< data::Tree::InheritanceTrait > > > inheritance_trait;
            std::optional< std::optional< data::Ptr< data::Tree::SizeTrait > > >        size_trait;
            Ptr< Tree::IContext >                                                       p_Tree_IContext;
            std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::IContext >,
                          data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                          data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >, data::Ptr< data::Tree::Link >,
                          data::Ptr< data::Tree::Table > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Event : public mega::io::Object
        {
            Event( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Event( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::AST::EventDef > > &event_defs );
            enum
            {
                Object_Part_Type_ID = 47
            };
            std::vector< data::Ptr< data::AST::EventDef > >                             event_defs;
            std::optional< std::vector< data::Ptr< data::Tree::DimensionTrait > > >     dimension_traits;
            std::optional< std::optional< data::Ptr< data::Tree::InheritanceTrait > > > inheritance_trait;
            std::optional< std::optional< data::Ptr< data::Tree::SizeTrait > > >        size_trait;
            Ptr< Tree::IContext >                                                       p_Tree_IContext;
            std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::IContext >,
                          data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                          data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >, data::Ptr< data::Tree::Link >,
                          data::Ptr< data::Tree::Table > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Function : public mega::io::Object
        {
            Function( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Function( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                      const std::vector< data::Ptr< data::AST::FunctionDef > > &function_defs );
            enum
            {
                Object_Part_Type_ID = 48
            };
            std::vector< data::Ptr< data::AST::FunctionDef > >          function_defs;
            std::optional< data::Ptr< data::Tree::ReturnTypeTrait > >   return_type_trait;
            std::optional< data::Ptr< data::Tree::ArgumentListTrait > > arguments_trait;
            Ptr< Tree::IContext >                                       p_Tree_IContext;
            std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::IContext >,
                          data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                          data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >, data::Ptr< data::Tree::Link >,
                          data::Ptr< data::Tree::Table > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Object : public mega::io::Object
        {
            Object( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Object( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::AST::ObjectDef > > &object_defs );
            enum
            {
                Object_Part_Type_ID = 49
            };
            std::vector< data::Ptr< data::AST::ObjectDef > >                            object_defs;
            std::optional< std::vector< data::Ptr< data::Tree::DimensionTrait > > >     dimension_traits;
            std::optional< std::optional< data::Ptr< data::Tree::InheritanceTrait > > > inheritance_trait;
            Ptr< Tree::IContext >                                                       p_Tree_IContext;
            std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::IContext >,
                          data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                          data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >, data::Ptr< data::Tree::Link >,
                          data::Ptr< data::Tree::Table > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Link : public mega::io::Object
        {
            Link( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Link( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::AST::LinkDef > > &link_defs );
            enum
            {
                Object_Part_Type_ID = 50
            };
            std::vector< data::Ptr< data::AST::LinkDef > >             link_defs;
            std::optional< data::Ptr< data::Tree::LinkTrait > >        link_trait;
            std::optional< data::Ptr< data::Tree::InheritanceTrait > > link_target;
            Ptr< Tree::IContext >                                      p_Tree_IContext;
            std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::IContext >,
                          data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                          data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >, data::Ptr< data::Tree::Link >,
                          data::Ptr< data::Tree::Table > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Table : public mega::io::Object
        {
            Table( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Table( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::AST::TableDef > > &table_defs );
            enum
            {
                Object_Part_Type_ID = 51
            };
            std::vector< data::Ptr< data::AST::TableDef > > table_defs;
            Ptr< Tree::IContext >                           p_Tree_IContext;
            std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::IContext >,
                          data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                          data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >, data::Ptr< data::Tree::Link >,
                          data::Ptr< data::Tree::Table > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
    } // namespace Tree
    namespace DPGraph
    {
        struct Glob : public mega::io::Object
        {
            Glob( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Glob( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const boost::filesystem::path &location, const std::string &glob );
            enum
            {
                Object_Part_Type_ID = 73
            };
            boost::filesystem::path                          location;
            std::string                                      glob;
            std::variant< data::Ptr< data::DPGraph::Glob > > m_inheritance;
            virtual bool                                     test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                     set_inheritance_pointer();
            virtual void                                     load( mega::io::Loader &loader );
            virtual void                                     store( mega::io::Storer &storer ) const;
            virtual void                                     to_json( nlohmann::json &data ) const;
        };
        struct ObjectDependencies : public mega::io::Object
        {
            ObjectDependencies( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            ObjectDependencies( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const mega::io::megaFilePath &source_file,
                                const std::size_t &hash_code, const std::vector< data::Ptr< data::DPGraph::Glob > > &globs,
                                const std::vector< boost::filesystem::path > &resolution );
            enum
            {
                Object_Part_Type_ID = 74
            };
            mega::io::megaFilePath                                         source_file;
            std::size_t                                                    hash_code;
            std::vector< data::Ptr< data::DPGraph::Glob > >                globs;
            std::vector< boost::filesystem::path >                         resolution;
            std::variant< data::Ptr< data::DPGraph::ObjectDependencies > > m_inheritance;
            virtual bool                                                   test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                   set_inheritance_pointer();
            virtual void                                                   load( mega::io::Loader &loader );
            virtual void                                                   store( mega::io::Storer &storer ) const;
            virtual void                                                   to_json( nlohmann::json &data ) const;
        };
        struct Analysis : public mega::io::Object
        {
            Analysis( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Analysis( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                      const std::vector< data::Ptr< data::DPGraph::ObjectDependencies > > &objects );
            enum
            {
                Object_Part_Type_ID = 75
            };
            std::vector< data::Ptr< data::DPGraph::ObjectDependencies > > objects;
            std::variant< data::Ptr< data::DPGraph::Analysis > >          m_inheritance;
            virtual bool                                                  test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                  set_inheritance_pointer();
            virtual void                                                  load( mega::io::Loader &loader );
            virtual void                                                  store( mega::io::Storer &storer ) const;
            virtual void                                                  to_json( nlohmann::json &data ) const;
        };
    } // namespace DPGraph
    namespace SymbolTable
    {
        struct Symbol : public mega::io::Object
        {
            Symbol( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Symbol( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &symbol, const std::int32_t &id,
                    const std::vector< data::Ptr< data::Tree::IContext > >       &contexts,
                    const std::vector< data::Ptr< data::Tree::DimensionTrait > > &dimensions );
            enum
            {
                Object_Part_Type_ID = 76
            };
            std::string                                            symbol;
            std::int32_t                                           id;
            std::vector< data::Ptr< data::Tree::IContext > >       contexts;
            std::vector< data::Ptr< data::Tree::DimensionTrait > > dimensions;
            std::variant< data::Ptr< data::SymbolTable::Symbol > > m_inheritance;
            virtual bool                                           test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                           set_inheritance_pointer();
            virtual void                                           load( mega::io::Loader &loader );
            virtual void                                           store( mega::io::Storer &storer ) const;
            virtual void                                           to_json( nlohmann::json &data ) const;
        };
        struct SymbolSet : public mega::io::Object
        {
            SymbolSet( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            SymbolSet( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                       const std::map< std::string, data::Ptr< data::SymbolTable::Symbol > > &symbols, const mega::io::megaFilePath &source_file,
                       const std::size_t                                                                                 &hash_code,
                       const std::map< data::Ptr< data::Tree::IContext >, data::Ptr< data::SymbolTable::Symbol > >       &context_symbols,
                       const std::map< data::Ptr< data::Tree::DimensionTrait >, data::Ptr< data::SymbolTable::Symbol > > &dimension_symbols,
                       const std::map< data::Ptr< data::Tree::IContext >, int32_t >                                      &context_type_ids,
                       const std::map< data::Ptr< data::Tree::DimensionTrait >, int32_t >                                &dimension_type_ids );
            enum
            {
                Object_Part_Type_ID = 77
            };
            std::map< std::string, data::Ptr< data::SymbolTable::Symbol > >                             symbols;
            mega::io::megaFilePath                                                                      source_file;
            std::size_t                                                                                 hash_code;
            std::map< data::Ptr< data::Tree::IContext >, data::Ptr< data::SymbolTable::Symbol > >       context_symbols;
            std::map< data::Ptr< data::Tree::DimensionTrait >, data::Ptr< data::SymbolTable::Symbol > > dimension_symbols;
            std::map< data::Ptr< data::Tree::IContext >, int32_t >                                      context_type_ids;
            std::map< data::Ptr< data::Tree::DimensionTrait >, int32_t >                                dimension_type_ids;
            std::variant< data::Ptr< data::SymbolTable::SymbolSet > >                                   m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct SymbolTable : public mega::io::Object
        {
            SymbolTable( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            SymbolTable( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                         const std::map< mega::io::megaFilePath, data::Ptr< data::SymbolTable::SymbolSet > > &symbol_sets,
                         const std::map< std::string, data::Ptr< data::SymbolTable::Symbol > >               &symbols,
                         const std::map< int32_t, data::Ptr< data::Tree::IContext > >                        &context_type_ids,
                         const std::map< int32_t, data::Ptr< data::Tree::DimensionTrait > >                  &dimension_type_ids,
                         const std::map< int32_t, data::Ptr< data::SymbolTable::Symbol > >                   &symbol_id_map );
            enum
            {
                Object_Part_Type_ID = 78
            };
            std::map< mega::io::megaFilePath, data::Ptr< data::SymbolTable::SymbolSet > > symbol_sets;
            std::map< std::string, data::Ptr< data::SymbolTable::Symbol > >               symbols;
            std::map< int32_t, data::Ptr< data::Tree::IContext > >                        context_type_ids;
            std::map< int32_t, data::Ptr< data::Tree::DimensionTrait > >                  dimension_type_ids;
            std::map< int32_t, data::Ptr< data::SymbolTable::Symbol > >                   symbol_id_map;
            std::variant< data::Ptr< data::SymbolTable::SymbolTable > >                   m_inheritance;
            virtual bool                                                                  test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                                  set_inheritance_pointer();
            virtual void                                                                  load( mega::io::Loader &loader );
            virtual void                                                                  store( mega::io::Storer &storer ) const;
            virtual void                                                                  to_json( nlohmann::json &data ) const;
        };
    } // namespace SymbolTable
    namespace PerSourceSymbols
    {
        struct DimensionTrait : public mega::io::Object
        {
            DimensionTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            DimensionTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, Ptr< Tree::DimensionTrait > p_Tree_DimensionTrait,
                            const std::int32_t &symbol );
            enum
            {
                Object_Part_Type_ID = 30
            };
            std::int32_t                  symbol;
            std::optional< std::int32_t > type_id;
            Ptr< Tree::DimensionTrait >   p_Tree_DimensionTrait;
            virtual bool                  test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                  set_inheritance_pointer();
            virtual void                  load( mega::io::Loader &loader );
            virtual void                  store( mega::io::Storer &storer ) const;
            virtual void                  to_json( nlohmann::json &data ) const;
        };
        struct IContext : public mega::io::Object
        {
            IContext( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            IContext( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, Ptr< Tree::IContext > p_Tree_IContext, const std::int32_t &symbol );
            enum
            {
                Object_Part_Type_ID = 42
            };
            std::int32_t                  symbol;
            std::optional< std::int32_t > type_id;
            Ptr< Tree::IContext >         p_Tree_IContext;
            virtual bool                  test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                  set_inheritance_pointer();
            virtual void                  load( mega::io::Loader &loader );
            virtual void                  store( mega::io::Storer &storer ) const;
            virtual void                  to_json( nlohmann::json &data ) const;
        };
    } // namespace PerSourceSymbols
    namespace Clang
    {
        struct DimensionTrait : public mega::io::Object
        {
            DimensionTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            DimensionTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, Ptr< Tree::DimensionTrait > p_Tree_DimensionTrait,
                            const std::string &canonical_type, const std::size_t &size, const bool &simple,
                            const std::vector< data::Ptr< data::SymbolTable::Symbol > > &symbols );
            enum
            {
                Object_Part_Type_ID = 32
            };
            std::string                                           canonical_type;
            std::size_t                                           size;
            bool                                                  simple;
            std::vector< data::Ptr< data::SymbolTable::Symbol > > symbols;
            Ptr< Tree::DimensionTrait >                           p_Tree_DimensionTrait;
            virtual bool                                          test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                          set_inheritance_pointer();
            virtual void                                          load( mega::io::Loader &loader );
            virtual void                                          store( mega::io::Storer &storer ) const;
            virtual void                                          to_json( nlohmann::json &data ) const;
        };
        struct InheritanceTrait : public mega::io::Object
        {
            InheritanceTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            InheritanceTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, Ptr< Tree::InheritanceTrait > p_Tree_InheritanceTrait,
                              const std::vector< data::Ptr< data::Tree::IContext > > &contexts );
            enum
            {
                Object_Part_Type_ID = 34
            };
            std::vector< data::Ptr< data::Tree::IContext > > contexts;
            Ptr< Tree::InheritanceTrait >                    p_Tree_InheritanceTrait;
            virtual bool                                     test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                     set_inheritance_pointer();
            virtual void                                     load( mega::io::Loader &loader );
            virtual void                                     store( mega::io::Storer &storer ) const;
            virtual void                                     to_json( nlohmann::json &data ) const;
        };
    } // namespace Clang
    namespace Concrete
    {
        struct DimensionTrait : public mega::io::Object
        {
            DimensionTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            DimensionTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, Ptr< Tree::DimensionTrait > p_Tree_DimensionTrait,
                            const std::optional< data::Ptr< data::Concrete::Dimension > > &concrete );
            enum
            {
                Object_Part_Type_ID = 31
            };
            std::optional< data::Ptr< data::Concrete::Dimension > > concrete;
            Ptr< Tree::DimensionTrait >                             p_Tree_DimensionTrait;
            virtual bool                                            test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                            set_inheritance_pointer();
            virtual void                                            load( mega::io::Loader &loader );
            virtual void                                            store( mega::io::Storer &storer ) const;
            virtual void                                            to_json( nlohmann::json &data ) const;
        };
        struct IContext : public mega::io::Object
        {
            IContext( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            IContext( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, Ptr< Tree::IContext > p_Tree_IContext,
                      const std::optional< data::Ptr< data::Concrete::Context > > &concrete );
            enum
            {
                Object_Part_Type_ID = 43
            };
            std::optional< data::Ptr< data::Concrete::Context > > concrete;
            Ptr< Tree::IContext >                                 p_Tree_IContext;
            virtual bool                                          test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                          set_inheritance_pointer();
            virtual void                                          load( mega::io::Loader &loader );
            virtual void                                          store( mega::io::Storer &storer ) const;
            virtual void                                          to_json( nlohmann::json &data ) const;
        };
        struct Dimension : public mega::io::Object
        {
            Dimension( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Dimension( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Tree::DimensionTrait > &interface_dimension );
            enum
            {
                Object_Part_Type_ID = 52
            };
            data::Ptr< data::Tree::DimensionTrait >                interface_dimension;
            std::variant< data::Ptr< data::Concrete::Dimension > > m_inheritance;
            virtual bool                                           test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                           set_inheritance_pointer();
            virtual void                                           load( mega::io::Loader &loader );
            virtual void                                           store( mega::io::Storer &storer ) const;
            virtual void                                           to_json( nlohmann::json &data ) const;
        };
        struct Context : public mega::io::Object
        {
            Context( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Context( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::Tree::IContext > > &inheritance,
                     const std::vector< data::Ptr< data::Concrete::Context > > &children );
            enum
            {
                Object_Part_Type_ID = 53
            };
            std::vector< data::Ptr< data::Tree::IContext > >    inheritance;
            std::vector< data::Ptr< data::Concrete::Context > > children;
            std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Namespace >, data::Ptr< data::Concrete::Action >,
                          data::Ptr< data::Concrete::Event >, data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >,
                          data::Ptr< data::Concrete::Link >, data::Ptr< data::Concrete::Table >, data::Ptr< data::Concrete::Root > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Namespace : public mega::io::Object
        {
            Namespace( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Namespace( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Tree::Namespace > &interface_namespace,
                       const std::vector< data::Ptr< data::Concrete::Dimension > > &dimensions );
            enum
            {
                Object_Part_Type_ID = 54
            };
            data::Ptr< data::Tree::Namespace >                    interface_namespace;
            std::vector< data::Ptr< data::Concrete::Dimension > > dimensions;
            Ptr< Concrete::Context >                              p_Concrete_Context;
            std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Namespace >, data::Ptr< data::Concrete::Action >,
                          data::Ptr< data::Concrete::Event >, data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >,
                          data::Ptr< data::Concrete::Link >, data::Ptr< data::Concrete::Table >, data::Ptr< data::Concrete::Root > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Action : public mega::io::Object
        {
            Action( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Action( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Tree::Action > &interface_action,
                    const std::vector< data::Ptr< data::Concrete::Dimension > > &dimensions );
            enum
            {
                Object_Part_Type_ID = 55
            };
            data::Ptr< data::Tree::Action >                       interface_action;
            std::vector< data::Ptr< data::Concrete::Dimension > > dimensions;
            Ptr< Concrete::Context >                              p_Concrete_Context;
            std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Namespace >, data::Ptr< data::Concrete::Action >,
                          data::Ptr< data::Concrete::Event >, data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >,
                          data::Ptr< data::Concrete::Link >, data::Ptr< data::Concrete::Table >, data::Ptr< data::Concrete::Root > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Event : public mega::io::Object
        {
            Event( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Event( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Tree::Event > &interface_event,
                   const std::vector< data::Ptr< data::Concrete::Dimension > > &dimensions );
            enum
            {
                Object_Part_Type_ID = 56
            };
            data::Ptr< data::Tree::Event >                        interface_event;
            std::vector< data::Ptr< data::Concrete::Dimension > > dimensions;
            Ptr< Concrete::Context >                              p_Concrete_Context;
            std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Namespace >, data::Ptr< data::Concrete::Action >,
                          data::Ptr< data::Concrete::Event >, data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >,
                          data::Ptr< data::Concrete::Link >, data::Ptr< data::Concrete::Table >, data::Ptr< data::Concrete::Root > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Function : public mega::io::Object
        {
            Function( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Function( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Tree::Function > &interface_function );
            enum
            {
                Object_Part_Type_ID = 57
            };
            data::Ptr< data::Tree::Function > interface_function;
            Ptr< Concrete::Context >          p_Concrete_Context;
            std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Namespace >, data::Ptr< data::Concrete::Action >,
                          data::Ptr< data::Concrete::Event >, data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >,
                          data::Ptr< data::Concrete::Link >, data::Ptr< data::Concrete::Table >, data::Ptr< data::Concrete::Root > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Object : public mega::io::Object
        {
            Object( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Object( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Tree::Object > &interface_object,
                    const std::vector< data::Ptr< data::Concrete::Dimension > > &dimensions );
            enum
            {
                Object_Part_Type_ID = 58
            };
            data::Ptr< data::Tree::Object >                       interface_object;
            std::vector< data::Ptr< data::Concrete::Dimension > > dimensions;
            Ptr< Concrete::Context >                              p_Concrete_Context;
            std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Namespace >, data::Ptr< data::Concrete::Action >,
                          data::Ptr< data::Concrete::Event >, data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >,
                          data::Ptr< data::Concrete::Link >, data::Ptr< data::Concrete::Table >, data::Ptr< data::Concrete::Root > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Link : public mega::io::Object
        {
            Link( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Link( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Tree::Link > &interface_link );
            enum
            {
                Object_Part_Type_ID = 59
            };
            data::Ptr< data::Tree::Link > interface_link;
            Ptr< Concrete::Context >      p_Concrete_Context;
            std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Namespace >, data::Ptr< data::Concrete::Action >,
                          data::Ptr< data::Concrete::Event >, data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >,
                          data::Ptr< data::Concrete::Link >, data::Ptr< data::Concrete::Table >, data::Ptr< data::Concrete::Root > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Table : public mega::io::Object
        {
            Table( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Table( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Tree::Table > &interface_table );
            enum
            {
                Object_Part_Type_ID = 60
            };
            data::Ptr< data::Tree::Table > interface_table;
            Ptr< Concrete::Context >       p_Concrete_Context;
            std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Namespace >, data::Ptr< data::Concrete::Action >,
                          data::Ptr< data::Concrete::Event >, data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >,
                          data::Ptr< data::Concrete::Link >, data::Ptr< data::Concrete::Table >, data::Ptr< data::Concrete::Root > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Root : public mega::io::Object
        {
            Root( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            enum
            {
                Object_Part_Type_ID = 61
            };
            Ptr< Concrete::Context > p_Concrete_Context;
            std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Namespace >, data::Ptr< data::Concrete::Action >,
                          data::Ptr< data::Concrete::Event >, data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >,
                          data::Ptr< data::Concrete::Link >, data::Ptr< data::Concrete::Table >, data::Ptr< data::Concrete::Root > >
                         m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
    } // namespace Concrete
    namespace Model
    {
        struct ObjectGraph : public mega::io::Object
        {
            ObjectGraph( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            ObjectGraph( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const mega::io::megaFilePath &source_file,
                         const std::size_t &hash_code );
            enum
            {
                Object_Part_Type_ID = 79
            };
            mega::io::megaFilePath                                source_file;
            std::size_t                                           hash_code;
            std::variant< data::Ptr< data::Model::ObjectGraph > > m_inheritance;
            virtual bool                                          test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                          set_inheritance_pointer();
            virtual void                                          load( mega::io::Loader &loader );
            virtual void                                          store( mega::io::Storer &storer ) const;
            virtual void                                          to_json( nlohmann::json &data ) const;
        };
        struct Graph : public mega::io::Object
        {
            Graph( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Graph( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::Model::ObjectGraph > > &objects );
            enum
            {
                Object_Part_Type_ID = 80
            };
            std::vector< data::Ptr< data::Model::ObjectGraph > > objects;
            std::variant< data::Ptr< data::Model::Graph > >      m_inheritance;
            virtual bool                                         test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                         set_inheritance_pointer();
            virtual void                                         load( mega::io::Loader &loader );
            virtual void                                         store( mega::io::Storer &storer ) const;
            virtual void                                         to_json( nlohmann::json &data ) const;
        };
    } // namespace Model
    namespace Derivations
    {
        struct ObjectMapping : public mega::io::Object
        {
            ObjectMapping( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            ObjectMapping( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const mega::io::megaFilePath &source_file,
                           const std::size_t                                                                           &hash_code,
                           const std::multimap< data::Ptr< data::Tree::IContext >, data::Ptr< data::Tree::IContext > > &inheritance );
            enum
            {
                Object_Part_Type_ID = 81
            };
            mega::io::megaFilePath                                                                source_file;
            std::size_t                                                                           hash_code;
            std::multimap< data::Ptr< data::Tree::IContext >, data::Ptr< data::Tree::IContext > > inheritance;
            std::variant< data::Ptr< data::Derivations::ObjectMapping > >                         m_inheritance;
            virtual bool                                                                          test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                                          set_inheritance_pointer();
            virtual void                                                                          load( mega::io::Loader &loader );
            virtual void                                                                          store( mega::io::Storer &storer ) const;
            virtual void                                                                          to_json( nlohmann::json &data ) const;
        };
        struct Mapping : public mega::io::Object
        {
            Mapping( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Mapping( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                     const std::vector< data::Ptr< data::Derivations::ObjectMapping > > &mappings );
            enum
            {
                Object_Part_Type_ID = 82
            };
            std::vector< data::Ptr< data::Derivations::ObjectMapping > >                                           mappings;
            std::optional< std::multimap< data::Ptr< data::Tree::IContext >, data::Ptr< data::Tree::IContext > > > inheritance;
            std::variant< data::Ptr< data::Derivations::Mapping > >                                                m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
    } // namespace Derivations
    namespace Operations
    {
        struct InterfaceVariant : public mega::io::Object
        {
            InterfaceVariant( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            InterfaceVariant( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                              const std::optional< data::Ptr< data::Tree::IContext > >       &context,
                              const std::optional< data::Ptr< data::Tree::DimensionTrait > > &dimension );
            enum
            {
                Object_Part_Type_ID = 62
            };
            std::optional< data::Ptr< data::Tree::IContext > >              context;
            std::optional< data::Ptr< data::Tree::DimensionTrait > >        dimension;
            std::variant< data::Ptr< data::Operations::InterfaceVariant > > m_inheritance;
            virtual bool                                                    test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                    set_inheritance_pointer();
            virtual void                                                    load( mega::io::Loader &loader );
            virtual void                                                    store( mega::io::Storer &storer ) const;
            virtual void                                                    to_json( nlohmann::json &data ) const;
        };
        struct ConcreteVariant : public mega::io::Object
        {
            ConcreteVariant( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            ConcreteVariant( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                             const std::optional< data::Ptr< data::Concrete::Context > >   &context,
                             const std::optional< data::Ptr< data::Concrete::Dimension > > &dimension );
            enum
            {
                Object_Part_Type_ID = 63
            };
            std::optional< data::Ptr< data::Concrete::Context > >          context;
            std::optional< data::Ptr< data::Concrete::Dimension > >        dimension;
            std::variant< data::Ptr< data::Operations::ConcreteVariant > > m_inheritance;
            virtual bool                                                   test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                   set_inheritance_pointer();
            virtual void                                                   load( mega::io::Loader &loader );
            virtual void                                                   store( mega::io::Storer &storer ) const;
            virtual void                                                   to_json( nlohmann::json &data ) const;
        };
        struct Element : public mega::io::Object
        {
            Element( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Element( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Operations::InterfaceVariant > &interface,
                     const data::Ptr< data::Operations::ConcreteVariant > &concrete );
            enum
            {
                Object_Part_Type_ID = 64
            };
            data::Ptr< data::Operations::InterfaceVariant >        interface;
            data::Ptr< data::Operations::ConcreteVariant >         concrete;
            std::variant< data::Ptr< data::Operations::Element > > m_inheritance;
            virtual bool                                           test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                           set_inheritance_pointer();
            virtual void                                           load( mega::io::Loader &loader );
            virtual void                                           store( mega::io::Storer &storer ) const;
            virtual void                                           to_json( nlohmann::json &data ) const;
        };
        struct ElementVector : public mega::io::Object
        {
            ElementVector( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            ElementVector( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                           const std::vector< data::Ptr< data::Operations::Element > > &elements );
            enum
            {
                Object_Part_Type_ID = 65
            };
            std::vector< data::Ptr< data::Operations::Element > >        elements;
            std::variant< data::Ptr< data::Operations::ElementVector > > m_inheritance;
            virtual bool                                                 test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                 set_inheritance_pointer();
            virtual void                                                 load( mega::io::Loader &loader );
            virtual void                                                 store( mega::io::Storer &storer ) const;
            virtual void                                                 to_json( nlohmann::json &data ) const;
        };
        struct Context : public mega::io::Object
        {
            Context( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Context( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                     const std::vector< data::Ptr< data::Operations::ElementVector > > &vectors );
            enum
            {
                Object_Part_Type_ID = 66
            };
            std::vector< data::Ptr< data::Operations::ElementVector > > vectors;
            std::variant< data::Ptr< data::Operations::Context > >      m_inheritance;
            virtual bool                                                test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                set_inheritance_pointer();
            virtual void                                                load( mega::io::Loader &loader );
            virtual void                                                store( mega::io::Storer &storer ) const;
            virtual void                                                to_json( nlohmann::json &data ) const;
        };
        struct TypePath : public mega::io::Object
        {
            TypePath( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            TypePath( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                      const std::vector< data::Ptr< data::Operations::ElementVector > > &vectors );
            enum
            {
                Object_Part_Type_ID = 67
            };
            std::vector< data::Ptr< data::Operations::ElementVector > > vectors;
            std::variant< data::Ptr< data::Operations::TypePath > >     m_inheritance;
            virtual bool                                                test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                set_inheritance_pointer();
            virtual void                                                load( mega::io::Loader &loader );
            virtual void                                                store( mega::io::Storer &storer ) const;
            virtual void                                                to_json( nlohmann::json &data ) const;
        };
        struct NameRoot : public mega::io::Object
        {
            NameRoot( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            NameRoot( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::Operations::Name > > &children );
            enum
            {
                Object_Part_Type_ID = 68
            };
            std::vector< data::Ptr< data::Operations::Name > >                                           children;
            std::variant< data::Ptr< data::Operations::NameRoot >, data::Ptr< data::Operations::Name > > m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct Name : public mega::io::Object
        {
            Name( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Name( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Operations::Element > &element,
                  const bool &is_member, const bool &is_reference );
            enum
            {
                Object_Part_Type_ID = 69
            };
            data::Ptr< data::Operations::Element >                                                       element;
            bool                                                                                         is_member;
            bool                                                                                         is_reference;
            Ptr< Operations::NameRoot >                                                                  p_Operations_NameRoot;
            std::variant< data::Ptr< data::Operations::NameRoot >, data::Ptr< data::Operations::Name > > m_inheritance;
            virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void set_inheritance_pointer();
            virtual void load( mega::io::Loader &loader );
            virtual void store( mega::io::Storer &storer ) const;
            virtual void to_json( nlohmann::json &data ) const;
        };
        struct NameResolution : public mega::io::Object
        {
            NameResolution( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            NameResolution( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Operations::NameRoot > &root_name );
            enum
            {
                Object_Part_Type_ID = 70
            };
            data::Ptr< data::Operations::NameRoot >                       root_name;
            std::variant< data::Ptr< data::Operations::NameResolution > > m_inheritance;
            virtual bool                                                  test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                  set_inheritance_pointer();
            virtual void                                                  load( mega::io::Loader &loader );
            virtual void                                                  store( mega::io::Storer &storer ) const;
            virtual void                                                  to_json( nlohmann::json &data ) const;
        };
        struct Invocation : public mega::io::Object
        {
            Invocation( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Invocation( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Operations::Context > &context,
                        const data::Ptr< data::Operations::TypePath > &type_path, const mega::OperationID &operation );
            enum
            {
                Object_Part_Type_ID = 71
            };
            data::Ptr< data::Operations::Context >                         context;
            data::Ptr< data::Operations::TypePath >                        type_path;
            mega::OperationID                                              operation;
            std::optional< data::Ptr< data::Operations::NameResolution > > name_resolution;
            std::variant< data::Ptr< data::Operations::Invocation > >      m_inheritance;
            virtual bool                                                   test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                   set_inheritance_pointer();
            virtual void                                                   load( mega::io::Loader &loader );
            virtual void                                                   store( mega::io::Storer &storer ) const;
            virtual void                                                   to_json( nlohmann::json &data ) const;
        };
        struct Invocations : public mega::io::Object
        {
            Invocations( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
            Invocations( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                         const std::map< mega::invocation::ID, data::Ptr< data::Operations::Invocation > > &invocations );
            enum
            {
                Object_Part_Type_ID = 72
            };
            std::map< mega::invocation::ID, data::Ptr< data::Operations::Invocation > > invocations;
            std::variant< data::Ptr< data::Operations::Invocations > >                  m_inheritance;
            virtual bool                                                                test_inheritance_pointer( ObjectPartLoader &loader ) const;
            virtual void                                                                set_inheritance_pointer();
            virtual void                                                                load( mega::io::Loader &loader );
            virtual void                                                                store( mega::io::Storer &storer ) const;
            virtual void                                                                to_json( nlohmann::json &data ) const;
        };
    } // namespace Operations

    template <> inline Ptr< Components::Component >  convert( const Ptr< Components::Component > &from ) { return from; }
    template <> inline Ptr< AST::Identifier >        convert( const Ptr< AST::Identifier > &from ) { return from; }
    template <> inline Ptr< AST::ScopedIdentifier >  convert( const Ptr< AST::ScopedIdentifier > &from ) { return from; }
    template <> inline Ptr< AST::ArgumentList >      convert( const Ptr< AST::ArgumentList > &from ) { return from; }
    template <> inline Ptr< AST::ReturnType >        convert( const Ptr< AST::ReturnType > &from ) { return from; }
    template <> inline Ptr< AST::Inheritance >       convert( const Ptr< AST::Inheritance > &from ) { return from; }
    template <> inline Ptr< AST::Size >              convert( const Ptr< AST::Size > &from ) { return from; }
    template <> inline Ptr< AST::Link >              convert( const Ptr< AST::Link > &from ) { return from; }
    template <> inline Ptr< AST::Dimension >         convert( const Ptr< AST::Dimension > &from ) { return from; }
    template <> inline Ptr< AST::Include >           convert( const Ptr< AST::Include > &from ) { return from; }
    template <> inline Ptr< AST::Include >           convert( const Ptr< AST::SystemInclude > &from ) { return from->p_AST_Include; }
    template <> inline Ptr< AST::SystemInclude >     convert( const Ptr< AST::SystemInclude > &from ) { return from; }
    template <> inline Ptr< AST::Include >           convert( const Ptr< AST::MegaInclude > &from ) { return from->p_AST_Include; }
    template <> inline Ptr< AST::MegaInclude >       convert( const Ptr< AST::MegaInclude > &from ) { return from; }
    template <> inline Ptr< AST::Include >           convert( const Ptr< AST::MegaIncludeInline > &from ) { return from->p_AST_MegaInclude->p_AST_Include; }
    template <> inline Ptr< AST::MegaInclude >       convert( const Ptr< AST::MegaIncludeInline > &from ) { return from->p_AST_MegaInclude; }
    template <> inline Ptr< AST::MegaIncludeInline > convert( const Ptr< AST::MegaIncludeInline > &from ) { return from; }
    template <> inline Ptr< AST::Include >           convert( const Ptr< AST::MegaIncludeNested > &from ) { return from->p_AST_MegaInclude->p_AST_Include; }
    template <> inline Ptr< AST::MegaInclude >       convert( const Ptr< AST::MegaIncludeNested > &from ) { return from->p_AST_MegaInclude; }
    template <> inline Ptr< AST::MegaIncludeNested > convert( const Ptr< AST::MegaIncludeNested > &from ) { return from; }
    template <> inline Ptr< AST::Include >           convert( const Ptr< AST::CPPInclude > &from ) { return from->p_AST_Include; }
    template <> inline Ptr< AST::CPPInclude >        convert( const Ptr< AST::CPPInclude > &from ) { return from; }
    template <> inline Ptr< AST::Dependency >        convert( const Ptr< AST::Dependency > &from ) { return from; }
    template <> inline Ptr< AST::ContextDef >        convert( const Ptr< AST::ContextDef > &from ) { return from; }
    template <> inline Ptr< Body::ContextDef >       convert( const Ptr< AST::ContextDef > &from ) { return from->p_Body_ContextDef; }
    template <> inline Ptr< AST::ContextDef >        convert( const Ptr< AST::NamespaceDef > &from ) { return from->p_AST_ContextDef; }
    template <> inline Ptr< Body::ContextDef >       convert( const Ptr< AST::NamespaceDef > &from ) { return from->p_AST_ContextDef->p_Body_ContextDef; }
    template <> inline Ptr< AST::NamespaceDef >      convert( const Ptr< AST::NamespaceDef > &from ) { return from; }
    template <> inline Ptr< AST::ContextDef >        convert( const Ptr< AST::AbstractDef > &from ) { return from->p_AST_ContextDef; }
    template <> inline Ptr< Body::ContextDef >       convert( const Ptr< AST::AbstractDef > &from ) { return from->p_AST_ContextDef->p_Body_ContextDef; }
    template <> inline Ptr< AST::AbstractDef >       convert( const Ptr< AST::AbstractDef > &from ) { return from; }
    template <> inline Ptr< AST::ContextDef >        convert( const Ptr< AST::ActionDef > &from ) { return from->p_AST_ContextDef; }
    template <> inline Ptr< Body::ContextDef >       convert( const Ptr< AST::ActionDef > &from ) { return from->p_AST_ContextDef->p_Body_ContextDef; }
    template <> inline Ptr< AST::ActionDef >         convert( const Ptr< AST::ActionDef > &from ) { return from; }
    template <> inline Ptr< AST::ContextDef >        convert( const Ptr< AST::EventDef > &from ) { return from->p_AST_ContextDef; }
    template <> inline Ptr< Body::ContextDef >       convert( const Ptr< AST::EventDef > &from ) { return from->p_AST_ContextDef->p_Body_ContextDef; }
    template <> inline Ptr< AST::EventDef >          convert( const Ptr< AST::EventDef > &from ) { return from; }
    template <> inline Ptr< AST::ContextDef >        convert( const Ptr< AST::FunctionDef > &from ) { return from->p_AST_ContextDef; }
    template <> inline Ptr< Body::ContextDef >       convert( const Ptr< AST::FunctionDef > &from ) { return from->p_AST_ContextDef->p_Body_ContextDef; }
    template <> inline Ptr< AST::FunctionDef >       convert( const Ptr< AST::FunctionDef > &from ) { return from; }
    template <> inline Ptr< AST::ContextDef >        convert( const Ptr< AST::ObjectDef > &from ) { return from->p_AST_ContextDef; }
    template <> inline Ptr< Body::ContextDef >       convert( const Ptr< AST::ObjectDef > &from ) { return from->p_AST_ContextDef->p_Body_ContextDef; }
    template <> inline Ptr< AST::ObjectDef >         convert( const Ptr< AST::ObjectDef > &from ) { return from; }
    template <> inline Ptr< AST::ContextDef >        convert( const Ptr< AST::LinkDef > &from ) { return from->p_AST_ContextDef; }
    template <> inline Ptr< Body::ContextDef >       convert( const Ptr< AST::LinkDef > &from ) { return from->p_AST_ContextDef->p_Body_ContextDef; }
    template <> inline Ptr< AST::LinkDef >           convert( const Ptr< AST::LinkDef > &from ) { return from; }
    template <> inline Ptr< AST::ContextDef >        convert( const Ptr< AST::TableDef > &from ) { return from->p_AST_ContextDef; }
    template <> inline Ptr< Body::ContextDef >       convert( const Ptr< AST::TableDef > &from ) { return from->p_AST_ContextDef->p_Body_ContextDef; }
    template <> inline Ptr< AST::TableDef >          convert( const Ptr< AST::TableDef > &from ) { return from; }
    template <> inline Ptr< AST::SourceRoot >        convert( const Ptr< AST::SourceRoot > &from ) { return from; }
    template <> inline Ptr< AST::SourceRoot >        convert( const Ptr< AST::IncludeRoot > &from ) { return from->p_AST_SourceRoot; }
    template <> inline Ptr< AST::IncludeRoot >       convert( const Ptr< AST::IncludeRoot > &from ) { return from; }
    template <> inline Ptr< AST::SourceRoot >        convert( const Ptr< AST::ObjectSourceRoot > &from ) { return from->p_AST_SourceRoot; }
    template <> inline Ptr< AST::ObjectSourceRoot >  convert( const Ptr< AST::ObjectSourceRoot > &from ) { return from; }
    template <> inline Ptr< AST::Dimension >         convert( const Ptr< Tree::DimensionTrait > &from ) { return from->p_AST_Dimension; }
    template <> inline Ptr< Tree::DimensionTrait >   convert( const Ptr< Tree::DimensionTrait > &from ) { return from; }
    template <> inline Ptr< PerSourceSymbols::DimensionTrait > convert( const Ptr< Tree::DimensionTrait > &from )
    {
        return from->p_PerSourceSymbols_DimensionTrait;
    }
    template <> inline Ptr< Concrete::DimensionTrait >   convert( const Ptr< Tree::DimensionTrait > &from ) { return from->p_Concrete_DimensionTrait; }
    template <> inline Ptr< Clang::DimensionTrait >      convert( const Ptr< Tree::DimensionTrait > &from ) { return from->p_Clang_DimensionTrait; }
    template <> inline Ptr< AST::Inheritance >           convert( const Ptr< Tree::InheritanceTrait > &from ) { return from->p_AST_Inheritance; }
    template <> inline Ptr< Tree::InheritanceTrait >     convert( const Ptr< Tree::InheritanceTrait > &from ) { return from; }
    template <> inline Ptr< Clang::InheritanceTrait >    convert( const Ptr< Tree::InheritanceTrait > &from ) { return from->p_Clang_InheritanceTrait; }
    template <> inline Ptr< AST::Link >                  convert( const Ptr< Tree::LinkTrait > &from ) { return from->p_AST_Link; }
    template <> inline Ptr< Tree::LinkTrait >            convert( const Ptr< Tree::LinkTrait > &from ) { return from; }
    template <> inline Ptr< AST::ReturnType >            convert( const Ptr< Tree::ReturnTypeTrait > &from ) { return from->p_AST_ReturnType; }
    template <> inline Ptr< Tree::ReturnTypeTrait >      convert( const Ptr< Tree::ReturnTypeTrait > &from ) { return from; }
    template <> inline Ptr< AST::ArgumentList >          convert( const Ptr< Tree::ArgumentListTrait > &from ) { return from->p_AST_ArgumentList; }
    template <> inline Ptr< Tree::ArgumentListTrait >    convert( const Ptr< Tree::ArgumentListTrait > &from ) { return from; }
    template <> inline Ptr< AST::Size >                  convert( const Ptr< Tree::SizeTrait > &from ) { return from->p_AST_Size; }
    template <> inline Ptr< Tree::SizeTrait >            convert( const Ptr< Tree::SizeTrait > &from ) { return from; }
    template <> inline Ptr< Tree::ContextGroup >         convert( const Ptr< Tree::ContextGroup > &from ) { return from; }
    template <> inline Ptr< Tree::ContextGroup >         convert( const Ptr< Tree::Root > &from ) { return from->p_Tree_ContextGroup; }
    template <> inline Ptr< Tree::Root >                 convert( const Ptr< Tree::Root > &from ) { return from; }
    template <> inline Ptr< Tree::ContextGroup >         convert( const Ptr< Tree::IContext > &from ) { return from->p_Tree_ContextGroup; }
    template <> inline Ptr< Tree::IContext >             convert( const Ptr< Tree::IContext > &from ) { return from; }
    template <> inline Ptr< PerSourceSymbols::IContext > convert( const Ptr< Tree::IContext > &from ) { return from->p_PerSourceSymbols_IContext; }
    template <> inline Ptr< Concrete::IContext >         convert( const Ptr< Tree::IContext > &from ) { return from->p_Concrete_IContext; }
    template <> inline Ptr< Tree::ContextGroup >         convert( const Ptr< Tree::Namespace > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    template <> inline Ptr< Tree::IContext >             convert( const Ptr< Tree::Namespace > &from ) { return from->p_Tree_IContext; }
    template <> inline Ptr< PerSourceSymbols::IContext > convert( const Ptr< Tree::Namespace > &from )
    {
        return from->p_Tree_IContext->p_PerSourceSymbols_IContext;
    }
    template <> inline Ptr< Concrete::IContext >         convert( const Ptr< Tree::Namespace > &from ) { return from->p_Tree_IContext->p_Concrete_IContext; }
    template <> inline Ptr< Tree::Namespace >            convert( const Ptr< Tree::Namespace > &from ) { return from; }
    template <> inline Ptr< Tree::ContextGroup >         convert( const Ptr< Tree::Abstract > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    template <> inline Ptr< Tree::IContext >             convert( const Ptr< Tree::Abstract > &from ) { return from->p_Tree_IContext; }
    template <> inline Ptr< PerSourceSymbols::IContext > convert( const Ptr< Tree::Abstract > &from )
    {
        return from->p_Tree_IContext->p_PerSourceSymbols_IContext;
    }
    template <> inline Ptr< Concrete::IContext >         convert( const Ptr< Tree::Abstract > &from ) { return from->p_Tree_IContext->p_Concrete_IContext; }
    template <> inline Ptr< Tree::Abstract >             convert( const Ptr< Tree::Abstract > &from ) { return from; }
    template <> inline Ptr< Tree::ContextGroup >         convert( const Ptr< Tree::Action > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    template <> inline Ptr< Tree::IContext >             convert( const Ptr< Tree::Action > &from ) { return from->p_Tree_IContext; }
    template <> inline Ptr< PerSourceSymbols::IContext > convert( const Ptr< Tree::Action > &from )
    {
        return from->p_Tree_IContext->p_PerSourceSymbols_IContext;
    }
    template <> inline Ptr< Concrete::IContext >         convert( const Ptr< Tree::Action > &from ) { return from->p_Tree_IContext->p_Concrete_IContext; }
    template <> inline Ptr< Tree::Action >               convert( const Ptr< Tree::Action > &from ) { return from; }
    template <> inline Ptr< Tree::ContextGroup >         convert( const Ptr< Tree::Event > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    template <> inline Ptr< Tree::IContext >             convert( const Ptr< Tree::Event > &from ) { return from->p_Tree_IContext; }
    template <> inline Ptr< PerSourceSymbols::IContext > convert( const Ptr< Tree::Event > &from )
    {
        return from->p_Tree_IContext->p_PerSourceSymbols_IContext;
    }
    template <> inline Ptr< Concrete::IContext >         convert( const Ptr< Tree::Event > &from ) { return from->p_Tree_IContext->p_Concrete_IContext; }
    template <> inline Ptr< Tree::Event >                convert( const Ptr< Tree::Event > &from ) { return from; }
    template <> inline Ptr< Tree::ContextGroup >         convert( const Ptr< Tree::Function > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    template <> inline Ptr< Tree::IContext >             convert( const Ptr< Tree::Function > &from ) { return from->p_Tree_IContext; }
    template <> inline Ptr< PerSourceSymbols::IContext > convert( const Ptr< Tree::Function > &from )
    {
        return from->p_Tree_IContext->p_PerSourceSymbols_IContext;
    }
    template <> inline Ptr< Concrete::IContext >         convert( const Ptr< Tree::Function > &from ) { return from->p_Tree_IContext->p_Concrete_IContext; }
    template <> inline Ptr< Tree::Function >             convert( const Ptr< Tree::Function > &from ) { return from; }
    template <> inline Ptr< Tree::ContextGroup >         convert( const Ptr< Tree::Object > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    template <> inline Ptr< Tree::IContext >             convert( const Ptr< Tree::Object > &from ) { return from->p_Tree_IContext; }
    template <> inline Ptr< PerSourceSymbols::IContext > convert( const Ptr< Tree::Object > &from )
    {
        return from->p_Tree_IContext->p_PerSourceSymbols_IContext;
    }
    template <> inline Ptr< Concrete::IContext >         convert( const Ptr< Tree::Object > &from ) { return from->p_Tree_IContext->p_Concrete_IContext; }
    template <> inline Ptr< Tree::Object >               convert( const Ptr< Tree::Object > &from ) { return from; }
    template <> inline Ptr< Tree::ContextGroup >         convert( const Ptr< Tree::Link > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    template <> inline Ptr< Tree::IContext >             convert( const Ptr< Tree::Link > &from ) { return from->p_Tree_IContext; }
    template <> inline Ptr< PerSourceSymbols::IContext > convert( const Ptr< Tree::Link > &from ) { return from->p_Tree_IContext->p_PerSourceSymbols_IContext; }
    template <> inline Ptr< Concrete::IContext >         convert( const Ptr< Tree::Link > &from ) { return from->p_Tree_IContext->p_Concrete_IContext; }
    template <> inline Ptr< Tree::Link >                 convert( const Ptr< Tree::Link > &from ) { return from; }
    template <> inline Ptr< Tree::ContextGroup >         convert( const Ptr< Tree::Table > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    template <> inline Ptr< Tree::IContext >             convert( const Ptr< Tree::Table > &from ) { return from->p_Tree_IContext; }
    template <> inline Ptr< PerSourceSymbols::IContext > convert( const Ptr< Tree::Table > &from )
    {
        return from->p_Tree_IContext->p_PerSourceSymbols_IContext;
    }
    template <> inline Ptr< Concrete::IContext >           convert( const Ptr< Tree::Table > &from ) { return from->p_Tree_IContext->p_Concrete_IContext; }
    template <> inline Ptr< Tree::Table >                  convert( const Ptr< Tree::Table > &from ) { return from; }
    template <> inline Ptr< Concrete::Dimension >          convert( const Ptr< Concrete::Dimension > &from ) { return from; }
    template <> inline Ptr< Concrete::Context >            convert( const Ptr< Concrete::Context > &from ) { return from; }
    template <> inline Ptr< Concrete::Context >            convert( const Ptr< Concrete::Namespace > &from ) { return from->p_Concrete_Context; }
    template <> inline Ptr< Concrete::Namespace >          convert( const Ptr< Concrete::Namespace > &from ) { return from; }
    template <> inline Ptr< Concrete::Context >            convert( const Ptr< Concrete::Action > &from ) { return from->p_Concrete_Context; }
    template <> inline Ptr< Concrete::Action >             convert( const Ptr< Concrete::Action > &from ) { return from; }
    template <> inline Ptr< Concrete::Context >            convert( const Ptr< Concrete::Event > &from ) { return from->p_Concrete_Context; }
    template <> inline Ptr< Concrete::Event >              convert( const Ptr< Concrete::Event > &from ) { return from; }
    template <> inline Ptr< Concrete::Context >            convert( const Ptr< Concrete::Function > &from ) { return from->p_Concrete_Context; }
    template <> inline Ptr< Concrete::Function >           convert( const Ptr< Concrete::Function > &from ) { return from; }
    template <> inline Ptr< Concrete::Context >            convert( const Ptr< Concrete::Object > &from ) { return from->p_Concrete_Context; }
    template <> inline Ptr< Concrete::Object >             convert( const Ptr< Concrete::Object > &from ) { return from; }
    template <> inline Ptr< Concrete::Context >            convert( const Ptr< Concrete::Link > &from ) { return from->p_Concrete_Context; }
    template <> inline Ptr< Concrete::Link >               convert( const Ptr< Concrete::Link > &from ) { return from; }
    template <> inline Ptr< Concrete::Context >            convert( const Ptr< Concrete::Table > &from ) { return from->p_Concrete_Context; }
    template <> inline Ptr< Concrete::Table >              convert( const Ptr< Concrete::Table > &from ) { return from; }
    template <> inline Ptr< Concrete::Context >            convert( const Ptr< Concrete::Root > &from ) { return from->p_Concrete_Context; }
    template <> inline Ptr< Concrete::Root >               convert( const Ptr< Concrete::Root > &from ) { return from; }
    template <> inline Ptr< Operations::InterfaceVariant > convert( const Ptr< Operations::InterfaceVariant > &from ) { return from; }
    template <> inline Ptr< Operations::ConcreteVariant >  convert( const Ptr< Operations::ConcreteVariant > &from ) { return from; }
    template <> inline Ptr< Operations::Element >          convert( const Ptr< Operations::Element > &from ) { return from; }
    template <> inline Ptr< Operations::ElementVector >    convert( const Ptr< Operations::ElementVector > &from ) { return from; }
    template <> inline Ptr< Operations::Context >          convert( const Ptr< Operations::Context > &from ) { return from; }
    template <> inline Ptr< Operations::TypePath >         convert( const Ptr< Operations::TypePath > &from ) { return from; }
    template <> inline Ptr< Operations::NameRoot >         convert( const Ptr< Operations::NameRoot > &from ) { return from; }
    template <> inline Ptr< Operations::NameRoot >         convert( const Ptr< Operations::Name > &from ) { return from->p_Operations_NameRoot; }
    template <> inline Ptr< Operations::Name >             convert( const Ptr< Operations::Name > &from ) { return from; }
    template <> inline Ptr< Operations::NameResolution >   convert( const Ptr< Operations::NameResolution > &from ) { return from; }
    template <> inline Ptr< Operations::Invocation >       convert( const Ptr< Operations::Invocation > &from ) { return from; }
    template <> inline Ptr< Operations::Invocations >      convert( const Ptr< Operations::Invocations > &from ) { return from; }
    template <> inline Ptr< DPGraph::Glob >                convert( const Ptr< DPGraph::Glob > &from ) { return from; }
    template <> inline Ptr< DPGraph::ObjectDependencies >  convert( const Ptr< DPGraph::ObjectDependencies > &from ) { return from; }
    template <> inline Ptr< DPGraph::Analysis >            convert( const Ptr< DPGraph::Analysis > &from ) { return from; }
    template <> inline Ptr< SymbolTable::Symbol >          convert( const Ptr< SymbolTable::Symbol > &from ) { return from; }
    template <> inline Ptr< SymbolTable::SymbolSet >       convert( const Ptr< SymbolTable::SymbolSet > &from ) { return from; }
    template <> inline Ptr< SymbolTable::SymbolTable >     convert( const Ptr< SymbolTable::SymbolTable > &from ) { return from; }
    template <> inline Ptr< Model::ObjectGraph >           convert( const Ptr< Model::ObjectGraph > &from ) { return from; }
    template <> inline Ptr< Model::Graph >                 convert( const Ptr< Model::Graph > &from ) { return from; }
    template <> inline Ptr< Derivations::ObjectMapping >   convert( const Ptr< Derivations::ObjectMapping > &from ) { return from; }
    template <> inline Ptr< Derivations::Mapping >         convert( const Ptr< Derivations::Mapping > &from ) { return from; }
    inline Ptr< Components::Component >                    to_base( const Ptr< Components::Component > &from ) { return from; }
    inline Ptr< AST::Identifier >                          to_base( const Ptr< AST::Identifier > &from ) { return from; }
    inline Ptr< AST::ScopedIdentifier >                    to_base( const Ptr< AST::ScopedIdentifier > &from ) { return from; }
    inline Ptr< AST::ArgumentList >                        to_base( const Ptr< AST::ArgumentList > &from ) { return from; }
    inline Ptr< AST::ReturnType >                          to_base( const Ptr< AST::ReturnType > &from ) { return from; }
    inline Ptr< AST::Inheritance >                         to_base( const Ptr< AST::Inheritance > &from ) { return from; }
    inline Ptr< AST::Size >                                to_base( const Ptr< AST::Size > &from ) { return from; }
    inline Ptr< AST::Link >                                to_base( const Ptr< AST::Link > &from ) { return from; }
    inline Ptr< AST::Dimension >                           to_base( const Ptr< AST::Dimension > &from ) { return from; }
    inline Ptr< AST::Include >                             to_base( const Ptr< AST::Include > &from ) { return from; }
    inline Ptr< AST::Include >                             to_base( const Ptr< AST::SystemInclude > &from ) { return from->p_AST_Include; }
    inline Ptr< AST::Include >                             to_base( const Ptr< AST::MegaInclude > &from ) { return from->p_AST_Include; }
    inline Ptr< AST::Include >                     to_base( const Ptr< AST::MegaIncludeInline > &from ) { return from->p_AST_MegaInclude->p_AST_Include; }
    inline Ptr< AST::Include >                     to_base( const Ptr< AST::MegaIncludeNested > &from ) { return from->p_AST_MegaInclude->p_AST_Include; }
    inline Ptr< AST::Include >                     to_base( const Ptr< AST::CPPInclude > &from ) { return from->p_AST_Include; }
    inline Ptr< AST::Dependency >                  to_base( const Ptr< AST::Dependency > &from ) { return from; }
    inline Ptr< AST::ContextDef >                  to_base( const Ptr< AST::ContextDef > &from ) { return from; }
    inline Ptr< AST::ContextDef >                  to_base( const Ptr< AST::NamespaceDef > &from ) { return from->p_AST_ContextDef; }
    inline Ptr< AST::ContextDef >                  to_base( const Ptr< AST::AbstractDef > &from ) { return from->p_AST_ContextDef; }
    inline Ptr< AST::ContextDef >                  to_base( const Ptr< AST::ActionDef > &from ) { return from->p_AST_ContextDef; }
    inline Ptr< AST::ContextDef >                  to_base( const Ptr< AST::EventDef > &from ) { return from->p_AST_ContextDef; }
    inline Ptr< AST::ContextDef >                  to_base( const Ptr< AST::FunctionDef > &from ) { return from->p_AST_ContextDef; }
    inline Ptr< AST::ContextDef >                  to_base( const Ptr< AST::ObjectDef > &from ) { return from->p_AST_ContextDef; }
    inline Ptr< AST::ContextDef >                  to_base( const Ptr< AST::LinkDef > &from ) { return from->p_AST_ContextDef; }
    inline Ptr< AST::ContextDef >                  to_base( const Ptr< AST::TableDef > &from ) { return from->p_AST_ContextDef; }
    inline Ptr< AST::SourceRoot >                  to_base( const Ptr< AST::SourceRoot > &from ) { return from; }
    inline Ptr< AST::SourceRoot >                  to_base( const Ptr< AST::IncludeRoot > &from ) { return from->p_AST_SourceRoot; }
    inline Ptr< AST::SourceRoot >                  to_base( const Ptr< AST::ObjectSourceRoot > &from ) { return from->p_AST_SourceRoot; }
    inline Ptr< AST::Dimension >                   to_base( const Ptr< Tree::DimensionTrait > &from ) { return from->p_AST_Dimension; }
    inline Ptr< AST::Inheritance >                 to_base( const Ptr< Tree::InheritanceTrait > &from ) { return from->p_AST_Inheritance; }
    inline Ptr< AST::Link >                        to_base( const Ptr< Tree::LinkTrait > &from ) { return from->p_AST_Link; }
    inline Ptr< AST::ReturnType >                  to_base( const Ptr< Tree::ReturnTypeTrait > &from ) { return from->p_AST_ReturnType; }
    inline Ptr< AST::ArgumentList >                to_base( const Ptr< Tree::ArgumentListTrait > &from ) { return from->p_AST_ArgumentList; }
    inline Ptr< AST::Size >                        to_base( const Ptr< Tree::SizeTrait > &from ) { return from->p_AST_Size; }
    inline Ptr< Tree::ContextGroup >               to_base( const Ptr< Tree::ContextGroup > &from ) { return from; }
    inline Ptr< Tree::ContextGroup >               to_base( const Ptr< Tree::Root > &from ) { return from->p_Tree_ContextGroup; }
    inline Ptr< Tree::ContextGroup >               to_base( const Ptr< Tree::IContext > &from ) { return from->p_Tree_ContextGroup; }
    inline Ptr< Tree::ContextGroup >               to_base( const Ptr< Tree::Namespace > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    inline Ptr< Tree::ContextGroup >               to_base( const Ptr< Tree::Abstract > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    inline Ptr< Tree::ContextGroup >               to_base( const Ptr< Tree::Action > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    inline Ptr< Tree::ContextGroup >               to_base( const Ptr< Tree::Event > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    inline Ptr< Tree::ContextGroup >               to_base( const Ptr< Tree::Function > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    inline Ptr< Tree::ContextGroup >               to_base( const Ptr< Tree::Object > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    inline Ptr< Tree::ContextGroup >               to_base( const Ptr< Tree::Link > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    inline Ptr< Tree::ContextGroup >               to_base( const Ptr< Tree::Table > &from ) { return from->p_Tree_IContext->p_Tree_ContextGroup; }
    inline Ptr< Concrete::Dimension >              to_base( const Ptr< Concrete::Dimension > &from ) { return from; }
    inline Ptr< Concrete::Context >                to_base( const Ptr< Concrete::Context > &from ) { return from; }
    inline Ptr< Concrete::Context >                to_base( const Ptr< Concrete::Namespace > &from ) { return from->p_Concrete_Context; }
    inline Ptr< Concrete::Context >                to_base( const Ptr< Concrete::Action > &from ) { return from->p_Concrete_Context; }
    inline Ptr< Concrete::Context >                to_base( const Ptr< Concrete::Event > &from ) { return from->p_Concrete_Context; }
    inline Ptr< Concrete::Context >                to_base( const Ptr< Concrete::Function > &from ) { return from->p_Concrete_Context; }
    inline Ptr< Concrete::Context >                to_base( const Ptr< Concrete::Object > &from ) { return from->p_Concrete_Context; }
    inline Ptr< Concrete::Context >                to_base( const Ptr< Concrete::Link > &from ) { return from->p_Concrete_Context; }
    inline Ptr< Concrete::Context >                to_base( const Ptr< Concrete::Table > &from ) { return from->p_Concrete_Context; }
    inline Ptr< Concrete::Context >                to_base( const Ptr< Concrete::Root > &from ) { return from->p_Concrete_Context; }
    inline Ptr< Operations::InterfaceVariant >     to_base( const Ptr< Operations::InterfaceVariant > &from ) { return from; }
    inline Ptr< Operations::ConcreteVariant >      to_base( const Ptr< Operations::ConcreteVariant > &from ) { return from; }
    inline Ptr< Operations::Element >              to_base( const Ptr< Operations::Element > &from ) { return from; }
    inline Ptr< Operations::ElementVector >        to_base( const Ptr< Operations::ElementVector > &from ) { return from; }
    inline Ptr< Operations::Context >              to_base( const Ptr< Operations::Context > &from ) { return from; }
    inline Ptr< Operations::TypePath >             to_base( const Ptr< Operations::TypePath > &from ) { return from; }
    inline Ptr< Operations::NameRoot >             to_base( const Ptr< Operations::NameRoot > &from ) { return from; }
    inline Ptr< Operations::NameRoot >             to_base( const Ptr< Operations::Name > &from ) { return from->p_Operations_NameRoot; }
    inline Ptr< Operations::NameResolution >       to_base( const Ptr< Operations::NameResolution > &from ) { return from; }
    inline Ptr< Operations::Invocation >           to_base( const Ptr< Operations::Invocation > &from ) { return from; }
    inline Ptr< Operations::Invocations >          to_base( const Ptr< Operations::Invocations > &from ) { return from; }
    inline Ptr< DPGraph::Glob >                    to_base( const Ptr< DPGraph::Glob > &from ) { return from; }
    inline Ptr< DPGraph::ObjectDependencies >      to_base( const Ptr< DPGraph::ObjectDependencies > &from ) { return from; }
    inline Ptr< DPGraph::Analysis >                to_base( const Ptr< DPGraph::Analysis > &from ) { return from; }
    inline Ptr< SymbolTable::Symbol >              to_base( const Ptr< SymbolTable::Symbol > &from ) { return from; }
    inline Ptr< SymbolTable::SymbolSet >           to_base( const Ptr< SymbolTable::SymbolSet > &from ) { return from; }
    inline Ptr< SymbolTable::SymbolTable >         to_base( const Ptr< SymbolTable::SymbolTable > &from ) { return from; }
    inline Ptr< Model::ObjectGraph >               to_base( const Ptr< Model::ObjectGraph > &from ) { return from; }
    inline Ptr< Model::Graph >                     to_base( const Ptr< Model::Graph > &from ) { return from; }
    inline Ptr< Derivations::ObjectMapping >       to_base( const Ptr< Derivations::ObjectMapping > &from ) { return from; }
    inline Ptr< Derivations::Mapping >             to_base( const Ptr< Derivations::Mapping > &from ) { return from; }

    template < typename TVariant > inline TVariant to_upper( TVariant &from )
    {
        TVariant up_most = from;
        for( ;; )
        {
            TVariant up_most_next = std::visit( []( auto &&arg ) -> TVariant { return arg->m_inheritance; }, up_most );
            if( up_most_next == up_most )
                break;
            up_most = up_most_next;
        }
        return up_most;
    }

    class Factory
    {
      public:
        static mega::io::Object *create( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo );
    };

} // namespace data
#endif // DATABASE_DATA_GUARD_4_APRIL_2022
