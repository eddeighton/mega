
#include "database/model/data.hxx"

#include "nlohmann/json.hpp"

namespace data
{

namespace Components
{
    // struct Component : public mega::io::Object
    Component::Component( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    Component::Component( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& name, const boost::filesystem::path& directory, const std::vector< boost::filesystem::path >& includeDirectories, const std::vector< boost::filesystem::path >& sourceFiles)
        :   mega::io::Object( objectInfo )
          , name( name )
          , directory( directory )
          , includeDirectories( includeDirectories )
          , sourceFiles( sourceFiles )
    {
    }
    void Component::load( mega::io::Loader& loader )
    {
        loader.load( name );
        loader.load( directory );
        loader.load( includeDirectories );
        loader.load( sourceFiles );
    }
    void Component::load_post( mega::io::Loader& loader )
    {
    }
    void Component::store( mega::io::Storer& storer ) const
    {
        storer.store( name );
        storer.store( directory );
        storer.store( includeDirectories );
        storer.store( sourceFiles );
    }
    void Component::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Component" },
                { "filetype" , "Components" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "name", name } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "directory", directory } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "includeDirectories", includeDirectories } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "sourceFiles", sourceFiles } } );
            part[ "properties" ].push_back( property );
        }
    }
        
}
namespace AST
{
    // struct Identifier : public mega::io::Object
    Identifier::Identifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    Identifier::Identifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo )
          , str( str )
    {
    }
    void Identifier::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void Identifier::load_post( mega::io::Loader& loader )
    {
    }
    void Identifier::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
    void Identifier::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Identifier" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "str", str } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct ScopedIdentifier : public mega::io::Object
    ScopedIdentifier::ScopedIdentifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    ScopedIdentifier::ScopedIdentifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Identifier > >& ids, const std::string& source_file, const std::size_t& line_number)
        :   mega::io::Object( objectInfo )
          , ids( ids )
          , source_file( source_file )
          , line_number( line_number )
    {
    }
    void ScopedIdentifier::load( mega::io::Loader& loader )
    {
        loader.load( ids );
        loader.load( source_file );
        loader.load( line_number );
    }
    void ScopedIdentifier::load_post( mega::io::Loader& loader )
    {
    }
    void ScopedIdentifier::store( mega::io::Storer& storer ) const
    {
        storer.store( ids );
        storer.store( source_file );
        storer.store( line_number );
    }
    void ScopedIdentifier::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "ScopedIdentifier" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "ids", ids } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "source_file", source_file } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "line_number", line_number } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct ArgumentList : public mega::io::Object
    ArgumentList::ArgumentList( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    ArgumentList::ArgumentList( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo )
          , str( str )
    {
    }
    void ArgumentList::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void ArgumentList::load_post( mega::io::Loader& loader )
    {
    }
    void ArgumentList::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
    void ArgumentList::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "ArgumentList" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "str", str } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct ReturnType : public mega::io::Object
    ReturnType::ReturnType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    ReturnType::ReturnType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo )
          , str( str )
    {
    }
    void ReturnType::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void ReturnType::load_post( mega::io::Loader& loader )
    {
    }
    void ReturnType::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
    void ReturnType::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "ReturnType" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "str", str } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Inheritance : public mega::io::Object
    Inheritance::Inheritance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    Inheritance::Inheritance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< std::string >& strings)
        :   mega::io::Object( objectInfo )
          , strings( strings )
    {
    }
    void Inheritance::load( mega::io::Loader& loader )
    {
        loader.load( strings );
    }
    void Inheritance::load_post( mega::io::Loader& loader )
    {
    }
    void Inheritance::store( mega::io::Storer& storer ) const
    {
        storer.store( strings );
    }
    void Inheritance::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Inheritance" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "strings", strings } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Size : public mega::io::Object
    Size::Size( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    Size::Size( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo )
          , str( str )
    {
    }
    void Size::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void Size::load_post( mega::io::Loader& loader )
    {
    }
    void Size::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
    void Size::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Size" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "str", str } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Dimension : public mega::io::Object
    Dimension::Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , id( loader )
    {
    }
    Dimension::Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const bool& isConst, const data::Ptr< data::AST::Identifier >& id, const std::string& type)
        :   mega::io::Object( objectInfo )
          , isConst( isConst )
          , id( id )
          , type( type )
    {
    }
    void Dimension::load( mega::io::Loader& loader )
    {
        loader.load( isConst );
        loader.load( id );
        loader.load( type );
    }
    void Dimension::load_post( mega::io::Loader& loader )
    {
    }
    void Dimension::store( mega::io::Storer& storer ) const
    {
        storer.store( isConst );
        storer.store( id );
        storer.store( type );
    }
    void Dimension::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Dimension" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "isConst", isConst } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "id", id } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "type", type } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Include : public mega::io::Object
    Include::Include( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    void Include::load( mega::io::Loader& loader )
    {
    }
    void Include::load_post( mega::io::Loader& loader )
    {
    }
    void Include::store( mega::io::Storer& storer ) const
    {
    }
    void Include::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Include" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct SystemInclude : public mega::io::Object
    SystemInclude::SystemInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_Include( loader )
    {
    }
    SystemInclude::SystemInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo )
          , p_AST_Include( loader )
          , str( str )
    {
    }
    void SystemInclude::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Include );
        loader.load( str );
    }
    void SystemInclude::load_post( mega::io::Loader& loader )
    {
        p_AST_Include->m_pInheritance = this;
    }
    void SystemInclude::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Include );
        storer.store( str );
    }
    void SystemInclude::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "SystemInclude" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "str", str } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct MegaInclude : public mega::io::Object
    MegaInclude::MegaInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_Include( loader )
          , root( loader )
    {
    }
    MegaInclude::MegaInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& megaSourceFilePath)
        :   mega::io::Object( objectInfo )
          , p_AST_Include( loader )
          , megaSourceFilePath( megaSourceFilePath )
    {
    }
    void MegaInclude::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Include );
        loader.load( megaSourceFilePath );
        loader.load( root );
    }
    void MegaInclude::load_post( mega::io::Loader& loader )
    {
        p_AST_Include->m_pInheritance = this;
    }
    void MegaInclude::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Include );
        storer.store( megaSourceFilePath );
        VERIFY_RTE_MSG( root.has_value(), "AST::MegaInclude.root has NOT been set" );
        storer.store( root );
    }
    void MegaInclude::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "MegaInclude" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "megaSourceFilePath", megaSourceFilePath } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "root", root.value() } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct MegaIncludeInline : public mega::io::Object
    MegaIncludeInline::MegaIncludeInline( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_MegaInclude( loader )
    {
    }
    void MegaIncludeInline::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_MegaInclude );
    }
    void MegaIncludeInline::load_post( mega::io::Loader& loader )
    {
        p_AST_MegaInclude->m_pInheritance = this;
    }
    void MegaIncludeInline::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_MegaInclude );
    }
    void MegaIncludeInline::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "MegaIncludeInline" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct MegaIncludeNested : public mega::io::Object
    MegaIncludeNested::MegaIncludeNested( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_MegaInclude( loader )
          , id( loader )
    {
    }
    MegaIncludeNested::MegaIncludeNested( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::ScopedIdentifier >& id)
        :   mega::io::Object( objectInfo )
          , p_AST_MegaInclude( loader )
          , id( id )
    {
    }
    void MegaIncludeNested::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_MegaInclude );
        loader.load( id );
    }
    void MegaIncludeNested::load_post( mega::io::Loader& loader )
    {
        p_AST_MegaInclude->m_pInheritance = this;
    }
    void MegaIncludeNested::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_MegaInclude );
        storer.store( id );
    }
    void MegaIncludeNested::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "MegaIncludeNested" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "id", id } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct CPPInclude : public mega::io::Object
    CPPInclude::CPPInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_Include( loader )
    {
    }
    CPPInclude::CPPInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& cppSourceFilePath)
        :   mega::io::Object( objectInfo )
          , p_AST_Include( loader )
          , cppSourceFilePath( cppSourceFilePath )
    {
    }
    void CPPInclude::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Include );
        loader.load( cppSourceFilePath );
    }
    void CPPInclude::load_post( mega::io::Loader& loader )
    {
        p_AST_Include->m_pInheritance = this;
    }
    void CPPInclude::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Include );
        storer.store( cppSourceFilePath );
    }
    void CPPInclude::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "CPPInclude" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "cppSourceFilePath", cppSourceFilePath } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Dependency : public mega::io::Object
    Dependency::Dependency( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    Dependency::Dependency( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo )
          , str( str )
    {
    }
    void Dependency::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void Dependency::load_post( mega::io::Loader& loader )
    {
    }
    void Dependency::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
    void Dependency::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Dependency" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "str", str } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct ContextDef : public mega::io::Object
    ContextDef::ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , id( loader )
    {
    }
    ContextDef::ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::ScopedIdentifier >& id, const std::vector< data::Ptr< data::AST::ContextDef > >& children, const std::vector< data::Ptr< data::AST::Dimension > >& dimensions, const std::vector< data::Ptr< data::AST::Include > >& includes, const std::vector< data::Ptr< data::AST::Dependency > >& dependencies, const std::string& body)
        :   mega::io::Object( objectInfo )
          , id( id )
          , children( children )
          , dimensions( dimensions )
          , includes( includes )
          , dependencies( dependencies )
          , body( body )
    {
    }
    void ContextDef::load( mega::io::Loader& loader )
    {
        loader.load( id );
        loader.load( children );
        loader.load( dimensions );
        loader.load( includes );
        loader.load( dependencies );
        loader.load( body );
    }
    void ContextDef::load_post( mega::io::Loader& loader )
    {
    }
    void ContextDef::store( mega::io::Storer& storer ) const
    {
        storer.store( id );
        storer.store( children );
        storer.store( dimensions );
        storer.store( includes );
        storer.store( dependencies );
        storer.store( body );
    }
    void ContextDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "ContextDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "id", id } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "children", children } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "includes", includes } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dependencies", dependencies } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "body", body } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct NamespaceDef : public mega::io::Object
    NamespaceDef::NamespaceDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
    {
    }
    void NamespaceDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_ContextDef );
    }
    void NamespaceDef::load_post( mega::io::Loader& loader )
    {
        p_AST_ContextDef->m_pInheritance = this;
    }
    void NamespaceDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_ContextDef );
    }
    void NamespaceDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "NamespaceDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct AbstractDef : public mega::io::Object
    AbstractDef::AbstractDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
    {
    }
    void AbstractDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_ContextDef );
    }
    void AbstractDef::load_post( mega::io::Loader& loader )
    {
        p_AST_ContextDef->m_pInheritance = this;
    }
    void AbstractDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_ContextDef );
    }
    void AbstractDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "AbstractDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct ActionDef : public mega::io::Object
    ActionDef::ActionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , size( loader )
          , inheritance( loader )
    {
    }
    ActionDef::ActionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Size >& size, const data::Ptr< data::AST::Inheritance >& inheritance)
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , size( size )
          , inheritance( inheritance )
    {
    }
    void ActionDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_ContextDef );
        loader.load( size );
        loader.load( inheritance );
    }
    void ActionDef::load_post( mega::io::Loader& loader )
    {
        p_AST_ContextDef->m_pInheritance = this;
    }
    void ActionDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_ContextDef );
        storer.store( size );
        storer.store( inheritance );
    }
    void ActionDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "ActionDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "size", size } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct EventDef : public mega::io::Object
    EventDef::EventDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , size( loader )
          , inheritance( loader )
    {
    }
    EventDef::EventDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Size >& size, const data::Ptr< data::AST::Inheritance >& inheritance)
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , size( size )
          , inheritance( inheritance )
    {
    }
    void EventDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_ContextDef );
        loader.load( size );
        loader.load( inheritance );
    }
    void EventDef::load_post( mega::io::Loader& loader )
    {
        p_AST_ContextDef->m_pInheritance = this;
    }
    void EventDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_ContextDef );
        storer.store( size );
        storer.store( inheritance );
    }
    void EventDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "EventDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "size", size } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct FunctionDef : public mega::io::Object
    FunctionDef::FunctionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , argumentList( loader )
          , returnType( loader )
    {
    }
    FunctionDef::FunctionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::ArgumentList >& argumentList, const data::Ptr< data::AST::ReturnType >& returnType)
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , argumentList( argumentList )
          , returnType( returnType )
    {
    }
    void FunctionDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_ContextDef );
        loader.load( argumentList );
        loader.load( returnType );
    }
    void FunctionDef::load_post( mega::io::Loader& loader )
    {
        p_AST_ContextDef->m_pInheritance = this;
    }
    void FunctionDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_ContextDef );
        storer.store( argumentList );
        storer.store( returnType );
    }
    void FunctionDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "FunctionDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "argumentList", argumentList } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "returnType", returnType } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct ObjectDef : public mega::io::Object
    ObjectDef::ObjectDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , inheritance( loader )
    {
    }
    ObjectDef::ObjectDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Inheritance >& inheritance)
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , inheritance( inheritance )
    {
    }
    void ObjectDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_ContextDef );
        loader.load( inheritance );
    }
    void ObjectDef::load_post( mega::io::Loader& loader )
    {
        p_AST_ContextDef->m_pInheritance = this;
    }
    void ObjectDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_ContextDef );
        storer.store( inheritance );
    }
    void ObjectDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "ObjectDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct LinkDef : public mega::io::Object
    LinkDef::LinkDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , inheritance( loader )
    {
    }
    LinkDef::LinkDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Inheritance >& inheritance)
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , inheritance( inheritance )
    {
    }
    void LinkDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_ContextDef );
        loader.load( inheritance );
    }
    void LinkDef::load_post( mega::io::Loader& loader )
    {
        p_AST_ContextDef->m_pInheritance = this;
    }
    void LinkDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_ContextDef );
        storer.store( inheritance );
    }
    void LinkDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "LinkDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct SourceRoot : public mega::io::Object
    SourceRoot::SourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , component( loader )
          , ast( loader )
    {
    }
    SourceRoot::SourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& sourceFile, const data::Ptr< data::Components::Component >& component, const data::Ptr< data::AST::ContextDef >& ast)
        :   mega::io::Object( objectInfo )
          , sourceFile( sourceFile )
          , component( component )
          , ast( ast )
    {
    }
    void SourceRoot::load( mega::io::Loader& loader )
    {
        loader.load( sourceFile );
        loader.load( component );
        loader.load( ast );
    }
    void SourceRoot::load_post( mega::io::Loader& loader )
    {
    }
    void SourceRoot::store( mega::io::Storer& storer ) const
    {
        storer.store( sourceFile );
        storer.store( component );
        storer.store( ast );
    }
    void SourceRoot::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "SourceRoot" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "sourceFile", sourceFile } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "component", component } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "ast", ast } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct IncludeRoot : public mega::io::Object
    IncludeRoot::IncludeRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_SourceRoot( loader )
    {
    }
    void IncludeRoot::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_SourceRoot );
    }
    void IncludeRoot::load_post( mega::io::Loader& loader )
    {
        p_AST_SourceRoot->m_pInheritance = this;
    }
    void IncludeRoot::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_SourceRoot );
    }
    void IncludeRoot::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "IncludeRoot" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct ObjectSourceRoot : public mega::io::Object
    ObjectSourceRoot::ObjectSourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_SourceRoot( loader )
    {
    }
    void ObjectSourceRoot::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_SourceRoot );
    }
    void ObjectSourceRoot::load_post( mega::io::Loader& loader )
    {
        p_AST_SourceRoot->m_pInheritance = this;
    }
    void ObjectSourceRoot::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_SourceRoot );
    }
    void ObjectSourceRoot::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "ObjectSourceRoot" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
}
namespace Body
{
}
namespace Tree
{
    // struct Dimension : public mega::io::Object
    Dimension::Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_Dimension( loader )
    {
    }
    void Dimension::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Dimension );
    }
    void Dimension::load_post( mega::io::Loader& loader )
    {
        p_AST_Dimension->m_pInheritance = this;
    }
    void Dimension::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Dimension );
    }
    void Dimension::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Dimension" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct ContextGroup : public mega::io::Object
    ContextGroup::ContextGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    ContextGroup::ContextGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Tree::Context > >& children)
        :   mega::io::Object( objectInfo )
          , children( children )
    {
    }
    void ContextGroup::load( mega::io::Loader& loader )
    {
        loader.load( children );
    }
    void ContextGroup::load_post( mega::io::Loader& loader )
    {
    }
    void ContextGroup::store( mega::io::Storer& storer ) const
    {
        storer.store( children );
    }
    void ContextGroup::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "ContextGroup" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "children", children } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Root : public mega::io::Object
    Root::Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_Tree_ContextGroup( loader )
          , root( loader )
    {
    }
    Root::Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::ObjectSourceRoot >& root)
        :   mega::io::Object( objectInfo )
          , p_Tree_ContextGroup( loader )
          , root( root )
    {
    }
    void Root::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_ContextGroup );
        loader.load( root );
    }
    void Root::load_post( mega::io::Loader& loader )
    {
        p_Tree_ContextGroup->m_pInheritance = this;
    }
    void Root::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_ContextGroup );
        storer.store( root );
    }
    void Root::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Root" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "root", root } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Context : public mega::io::Object
    Context::Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_Tree_ContextGroup( loader )
          , parent( loader )
    {
    }
    Context::Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& identifier, const data::Ptr< data::Tree::ContextGroup >& parent)
        :   mega::io::Object( objectInfo )
          , p_Tree_ContextGroup( loader )
          , identifier( identifier )
          , parent( parent )
    {
    }
    void Context::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_ContextGroup );
        loader.load( identifier );
        loader.load( parent );
    }
    void Context::load_post( mega::io::Loader& loader )
    {
        p_Tree_ContextGroup->m_pInheritance = this;
    }
    void Context::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_ContextGroup );
        storer.store( identifier );
        storer.store( parent );
    }
    void Context::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Context" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "identifier", identifier } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent", parent } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Namespace : public mega::io::Object
    Namespace::Namespace( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_Tree_Context( loader )
    {
    }
    Namespace::Namespace( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const bool& is_global, const std::vector< data::Ptr< data::AST::ContextDef > >& namespace_defs, const std::vector< data::Ptr< data::Tree::Dimension > >& dimensions)
        :   mega::io::Object( objectInfo )
          , p_Tree_Context( loader )
          , is_global( is_global )
          , namespace_defs( namespace_defs )
          , dimensions( dimensions )
    {
    }
    void Namespace::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Context );
        loader.load( is_global );
        loader.load( namespace_defs );
        loader.load( dimensions );
    }
    void Namespace::load_post( mega::io::Loader& loader )
    {
        p_Tree_Context->m_pInheritance = this;
    }
    void Namespace::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Context );
        storer.store( is_global );
        storer.store( namespace_defs );
        storer.store( dimensions );
    }
    void Namespace::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Namespace" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "is_global", is_global } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "namespace_defs", namespace_defs } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Abstract : public mega::io::Object
    Abstract::Abstract( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_Tree_Context( loader )
    {
    }
    Abstract::Abstract( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::AbstractDef > >& abstract_defs, const std::vector< data::Ptr< data::Tree::Dimension > >& dimensions)
        :   mega::io::Object( objectInfo )
          , p_Tree_Context( loader )
          , abstract_defs( abstract_defs )
          , dimensions( dimensions )
    {
    }
    void Abstract::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Context );
        loader.load( abstract_defs );
        loader.load( dimensions );
    }
    void Abstract::load_post( mega::io::Loader& loader )
    {
        p_Tree_Context->m_pInheritance = this;
    }
    void Abstract::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Context );
        storer.store( abstract_defs );
        storer.store( dimensions );
    }
    void Abstract::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Abstract" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "abstract_defs", abstract_defs } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Action : public mega::io::Object
    Action::Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_Tree_Context( loader )
    {
    }
    Action::Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::ActionDef > >& action_defs, const std::vector< data::Ptr< data::Tree::Dimension > >& dimensions)
        :   mega::io::Object( objectInfo )
          , p_Tree_Context( loader )
          , action_defs( action_defs )
          , dimensions( dimensions )
    {
    }
    void Action::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Context );
        loader.load( action_defs );
        loader.load( dimensions );
    }
    void Action::load_post( mega::io::Loader& loader )
    {
        p_Tree_Context->m_pInheritance = this;
    }
    void Action::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Context );
        storer.store( action_defs );
        storer.store( dimensions );
    }
    void Action::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Action" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "action_defs", action_defs } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Event : public mega::io::Object
    Event::Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_Tree_Context( loader )
    {
    }
    Event::Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::EventDef > >& event_defs, const std::vector< data::Ptr< data::Tree::Dimension > >& dimensions)
        :   mega::io::Object( objectInfo )
          , p_Tree_Context( loader )
          , event_defs( event_defs )
          , dimensions( dimensions )
    {
    }
    void Event::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Context );
        loader.load( event_defs );
        loader.load( dimensions );
    }
    void Event::load_post( mega::io::Loader& loader )
    {
        p_Tree_Context->m_pInheritance = this;
    }
    void Event::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Context );
        storer.store( event_defs );
        storer.store( dimensions );
    }
    void Event::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Event" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "event_defs", event_defs } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Function : public mega::io::Object
    Function::Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_Tree_Context( loader )
    {
    }
    Function::Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::FunctionDef > >& function_defs)
        :   mega::io::Object( objectInfo )
          , p_Tree_Context( loader )
          , function_defs( function_defs )
    {
    }
    void Function::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Context );
        loader.load( function_defs );
    }
    void Function::load_post( mega::io::Loader& loader )
    {
        p_Tree_Context->m_pInheritance = this;
    }
    void Function::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Context );
        storer.store( function_defs );
    }
    void Function::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Function" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "function_defs", function_defs } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Object : public mega::io::Object
    Object::Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_Tree_Context( loader )
    {
    }
    Object::Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::ObjectDef > >& object_defs, const std::vector< data::Ptr< data::Tree::Dimension > >& dimensions)
        :   mega::io::Object( objectInfo )
          , p_Tree_Context( loader )
          , object_defs( object_defs )
          , dimensions( dimensions )
    {
    }
    void Object::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Context );
        loader.load( object_defs );
        loader.load( dimensions );
    }
    void Object::load_post( mega::io::Loader& loader )
    {
        p_Tree_Context->m_pInheritance = this;
    }
    void Object::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Context );
        storer.store( object_defs );
        storer.store( dimensions );
    }
    void Object::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Object" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "object_defs", object_defs } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Link : public mega::io::Object
    Link::Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_Tree_Context( loader )
    {
    }
    Link::Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::LinkDef > >& link_defs)
        :   mega::io::Object( objectInfo )
          , p_Tree_Context( loader )
          , link_defs( link_defs )
    {
    }
    void Link::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Context );
        loader.load( link_defs );
    }
    void Link::load_post( mega::io::Loader& loader )
    {
        p_Tree_Context->m_pInheritance = this;
    }
    void Link::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Context );
        storer.store( link_defs );
    }
    void Link::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Link" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "link_defs", link_defs } } );
            part[ "properties" ].push_back( property );
        }
    }
        
}
namespace DPGraph
{
    // struct Glob : public mega::io::Object
    Glob::Glob( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    Glob::Glob( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& location, const std::string& glob)
        :   mega::io::Object( objectInfo )
          , location( location )
          , glob( glob )
    {
    }
    void Glob::load( mega::io::Loader& loader )
    {
        loader.load( location );
        loader.load( glob );
    }
    void Glob::load_post( mega::io::Loader& loader )
    {
    }
    void Glob::store( mega::io::Storer& storer ) const
    {
        storer.store( location );
        storer.store( glob );
    }
    void Glob::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Glob" },
                { "filetype" , "DPGraph" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "location", location } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "glob", glob } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct ObjectDependencies : public mega::io::Object
    ObjectDependencies::ObjectDependencies( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    ObjectDependencies::ObjectDependencies( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& source_file, const std::size_t& hash_code, const std::vector< data::Ptr< data::DPGraph::Glob > >& globs, const std::vector< boost::filesystem::path >& resolution)
        :   mega::io::Object( objectInfo )
          , source_file( source_file )
          , hash_code( hash_code )
          , globs( globs )
          , resolution( resolution )
    {
    }
    void ObjectDependencies::load( mega::io::Loader& loader )
    {
        loader.load( source_file );
        loader.load( hash_code );
        loader.load( globs );
        loader.load( resolution );
    }
    void ObjectDependencies::load_post( mega::io::Loader& loader )
    {
    }
    void ObjectDependencies::store( mega::io::Storer& storer ) const
    {
        storer.store( source_file );
        storer.store( hash_code );
        storer.store( globs );
        storer.store( resolution );
    }
    void ObjectDependencies::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "ObjectDependencies" },
                { "filetype" , "DPGraph" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "source_file", source_file } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "hash_code", hash_code } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "globs", globs } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "resolution", resolution } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Analysis : public mega::io::Object
    Analysis::Analysis( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    Analysis::Analysis( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::DPGraph::ObjectDependencies > >& objects)
        :   mega::io::Object( objectInfo )
          , objects( objects )
    {
    }
    void Analysis::load( mega::io::Loader& loader )
    {
        loader.load( objects );
    }
    void Analysis::load_post( mega::io::Loader& loader )
    {
    }
    void Analysis::store( mega::io::Storer& storer ) const
    {
        storer.store( objects );
    }
    void Analysis::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Analysis" },
                { "filetype" , "DPGraph" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "objects", objects } } );
            part[ "properties" ].push_back( property );
        }
    }
        
}


