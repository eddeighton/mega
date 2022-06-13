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
    struct Components_Component;
}
namespace AST
{
    struct Parser_Identifier;
    struct Parser_ScopedIdentifier;
    struct Parser_ArgumentList;
    struct Parser_ReturnType;
    struct Parser_Inheritance;
    struct Parser_Size;
    struct Parser_Link;
    struct Parser_Dimension;
    struct Parser_Include;
    struct Parser_SystemInclude;
    struct Parser_MegaInclude;
    struct Parser_MegaIncludeInline;
    struct Parser_MegaIncludeNested;
    struct Parser_CPPInclude;
    struct Parser_Dependency;
    struct Parser_ContextDef;
    struct Parser_NamespaceDef;
    struct Parser_AbstractDef;
    struct Parser_ActionDef;
    struct Parser_EventDef;
    struct Parser_FunctionDef;
    struct Parser_ObjectDef;
    struct Parser_LinkDef;
    struct Parser_TableDef;
    struct Parser_SourceRoot;
    struct Parser_IncludeRoot;
    struct Parser_ObjectSourceRoot;
}
namespace Body
{
    struct Parser_ContextDef;
}
namespace Tree
{
    struct Interface_DimensionTrait;
    struct Interface_InheritanceTrait;
    struct Interface_LinkTrait;
    struct Interface_ReturnTypeTrait;
    struct Interface_ArgumentListTrait;
    struct Interface_SizeTrait;
    struct Interface_ContextGroup;
    struct Interface_Root;
    struct Interface_IContext;
    struct Interface_Namespace;
    struct Interface_Abstract;
    struct Interface_Action;
    struct Interface_Event;
    struct Interface_Function;
    struct Interface_Object;
    struct Interface_Link;
    struct Interface_Table;
}
namespace DPGraph
{
    struct Dependencies_Glob;
    struct Dependencies_ObjectDependencies;
    struct Dependencies_Analysis;
}
namespace SymbolTable
{
    struct Symbols_Symbol;
    struct Symbols_SymbolSet;
    struct Symbols_SymbolTable;
}
namespace PerSourceSymbols
{
    struct Interface_DimensionTrait;
    struct Interface_IContext;
}
namespace Clang
{
    struct Interface_DimensionTrait;
    struct Interface_InheritanceTrait;
    struct Interface_ReturnTypeTrait;
    struct Interface_ArgumentListTrait;
    struct Interface_SizeTrait;
}
namespace Concrete
{
    struct Interface_DimensionTrait;
    struct Interface_IContext;
    struct Concrete_Dimension;
    struct Concrete_ContextGroup;
    struct Concrete_Context;
    struct Concrete_Namespace;
    struct Concrete_Action;
    struct Concrete_Event;
    struct Concrete_Function;
    struct Concrete_Object;
    struct Concrete_Link;
    struct Concrete_Table;
    struct Concrete_Root;
}
namespace Model
{
    struct HyperGraph_ObjectGraph;
    struct HyperGraph_Graph;
}
namespace Derivations
{
    struct Derivation_ObjectMapping;
    struct Derivation_Mapping;
}
namespace Operations
{
    struct Invocations_Variables_Variable;
    struct Invocations_Variables_Instance;
    struct Invocations_Variables_Reference;
    struct Invocations_Variables_Dimension;
    struct Invocations_Variables_Context;
    struct Invocations_Instructions_Instruction;
    struct Invocations_Instructions_InstructionGroup;
    struct Invocations_Instructions_Root;
    struct Invocations_Instructions_ParentDerivation;
    struct Invocations_Instructions_ChildDerivation;
    struct Invocations_Instructions_EnumDerivation;
    struct Invocations_Instructions_Enumeration;
    struct Invocations_Instructions_DimensionReferenceRead;
    struct Invocations_Instructions_MonoReference;
    struct Invocations_Instructions_PolyReference;
    struct Invocations_Instructions_PolyCase;
    struct Invocations_Instructions_Failure;
    struct Invocations_Instructions_Elimination;
    struct Invocations_Instructions_Prune;
    struct Invocations_Operations_Operation;
    struct Invocations_Operations_BasicOperation;
    struct Invocations_Operations_DimensionOperation;
    struct Invocations_Operations_Call;
    struct Invocations_Operations_Start;
    struct Invocations_Operations_Stop;
    struct Invocations_Operations_Pause;
    struct Invocations_Operations_Resume;
    struct Invocations_Operations_Done;
    struct Invocations_Operations_WaitAction;
    struct Invocations_Operations_WaitDimension;
    struct Invocations_Operations_GetAction;
    struct Invocations_Operations_GetDimension;
    struct Invocations_Operations_Read;
    struct Invocations_Operations_Write;
    struct Invocations_Operations_WriteLink;
    struct Invocations_Operations_Range;
    struct Operations_InterfaceVariant;
    struct Operations_ConcreteVariant;
    struct Operations_Element;
    struct Operations_ElementVector;
    struct Operations_Context;
    struct Operations_TypePath;
    struct Operations_NameRoot;
    struct Operations_Name;
    struct Operations_NameResolution;
    struct Operations_Invocation;
    struct Operations_Invocations;
}