mega::io::Object* Factory::create( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
{
    switch( objectInfo.getType() )
    {
        case 0: return new Components::Component( loader, objectInfo );
        case 1: return new AST::Identifier( loader, objectInfo );
        case 2: return new AST::ScopedIdentifier( loader, objectInfo );
        case 3: return new AST::ArgumentList( loader, objectInfo );
        case 4: return new AST::ReturnType( loader, objectInfo );
        case 5: return new AST::Inheritance( loader, objectInfo );
        case 6: return new AST::Size( loader, objectInfo );
        case 7: return new AST::Dimension( loader, objectInfo );
        case 8: return new AST::Include( loader, objectInfo );
        case 9: return new AST::SystemInclude( loader, objectInfo );
        case 10: return new AST::MegaInclude( loader, objectInfo );
        case 11: return new AST::MegaIncludeInline( loader, objectInfo );
        case 12: return new AST::MegaIncludeNested( loader, objectInfo );
        case 13: return new AST::CPPInclude( loader, objectInfo );
        case 14: return new AST::Dependency( loader, objectInfo );
        case 15: return new AST::ContextDef( loader, objectInfo );
        case 16: return new AST::NamespaceDef( loader, objectInfo );
        case 17: return new AST::AbstractDef( loader, objectInfo );
        case 18: return new AST::ActionDef( loader, objectInfo );
        case 19: return new AST::EventDef( loader, objectInfo );
        case 20: return new AST::FunctionDef( loader, objectInfo );
        case 21: return new AST::ObjectDef( loader, objectInfo );
        case 22: return new AST::LinkDef( loader, objectInfo );
        case 23: return new AST::SourceRoot( loader, objectInfo );
        case 24: return new AST::IncludeRoot( loader, objectInfo );
        case 25: return new AST::ObjectSourceRoot( loader, objectInfo );
        case 26: return new Tree::Dimension( loader, objectInfo );
        case 27: return new Tree::ContextGroup( loader, objectInfo );
        case 28: return new Tree::Root( loader, objectInfo );
        case 29: return new Tree::Context( loader, objectInfo );
        case 30: return new Tree::Namespace( loader, objectInfo );
        case 31: return new Tree::Abstract( loader, objectInfo );
        case 32: return new Tree::Action( loader, objectInfo );
        case 33: return new Tree::Event( loader, objectInfo );
        case 34: return new Tree::Function( loader, objectInfo );
        case 35: return new Tree::Object( loader, objectInfo );
        case 36: return new Tree::Link( loader, objectInfo );
        case 37: return new DPGraph::Glob( loader, objectInfo );
        case 38: return new DPGraph::ObjectDependencies( loader, objectInfo );
        case 39: return new DPGraph::Analysis( loader, objectInfo );
        default:
            THROW_RTE( "Unrecognised object type ID" );
    }
}

}