// definitions
namespace Components
{
    struct Components_Component : public mega::io::Object
    {
        Components_Component( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Components_Component( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& name, const boost::filesystem::path& directory, const std::vector< std::string >& cpp_flags, const std::vector< std::string >& cpp_defines, const std::vector< boost::filesystem::path >& includeDirectories, const std::vector< boost::filesystem::path >& sourceFiles);
        enum 
        {
            Object_Part_Type_ID = 0
        };
        std::string name;
        boost::filesystem::path directory;
        std::vector< std::string > cpp_flags;
        std::vector< std::string > cpp_defines;
        std::vector< boost::filesystem::path > includeDirectories;
        std::vector< boost::filesystem::path > sourceFiles;
        std::variant< data::Ptr< data::Components::Components_Component > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
}
namespace AST
{
    struct Parser_Identifier : public mega::io::Object
    {
        Parser_Identifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_Identifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str);
        enum 
        {
            Object_Part_Type_ID = 1
        };
        std::string str;
        std::variant< data::Ptr< data::AST::Parser_Identifier > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_ScopedIdentifier : public mega::io::Object
    {
        Parser_ScopedIdentifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_ScopedIdentifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_Identifier > >& ids, const std::string& source_file, const std::size_t& line_number);
        enum 
        {
            Object_Part_Type_ID = 2
        };
        std::vector< data::Ptr< data::AST::Parser_Identifier > > ids;
        std::string source_file;
        std::size_t line_number;
        std::variant< data::Ptr< data::AST::Parser_ScopedIdentifier > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_ArgumentList : public mega::io::Object
    {
        Parser_ArgumentList( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_ArgumentList( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str);
        enum 
        {
            Object_Part_Type_ID = 3
        };
        std::string str;
        std::variant< data::Ptr< data::AST::Parser_ArgumentList >, data::Ptr< data::Tree::Interface_ArgumentListTrait > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_ReturnType : public mega::io::Object
    {
        Parser_ReturnType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_ReturnType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str);
        enum 
        {
            Object_Part_Type_ID = 4
        };
        std::string str;
        std::variant< data::Ptr< data::AST::Parser_ReturnType >, data::Ptr< data::Tree::Interface_ReturnTypeTrait > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_Inheritance : public mega::io::Object
    {
        Parser_Inheritance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_Inheritance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< std::string >& strings);
        enum 
        {
            Object_Part_Type_ID = 5
        };
        std::vector< std::string > strings;
        std::variant< data::Ptr< data::AST::Parser_Inheritance >, data::Ptr< data::Tree::Interface_InheritanceTrait > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_Size : public mega::io::Object
    {
        Parser_Size( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_Size( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str);
        enum 
        {
            Object_Part_Type_ID = 6
        };
        std::string str;
        std::variant< data::Ptr< data::AST::Parser_Size >, data::Ptr< data::Tree::Interface_SizeTrait > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_Link : public mega::io::Object
    {
        Parser_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::CardinalityRange& linker, const mega::CardinalityRange& linkee, const bool& derive_from, const bool& derive_to, const mega::Ownership& ownership);
        enum 
        {
            Object_Part_Type_ID = 7
        };
        mega::CardinalityRange linker;
        mega::CardinalityRange linkee;
        bool derive_from;
        bool derive_to;
        mega::Ownership ownership;
        std::variant< data::Ptr< data::AST::Parser_Link >, data::Ptr< data::Tree::Interface_LinkTrait > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_Dimension : public mega::io::Object
    {
        Parser_Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const bool& isConst, const data::Ptr< data::AST::Parser_Identifier >& id, const std::string& type);
        enum 
        {
            Object_Part_Type_ID = 8
        };
        bool isConst;
        data::Ptr< data::AST::Parser_Identifier > id;
        std::string type;
        std::variant< data::Ptr< data::AST::Parser_Dimension >, data::Ptr< data::Tree::Interface_DimensionTrait > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_Include : public mega::io::Object
    {
        Parser_Include( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 9
        };
        std::variant< data::Ptr< data::AST::Parser_Include >, data::Ptr< data::AST::Parser_SystemInclude >, data::Ptr< data::AST::Parser_MegaInclude >, data::Ptr< data::AST::Parser_MegaIncludeInline >, data::Ptr< data::AST::Parser_MegaIncludeNested >, data::Ptr< data::AST::Parser_CPPInclude > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_SystemInclude : public mega::io::Object
    {
        Parser_SystemInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_SystemInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str);
        enum 
        {
            Object_Part_Type_ID = 10
        };
        std::string str;
        Ptr< AST::Parser_Include > p_AST_Parser_Include;
        std::variant< data::Ptr< data::AST::Parser_Include >, data::Ptr< data::AST::Parser_SystemInclude >, data::Ptr< data::AST::Parser_MegaInclude >, data::Ptr< data::AST::Parser_MegaIncludeInline >, data::Ptr< data::AST::Parser_MegaIncludeNested >, data::Ptr< data::AST::Parser_CPPInclude > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_MegaInclude : public mega::io::Object
    {
        Parser_MegaInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_MegaInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& megaSourceFilePath);
        enum 
        {
            Object_Part_Type_ID = 11
        };
        boost::filesystem::path megaSourceFilePath;
        std::optional< data::Ptr< data::AST::Parser_IncludeRoot > > root;
        Ptr< AST::Parser_Include > p_AST_Parser_Include;
        std::variant< data::Ptr< data::AST::Parser_Include >, data::Ptr< data::AST::Parser_SystemInclude >, data::Ptr< data::AST::Parser_MegaInclude >, data::Ptr< data::AST::Parser_MegaIncludeInline >, data::Ptr< data::AST::Parser_MegaIncludeNested >, data::Ptr< data::AST::Parser_CPPInclude > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_MegaIncludeInline : public mega::io::Object
    {
        Parser_MegaIncludeInline( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 12
        };
        Ptr< AST::Parser_MegaInclude > p_AST_Parser_MegaInclude;
        std::variant< data::Ptr< data::AST::Parser_Include >, data::Ptr< data::AST::Parser_SystemInclude >, data::Ptr< data::AST::Parser_MegaInclude >, data::Ptr< data::AST::Parser_MegaIncludeInline >, data::Ptr< data::AST::Parser_MegaIncludeNested >, data::Ptr< data::AST::Parser_CPPInclude > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_MegaIncludeNested : public mega::io::Object
    {
        Parser_MegaIncludeNested( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_MegaIncludeNested( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_ScopedIdentifier >& id);
        enum 
        {
            Object_Part_Type_ID = 13
        };
        data::Ptr< data::AST::Parser_ScopedIdentifier > id;
        Ptr< AST::Parser_MegaInclude > p_AST_Parser_MegaInclude;
        std::variant< data::Ptr< data::AST::Parser_Include >, data::Ptr< data::AST::Parser_SystemInclude >, data::Ptr< data::AST::Parser_MegaInclude >, data::Ptr< data::AST::Parser_MegaIncludeInline >, data::Ptr< data::AST::Parser_MegaIncludeNested >, data::Ptr< data::AST::Parser_CPPInclude > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_CPPInclude : public mega::io::Object
    {
        Parser_CPPInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_CPPInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& cppSourceFilePath);
        enum 
        {
            Object_Part_Type_ID = 14
        };
        boost::filesystem::path cppSourceFilePath;
        Ptr< AST::Parser_Include > p_AST_Parser_Include;
        std::variant< data::Ptr< data::AST::Parser_Include >, data::Ptr< data::AST::Parser_SystemInclude >, data::Ptr< data::AST::Parser_MegaInclude >, data::Ptr< data::AST::Parser_MegaIncludeInline >, data::Ptr< data::AST::Parser_MegaIncludeNested >, data::Ptr< data::AST::Parser_CPPInclude > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_Dependency : public mega::io::Object
    {
        Parser_Dependency( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_Dependency( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str);
        enum 
        {
            Object_Part_Type_ID = 15
        };
        std::string str;
        std::variant< data::Ptr< data::AST::Parser_Dependency > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_ContextDef : public mega::io::Object
    {
        Parser_ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_ScopedIdentifier >& id, const std::vector< data::Ptr< data::AST::Parser_ContextDef > >& children, const std::vector< data::Ptr< data::AST::Parser_Dimension > >& dimensions, const std::vector< data::Ptr< data::AST::Parser_Include > >& includes, const std::vector< data::Ptr< data::AST::Parser_Dependency > >& dependencies);
        enum 
        {
            Object_Part_Type_ID = 16
        };
        data::Ptr< data::AST::Parser_ScopedIdentifier > id;
        std::vector< data::Ptr< data::AST::Parser_ContextDef > > children;
        std::vector< data::Ptr< data::AST::Parser_Dimension > > dimensions;
        std::vector< data::Ptr< data::AST::Parser_Include > > includes;
        std::vector< data::Ptr< data::AST::Parser_Dependency > > dependencies;
        Ptr< Body::Parser_ContextDef > p_Body_Parser_ContextDef;
        std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_NamespaceDef : public mega::io::Object
    {
        Parser_NamespaceDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 18
        };
        Ptr< AST::Parser_ContextDef > p_AST_Parser_ContextDef;
        std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_AbstractDef : public mega::io::Object
    {
        Parser_AbstractDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_AbstractDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Inheritance >& inheritance);
        enum 
        {
            Object_Part_Type_ID = 19
        };
        data::Ptr< data::AST::Parser_Inheritance > inheritance;
        Ptr< AST::Parser_ContextDef > p_AST_Parser_ContextDef;
        std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_ActionDef : public mega::io::Object
    {
        Parser_ActionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_ActionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Size >& size, const data::Ptr< data::AST::Parser_Inheritance >& inheritance);
        enum 
        {
            Object_Part_Type_ID = 20
        };
        data::Ptr< data::AST::Parser_Size > size;
        data::Ptr< data::AST::Parser_Inheritance > inheritance;
        Ptr< AST::Parser_ContextDef > p_AST_Parser_ContextDef;
        std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_EventDef : public mega::io::Object
    {
        Parser_EventDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_EventDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Size >& size, const data::Ptr< data::AST::Parser_Inheritance >& inheritance);
        enum 
        {
            Object_Part_Type_ID = 21
        };
        data::Ptr< data::AST::Parser_Size > size;
        data::Ptr< data::AST::Parser_Inheritance > inheritance;
        Ptr< AST::Parser_ContextDef > p_AST_Parser_ContextDef;
        std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_FunctionDef : public mega::io::Object
    {
        Parser_FunctionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_FunctionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_ArgumentList >& argumentList, const data::Ptr< data::AST::Parser_ReturnType >& returnType);
        enum 
        {
            Object_Part_Type_ID = 22
        };
        data::Ptr< data::AST::Parser_ArgumentList > argumentList;
        data::Ptr< data::AST::Parser_ReturnType > returnType;
        Ptr< AST::Parser_ContextDef > p_AST_Parser_ContextDef;
        std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_ObjectDef : public mega::io::Object
    {
        Parser_ObjectDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_ObjectDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Inheritance >& inheritance);
        enum 
        {
            Object_Part_Type_ID = 23
        };
        data::Ptr< data::AST::Parser_Inheritance > inheritance;
        Ptr< AST::Parser_ContextDef > p_AST_Parser_ContextDef;
        std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_LinkDef : public mega::io::Object
    {
        Parser_LinkDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_LinkDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Link >& link, const data::Ptr< data::AST::Parser_Inheritance >& target);
        enum 
        {
            Object_Part_Type_ID = 24
        };
        data::Ptr< data::AST::Parser_Link > link;
        data::Ptr< data::AST::Parser_Inheritance > target;
        Ptr< AST::Parser_ContextDef > p_AST_Parser_ContextDef;
        std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_TableDef : public mega::io::Object
    {
        Parser_TableDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_TableDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Inheritance >& inheritance);
        enum 
        {
            Object_Part_Type_ID = 25
        };
        data::Ptr< data::AST::Parser_Inheritance > inheritance;
        Ptr< AST::Parser_ContextDef > p_AST_Parser_ContextDef;
        std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_SourceRoot : public mega::io::Object
    {
        Parser_SourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_SourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& sourceFile, const data::Ptr< data::Components::Components_Component >& component, const data::Ptr< data::AST::Parser_ContextDef >& ast);
        enum 
        {
            Object_Part_Type_ID = 26
        };
        boost::filesystem::path sourceFile;
        data::Ptr< data::Components::Components_Component > component;
        data::Ptr< data::AST::Parser_ContextDef > ast;
        std::variant< data::Ptr< data::AST::Parser_SourceRoot >, data::Ptr< data::AST::Parser_IncludeRoot >, data::Ptr< data::AST::Parser_ObjectSourceRoot > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_IncludeRoot : public mega::io::Object
    {
        Parser_IncludeRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 27
        };
        Ptr< AST::Parser_SourceRoot > p_AST_Parser_SourceRoot;
        std::variant< data::Ptr< data::AST::Parser_SourceRoot >, data::Ptr< data::AST::Parser_IncludeRoot >, data::Ptr< data::AST::Parser_ObjectSourceRoot > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Parser_ObjectSourceRoot : public mega::io::Object
    {
        Parser_ObjectSourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 28
        };
        Ptr< AST::Parser_SourceRoot > p_AST_Parser_SourceRoot;
        std::variant< data::Ptr< data::AST::Parser_SourceRoot >, data::Ptr< data::AST::Parser_IncludeRoot >, data::Ptr< data::AST::Parser_ObjectSourceRoot > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
}
namespace Body
{
    struct Parser_ContextDef : public mega::io::Object
    {
        Parser_ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Parser_ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& body);
        enum 
        {
            Object_Part_Type_ID = 17
        };
        std::string body;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
}
namespace Tree
{
    struct Interface_DimensionTrait : public mega::io::Object
    {
        Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_IContext >& parent);
        enum 
        {
            Object_Part_Type_ID = 29
        };
        data::Ptr< data::Tree::Interface_IContext > parent;
        Ptr< AST::Parser_Dimension > p_AST_Parser_Dimension;
        Ptr< PerSourceSymbols::Interface_DimensionTrait > p_PerSourceSymbols_Interface_DimensionTrait;
        Ptr< Concrete::Interface_DimensionTrait > p_Concrete_Interface_DimensionTrait;
        Ptr< Clang::Interface_DimensionTrait > p_Clang_Interface_DimensionTrait;
        std::variant< data::Ptr< data::AST::Parser_Dimension >, data::Ptr< data::Tree::Interface_DimensionTrait > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_InheritanceTrait : public mega::io::Object
    {
        Interface_InheritanceTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 33
        };
        Ptr< AST::Parser_Inheritance > p_AST_Parser_Inheritance;
        Ptr< Clang::Interface_InheritanceTrait > p_Clang_Interface_InheritanceTrait;
        std::variant< data::Ptr< data::AST::Parser_Inheritance >, data::Ptr< data::Tree::Interface_InheritanceTrait > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_LinkTrait : public mega::io::Object
    {
        Interface_LinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 35
        };
        Ptr< AST::Parser_Link > p_AST_Parser_Link;
        std::variant< data::Ptr< data::AST::Parser_Link >, data::Ptr< data::Tree::Interface_LinkTrait > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_ReturnTypeTrait : public mega::io::Object
    {
        Interface_ReturnTypeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 36
        };
        Ptr< AST::Parser_ReturnType > p_AST_Parser_ReturnType;
        Ptr< Clang::Interface_ReturnTypeTrait > p_Clang_Interface_ReturnTypeTrait;
        std::variant< data::Ptr< data::AST::Parser_ReturnType >, data::Ptr< data::Tree::Interface_ReturnTypeTrait > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_ArgumentListTrait : public mega::io::Object
    {
        Interface_ArgumentListTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 38
        };
        Ptr< AST::Parser_ArgumentList > p_AST_Parser_ArgumentList;
        Ptr< Clang::Interface_ArgumentListTrait > p_Clang_Interface_ArgumentListTrait;
        std::variant< data::Ptr< data::AST::Parser_ArgumentList >, data::Ptr< data::Tree::Interface_ArgumentListTrait > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_SizeTrait : public mega::io::Object
    {
        Interface_SizeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 40
        };
        Ptr< AST::Parser_Size > p_AST_Parser_Size;
        Ptr< Clang::Interface_SizeTrait > p_Clang_Interface_SizeTrait;
        std::variant< data::Ptr< data::AST::Parser_Size >, data::Ptr< data::Tree::Interface_SizeTrait > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_ContextGroup : public mega::io::Object
    {
        Interface_ContextGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_ContextGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Tree::Interface_IContext > >& children);
        enum 
        {
            Object_Part_Type_ID = 42
        };
        std::vector< data::Ptr< data::Tree::Interface_IContext > > children;
        std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_Root : public mega::io::Object
    {
        Interface_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_ObjectSourceRoot >& root);
        enum 
        {
            Object_Part_Type_ID = 43
        };
        data::Ptr< data::AST::Parser_ObjectSourceRoot > root;
        Ptr< Tree::Interface_ContextGroup > p_Tree_Interface_ContextGroup;
        std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_IContext : public mega::io::Object
    {
        Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& identifier, const data::Ptr< data::Tree::Interface_ContextGroup >& parent);
        enum 
        {
            Object_Part_Type_ID = 44
        };
        std::string identifier;
        data::Ptr< data::Tree::Interface_ContextGroup > parent;
        Ptr< Tree::Interface_ContextGroup > p_Tree_Interface_ContextGroup;
        Ptr< PerSourceSymbols::Interface_IContext > p_PerSourceSymbols_Interface_IContext;
        Ptr< Concrete::Interface_IContext > p_Concrete_Interface_IContext;
        std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_Namespace : public mega::io::Object
    {
        Interface_Namespace( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_Namespace( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const bool& is_global, const std::vector< data::Ptr< data::AST::Parser_ContextDef > >& namespace_defs);
        enum 
        {
            Object_Part_Type_ID = 47
        };
        bool is_global;
        std::vector< data::Ptr< data::AST::Parser_ContextDef > > namespace_defs;
        std::optional< std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > > > dimension_traits;
        Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext;
        std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_Abstract : public mega::io::Object
    {
        Interface_Abstract( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_Abstract( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_AbstractDef > >& abstract_defs);
        enum 
        {
            Object_Part_Type_ID = 48
        };
        std::vector< data::Ptr< data::AST::Parser_AbstractDef > > abstract_defs;
        std::optional< std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > > > dimension_traits;
        std::optional< std::optional< data::Ptr< data::Tree::Interface_InheritanceTrait > > > inheritance_trait;
        Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext;
        std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_Action : public mega::io::Object
    {
        Interface_Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_ActionDef > >& action_defs);
        enum 
        {
            Object_Part_Type_ID = 49
        };
        std::vector< data::Ptr< data::AST::Parser_ActionDef > > action_defs;
        std::optional< std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > > > dimension_traits;
        std::optional< std::optional< data::Ptr< data::Tree::Interface_InheritanceTrait > > > inheritance_trait;
        std::optional< std::optional< data::Ptr< data::Tree::Interface_SizeTrait > > > size_trait;
        Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext;
        std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_Event : public mega::io::Object
    {
        Interface_Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_EventDef > >& event_defs);
        enum 
        {
            Object_Part_Type_ID = 50
        };
        std::vector< data::Ptr< data::AST::Parser_EventDef > > event_defs;
        std::optional< std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > > > dimension_traits;
        std::optional< std::optional< data::Ptr< data::Tree::Interface_InheritanceTrait > > > inheritance_trait;
        std::optional< std::optional< data::Ptr< data::Tree::Interface_SizeTrait > > > size_trait;
        Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext;
        std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_Function : public mega::io::Object
    {
        Interface_Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_FunctionDef > >& function_defs);
        enum 
        {
            Object_Part_Type_ID = 51
        };
        std::vector< data::Ptr< data::AST::Parser_FunctionDef > > function_defs;
        std::optional< data::Ptr< data::Tree::Interface_ReturnTypeTrait > > return_type_trait;
        std::optional< data::Ptr< data::Tree::Interface_ArgumentListTrait > > arguments_trait;
        Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext;
        std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_Object : public mega::io::Object
    {
        Interface_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_ObjectDef > >& object_defs);
        enum 
        {
            Object_Part_Type_ID = 52
        };
        std::vector< data::Ptr< data::AST::Parser_ObjectDef > > object_defs;
        std::optional< std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > > > dimension_traits;
        std::optional< std::optional< data::Ptr< data::Tree::Interface_InheritanceTrait > > > inheritance_trait;
        Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext;
        std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_Link : public mega::io::Object
    {
        Interface_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_LinkDef > >& link_defs);
        enum 
        {
            Object_Part_Type_ID = 53
        };
        std::vector< data::Ptr< data::AST::Parser_LinkDef > > link_defs;
        std::optional< data::Ptr< data::Tree::Interface_LinkTrait > > link_trait;
        std::optional< data::Ptr< data::Tree::Interface_InheritanceTrait > > link_target;
        Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext;
        std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_Table : public mega::io::Object
    {
        Interface_Table( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_Table( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_TableDef > >& table_defs);
        enum 
        {
            Object_Part_Type_ID = 54
        };
        std::vector< data::Ptr< data::AST::Parser_TableDef > > table_defs;
        Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext;
        std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
}
namespace DPGraph
{
    struct Dependencies_Glob : public mega::io::Object
    {
        Dependencies_Glob( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Dependencies_Glob( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& location, const std::string& glob);
        enum 
        {
            Object_Part_Type_ID = 113
        };
        boost::filesystem::path location;
        std::string glob;
        std::variant< data::Ptr< data::DPGraph::Dependencies_Glob > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Dependencies_ObjectDependencies : public mega::io::Object
    {
        Dependencies_ObjectDependencies( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Dependencies_ObjectDependencies( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::io::megaFilePath& source_file, const std::size_t& hash_code, const std::vector< data::Ptr< data::DPGraph::Dependencies_Glob > >& globs, const std::vector< boost::filesystem::path >& resolution);
        enum 
        {
            Object_Part_Type_ID = 114
        };
        mega::io::megaFilePath source_file;
        std::size_t hash_code;
        std::vector< data::Ptr< data::DPGraph::Dependencies_Glob > > globs;
        std::vector< boost::filesystem::path > resolution;
        std::variant< data::Ptr< data::DPGraph::Dependencies_ObjectDependencies > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Dependencies_Analysis : public mega::io::Object
    {
        Dependencies_Analysis( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Dependencies_Analysis( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::DPGraph::Dependencies_ObjectDependencies > >& objects);
        enum 
        {
            Object_Part_Type_ID = 115
        };
        std::vector< data::Ptr< data::DPGraph::Dependencies_ObjectDependencies > > objects;
        std::variant< data::Ptr< data::DPGraph::Dependencies_Analysis > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
}
namespace SymbolTable
{
    struct Symbols_Symbol : public mega::io::Object
    {
        Symbols_Symbol( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Symbols_Symbol( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& symbol, const std::int32_t& id, const std::vector< data::Ptr< data::Tree::Interface_IContext > >& contexts, const std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > >& dimensions);
        enum 
        {
            Object_Part_Type_ID = 116
        };
        std::string symbol;
        std::int32_t id;
        std::vector< data::Ptr< data::Tree::Interface_IContext > > contexts;
        std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > > dimensions;
        std::variant< data::Ptr< data::SymbolTable::Symbols_Symbol > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Symbols_SymbolSet : public mega::io::Object
    {
        Symbols_SymbolSet( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Symbols_SymbolSet( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::map< std::string, data::Ptr< data::SymbolTable::Symbols_Symbol > >& symbols, const mega::io::megaFilePath& source_file, const std::size_t& hash_code, const std::map< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::SymbolTable::Symbols_Symbol > >& context_symbols, const std::map< data::Ptr< data::Tree::Interface_DimensionTrait >, data::Ptr< data::SymbolTable::Symbols_Symbol > >& dimension_symbols, const std::map< data::Ptr< data::Tree::Interface_IContext >, int32_t >& context_type_ids, const std::map< data::Ptr< data::Tree::Interface_DimensionTrait >, int32_t >& dimension_type_ids);
        enum 
        {
            Object_Part_Type_ID = 117
        };
        std::map< std::string, data::Ptr< data::SymbolTable::Symbols_Symbol > > symbols;
        mega::io::megaFilePath source_file;
        std::size_t hash_code;
        std::map< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::SymbolTable::Symbols_Symbol > > context_symbols;
        std::map< data::Ptr< data::Tree::Interface_DimensionTrait >, data::Ptr< data::SymbolTable::Symbols_Symbol > > dimension_symbols;
        std::map< data::Ptr< data::Tree::Interface_IContext >, int32_t > context_type_ids;
        std::map< data::Ptr< data::Tree::Interface_DimensionTrait >, int32_t > dimension_type_ids;
        std::variant< data::Ptr< data::SymbolTable::Symbols_SymbolSet > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Symbols_SymbolTable : public mega::io::Object
    {
        Symbols_SymbolTable( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Symbols_SymbolTable( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::map< mega::io::megaFilePath, data::Ptr< data::SymbolTable::Symbols_SymbolSet > >& symbol_sets, const std::map< std::string, data::Ptr< data::SymbolTable::Symbols_Symbol > >& symbols, const std::map< int32_t, data::Ptr< data::Tree::Interface_IContext > >& context_type_ids, const std::map< int32_t, data::Ptr< data::Tree::Interface_DimensionTrait > >& dimension_type_ids, const std::map< int32_t, data::Ptr< data::SymbolTable::Symbols_Symbol > >& symbol_id_map);
        enum 
        {
            Object_Part_Type_ID = 118
        };
        std::map< mega::io::megaFilePath, data::Ptr< data::SymbolTable::Symbols_SymbolSet > > symbol_sets;
        std::map< std::string, data::Ptr< data::SymbolTable::Symbols_Symbol > > symbols;
        std::map< int32_t, data::Ptr< data::Tree::Interface_IContext > > context_type_ids;
        std::map< int32_t, data::Ptr< data::Tree::Interface_DimensionTrait > > dimension_type_ids;
        std::map< int32_t, data::Ptr< data::SymbolTable::Symbols_Symbol > > symbol_id_map;
        std::variant< data::Ptr< data::SymbolTable::Symbols_SymbolTable > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
}
namespace PerSourceSymbols
{
    struct Interface_DimensionTrait : public mega::io::Object
    {
        Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_DimensionTrait > p_Tree_Interface_DimensionTrait, const std::int32_t& symbol);
        enum 
        {
            Object_Part_Type_ID = 30
        };
        std::int32_t symbol;
        std::optional< std::int32_t > type_id;
        Ptr< Tree::Interface_DimensionTrait > p_Tree_Interface_DimensionTrait;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_IContext : public mega::io::Object
    {
        Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext, const std::int32_t& symbol);
        enum 
        {
            Object_Part_Type_ID = 45
        };
        std::int32_t symbol;
        std::optional< std::int32_t > type_id;
        Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
}
namespace Clang
{
    struct Interface_DimensionTrait : public mega::io::Object
    {
        Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_DimensionTrait > p_Tree_Interface_DimensionTrait, const std::string& canonical_type, const std::size_t& size, const bool& simple, const std::vector< data::Ptr< data::SymbolTable::Symbols_Symbol > >& symbols);
        enum 
        {
            Object_Part_Type_ID = 32
        };
        std::string canonical_type;
        std::size_t size;
        bool simple;
        std::vector< data::Ptr< data::SymbolTable::Symbols_Symbol > > symbols;
        Ptr< Tree::Interface_DimensionTrait > p_Tree_Interface_DimensionTrait;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_InheritanceTrait : public mega::io::Object
    {
        Interface_InheritanceTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_InheritanceTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_InheritanceTrait > p_Tree_Interface_InheritanceTrait, const std::vector< data::Ptr< data::Tree::Interface_IContext > >& contexts);
        enum 
        {
            Object_Part_Type_ID = 34
        };
        std::vector< data::Ptr< data::Tree::Interface_IContext > > contexts;
        Ptr< Tree::Interface_InheritanceTrait > p_Tree_Interface_InheritanceTrait;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_ReturnTypeTrait : public mega::io::Object
    {
        Interface_ReturnTypeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_ReturnTypeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_ReturnTypeTrait > p_Tree_Interface_ReturnTypeTrait, const std::string& canonical_type);
        enum 
        {
            Object_Part_Type_ID = 37
        };
        std::string canonical_type;
        Ptr< Tree::Interface_ReturnTypeTrait > p_Tree_Interface_ReturnTypeTrait;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_ArgumentListTrait : public mega::io::Object
    {
        Interface_ArgumentListTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_ArgumentListTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_ArgumentListTrait > p_Tree_Interface_ArgumentListTrait, const std::vector< std::string >& canonical_types);
        enum 
        {
            Object_Part_Type_ID = 39
        };
        std::vector< std::string > canonical_types;
        Ptr< Tree::Interface_ArgumentListTrait > p_Tree_Interface_ArgumentListTrait;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_SizeTrait : public mega::io::Object
    {
        Interface_SizeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_SizeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_SizeTrait > p_Tree_Interface_SizeTrait, const std::size_t& size);
        enum 
        {
            Object_Part_Type_ID = 41
        };
        std::size_t size;
        Ptr< Tree::Interface_SizeTrait > p_Tree_Interface_SizeTrait;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
}
namespace Concrete
{
    struct Interface_DimensionTrait : public mega::io::Object
    {
        Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_DimensionTrait > p_Tree_Interface_DimensionTrait, const std::optional< data::Ptr< data::Concrete::Concrete_Dimension > >& concrete);
        enum 
        {
            Object_Part_Type_ID = 31
        };
        std::optional< data::Ptr< data::Concrete::Concrete_Dimension > > concrete;
        Ptr< Tree::Interface_DimensionTrait > p_Tree_Interface_DimensionTrait;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Interface_IContext : public mega::io::Object
    {
        Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext, const std::optional< data::Ptr< data::Concrete::Concrete_Context > >& concrete);
        enum 
        {
            Object_Part_Type_ID = 46
        };
        std::optional< data::Ptr< data::Concrete::Concrete_Context > > concrete;
        Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Concrete_Dimension : public mega::io::Object
    {
        Concrete_Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Concrete_Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Context >& parent, const data::Ptr< data::Tree::Interface_DimensionTrait >& interface_dimension);
        enum 
        {
            Object_Part_Type_ID = 91
        };
        data::Ptr< data::Concrete::Concrete_Context > parent;
        data::Ptr< data::Tree::Interface_DimensionTrait > interface_dimension;
        std::variant< data::Ptr< data::Concrete::Concrete_Dimension > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Concrete_ContextGroup : public mega::io::Object
    {
        Concrete_ContextGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Concrete_ContextGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Concrete::Concrete_Context > >& children);
        enum 
        {
            Object_Part_Type_ID = 92
        };
        std::vector< data::Ptr< data::Concrete::Concrete_Context > > children;
        std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Concrete_Context : public mega::io::Object
    {
        Concrete_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Concrete_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_ContextGroup >& parent, const data::Ptr< data::Tree::Interface_IContext >& interface, const std::vector< data::Ptr< data::Tree::Interface_IContext > >& inheritance);
        enum 
        {
            Object_Part_Type_ID = 93
        };
        data::Ptr< data::Concrete::Concrete_ContextGroup > parent;
        data::Ptr< data::Tree::Interface_IContext > interface;
        std::vector< data::Ptr< data::Tree::Interface_IContext > > inheritance;
        Ptr< Concrete::Concrete_ContextGroup > p_Concrete_Concrete_ContextGroup;
        std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Concrete_Namespace : public mega::io::Object
    {
        Concrete_Namespace( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Concrete_Namespace( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Namespace >& interface_namespace, const std::vector< data::Ptr< data::Concrete::Concrete_Dimension > >& dimensions);
        enum 
        {
            Object_Part_Type_ID = 94
        };
        data::Ptr< data::Tree::Interface_Namespace > interface_namespace;
        std::vector< data::Ptr< data::Concrete::Concrete_Dimension > > dimensions;
        Ptr< Concrete::Concrete_Context > p_Concrete_Concrete_Context;
        std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Concrete_Action : public mega::io::Object
    {
        Concrete_Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Concrete_Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Action >& interface_action, const std::vector< data::Ptr< data::Concrete::Concrete_Dimension > >& dimensions);
        enum 
        {
            Object_Part_Type_ID = 95
        };
        data::Ptr< data::Tree::Interface_Action > interface_action;
        std::vector< data::Ptr< data::Concrete::Concrete_Dimension > > dimensions;
        Ptr< Concrete::Concrete_Context > p_Concrete_Concrete_Context;
        std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Concrete_Event : public mega::io::Object
    {
        Concrete_Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Concrete_Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Event >& interface_event, const std::vector< data::Ptr< data::Concrete::Concrete_Dimension > >& dimensions);
        enum 
        {
            Object_Part_Type_ID = 96
        };
        data::Ptr< data::Tree::Interface_Event > interface_event;
        std::vector< data::Ptr< data::Concrete::Concrete_Dimension > > dimensions;
        Ptr< Concrete::Concrete_Context > p_Concrete_Concrete_Context;
        std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Concrete_Function : public mega::io::Object
    {
        Concrete_Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Concrete_Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Function >& interface_function);
        enum 
        {
            Object_Part_Type_ID = 97
        };
        data::Ptr< data::Tree::Interface_Function > interface_function;
        Ptr< Concrete::Concrete_Context > p_Concrete_Concrete_Context;
        std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Concrete_Object : public mega::io::Object
    {
        Concrete_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Concrete_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Object >& interface_object, const std::vector< data::Ptr< data::Concrete::Concrete_Dimension > >& dimensions);
        enum 
        {
            Object_Part_Type_ID = 98
        };
        data::Ptr< data::Tree::Interface_Object > interface_object;
        std::vector< data::Ptr< data::Concrete::Concrete_Dimension > > dimensions;
        Ptr< Concrete::Concrete_Context > p_Concrete_Concrete_Context;
        std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Concrete_Link : public mega::io::Object
    {
        Concrete_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Concrete_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Link >& interface_link);
        enum 
        {
            Object_Part_Type_ID = 99
        };
        data::Ptr< data::Tree::Interface_Link > interface_link;
        Ptr< Concrete::Concrete_Context > p_Concrete_Concrete_Context;
        std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Concrete_Table : public mega::io::Object
    {
        Concrete_Table( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Concrete_Table( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Table >& interface_table);
        enum 
        {
            Object_Part_Type_ID = 100
        };
        data::Ptr< data::Tree::Interface_Table > interface_table;
        Ptr< Concrete::Concrete_Context > p_Concrete_Concrete_Context;
        std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Concrete_Root : public mega::io::Object
    {
        Concrete_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Concrete_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Root >& interface_root);
        enum 
        {
            Object_Part_Type_ID = 101
        };
        data::Ptr< data::Tree::Interface_Root > interface_root;
        Ptr< Concrete::Concrete_ContextGroup > p_Concrete_Concrete_ContextGroup;
        std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
}
namespace Model
{
    struct HyperGraph_ObjectGraph : public mega::io::Object
    {
        HyperGraph_ObjectGraph( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        HyperGraph_ObjectGraph( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::io::megaFilePath& source_file, const std::size_t& hash_code);
        enum 
        {
            Object_Part_Type_ID = 119
        };
        mega::io::megaFilePath source_file;
        std::size_t hash_code;
        std::variant< data::Ptr< data::Model::HyperGraph_ObjectGraph > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct HyperGraph_Graph : public mega::io::Object
    {
        HyperGraph_Graph( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        HyperGraph_Graph( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Model::HyperGraph_ObjectGraph > >& objects);
        enum 
        {
            Object_Part_Type_ID = 120
        };
        std::vector< data::Ptr< data::Model::HyperGraph_ObjectGraph > > objects;
        std::variant< data::Ptr< data::Model::HyperGraph_Graph > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
}
namespace Derivations
{
    struct Derivation_ObjectMapping : public mega::io::Object
    {
        Derivation_ObjectMapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Derivation_ObjectMapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::io::megaFilePath& source_file, const std::size_t& hash_code, const std::multimap< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_IContext > >& inheritance);
        enum 
        {
            Object_Part_Type_ID = 121
        };
        mega::io::megaFilePath source_file;
        std::size_t hash_code;
        std::multimap< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_IContext > > inheritance;
        std::variant< data::Ptr< data::Derivations::Derivation_ObjectMapping > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Derivation_Mapping : public mega::io::Object
    {
        Derivation_Mapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Derivation_Mapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Derivations::Derivation_ObjectMapping > >& mappings);
        enum 
        {
            Object_Part_Type_ID = 122
        };
        std::vector< data::Ptr< data::Derivations::Derivation_ObjectMapping > > mappings;
        std::optional< std::multimap< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_IContext > > > inheritance;
        std::variant< data::Ptr< data::Derivations::Derivation_Mapping > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
}
namespace Operations
{
    struct Invocations_Variables_Variable : public mega::io::Object
    {
        Invocations_Variables_Variable( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Variables_Variable( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::optional< data::Ptr< data::Operations::Invocations_Variables_Variable > >& parent);
        enum 
        {
            Object_Part_Type_ID = 55
        };
        std::optional< data::Ptr< data::Operations::Invocations_Variables_Variable > > parent;
        std::variant< data::Ptr< data::Operations::Invocations_Variables_Variable >, data::Ptr< data::Operations::Invocations_Variables_Instance >, data::Ptr< data::Operations::Invocations_Variables_Reference >, data::Ptr< data::Operations::Invocations_Variables_Dimension >, data::Ptr< data::Operations::Invocations_Variables_Context > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Variables_Instance : public mega::io::Object
    {
        Invocations_Variables_Instance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Variables_Instance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Context >& concrete);
        enum 
        {
            Object_Part_Type_ID = 56
        };
        data::Ptr< data::Concrete::Concrete_Context > concrete;
        Ptr< Operations::Invocations_Variables_Variable > p_Operations_Invocations_Variables_Variable;
        std::variant< data::Ptr< data::Operations::Invocations_Variables_Variable >, data::Ptr< data::Operations::Invocations_Variables_Instance >, data::Ptr< data::Operations::Invocations_Variables_Reference >, data::Ptr< data::Operations::Invocations_Variables_Dimension >, data::Ptr< data::Operations::Invocations_Variables_Context > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Variables_Reference : public mega::io::Object
    {
        Invocations_Variables_Reference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Variables_Reference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Concrete::Concrete_Context > >& types);
        enum 
        {
            Object_Part_Type_ID = 57
        };
        std::vector< data::Ptr< data::Concrete::Concrete_Context > > types;
        Ptr< Operations::Invocations_Variables_Variable > p_Operations_Invocations_Variables_Variable;
        std::variant< data::Ptr< data::Operations::Invocations_Variables_Variable >, data::Ptr< data::Operations::Invocations_Variables_Instance >, data::Ptr< data::Operations::Invocations_Variables_Reference >, data::Ptr< data::Operations::Invocations_Variables_Dimension >, data::Ptr< data::Operations::Invocations_Variables_Context > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Variables_Dimension : public mega::io::Object
    {
        Invocations_Variables_Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 58
        };
        Ptr< Operations::Invocations_Variables_Reference > p_Operations_Invocations_Variables_Reference;
        std::variant< data::Ptr< data::Operations::Invocations_Variables_Variable >, data::Ptr< data::Operations::Invocations_Variables_Instance >, data::Ptr< data::Operations::Invocations_Variables_Reference >, data::Ptr< data::Operations::Invocations_Variables_Dimension >, data::Ptr< data::Operations::Invocations_Variables_Context > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Variables_Context : public mega::io::Object
    {
        Invocations_Variables_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 59
        };
        Ptr< Operations::Invocations_Variables_Reference > p_Operations_Invocations_Variables_Reference;
        std::variant< data::Ptr< data::Operations::Invocations_Variables_Variable >, data::Ptr< data::Operations::Invocations_Variables_Instance >, data::Ptr< data::Operations::Invocations_Variables_Reference >, data::Ptr< data::Operations::Invocations_Variables_Dimension >, data::Ptr< data::Operations::Invocations_Variables_Context > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Instructions_Instruction : public mega::io::Object
    {
        Invocations_Instructions_Instruction( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 60
        };
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Instructions_InstructionGroup : public mega::io::Object
    {
        Invocations_Instructions_InstructionGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Instructions_InstructionGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Operations::Invocations_Instructions_Instruction > >& children);
        enum 
        {
            Object_Part_Type_ID = 61
        };
        std::vector< data::Ptr< data::Operations::Invocations_Instructions_Instruction > > children;
        Ptr< Operations::Invocations_Instructions_Instruction > p_Operations_Invocations_Instructions_Instruction;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Instructions_Root : public mega::io::Object
    {
        Invocations_Instructions_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Instructions_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Context >& context);
        enum 
        {
            Object_Part_Type_ID = 62
        };
        data::Ptr< data::Operations::Invocations_Variables_Context > context;
        Ptr< Operations::Invocations_Instructions_InstructionGroup > p_Operations_Invocations_Instructions_InstructionGroup;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Instructions_ParentDerivation : public mega::io::Object
    {
        Invocations_Instructions_ParentDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Instructions_ParentDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Instance >& from, const data::Ptr< data::Operations::Invocations_Variables_Instance >& to);
        enum 
        {
            Object_Part_Type_ID = 63
        };
        data::Ptr< data::Operations::Invocations_Variables_Instance > from;
        data::Ptr< data::Operations::Invocations_Variables_Instance > to;
        Ptr< Operations::Invocations_Instructions_InstructionGroup > p_Operations_Invocations_Instructions_InstructionGroup;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Instructions_ChildDerivation : public mega::io::Object
    {
        Invocations_Instructions_ChildDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Instructions_ChildDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Instance >& from, const data::Ptr< data::Operations::Invocations_Variables_Instance >& to);
        enum 
        {
            Object_Part_Type_ID = 64
        };
        data::Ptr< data::Operations::Invocations_Variables_Instance > from;
        data::Ptr< data::Operations::Invocations_Variables_Instance > to;
        Ptr< Operations::Invocations_Instructions_InstructionGroup > p_Operations_Invocations_Instructions_InstructionGroup;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Instructions_EnumDerivation : public mega::io::Object
    {
        Invocations_Instructions_EnumDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Instructions_EnumDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Instance >& from, const data::Ptr< data::Operations::Invocations_Variables_Instance >& to);
        enum 
        {
            Object_Part_Type_ID = 65
        };
        data::Ptr< data::Operations::Invocations_Variables_Instance > from;
        data::Ptr< data::Operations::Invocations_Variables_Instance > to;
        Ptr< Operations::Invocations_Instructions_InstructionGroup > p_Operations_Invocations_Instructions_InstructionGroup;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Instructions_Enumeration : public mega::io::Object
    {
        Invocations_Instructions_Enumeration( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Instructions_Enumeration( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Instance >& instance);
        enum 
        {
            Object_Part_Type_ID = 66
        };
        data::Ptr< data::Operations::Invocations_Variables_Instance > instance;
        Ptr< Operations::Invocations_Instructions_InstructionGroup > p_Operations_Invocations_Instructions_InstructionGroup;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Instructions_DimensionReferenceRead : public mega::io::Object
    {
        Invocations_Instructions_DimensionReferenceRead( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Instructions_DimensionReferenceRead( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Instance >& instance, const data::Ptr< data::Operations::Invocations_Variables_Dimension >& dimension_reference, const data::Ptr< data::Concrete::Concrete_Dimension >& concrete);
        enum 
        {
            Object_Part_Type_ID = 67
        };
        data::Ptr< data::Operations::Invocations_Variables_Instance > instance;
        data::Ptr< data::Operations::Invocations_Variables_Dimension > dimension_reference;
        data::Ptr< data::Concrete::Concrete_Dimension > concrete;
        Ptr< Operations::Invocations_Instructions_InstructionGroup > p_Operations_Invocations_Instructions_InstructionGroup;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Instructions_MonoReference : public mega::io::Object
    {
        Invocations_Instructions_MonoReference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Instructions_MonoReference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Reference >& reference, const data::Ptr< data::Operations::Invocations_Variables_Instance >& instance);
        enum 
        {
            Object_Part_Type_ID = 68
        };
        data::Ptr< data::Operations::Invocations_Variables_Reference > reference;
        data::Ptr< data::Operations::Invocations_Variables_Instance > instance;
        Ptr< Operations::Invocations_Instructions_InstructionGroup > p_Operations_Invocations_Instructions_InstructionGroup;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Instructions_PolyReference : public mega::io::Object
    {
        Invocations_Instructions_PolyReference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Instructions_PolyReference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Reference >& from_reference);
        enum 
        {
            Object_Part_Type_ID = 69
        };
        data::Ptr< data::Operations::Invocations_Variables_Reference > from_reference;
        Ptr< Operations::Invocations_Instructions_InstructionGroup > p_Operations_Invocations_Instructions_InstructionGroup;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Instructions_PolyCase : public mega::io::Object
    {
        Invocations_Instructions_PolyCase( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Instructions_PolyCase( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Reference >& reference, const data::Ptr< data::Operations::Invocations_Variables_Instance >& to);
        enum 
        {
            Object_Part_Type_ID = 70
        };
        data::Ptr< data::Operations::Invocations_Variables_Reference > reference;
        data::Ptr< data::Operations::Invocations_Variables_Instance > to;
        Ptr< Operations::Invocations_Instructions_InstructionGroup > p_Operations_Invocations_Instructions_InstructionGroup;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Instructions_Failure : public mega::io::Object
    {
        Invocations_Instructions_Failure( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 71
        };
        Ptr< Operations::Invocations_Instructions_InstructionGroup > p_Operations_Invocations_Instructions_InstructionGroup;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Instructions_Elimination : public mega::io::Object
    {
        Invocations_Instructions_Elimination( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 72
        };
        Ptr< Operations::Invocations_Instructions_InstructionGroup > p_Operations_Invocations_Instructions_InstructionGroup;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Instructions_Prune : public mega::io::Object
    {
        Invocations_Instructions_Prune( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 73
        };
        Ptr< Operations::Invocations_Instructions_InstructionGroup > p_Operations_Invocations_Instructions_InstructionGroup;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_Operation : public mega::io::Object
    {
        Invocations_Operations_Operation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Operations_Operation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Instance >& instance, const std::vector< data::Ptr< data::Operations::Operations_InterfaceVariant > >& return_types, const std::vector< data::Ptr< data::Operations::Operations_InterfaceVariant > >& parameter_types);
        enum 
        {
            Object_Part_Type_ID = 74
        };
        data::Ptr< data::Operations::Invocations_Variables_Instance > instance;
        std::vector< data::Ptr< data::Operations::Operations_InterfaceVariant > > return_types;
        std::vector< data::Ptr< data::Operations::Operations_InterfaceVariant > > parameter_types;
        Ptr< Operations::Invocations_Instructions_Instruction > p_Operations_Invocations_Instructions_Instruction;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_BasicOperation : public mega::io::Object
    {
        Invocations_Operations_BasicOperation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Operations_BasicOperation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_IContext >& interface, const data::Ptr< data::Concrete::Concrete_Context >& concrete_target);
        enum 
        {
            Object_Part_Type_ID = 75
        };
        data::Ptr< data::Tree::Interface_IContext > interface;
        data::Ptr< data::Concrete::Concrete_Context > concrete_target;
        Ptr< Operations::Invocations_Operations_Operation > p_Operations_Invocations_Operations_Operation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_DimensionOperation : public mega::io::Object
    {
        Invocations_Operations_DimensionOperation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Operations_DimensionOperation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_DimensionTrait >& interface_dimension, const data::Ptr< data::Concrete::Concrete_Dimension >& concrete_dimension);
        enum 
        {
            Object_Part_Type_ID = 76
        };
        data::Ptr< data::Tree::Interface_DimensionTrait > interface_dimension;
        data::Ptr< data::Concrete::Concrete_Dimension > concrete_dimension;
        Ptr< Operations::Invocations_Operations_Operation > p_Operations_Invocations_Operations_Operation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_Call : public mega::io::Object
    {
        Invocations_Operations_Call( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 77
        };
        Ptr< Operations::Invocations_Operations_BasicOperation > p_Operations_Invocations_Operations_BasicOperation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_Start : public mega::io::Object
    {
        Invocations_Operations_Start( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 78
        };
        Ptr< Operations::Invocations_Operations_BasicOperation > p_Operations_Invocations_Operations_BasicOperation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_Stop : public mega::io::Object
    {
        Invocations_Operations_Stop( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 79
        };
        Ptr< Operations::Invocations_Operations_BasicOperation > p_Operations_Invocations_Operations_BasicOperation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_Pause : public mega::io::Object
    {
        Invocations_Operations_Pause( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 80
        };
        Ptr< Operations::Invocations_Operations_BasicOperation > p_Operations_Invocations_Operations_BasicOperation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_Resume : public mega::io::Object
    {
        Invocations_Operations_Resume( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 81
        };
        Ptr< Operations::Invocations_Operations_BasicOperation > p_Operations_Invocations_Operations_BasicOperation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_Done : public mega::io::Object
    {
        Invocations_Operations_Done( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 82
        };
        Ptr< Operations::Invocations_Operations_BasicOperation > p_Operations_Invocations_Operations_BasicOperation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_WaitAction : public mega::io::Object
    {
        Invocations_Operations_WaitAction( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 83
        };
        Ptr< Operations::Invocations_Operations_BasicOperation > p_Operations_Invocations_Operations_BasicOperation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_WaitDimension : public mega::io::Object
    {
        Invocations_Operations_WaitDimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 84
        };
        Ptr< Operations::Invocations_Operations_DimensionOperation > p_Operations_Invocations_Operations_DimensionOperation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_GetAction : public mega::io::Object
    {
        Invocations_Operations_GetAction( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 85
        };
        Ptr< Operations::Invocations_Operations_BasicOperation > p_Operations_Invocations_Operations_BasicOperation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_GetDimension : public mega::io::Object
    {
        Invocations_Operations_GetDimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 86
        };
        Ptr< Operations::Invocations_Operations_DimensionOperation > p_Operations_Invocations_Operations_DimensionOperation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_Read : public mega::io::Object
    {
        Invocations_Operations_Read( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 87
        };
        Ptr< Operations::Invocations_Operations_DimensionOperation > p_Operations_Invocations_Operations_DimensionOperation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_Write : public mega::io::Object
    {
        Invocations_Operations_Write( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 88
        };
        Ptr< Operations::Invocations_Operations_DimensionOperation > p_Operations_Invocations_Operations_DimensionOperation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_WriteLink : public mega::io::Object
    {
        Invocations_Operations_WriteLink( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Invocations_Operations_WriteLink( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Dimension >& dimension_reference);
        enum 
        {
            Object_Part_Type_ID = 89
        };
        data::Ptr< data::Operations::Invocations_Variables_Dimension > dimension_reference;
        Ptr< Operations::Invocations_Operations_DimensionOperation > p_Operations_Invocations_Operations_DimensionOperation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Invocations_Operations_Range : public mega::io::Object
    {
        Invocations_Operations_Range( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        enum 
        {
            Object_Part_Type_ID = 90
        };
        Ptr< Operations::Invocations_Operations_BasicOperation > p_Operations_Invocations_Operations_BasicOperation;
        std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Operations_InterfaceVariant : public mega::io::Object
    {
        Operations_InterfaceVariant( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Operations_InterfaceVariant( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::optional< data::Ptr< data::Tree::Interface_IContext > >& context, const std::optional< data::Ptr< data::Tree::Interface_DimensionTrait > >& dimension);
        enum 
        {
            Object_Part_Type_ID = 102
        };
        std::optional< data::Ptr< data::Tree::Interface_IContext > > context;
        std::optional< data::Ptr< data::Tree::Interface_DimensionTrait > > dimension;
        std::variant< data::Ptr< data::Operations::Operations_InterfaceVariant > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Operations_ConcreteVariant : public mega::io::Object
    {
        Operations_ConcreteVariant( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Operations_ConcreteVariant( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::optional< data::Ptr< data::Concrete::Concrete_Context > >& context, const std::optional< data::Ptr< data::Concrete::Concrete_Dimension > >& dimension);
        enum 
        {
            Object_Part_Type_ID = 103
        };
        std::optional< data::Ptr< data::Concrete::Concrete_Context > > context;
        std::optional< data::Ptr< data::Concrete::Concrete_Dimension > > dimension;
        std::variant< data::Ptr< data::Operations::Operations_ConcreteVariant > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Operations_Element : public mega::io::Object
    {
        Operations_Element( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Operations_Element( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Operations_InterfaceVariant >& interface, const data::Ptr< data::Operations::Operations_ConcreteVariant >& concrete);
        enum 
        {
            Object_Part_Type_ID = 104
        };
        data::Ptr< data::Operations::Operations_InterfaceVariant > interface;
        data::Ptr< data::Operations::Operations_ConcreteVariant > concrete;
        std::variant< data::Ptr< data::Operations::Operations_Element > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Operations_ElementVector : public mega::io::Object
    {
        Operations_ElementVector( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Operations_ElementVector( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Operations::Operations_Element > >& elements);
        enum 
        {
            Object_Part_Type_ID = 105
        };
        std::vector< data::Ptr< data::Operations::Operations_Element > > elements;
        std::variant< data::Ptr< data::Operations::Operations_ElementVector > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Operations_Context : public mega::io::Object
    {
        Operations_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Operations_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Operations::Operations_ElementVector > >& vectors);
        enum 
        {
            Object_Part_Type_ID = 106
        };
        std::vector< data::Ptr< data::Operations::Operations_ElementVector > > vectors;
        std::variant< data::Ptr< data::Operations::Operations_Context > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Operations_TypePath : public mega::io::Object
    {
        Operations_TypePath( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Operations_TypePath( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Operations::Operations_ElementVector > >& vectors);
        enum 
        {
            Object_Part_Type_ID = 107
        };
        std::vector< data::Ptr< data::Operations::Operations_ElementVector > > vectors;
        std::variant< data::Ptr< data::Operations::Operations_TypePath > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Operations_NameRoot : public mega::io::Object
    {
        Operations_NameRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Operations_NameRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Operations::Operations_Name > >& children);
        enum 
        {
            Object_Part_Type_ID = 108
        };
        std::vector< data::Ptr< data::Operations::Operations_Name > > children;
        std::variant< data::Ptr< data::Operations::Operations_NameRoot >, data::Ptr< data::Operations::Operations_Name > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Operations_Name : public mega::io::Object
    {
        Operations_Name( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Operations_Name( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Operations_Element >& element, const bool& is_member, const bool& is_reference);
        enum 
        {
            Object_Part_Type_ID = 109
        };
        data::Ptr< data::Operations::Operations_Element > element;
        bool is_member;
        bool is_reference;
        Ptr< Operations::Operations_NameRoot > p_Operations_Operations_NameRoot;
        std::variant< data::Ptr< data::Operations::Operations_NameRoot >, data::Ptr< data::Operations::Operations_Name > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Operations_NameResolution : public mega::io::Object
    {
        Operations_NameResolution( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Operations_NameResolution( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Operations_NameRoot >& root_name);
        enum 
        {
            Object_Part_Type_ID = 110
        };
        data::Ptr< data::Operations::Operations_NameRoot > root_name;
        std::variant< data::Ptr< data::Operations::Operations_NameResolution > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Operations_Invocation : public mega::io::Object
    {
        Operations_Invocation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Operations_Invocation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Operations_Context >& context, const data::Ptr< data::Operations::Operations_TypePath >& type_path, const mega::OperationID& operation, const std::string& name, const std::string& context_str, const std::string& type_path_str);
        enum 
        {
            Object_Part_Type_ID = 111
        };
        data::Ptr< data::Operations::Operations_Context > context;
        data::Ptr< data::Operations::Operations_TypePath > type_path;
        mega::OperationID operation;
        std::string name;
        std::string context_str;
        std::string type_path_str;
        std::optional< std::string > return_type_str;
        std::optional< data::Ptr< data::Operations::Operations_NameResolution > > name_resolution;
        std::optional< data::Ptr< data::Operations::Invocations_Variables_Context > > root_variable;
        std::optional< data::Ptr< data::Operations::Invocations_Instructions_Root > > root_instruction;
        std::optional< std::vector< data::Ptr< data::Tree::Interface_IContext > > > return_types_context;
        std::optional< std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > > > return_types_dimension;
        std::optional< bool > homogeneous;
        std::variant< data::Ptr< data::Operations::Operations_Invocation > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
    struct Operations_Invocations : public mega::io::Object
    {
        Operations_Invocations( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
        Operations_Invocations( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::map< mega::invocation::ID, data::Ptr< data::Operations::Operations_Invocation > >& invocations);
        enum 
        {
            Object_Part_Type_ID = 112
        };
        std::map< mega::invocation::ID, data::Ptr< data::Operations::Operations_Invocation > > invocations;
        std::variant< data::Ptr< data::Operations::Operations_Invocations > > m_inheritance;
        virtual bool test_inheritance_pointer( ObjectPartLoader &loader ) const;
        virtual void set_inheritance_pointer();
        virtual void load( mega::io::Loader& loader );
        virtual void store( mega::io::Storer& storer ) const;
        virtual void to_json( nlohmann::json& data ) const;
    };
}

template <>
inline Ptr< Components::Components_Component > convert( const Ptr< Components::Components_Component >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_Identifier > convert( const Ptr< AST::Parser_Identifier >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_ScopedIdentifier > convert( const Ptr< AST::Parser_ScopedIdentifier >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_ArgumentList > convert( const Ptr< AST::Parser_ArgumentList >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_ReturnType > convert( const Ptr< AST::Parser_ReturnType >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_Inheritance > convert( const Ptr< AST::Parser_Inheritance >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_Size > convert( const Ptr< AST::Parser_Size >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_Link > convert( const Ptr< AST::Parser_Link >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_Dimension > convert( const Ptr< AST::Parser_Dimension >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_Include > convert( const Ptr< AST::Parser_Include >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_Include > convert( const Ptr< AST::Parser_SystemInclude >& from )
{
    return from->p_AST_Parser_Include;
}
template <>
inline Ptr< AST::Parser_SystemInclude > convert( const Ptr< AST::Parser_SystemInclude >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_Include > convert( const Ptr< AST::Parser_MegaInclude >& from )
{
    return from->p_AST_Parser_Include;
}
template <>
inline Ptr< AST::Parser_MegaInclude > convert( const Ptr< AST::Parser_MegaInclude >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_Include > convert( const Ptr< AST::Parser_MegaIncludeInline >& from )
{
    return from->p_AST_Parser_MegaInclude->p_AST_Parser_Include;
}
template <>
inline Ptr< AST::Parser_MegaInclude > convert( const Ptr< AST::Parser_MegaIncludeInline >& from )
{
    return from->p_AST_Parser_MegaInclude;
}
template <>
inline Ptr< AST::Parser_MegaIncludeInline > convert( const Ptr< AST::Parser_MegaIncludeInline >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_Include > convert( const Ptr< AST::Parser_MegaIncludeNested >& from )
{
    return from->p_AST_Parser_MegaInclude->p_AST_Parser_Include;
}
template <>
inline Ptr< AST::Parser_MegaInclude > convert( const Ptr< AST::Parser_MegaIncludeNested >& from )
{
    return from->p_AST_Parser_MegaInclude;
}
template <>
inline Ptr< AST::Parser_MegaIncludeNested > convert( const Ptr< AST::Parser_MegaIncludeNested >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_Include > convert( const Ptr< AST::Parser_CPPInclude >& from )
{
    return from->p_AST_Parser_Include;
}
template <>
inline Ptr< AST::Parser_CPPInclude > convert( const Ptr< AST::Parser_CPPInclude >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_Dependency > convert( const Ptr< AST::Parser_Dependency >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_ContextDef > convert( const Ptr< AST::Parser_ContextDef >& from )
{
    return from;
}
template <>
inline Ptr< Body::Parser_ContextDef > convert( const Ptr< AST::Parser_ContextDef >& from )
{
    return from->p_Body_Parser_ContextDef;
}
template <>
inline Ptr< AST::Parser_ContextDef > convert( const Ptr< AST::Parser_NamespaceDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
template <>
inline Ptr< Body::Parser_ContextDef > convert( const Ptr< AST::Parser_NamespaceDef >& from )
{
    return from->p_AST_Parser_ContextDef->p_Body_Parser_ContextDef;
}
template <>
inline Ptr< AST::Parser_NamespaceDef > convert( const Ptr< AST::Parser_NamespaceDef >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_ContextDef > convert( const Ptr< AST::Parser_AbstractDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
template <>
inline Ptr< Body::Parser_ContextDef > convert( const Ptr< AST::Parser_AbstractDef >& from )
{
    return from->p_AST_Parser_ContextDef->p_Body_Parser_ContextDef;
}
template <>
inline Ptr< AST::Parser_AbstractDef > convert( const Ptr< AST::Parser_AbstractDef >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_ContextDef > convert( const Ptr< AST::Parser_ActionDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
template <>
inline Ptr< Body::Parser_ContextDef > convert( const Ptr< AST::Parser_ActionDef >& from )
{
    return from->p_AST_Parser_ContextDef->p_Body_Parser_ContextDef;
}
template <>
inline Ptr< AST::Parser_ActionDef > convert( const Ptr< AST::Parser_ActionDef >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_ContextDef > convert( const Ptr< AST::Parser_EventDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
template <>
inline Ptr< Body::Parser_ContextDef > convert( const Ptr< AST::Parser_EventDef >& from )
{
    return from->p_AST_Parser_ContextDef->p_Body_Parser_ContextDef;
}
template <>
inline Ptr< AST::Parser_EventDef > convert( const Ptr< AST::Parser_EventDef >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_ContextDef > convert( const Ptr< AST::Parser_FunctionDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
template <>
inline Ptr< Body::Parser_ContextDef > convert( const Ptr< AST::Parser_FunctionDef >& from )
{
    return from->p_AST_Parser_ContextDef->p_Body_Parser_ContextDef;
}
template <>
inline Ptr< AST::Parser_FunctionDef > convert( const Ptr< AST::Parser_FunctionDef >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_ContextDef > convert( const Ptr< AST::Parser_ObjectDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
template <>
inline Ptr< Body::Parser_ContextDef > convert( const Ptr< AST::Parser_ObjectDef >& from )
{
    return from->p_AST_Parser_ContextDef->p_Body_Parser_ContextDef;
}
template <>
inline Ptr< AST::Parser_ObjectDef > convert( const Ptr< AST::Parser_ObjectDef >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_ContextDef > convert( const Ptr< AST::Parser_LinkDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
template <>
inline Ptr< Body::Parser_ContextDef > convert( const Ptr< AST::Parser_LinkDef >& from )
{
    return from->p_AST_Parser_ContextDef->p_Body_Parser_ContextDef;
}
template <>
inline Ptr< AST::Parser_LinkDef > convert( const Ptr< AST::Parser_LinkDef >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_ContextDef > convert( const Ptr< AST::Parser_TableDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
template <>
inline Ptr< Body::Parser_ContextDef > convert( const Ptr< AST::Parser_TableDef >& from )
{
    return from->p_AST_Parser_ContextDef->p_Body_Parser_ContextDef;
}
template <>
inline Ptr< AST::Parser_TableDef > convert( const Ptr< AST::Parser_TableDef >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_SourceRoot > convert( const Ptr< AST::Parser_SourceRoot >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_SourceRoot > convert( const Ptr< AST::Parser_IncludeRoot >& from )
{
    return from->p_AST_Parser_SourceRoot;
}
template <>
inline Ptr< AST::Parser_IncludeRoot > convert( const Ptr< AST::Parser_IncludeRoot >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_SourceRoot > convert( const Ptr< AST::Parser_ObjectSourceRoot >& from )
{
    return from->p_AST_Parser_SourceRoot;
}
template <>
inline Ptr< AST::Parser_ObjectSourceRoot > convert( const Ptr< AST::Parser_ObjectSourceRoot >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_Dimension > convert( const Ptr< Tree::Interface_DimensionTrait >& from )
{
    return from->p_AST_Parser_Dimension;
}
template <>
inline Ptr< Tree::Interface_DimensionTrait > convert( const Ptr< Tree::Interface_DimensionTrait >& from )
{
    return from;
}
template <>
inline Ptr< PerSourceSymbols::Interface_DimensionTrait > convert( const Ptr< Tree::Interface_DimensionTrait >& from )
{
    return from->p_PerSourceSymbols_Interface_DimensionTrait;
}
template <>
inline Ptr< Concrete::Interface_DimensionTrait > convert( const Ptr< Tree::Interface_DimensionTrait >& from )
{
    return from->p_Concrete_Interface_DimensionTrait;
}
template <>
inline Ptr< Clang::Interface_DimensionTrait > convert( const Ptr< Tree::Interface_DimensionTrait >& from )
{
    return from->p_Clang_Interface_DimensionTrait;
}
template <>
inline Ptr< AST::Parser_Inheritance > convert( const Ptr< Tree::Interface_InheritanceTrait >& from )
{
    return from->p_AST_Parser_Inheritance;
}
template <>
inline Ptr< Tree::Interface_InheritanceTrait > convert( const Ptr< Tree::Interface_InheritanceTrait >& from )
{
    return from;
}
template <>
inline Ptr< Clang::Interface_InheritanceTrait > convert( const Ptr< Tree::Interface_InheritanceTrait >& from )
{
    return from->p_Clang_Interface_InheritanceTrait;
}
template <>
inline Ptr< AST::Parser_Link > convert( const Ptr< Tree::Interface_LinkTrait >& from )
{
    return from->p_AST_Parser_Link;
}
template <>
inline Ptr< Tree::Interface_LinkTrait > convert( const Ptr< Tree::Interface_LinkTrait >& from )
{
    return from;
}
template <>
inline Ptr< AST::Parser_ReturnType > convert( const Ptr< Tree::Interface_ReturnTypeTrait >& from )
{
    return from->p_AST_Parser_ReturnType;
}
template <>
inline Ptr< Tree::Interface_ReturnTypeTrait > convert( const Ptr< Tree::Interface_ReturnTypeTrait >& from )
{
    return from;
}
template <>
inline Ptr< Clang::Interface_ReturnTypeTrait > convert( const Ptr< Tree::Interface_ReturnTypeTrait >& from )
{
    return from->p_Clang_Interface_ReturnTypeTrait;
}
template <>
inline Ptr< AST::Parser_ArgumentList > convert( const Ptr< Tree::Interface_ArgumentListTrait >& from )
{
    return from->p_AST_Parser_ArgumentList;
}
template <>
inline Ptr< Tree::Interface_ArgumentListTrait > convert( const Ptr< Tree::Interface_ArgumentListTrait >& from )
{
    return from;
}
template <>
inline Ptr< Clang::Interface_ArgumentListTrait > convert( const Ptr< Tree::Interface_ArgumentListTrait >& from )
{
    return from->p_Clang_Interface_ArgumentListTrait;
}
template <>
inline Ptr< AST::Parser_Size > convert( const Ptr< Tree::Interface_SizeTrait >& from )
{
    return from->p_AST_Parser_Size;
}
template <>
inline Ptr< Tree::Interface_SizeTrait > convert( const Ptr< Tree::Interface_SizeTrait >& from )
{
    return from;
}
template <>
inline Ptr< Clang::Interface_SizeTrait > convert( const Ptr< Tree::Interface_SizeTrait >& from )
{
    return from->p_Clang_Interface_SizeTrait;
}
template <>
inline Ptr< Tree::Interface_ContextGroup > convert( const Ptr< Tree::Interface_ContextGroup >& from )
{
    return from;
}
template <>
inline Ptr< Tree::Interface_ContextGroup > convert( const Ptr< Tree::Interface_Root >& from )
{
    return from->p_Tree_Interface_ContextGroup;
}
template <>
inline Ptr< Tree::Interface_Root > convert( const Ptr< Tree::Interface_Root >& from )
{
    return from;
}
template <>
inline Ptr< Tree::Interface_ContextGroup > convert( const Ptr< Tree::Interface_IContext >& from )
{
    return from->p_Tree_Interface_ContextGroup;
}
template <>
inline Ptr< Tree::Interface_IContext > convert( const Ptr< Tree::Interface_IContext >& from )
{
    return from;
}
template <>
inline Ptr< PerSourceSymbols::Interface_IContext > convert( const Ptr< Tree::Interface_IContext >& from )
{
    return from->p_PerSourceSymbols_Interface_IContext;
}
template <>
inline Ptr< Concrete::Interface_IContext > convert( const Ptr< Tree::Interface_IContext >& from )
{
    return from->p_Concrete_Interface_IContext;
}
template <>
inline Ptr< Tree::Interface_ContextGroup > convert( const Ptr< Tree::Interface_Namespace >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
template <>
inline Ptr< Tree::Interface_IContext > convert( const Ptr< Tree::Interface_Namespace >& from )
{
    return from->p_Tree_Interface_IContext;
}
template <>
inline Ptr< PerSourceSymbols::Interface_IContext > convert( const Ptr< Tree::Interface_Namespace >& from )
{
    return from->p_Tree_Interface_IContext->p_PerSourceSymbols_Interface_IContext;
}
template <>
inline Ptr< Concrete::Interface_IContext > convert( const Ptr< Tree::Interface_Namespace >& from )
{
    return from->p_Tree_Interface_IContext->p_Concrete_Interface_IContext;
}
template <>
inline Ptr< Tree::Interface_Namespace > convert( const Ptr< Tree::Interface_Namespace >& from )
{
    return from;
}
template <>
inline Ptr< Tree::Interface_ContextGroup > convert( const Ptr< Tree::Interface_Abstract >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
template <>
inline Ptr< Tree::Interface_IContext > convert( const Ptr< Tree::Interface_Abstract >& from )
{
    return from->p_Tree_Interface_IContext;
}
template <>
inline Ptr< PerSourceSymbols::Interface_IContext > convert( const Ptr< Tree::Interface_Abstract >& from )
{
    return from->p_Tree_Interface_IContext->p_PerSourceSymbols_Interface_IContext;
}
template <>
inline Ptr< Concrete::Interface_IContext > convert( const Ptr< Tree::Interface_Abstract >& from )
{
    return from->p_Tree_Interface_IContext->p_Concrete_Interface_IContext;
}
template <>
inline Ptr< Tree::Interface_Abstract > convert( const Ptr< Tree::Interface_Abstract >& from )
{
    return from;
}
template <>
inline Ptr< Tree::Interface_ContextGroup > convert( const Ptr< Tree::Interface_Action >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
template <>
inline Ptr< Tree::Interface_IContext > convert( const Ptr< Tree::Interface_Action >& from )
{
    return from->p_Tree_Interface_IContext;
}
template <>
inline Ptr< PerSourceSymbols::Interface_IContext > convert( const Ptr< Tree::Interface_Action >& from )
{
    return from->p_Tree_Interface_IContext->p_PerSourceSymbols_Interface_IContext;
}
template <>
inline Ptr< Concrete::Interface_IContext > convert( const Ptr< Tree::Interface_Action >& from )
{
    return from->p_Tree_Interface_IContext->p_Concrete_Interface_IContext;
}
template <>
inline Ptr< Tree::Interface_Action > convert( const Ptr< Tree::Interface_Action >& from )
{
    return from;
}
template <>
inline Ptr< Tree::Interface_ContextGroup > convert( const Ptr< Tree::Interface_Event >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
template <>
inline Ptr< Tree::Interface_IContext > convert( const Ptr< Tree::Interface_Event >& from )
{
    return from->p_Tree_Interface_IContext;
}
template <>
inline Ptr< PerSourceSymbols::Interface_IContext > convert( const Ptr< Tree::Interface_Event >& from )
{
    return from->p_Tree_Interface_IContext->p_PerSourceSymbols_Interface_IContext;
}
template <>
inline Ptr< Concrete::Interface_IContext > convert( const Ptr< Tree::Interface_Event >& from )
{
    return from->p_Tree_Interface_IContext->p_Concrete_Interface_IContext;
}
template <>
inline Ptr< Tree::Interface_Event > convert( const Ptr< Tree::Interface_Event >& from )
{
    return from;
}
template <>
inline Ptr< Tree::Interface_ContextGroup > convert( const Ptr< Tree::Interface_Function >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
template <>
inline Ptr< Tree::Interface_IContext > convert( const Ptr< Tree::Interface_Function >& from )
{
    return from->p_Tree_Interface_IContext;
}
template <>
inline Ptr< PerSourceSymbols::Interface_IContext > convert( const Ptr< Tree::Interface_Function >& from )
{
    return from->p_Tree_Interface_IContext->p_PerSourceSymbols_Interface_IContext;
}
template <>
inline Ptr< Concrete::Interface_IContext > convert( const Ptr< Tree::Interface_Function >& from )
{
    return from->p_Tree_Interface_IContext->p_Concrete_Interface_IContext;
}
template <>
inline Ptr< Tree::Interface_Function > convert( const Ptr< Tree::Interface_Function >& from )
{
    return from;
}
template <>
inline Ptr< Tree::Interface_ContextGroup > convert( const Ptr< Tree::Interface_Object >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
template <>
inline Ptr< Tree::Interface_IContext > convert( const Ptr< Tree::Interface_Object >& from )
{
    return from->p_Tree_Interface_IContext;
}
template <>
inline Ptr< PerSourceSymbols::Interface_IContext > convert( const Ptr< Tree::Interface_Object >& from )
{
    return from->p_Tree_Interface_IContext->p_PerSourceSymbols_Interface_IContext;
}
template <>
inline Ptr< Concrete::Interface_IContext > convert( const Ptr< Tree::Interface_Object >& from )
{
    return from->p_Tree_Interface_IContext->p_Concrete_Interface_IContext;
}
template <>
inline Ptr< Tree::Interface_Object > convert( const Ptr< Tree::Interface_Object >& from )
{
    return from;
}
template <>
inline Ptr< Tree::Interface_ContextGroup > convert( const Ptr< Tree::Interface_Link >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
template <>
inline Ptr< Tree::Interface_IContext > convert( const Ptr< Tree::Interface_Link >& from )
{
    return from->p_Tree_Interface_IContext;
}
template <>
inline Ptr< PerSourceSymbols::Interface_IContext > convert( const Ptr< Tree::Interface_Link >& from )
{
    return from->p_Tree_Interface_IContext->p_PerSourceSymbols_Interface_IContext;
}
template <>
inline Ptr< Concrete::Interface_IContext > convert( const Ptr< Tree::Interface_Link >& from )
{
    return from->p_Tree_Interface_IContext->p_Concrete_Interface_IContext;
}
template <>
inline Ptr< Tree::Interface_Link > convert( const Ptr< Tree::Interface_Link >& from )
{
    return from;
}
template <>
inline Ptr< Tree::Interface_ContextGroup > convert( const Ptr< Tree::Interface_Table >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
template <>
inline Ptr< Tree::Interface_IContext > convert( const Ptr< Tree::Interface_Table >& from )
{
    return from->p_Tree_Interface_IContext;
}
template <>
inline Ptr< PerSourceSymbols::Interface_IContext > convert( const Ptr< Tree::Interface_Table >& from )
{
    return from->p_Tree_Interface_IContext->p_PerSourceSymbols_Interface_IContext;
}
template <>
inline Ptr< Concrete::Interface_IContext > convert( const Ptr< Tree::Interface_Table >& from )
{
    return from->p_Tree_Interface_IContext->p_Concrete_Interface_IContext;
}
template <>
inline Ptr< Tree::Interface_Table > convert( const Ptr< Tree::Interface_Table >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Variables_Variable > convert( const Ptr< Operations::Invocations_Variables_Variable >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Variables_Variable > convert( const Ptr< Operations::Invocations_Variables_Instance >& from )
{
    return from->p_Operations_Invocations_Variables_Variable;
}
template <>
inline Ptr< Operations::Invocations_Variables_Instance > convert( const Ptr< Operations::Invocations_Variables_Instance >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Variables_Variable > convert( const Ptr< Operations::Invocations_Variables_Reference >& from )
{
    return from->p_Operations_Invocations_Variables_Variable;
}
template <>
inline Ptr< Operations::Invocations_Variables_Reference > convert( const Ptr< Operations::Invocations_Variables_Reference >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Variables_Variable > convert( const Ptr< Operations::Invocations_Variables_Dimension >& from )
{
    return from->p_Operations_Invocations_Variables_Reference->p_Operations_Invocations_Variables_Variable;
}
template <>
inline Ptr< Operations::Invocations_Variables_Reference > convert( const Ptr< Operations::Invocations_Variables_Dimension >& from )
{
    return from->p_Operations_Invocations_Variables_Reference;
}
template <>
inline Ptr< Operations::Invocations_Variables_Dimension > convert( const Ptr< Operations::Invocations_Variables_Dimension >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Variables_Variable > convert( const Ptr< Operations::Invocations_Variables_Context >& from )
{
    return from->p_Operations_Invocations_Variables_Reference->p_Operations_Invocations_Variables_Variable;
}
template <>
inline Ptr< Operations::Invocations_Variables_Reference > convert( const Ptr< Operations::Invocations_Variables_Context >& from )
{
    return from->p_Operations_Invocations_Variables_Reference;
}
template <>
inline Ptr< Operations::Invocations_Variables_Context > convert( const Ptr< Operations::Invocations_Variables_Context >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Instructions_Instruction >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Instructions_InstructionGroup >& from )
{
    return from->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Instructions_InstructionGroup > convert( const Ptr< Operations::Invocations_Instructions_InstructionGroup >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Instructions_Root >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Instructions_InstructionGroup > convert( const Ptr< Operations::Invocations_Instructions_Root >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Root > convert( const Ptr< Operations::Invocations_Instructions_Root >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Instructions_ParentDerivation >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Instructions_InstructionGroup > convert( const Ptr< Operations::Invocations_Instructions_ParentDerivation >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup;
}
template <>
inline Ptr< Operations::Invocations_Instructions_ParentDerivation > convert( const Ptr< Operations::Invocations_Instructions_ParentDerivation >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Instructions_ChildDerivation >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Instructions_InstructionGroup > convert( const Ptr< Operations::Invocations_Instructions_ChildDerivation >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup;
}
template <>
inline Ptr< Operations::Invocations_Instructions_ChildDerivation > convert( const Ptr< Operations::Invocations_Instructions_ChildDerivation >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Instructions_EnumDerivation >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Instructions_InstructionGroup > convert( const Ptr< Operations::Invocations_Instructions_EnumDerivation >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup;
}
template <>
inline Ptr< Operations::Invocations_Instructions_EnumDerivation > convert( const Ptr< Operations::Invocations_Instructions_EnumDerivation >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Instructions_Enumeration >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Instructions_InstructionGroup > convert( const Ptr< Operations::Invocations_Instructions_Enumeration >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Enumeration > convert( const Ptr< Operations::Invocations_Instructions_Enumeration >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Instructions_DimensionReferenceRead >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Instructions_InstructionGroup > convert( const Ptr< Operations::Invocations_Instructions_DimensionReferenceRead >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup;
}
template <>
inline Ptr< Operations::Invocations_Instructions_DimensionReferenceRead > convert( const Ptr< Operations::Invocations_Instructions_DimensionReferenceRead >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Instructions_MonoReference >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Instructions_InstructionGroup > convert( const Ptr< Operations::Invocations_Instructions_MonoReference >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup;
}
template <>
inline Ptr< Operations::Invocations_Instructions_MonoReference > convert( const Ptr< Operations::Invocations_Instructions_MonoReference >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Instructions_PolyReference >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Instructions_InstructionGroup > convert( const Ptr< Operations::Invocations_Instructions_PolyReference >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup;
}
template <>
inline Ptr< Operations::Invocations_Instructions_PolyReference > convert( const Ptr< Operations::Invocations_Instructions_PolyReference >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Instructions_PolyCase >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Instructions_InstructionGroup > convert( const Ptr< Operations::Invocations_Instructions_PolyCase >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup;
}
template <>
inline Ptr< Operations::Invocations_Instructions_PolyCase > convert( const Ptr< Operations::Invocations_Instructions_PolyCase >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Instructions_Failure >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Instructions_InstructionGroup > convert( const Ptr< Operations::Invocations_Instructions_Failure >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Failure > convert( const Ptr< Operations::Invocations_Instructions_Failure >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Instructions_Elimination >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Instructions_InstructionGroup > convert( const Ptr< Operations::Invocations_Instructions_Elimination >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Elimination > convert( const Ptr< Operations::Invocations_Instructions_Elimination >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Instructions_Prune >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Instructions_InstructionGroup > convert( const Ptr< Operations::Invocations_Instructions_Prune >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Prune > convert( const Ptr< Operations::Invocations_Instructions_Prune >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_Operation >& from )
{
    return from->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_Operation >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_BasicOperation >& from )
{
    return from->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_BasicOperation >& from )
{
    return from->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_BasicOperation > convert( const Ptr< Operations::Invocations_Operations_BasicOperation >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_DimensionOperation >& from )
{
    return from->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_DimensionOperation >& from )
{
    return from->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_DimensionOperation > convert( const Ptr< Operations::Invocations_Operations_DimensionOperation >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_Call >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_Call >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_BasicOperation > convert( const Ptr< Operations::Invocations_Operations_Call >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation;
}
template <>
inline Ptr< Operations::Invocations_Operations_Call > convert( const Ptr< Operations::Invocations_Operations_Call >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_Start >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_Start >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_BasicOperation > convert( const Ptr< Operations::Invocations_Operations_Start >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation;
}
template <>
inline Ptr< Operations::Invocations_Operations_Start > convert( const Ptr< Operations::Invocations_Operations_Start >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_Stop >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_Stop >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_BasicOperation > convert( const Ptr< Operations::Invocations_Operations_Stop >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation;
}
template <>
inline Ptr< Operations::Invocations_Operations_Stop > convert( const Ptr< Operations::Invocations_Operations_Stop >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_Pause >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_Pause >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_BasicOperation > convert( const Ptr< Operations::Invocations_Operations_Pause >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation;
}
template <>
inline Ptr< Operations::Invocations_Operations_Pause > convert( const Ptr< Operations::Invocations_Operations_Pause >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_Resume >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_Resume >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_BasicOperation > convert( const Ptr< Operations::Invocations_Operations_Resume >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation;
}
template <>
inline Ptr< Operations::Invocations_Operations_Resume > convert( const Ptr< Operations::Invocations_Operations_Resume >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_Done >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_Done >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_BasicOperation > convert( const Ptr< Operations::Invocations_Operations_Done >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation;
}
template <>
inline Ptr< Operations::Invocations_Operations_Done > convert( const Ptr< Operations::Invocations_Operations_Done >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_WaitAction >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_WaitAction >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_BasicOperation > convert( const Ptr< Operations::Invocations_Operations_WaitAction >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation;
}
template <>
inline Ptr< Operations::Invocations_Operations_WaitAction > convert( const Ptr< Operations::Invocations_Operations_WaitAction >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_WaitDimension >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_WaitDimension >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_DimensionOperation > convert( const Ptr< Operations::Invocations_Operations_WaitDimension >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation;
}
template <>
inline Ptr< Operations::Invocations_Operations_WaitDimension > convert( const Ptr< Operations::Invocations_Operations_WaitDimension >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_GetAction >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_GetAction >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_BasicOperation > convert( const Ptr< Operations::Invocations_Operations_GetAction >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation;
}
template <>
inline Ptr< Operations::Invocations_Operations_GetAction > convert( const Ptr< Operations::Invocations_Operations_GetAction >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_GetDimension >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_GetDimension >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_DimensionOperation > convert( const Ptr< Operations::Invocations_Operations_GetDimension >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation;
}
template <>
inline Ptr< Operations::Invocations_Operations_GetDimension > convert( const Ptr< Operations::Invocations_Operations_GetDimension >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_Read >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_Read >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_DimensionOperation > convert( const Ptr< Operations::Invocations_Operations_Read >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation;
}
template <>
inline Ptr< Operations::Invocations_Operations_Read > convert( const Ptr< Operations::Invocations_Operations_Read >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_Write >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_Write >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_DimensionOperation > convert( const Ptr< Operations::Invocations_Operations_Write >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation;
}
template <>
inline Ptr< Operations::Invocations_Operations_Write > convert( const Ptr< Operations::Invocations_Operations_Write >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_WriteLink >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_WriteLink >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_DimensionOperation > convert( const Ptr< Operations::Invocations_Operations_WriteLink >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation;
}
template <>
inline Ptr< Operations::Invocations_Operations_WriteLink > convert( const Ptr< Operations::Invocations_Operations_WriteLink >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Invocations_Instructions_Instruction > convert( const Ptr< Operations::Invocations_Operations_Range >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
template <>
inline Ptr< Operations::Invocations_Operations_Operation > convert( const Ptr< Operations::Invocations_Operations_Range >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation;
}
template <>
inline Ptr< Operations::Invocations_Operations_BasicOperation > convert( const Ptr< Operations::Invocations_Operations_Range >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation;
}
template <>
inline Ptr< Operations::Invocations_Operations_Range > convert( const Ptr< Operations::Invocations_Operations_Range >& from )
{
    return from;
}
template <>
inline Ptr< Concrete::Concrete_Dimension > convert( const Ptr< Concrete::Concrete_Dimension >& from )
{
    return from;
}
template <>
inline Ptr< Concrete::Concrete_ContextGroup > convert( const Ptr< Concrete::Concrete_ContextGroup >& from )
{
    return from;
}
template <>
inline Ptr< Concrete::Concrete_ContextGroup > convert( const Ptr< Concrete::Concrete_Context >& from )
{
    return from->p_Concrete_Concrete_ContextGroup;
}
template <>
inline Ptr< Concrete::Concrete_Context > convert( const Ptr< Concrete::Concrete_Context >& from )
{
    return from;
}
template <>
inline Ptr< Concrete::Concrete_ContextGroup > convert( const Ptr< Concrete::Concrete_Namespace >& from )
{
    return from->p_Concrete_Concrete_Context->p_Concrete_Concrete_ContextGroup;
}
template <>
inline Ptr< Concrete::Concrete_Context > convert( const Ptr< Concrete::Concrete_Namespace >& from )
{
    return from->p_Concrete_Concrete_Context;
}
template <>
inline Ptr< Concrete::Concrete_Namespace > convert( const Ptr< Concrete::Concrete_Namespace >& from )
{
    return from;
}
template <>
inline Ptr< Concrete::Concrete_ContextGroup > convert( const Ptr< Concrete::Concrete_Action >& from )
{
    return from->p_Concrete_Concrete_Context->p_Concrete_Concrete_ContextGroup;
}
template <>
inline Ptr< Concrete::Concrete_Context > convert( const Ptr< Concrete::Concrete_Action >& from )
{
    return from->p_Concrete_Concrete_Context;
}
template <>
inline Ptr< Concrete::Concrete_Action > convert( const Ptr< Concrete::Concrete_Action >& from )
{
    return from;
}
template <>
inline Ptr< Concrete::Concrete_ContextGroup > convert( const Ptr< Concrete::Concrete_Event >& from )
{
    return from->p_Concrete_Concrete_Context->p_Concrete_Concrete_ContextGroup;
}
template <>
inline Ptr< Concrete::Concrete_Context > convert( const Ptr< Concrete::Concrete_Event >& from )
{
    return from->p_Concrete_Concrete_Context;
}
template <>
inline Ptr< Concrete::Concrete_Event > convert( const Ptr< Concrete::Concrete_Event >& from )
{
    return from;
}
template <>
inline Ptr< Concrete::Concrete_ContextGroup > convert( const Ptr< Concrete::Concrete_Function >& from )
{
    return from->p_Concrete_Concrete_Context->p_Concrete_Concrete_ContextGroup;
}
template <>
inline Ptr< Concrete::Concrete_Context > convert( const Ptr< Concrete::Concrete_Function >& from )
{
    return from->p_Concrete_Concrete_Context;
}
template <>
inline Ptr< Concrete::Concrete_Function > convert( const Ptr< Concrete::Concrete_Function >& from )
{
    return from;
}
template <>
inline Ptr< Concrete::Concrete_ContextGroup > convert( const Ptr< Concrete::Concrete_Object >& from )
{
    return from->p_Concrete_Concrete_Context->p_Concrete_Concrete_ContextGroup;
}
template <>
inline Ptr< Concrete::Concrete_Context > convert( const Ptr< Concrete::Concrete_Object >& from )
{
    return from->p_Concrete_Concrete_Context;
}
template <>
inline Ptr< Concrete::Concrete_Object > convert( const Ptr< Concrete::Concrete_Object >& from )
{
    return from;
}
template <>
inline Ptr< Concrete::Concrete_ContextGroup > convert( const Ptr< Concrete::Concrete_Link >& from )
{
    return from->p_Concrete_Concrete_Context->p_Concrete_Concrete_ContextGroup;
}
template <>
inline Ptr< Concrete::Concrete_Context > convert( const Ptr< Concrete::Concrete_Link >& from )
{
    return from->p_Concrete_Concrete_Context;
}
template <>
inline Ptr< Concrete::Concrete_Link > convert( const Ptr< Concrete::Concrete_Link >& from )
{
    return from;
}
template <>
inline Ptr< Concrete::Concrete_ContextGroup > convert( const Ptr< Concrete::Concrete_Table >& from )
{
    return from->p_Concrete_Concrete_Context->p_Concrete_Concrete_ContextGroup;
}
template <>
inline Ptr< Concrete::Concrete_Context > convert( const Ptr< Concrete::Concrete_Table >& from )
{
    return from->p_Concrete_Concrete_Context;
}
template <>
inline Ptr< Concrete::Concrete_Table > convert( const Ptr< Concrete::Concrete_Table >& from )
{
    return from;
}
template <>
inline Ptr< Concrete::Concrete_ContextGroup > convert( const Ptr< Concrete::Concrete_Root >& from )
{
    return from->p_Concrete_Concrete_ContextGroup;
}
template <>
inline Ptr< Concrete::Concrete_Root > convert( const Ptr< Concrete::Concrete_Root >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Operations_InterfaceVariant > convert( const Ptr< Operations::Operations_InterfaceVariant >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Operations_ConcreteVariant > convert( const Ptr< Operations::Operations_ConcreteVariant >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Operations_Element > convert( const Ptr< Operations::Operations_Element >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Operations_ElementVector > convert( const Ptr< Operations::Operations_ElementVector >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Operations_Context > convert( const Ptr< Operations::Operations_Context >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Operations_TypePath > convert( const Ptr< Operations::Operations_TypePath >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Operations_NameRoot > convert( const Ptr< Operations::Operations_NameRoot >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Operations_NameRoot > convert( const Ptr< Operations::Operations_Name >& from )
{
    return from->p_Operations_Operations_NameRoot;
}
template <>
inline Ptr< Operations::Operations_Name > convert( const Ptr< Operations::Operations_Name >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Operations_NameResolution > convert( const Ptr< Operations::Operations_NameResolution >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Operations_Invocation > convert( const Ptr< Operations::Operations_Invocation >& from )
{
    return from;
}
template <>
inline Ptr< Operations::Operations_Invocations > convert( const Ptr< Operations::Operations_Invocations >& from )
{
    return from;
}
template <>
inline Ptr< DPGraph::Dependencies_Glob > convert( const Ptr< DPGraph::Dependencies_Glob >& from )
{
    return from;
}
template <>
inline Ptr< DPGraph::Dependencies_ObjectDependencies > convert( const Ptr< DPGraph::Dependencies_ObjectDependencies >& from )
{
    return from;
}
template <>
inline Ptr< DPGraph::Dependencies_Analysis > convert( const Ptr< DPGraph::Dependencies_Analysis >& from )
{
    return from;
}
template <>
inline Ptr< SymbolTable::Symbols_Symbol > convert( const Ptr< SymbolTable::Symbols_Symbol >& from )
{
    return from;
}
template <>
inline Ptr< SymbolTable::Symbols_SymbolSet > convert( const Ptr< SymbolTable::Symbols_SymbolSet >& from )
{
    return from;
}
template <>
inline Ptr< SymbolTable::Symbols_SymbolTable > convert( const Ptr< SymbolTable::Symbols_SymbolTable >& from )
{
    return from;
}
template <>
inline Ptr< Model::HyperGraph_ObjectGraph > convert( const Ptr< Model::HyperGraph_ObjectGraph >& from )
{
    return from;
}
template <>
inline Ptr< Model::HyperGraph_Graph > convert( const Ptr< Model::HyperGraph_Graph >& from )
{
    return from;
}
template <>
inline Ptr< Derivations::Derivation_ObjectMapping > convert( const Ptr< Derivations::Derivation_ObjectMapping >& from )
{
    return from;
}
template <>
inline Ptr< Derivations::Derivation_Mapping > convert( const Ptr< Derivations::Derivation_Mapping >& from )
{
    return from;
}
inline Ptr< Components::Components_Component > to_base( const Ptr< Components::Components_Component >& from )
{
    return from;
}
inline Ptr< AST::Parser_Identifier > to_base( const Ptr< AST::Parser_Identifier >& from )
{
    return from;
}
inline Ptr< AST::Parser_ScopedIdentifier > to_base( const Ptr< AST::Parser_ScopedIdentifier >& from )
{
    return from;
}
inline Ptr< AST::Parser_ArgumentList > to_base( const Ptr< AST::Parser_ArgumentList >& from )
{
    return from;
}
inline Ptr< AST::Parser_ReturnType > to_base( const Ptr< AST::Parser_ReturnType >& from )
{
    return from;
}
inline Ptr< AST::Parser_Inheritance > to_base( const Ptr< AST::Parser_Inheritance >& from )
{
    return from;
}
inline Ptr< AST::Parser_Size > to_base( const Ptr< AST::Parser_Size >& from )
{
    return from;
}
inline Ptr< AST::Parser_Link > to_base( const Ptr< AST::Parser_Link >& from )
{
    return from;
}
inline Ptr< AST::Parser_Dimension > to_base( const Ptr< AST::Parser_Dimension >& from )
{
    return from;
}
inline Ptr< AST::Parser_Include > to_base( const Ptr< AST::Parser_Include >& from )
{
    return from;
}
inline Ptr< AST::Parser_Include > to_base( const Ptr< AST::Parser_SystemInclude >& from )
{
    return from->p_AST_Parser_Include;
}
inline Ptr< AST::Parser_Include > to_base( const Ptr< AST::Parser_MegaInclude >& from )
{
    return from->p_AST_Parser_Include;
}
inline Ptr< AST::Parser_Include > to_base( const Ptr< AST::Parser_MegaIncludeInline >& from )
{
    return from->p_AST_Parser_MegaInclude->p_AST_Parser_Include;
}
inline Ptr< AST::Parser_Include > to_base( const Ptr< AST::Parser_MegaIncludeNested >& from )
{
    return from->p_AST_Parser_MegaInclude->p_AST_Parser_Include;
}
inline Ptr< AST::Parser_Include > to_base( const Ptr< AST::Parser_CPPInclude >& from )
{
    return from->p_AST_Parser_Include;
}
inline Ptr< AST::Parser_Dependency > to_base( const Ptr< AST::Parser_Dependency >& from )
{
    return from;
}
inline Ptr< AST::Parser_ContextDef > to_base( const Ptr< AST::Parser_ContextDef >& from )
{
    return from;
}
inline Ptr< AST::Parser_ContextDef > to_base( const Ptr< AST::Parser_NamespaceDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
inline Ptr< AST::Parser_ContextDef > to_base( const Ptr< AST::Parser_AbstractDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
inline Ptr< AST::Parser_ContextDef > to_base( const Ptr< AST::Parser_ActionDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
inline Ptr< AST::Parser_ContextDef > to_base( const Ptr< AST::Parser_EventDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
inline Ptr< AST::Parser_ContextDef > to_base( const Ptr< AST::Parser_FunctionDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
inline Ptr< AST::Parser_ContextDef > to_base( const Ptr< AST::Parser_ObjectDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
inline Ptr< AST::Parser_ContextDef > to_base( const Ptr< AST::Parser_LinkDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
inline Ptr< AST::Parser_ContextDef > to_base( const Ptr< AST::Parser_TableDef >& from )
{
    return from->p_AST_Parser_ContextDef;
}
inline Ptr< AST::Parser_SourceRoot > to_base( const Ptr< AST::Parser_SourceRoot >& from )
{
    return from;
}
inline Ptr< AST::Parser_SourceRoot > to_base( const Ptr< AST::Parser_IncludeRoot >& from )
{
    return from->p_AST_Parser_SourceRoot;
}
inline Ptr< AST::Parser_SourceRoot > to_base( const Ptr< AST::Parser_ObjectSourceRoot >& from )
{
    return from->p_AST_Parser_SourceRoot;
}
inline Ptr< AST::Parser_Dimension > to_base( const Ptr< Tree::Interface_DimensionTrait >& from )
{
    return from->p_AST_Parser_Dimension;
}
inline Ptr< AST::Parser_Inheritance > to_base( const Ptr< Tree::Interface_InheritanceTrait >& from )
{
    return from->p_AST_Parser_Inheritance;
}
inline Ptr< AST::Parser_Link > to_base( const Ptr< Tree::Interface_LinkTrait >& from )
{
    return from->p_AST_Parser_Link;
}
inline Ptr< AST::Parser_ReturnType > to_base( const Ptr< Tree::Interface_ReturnTypeTrait >& from )
{
    return from->p_AST_Parser_ReturnType;
}
inline Ptr< AST::Parser_ArgumentList > to_base( const Ptr< Tree::Interface_ArgumentListTrait >& from )
{
    return from->p_AST_Parser_ArgumentList;
}
inline Ptr< AST::Parser_Size > to_base( const Ptr< Tree::Interface_SizeTrait >& from )
{
    return from->p_AST_Parser_Size;
}
inline Ptr< Tree::Interface_ContextGroup > to_base( const Ptr< Tree::Interface_ContextGroup >& from )
{
    return from;
}
inline Ptr< Tree::Interface_ContextGroup > to_base( const Ptr< Tree::Interface_Root >& from )
{
    return from->p_Tree_Interface_ContextGroup;
}
inline Ptr< Tree::Interface_ContextGroup > to_base( const Ptr< Tree::Interface_IContext >& from )
{
    return from->p_Tree_Interface_ContextGroup;
}
inline Ptr< Tree::Interface_ContextGroup > to_base( const Ptr< Tree::Interface_Namespace >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
inline Ptr< Tree::Interface_ContextGroup > to_base( const Ptr< Tree::Interface_Abstract >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
inline Ptr< Tree::Interface_ContextGroup > to_base( const Ptr< Tree::Interface_Action >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
inline Ptr< Tree::Interface_ContextGroup > to_base( const Ptr< Tree::Interface_Event >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
inline Ptr< Tree::Interface_ContextGroup > to_base( const Ptr< Tree::Interface_Function >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
inline Ptr< Tree::Interface_ContextGroup > to_base( const Ptr< Tree::Interface_Object >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
inline Ptr< Tree::Interface_ContextGroup > to_base( const Ptr< Tree::Interface_Link >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
inline Ptr< Tree::Interface_ContextGroup > to_base( const Ptr< Tree::Interface_Table >& from )
{
    return from->p_Tree_Interface_IContext->p_Tree_Interface_ContextGroup;
}
inline Ptr< Operations::Invocations_Variables_Variable > to_base( const Ptr< Operations::Invocations_Variables_Variable >& from )
{
    return from;
}
inline Ptr< Operations::Invocations_Variables_Variable > to_base( const Ptr< Operations::Invocations_Variables_Instance >& from )
{
    return from->p_Operations_Invocations_Variables_Variable;
}
inline Ptr< Operations::Invocations_Variables_Variable > to_base( const Ptr< Operations::Invocations_Variables_Reference >& from )
{
    return from->p_Operations_Invocations_Variables_Variable;
}
inline Ptr< Operations::Invocations_Variables_Variable > to_base( const Ptr< Operations::Invocations_Variables_Dimension >& from )
{
    return from->p_Operations_Invocations_Variables_Reference->p_Operations_Invocations_Variables_Variable;
}
inline Ptr< Operations::Invocations_Variables_Variable > to_base( const Ptr< Operations::Invocations_Variables_Context >& from )
{
    return from->p_Operations_Invocations_Variables_Reference->p_Operations_Invocations_Variables_Variable;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Instructions_Instruction >& from )
{
    return from;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Instructions_InstructionGroup >& from )
{
    return from->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Instructions_Root >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Instructions_ParentDerivation >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Instructions_ChildDerivation >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Instructions_EnumDerivation >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Instructions_Enumeration >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Instructions_DimensionReferenceRead >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Instructions_MonoReference >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Instructions_PolyReference >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Instructions_PolyCase >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Instructions_Failure >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Instructions_Elimination >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Instructions_Prune >& from )
{
    return from->p_Operations_Invocations_Instructions_InstructionGroup->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_Operation >& from )
{
    return from->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_BasicOperation >& from )
{
    return from->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_DimensionOperation >& from )
{
    return from->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_Call >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_Start >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_Stop >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_Pause >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_Resume >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_Done >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_WaitAction >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_WaitDimension >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_GetAction >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_GetDimension >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_Read >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_Write >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_WriteLink >& from )
{
    return from->p_Operations_Invocations_Operations_DimensionOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Operations::Invocations_Instructions_Instruction > to_base( const Ptr< Operations::Invocations_Operations_Range >& from )
{
    return from->p_Operations_Invocations_Operations_BasicOperation->p_Operations_Invocations_Operations_Operation->p_Operations_Invocations_Instructions_Instruction;
}
inline Ptr< Concrete::Concrete_Dimension > to_base( const Ptr< Concrete::Concrete_Dimension >& from )
{
    return from;
}
inline Ptr< Concrete::Concrete_ContextGroup > to_base( const Ptr< Concrete::Concrete_ContextGroup >& from )
{
    return from;
}
inline Ptr< Concrete::Concrete_ContextGroup > to_base( const Ptr< Concrete::Concrete_Context >& from )
{
    return from->p_Concrete_Concrete_ContextGroup;
}
inline Ptr< Concrete::Concrete_ContextGroup > to_base( const Ptr< Concrete::Concrete_Namespace >& from )
{
    return from->p_Concrete_Concrete_Context->p_Concrete_Concrete_ContextGroup;
}
inline Ptr< Concrete::Concrete_ContextGroup > to_base( const Ptr< Concrete::Concrete_Action >& from )
{
    return from->p_Concrete_Concrete_Context->p_Concrete_Concrete_ContextGroup;
}
inline Ptr< Concrete::Concrete_ContextGroup > to_base( const Ptr< Concrete::Concrete_Event >& from )
{
    return from->p_Concrete_Concrete_Context->p_Concrete_Concrete_ContextGroup;
}
inline Ptr< Concrete::Concrete_ContextGroup > to_base( const Ptr< Concrete::Concrete_Function >& from )
{
    return from->p_Concrete_Concrete_Context->p_Concrete_Concrete_ContextGroup;
}
inline Ptr< Concrete::Concrete_ContextGroup > to_base( const Ptr< Concrete::Concrete_Object >& from )
{
    return from->p_Concrete_Concrete_Context->p_Concrete_Concrete_ContextGroup;
}
inline Ptr< Concrete::Concrete_ContextGroup > to_base( const Ptr< Concrete::Concrete_Link >& from )
{
    return from->p_Concrete_Concrete_Context->p_Concrete_Concrete_ContextGroup;
}
inline Ptr< Concrete::Concrete_ContextGroup > to_base( const Ptr< Concrete::Concrete_Table >& from )
{
    return from->p_Concrete_Concrete_Context->p_Concrete_Concrete_ContextGroup;
}
inline Ptr< Concrete::Concrete_ContextGroup > to_base( const Ptr< Concrete::Concrete_Root >& from )
{
    return from->p_Concrete_Concrete_ContextGroup;
}
inline Ptr< Operations::Operations_InterfaceVariant > to_base( const Ptr< Operations::Operations_InterfaceVariant >& from )
{
    return from;
}
inline Ptr< Operations::Operations_ConcreteVariant > to_base( const Ptr< Operations::Operations_ConcreteVariant >& from )
{
    return from;
}
inline Ptr< Operations::Operations_Element > to_base( const Ptr< Operations::Operations_Element >& from )
{
    return from;
}
inline Ptr< Operations::Operations_ElementVector > to_base( const Ptr< Operations::Operations_ElementVector >& from )
{
    return from;
}
inline Ptr< Operations::Operations_Context > to_base( const Ptr< Operations::Operations_Context >& from )
{
    return from;
}
inline Ptr< Operations::Operations_TypePath > to_base( const Ptr< Operations::Operations_TypePath >& from )
{
    return from;
}
inline Ptr< Operations::Operations_NameRoot > to_base( const Ptr< Operations::Operations_NameRoot >& from )
{
    return from;
}
inline Ptr< Operations::Operations_NameRoot > to_base( const Ptr< Operations::Operations_Name >& from )
{
    return from->p_Operations_Operations_NameRoot;
}
inline Ptr< Operations::Operations_NameResolution > to_base( const Ptr< Operations::Operations_NameResolution >& from )
{
    return from;
}
inline Ptr< Operations::Operations_Invocation > to_base( const Ptr< Operations::Operations_Invocation >& from )
{
    return from;
}
inline Ptr< Operations::Operations_Invocations > to_base( const Ptr< Operations::Operations_Invocations >& from )
{
    return from;
}
inline Ptr< DPGraph::Dependencies_Glob > to_base( const Ptr< DPGraph::Dependencies_Glob >& from )
{
    return from;
}
inline Ptr< DPGraph::Dependencies_ObjectDependencies > to_base( const Ptr< DPGraph::Dependencies_ObjectDependencies >& from )
{
    return from;
}
inline Ptr< DPGraph::Dependencies_Analysis > to_base( const Ptr< DPGraph::Dependencies_Analysis >& from )
{
    return from;
}
inline Ptr< SymbolTable::Symbols_Symbol > to_base( const Ptr< SymbolTable::Symbols_Symbol >& from )
{
    return from;
}
inline Ptr< SymbolTable::Symbols_SymbolSet > to_base( const Ptr< SymbolTable::Symbols_SymbolSet >& from )
{
    return from;
}
inline Ptr< SymbolTable::Symbols_SymbolTable > to_base( const Ptr< SymbolTable::Symbols_SymbolTable >& from )
{
    return from;
}
inline Ptr< Model::HyperGraph_ObjectGraph > to_base( const Ptr< Model::HyperGraph_ObjectGraph >& from )
{
    return from;
}
inline Ptr< Model::HyperGraph_Graph > to_base( const Ptr< Model::HyperGraph_Graph >& from )
{
    return from;
}
inline Ptr< Derivations::Derivation_ObjectMapping > to_base( const Ptr< Derivations::Derivation_ObjectMapping >& from )
{
    return from;
}
inline Ptr< Derivations::Derivation_Mapping > to_base( const Ptr< Derivations::Derivation_Mapping >& from )
{
    return from;
}

template < typename TVariant > 
inline TVariant to_upper( TVariant &from )
{
    TVariant up_most = from;
    for( ;; )
    {
        TVariant up_most_next = std::visit( []( auto&& arg ) -> TVariant { return arg->m_inheritance; }, up_most );
        if( up_most_next == up_most )
            break;
        up_most = up_most_next;
    }
    return up_most;
}

class Factory
{
public:
    static mega::io::Object* create( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo );
};

}
#endif //DATABASE_DATA_GUARD_4_APRIL_2022
